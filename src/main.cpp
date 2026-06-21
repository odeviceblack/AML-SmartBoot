#include <mod/config.h>

#include "main.hpp"
#include "socialclub.hpp"
#include "startmode.hpp"

MYMODCFG(net.deviceblack.fastboot, FastBoot, 3.1.0.5, DeviceBlack);
NEEDGAME(com.rockstargames.gtasa)

uintptr_t g_pLibSCAnd = 0;
uintptr_t g_pLibGTASA = 0;

bool removeSocialClub = true;
bool showVersion = true;

char startMode[16];
char slotList[128];

ON_MOD_PRELOAD()
{
	g_pLibSCAnd = aml->GetLib("libSCAnd.so");
	g_pLibGTASA = aml->GetLib("libGTASA.so");

	removeSocialClub = cfg->GetBool("Remove Social Club", removeSocialClub);
	showVersion = cfg->GetBool("Show Version", showVersion);
	
	cfg->GetString(startMode, sizeof(startMode), "Start Mode", "auto");
	cfg->GetString(slotList, sizeof(slotList), "Slot List", "GTASAsf10.b GTASAsf9.b");

	if(!g_pLibSCAnd || !removeSocialClub)
	{
		removeSocialClub = false;
		return;
	}

	if(aml->HasModOfBiggerVersion("net.rusjj.jpatch", "1.10.0"))
		removeSocialClub = !Config("net.rusjj.jpatch").GetBool("RemoveSocialClub", false, "Gameplay");
}

// Fullscreen Force 😽
DECL_HOOK(void, MainMenuScreen_Update, void* self, float dt)
{
	MainMenuScreen_Update(self, dt);

	static const bool once = [&self, dt] {
		jobject instance = aml->InjectSmaliDEX(classes_dex, classes_dex_len, "net.deviceblack.fastboot.ForceFullScreen");
		CallJavaMethod<void>(instance, "enableFullScreen", "(Z)V", showVersion ? JNI_TRUE : JNI_FALSE);
		aml->GetJNIEnvironment()->DeleteGlobalRef(instance);

		MenuEntryPoint(self, dt); // startmode.cpp
		return true;
	}();
}

ON_MOD_LOAD()
{
	if(!g_pLibGTASA)
		return;

	HOOKSYM(MainMenuScreen_Update, g_pLibGTASA, "_ZN14MainMenuScreen6UpdateEf");

	if(removeSocialClub)
		RemoveSocialClub();

	StartMode(startMode, slotList);
}
