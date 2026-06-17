#include "Main.hpp"

MYMODCFG(deviceblack.FastBoot, FastBoot, 3.0.1.2, DeviceBlack)
NEEDGAME(com.rockstargames.gtasa)

uintptr_t g_pLibSCAnd = 0;
uintptr_t g_pLibGTASA = 0;

// Forward declarations
void exitSocialClub();
void startModeProcess(const char* mode, const char* slots);

// PRELOAD
ON_MOD_PRELOAD() {
	logger->SetTag("FastBoot");
	logger->SetMessageCB(LoggerOutputProcess);
	logger->ToggleOutput(cfg->GetBool("Log Output", false));

	logger->Info("=== FastBoot PRELOAD START ===");

	g_pLibSCAnd = aml->GetLib("libSCAnd.so");
	g_pLibGTASA = aml->GetLib("libGTASA.so");

	logger->Info("libSCAnd.so handle: %p", (void*)g_pLibSCAnd);
	logger->Info("libGTASA.so handle: %p", (void*)g_pLibGTASA);

	if(!g_pLibSCAnd || !g_pLibGTASA) {
		logger->Print(LogP_Fatal, "Missing required game libraries!");
		logger->Info("libSCAnd status: %s", g_pLibSCAnd ? "OK" : "MISSING");
		logger->Info("libGTASA status: %s", g_pLibGTASA ? "OK" : "MISSING");
		logger->Info("=== PRELOAD ABORTED ===");
		return;
	}

	logger->Info("Libraries loaded successfully");
	logger->Info("=== FastBoot PRELOAD OK ===");
}


// LOAD
ON_MOD_LOAD() {
	if(!g_pLibSCAnd || !g_pLibGTASA)
		return;

	logger->Info("=== FastBoot LOAD START ===");

	// Config
	const bool skipEula = cfg->GetBool("Eula Skip", true);
	logger->Info("Config: Eula Skip = %s", skipEula ? "true" : "false");

	if(skipEula) {
		logger->Info("Skipping Social Club / EULA");
		exitSocialClub();
	} else {
		logger->Info("EULA skip disabled");
	}

	char startMode[16], saveGames[128];
	cfg->GetString(startMode, sizeof(startMode), "Start Mode", "auto");
	cfg->GetString(saveGames, sizeof(saveGames), "Slot List", "GTASAsf9.b GTASAsf10.b");

	logger->Info("Config loaded:");
	logger->Info("  Start Mode: %s", startMode);
	logger->Info("  Slot List : %s", saveGames);
	logger->Info("Starting mode process...");

	startModeProcess(startMode, saveGames);
	logger->Info("=== FastBoot LOAD END ===");

	LoggerFlush();

	aml->ShowToast(true, "%s %s © %s", modinfo->Name(), modinfo->VersionString(), modinfo->Author());
}
