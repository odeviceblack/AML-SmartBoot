#include "Main.hpp"
#include <sstream>

// Ponteiros de funções
void (*StartGameScreen_onNewGameCheck)() = nullptr;
void (*StartGameScreen_onLoadGame)() = nullptr;
void (*MainMenuScreen_onResume)(void*) = nullptr;

// Enumerador
enum class StartMode : int {
	None = 0,
	NewGame,
	LoadGame,
	LoadSlot,
	AutoNewGame,
	AutoLoadGame
};

// Estado global
struct State {
	StartMode mode = StartMode::None;
	int resolved_slot = -1;
	std::vector<std::string> save_slots;

	inline void Reset() {
		*this = {};
	}

	inline bool IsEnabled() const {
		return mode != StartMode::None;
	}

	inline bool HasSlot() const {
		return resolved_slot != -1;
	}
};

static State state;


// Parsing mode
inline StartMode parseMode(const char* mode) {
	static constexpr const char* modes[] = {
		"none", "newgame", "loadgame",
		"loadslot", "auto", "auto2"
	};

	for(size_t i = 0; i < std::size(modes); i++) {
		if(strcmp(mode, modes[i]) == 0)
			return static_cast<StartMode>(i);
	}

	return StartMode::None;
}


// Parsing slots
inline std::vector<std::string> parseSlots(const char* slots) {
	if(!slots || !*slots) {
		logger->Print(LogP_Debug, "parseSlots: empty input");
		return {};
	}

	std::stringstream ss(slots);
	std::vector<std::string> out;

	for(std::string s; ss >> s;) {
		out.emplace_back(std::move(s));
	}

	logger->Print(LogP_Debug, "parseSlots: %d slots parsed", (int)out.size());
	return out;
}


// Parse save slot
inline int parseSaveSlot(const std::string& slot) {
	static constexpr char prefix[] = "GTASAsf";

	if(slot.rfind(prefix, 0) != 0) {
		logger->Print(LogP_Verbose, "parseSaveSlot: invalid prefix %s", slot.c_str());
		return -1;
	}

	char* end = nullptr;
	const long value = strtol(slot.c_str() + sizeof(prefix) - 1, &end, 10);

	if(strcmp(end, ".b") != 0) {
		logger->Print(LogP_Warn, "parseSaveSlot: invalid suffix %s", slot.c_str());
		return -1;
	}

	logger->Print(LogP_Debug, "parseSaveSlot: %s -> %ld", slot.c_str(), value);
	return (int)value;
}


// Resolve slot
inline int resolveSlot() {
	const std::string base = aml->GetAndroidDataPath();
	const std::string autoPath = base + "GTASAsf10.b";

	logger->Print(LogP_Info, "resolveSlot: base=%s", base.c_str());

	for(const auto& slot : state.save_slots) {
		const std::string full = base + slot;

		if(!aml->FileExists(full.c_str())) {
			logger->Print(LogP_Verbose, "missing file: %s", full.c_str());
			continue;
		}

		const int s = parseSaveSlot(slot);
		logger->Print(LogP_Debug, "slot=%s parsed=%d", slot.c_str(), s);

		if(s >= 1 && s <= 6) {
			logger->Print(LogP_Info, "copying save slot %d -> autosave", s);

			bool ok = aml->CopyFile(full.c_str(), autoPath.c_str());

			logger->Print(
				ok ? LogP_Info : LogP_Error, "copy result: %s -> %s = %s",
				full.c_str(), autoPath.c_str(), ok ? "OK" : "FAIL"
			);

			return ok ? 9 : -1;
		}

		if(s == 9) {
			logger->Print(LogP_Info, "slot 9 mapped to 8");
			return 8;
		}

		if(s == 10) {
			logger->Print(LogP_Info, "slot 10 mapped to 9");
			return 9;
		}
	}

	logger->Print(LogP_Warn, "resolveSlot: no valid slot found");
	return -1;
}


// Execute action
inline void executeAction(void* self) {
	logger->Print(LogP_Debug, "executeAction mode=%d", (int)state.mode);

	if(state.mode >= StartMode::LoadSlot) {
		if(state.HasSlot()) {
			logger->Print(LogP_Info, "resuming main menu");
			return MainMenuScreen_onResume(self);
		}

		logger->Print(LogP_Warn, "LoadSlot mode but no resolved slot");
	}

	switch(state.mode) {
		case StartMode::NewGame:
		case StartMode::AutoNewGame:
			logger->Print(LogP_Info, "start new game");
			return StartGameScreen_onNewGameCheck();

		case StartMode::LoadGame:
		case StartMode::AutoLoadGame:
			logger->Print(LogP_Info, "load game");
			return StartGameScreen_onLoadGame();

		default:
			logger->Print(LogP_Verbose, "no action executed");
			return;
	}
}


// Hooks
DECL_HOOK(void, MainMenuScreen_Update, void* self, float delta) {
	MainMenuScreen_Update(self, delta);

	if(!state.IsEnabled())
		return;

	logger->Print(LogP_Debug, "MainMenuScreen_Update intercepted");
	executeAction(self);

	state.Reset();
	logger->Print(LogP_Debug, "state reset");

	LoggerFlush();
}

DECL_HOOK(void, Menu_LoadSlot, int slot) {
	logger->Print(LogP_Info, "Menu_LoadSlot called slot=%d resolved=%d", slot, state.resolved_slot);

	if(!state.HasSlot())
		return Menu_LoadSlot(slot);

	logger->Print(LogP_Info, "redirecting to resolved slot %d", state.resolved_slot);

	Menu_LoadSlot(state.resolved_slot);
	state.resolved_slot = -1;
}


// Initialization
void startModeProcess(const char* mode, const char* slots) {
	logger->Print(
		LogP_Info, "startModeProcess mode=%s slots=%s",
		mode ? mode : "null", slots ? slots : "null"
	);

	state.Reset();
	state.mode = parseMode(mode);

	if(!state.IsEnabled()) {
		logger->Print(LogP_Warn, "mode disabled after parse");
		return;
	}

	state.save_slots = parseSlots(slots);
	state.resolved_slot = resolveSlot();

	logger->Print(LogP_Info, "resolved_slot=%d", state.resolved_slot);

	SETSYM_TO(StartGameScreen_onNewGameCheck, g_pLibGTASA, "_ZN15StartGameScreen14OnNewGameCheckEv");
	SETSYM_TO(StartGameScreen_onLoadGame, g_pLibGTASA, "_ZN15StartGameScreen10OnLoadGameEv");
	SETSYM_TO(MainMenuScreen_onResume, g_pLibGTASA, "_ZN14MainMenuScreen8OnResumeEv");

	if(state.HasSlot()) {
		logger->Print(LogP_Info, "hooking Menu_LoadSlot");
		HOOKSYM(Menu_LoadSlot, g_pLibGTASA, "_Z13Menu_LoadSloti");
	}

	HOOKSYM(MainMenuScreen_Update, g_pLibGTASA, "_ZN14MainMenuScreen6UpdateEf");

	logger->Print(LogP_Info, "hooks installed");
}
