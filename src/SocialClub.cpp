#include "Main.hpp"
#include "SmaliDEX.hpp"

JNIEnv* env = nullptr;
jobject instance = nullptr;
bool forceFullScreen = false;

void (*GTASA_exitSocialClub)() = nullptr;

// Hooks
DECL_HOOK(void, SocialClub_LoadScreen, void* self) {
	logger->Info("[SocialClub] LoadScreen called");	

	SocialClub_LoadScreen(self);
	logger->Info("[SocialClub] Post-load execution started");

	if(forceFullScreen) {
		logger->Info("[SocialClub] Force Full Screen enabled -> hiding system UI");

		CallJavaMethod<void>(instance, "hideSystemUI", "()V");
		logger->Info("[SocialClub] hideSystemUI executed");

		env->DeleteGlobalRef(instance);
		logger->Info("[SocialClub] DEX instance consumed and deleted");
	} else {
		logger->Info("[SocialClub] Force Full Screen disabled");
	}

	logger->Info("[SocialClub] calling GTASA_exitSocialClub()");
	GTASA_exitSocialClub();

	logger->Info("[SocialClub] exitSocialClub completed");
	LoggerFlush();
}

// Exit Social Club setup
void exitSocialClub() {
	logger->Info("=== exitSocialClub INIT START ===");
	
	forceFullScreen = cfg->GetBool("Force Full Screen", false);
	logger->Info("Config: Force Full Screen = %s", forceFullScreen ? "true" : "false");

	if(forceFullScreen) {
		env = aml->GetJNIEnvironment();

		if(!env) {
			logger->Print(LogP_Error, "JNIEnv is NULL!");
			return;
		}

		logger->Info("JNIEnv acquired: %p", env);

		instance = aml->InjectSmaliDEX(classes_dex, classes_dex_len, "com.rockstargames.gtasa.FastBoot");

		if(!instance) {
			logger->Print(LogP_Error, "DEX injection failed!");
			return;
		}

		logger->Info("DEX injected successfully: instance=%p", instance);
	}

	SETSYM_TO(GTASA_exitSocialClub, g_pLibSCAnd, "_ZN5GTASA14exitSocialClubEv");

	if(!GTASA_exitSocialClub) {
		logger->Print(LogP_Fatal, "Failed to resolve GTASA_exitSocialClub symbol!");
		return;
	}

	logger->Info("GTASA_exitSocialClub resolved: %p", (void*)GTASA_exitSocialClub);

	HOOKSYM(SocialClub_LoadScreen, g_pLibSCAnd, "_ZN10SocialClub10LoadScreenEv");

	logger->Info("SocialClub_LoadScreen hook installed");
	logger->Info("=== exitSocialClub INIT END ===");
}
