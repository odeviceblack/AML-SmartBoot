#pragma once

#include <mod/logger.h>
#include <fstream>
#include <string>
#include <ctime>

inline const char* GetPriorityString(eLogPrio prio) {
	static constexpr const char* kPrioStr[] = {
		"unknown", "default", "verbose", "debug",
		"info", "warn", "error", "fatal", "silent"
	};

	const int idx = static_cast<int>(prio);
	if(idx < static_cast<int>(std::size(kPrioStr)))
		return kPrioStr[idx];

	return "unknown";
}

inline std::ofstream& GetLogFile() {
	static const std::string logPath = std::string(aml->GetAndroidDataRootPath()) + "log_FastBoot.txt";
	static std::ofstream file(logPath.c_str(), std::ios::app);
	return file;
}

inline void LoggerFlush() {
	if(logger->HasOutput()) {
		auto& file = GetLogFile();
		if(file) file.flush();
	}
}

inline void LoggerOutputProcess(eLogPrio prio, const char* msg) {
	auto& file = GetLogFile();
	if(!file) return;

	char timebuf[20];
	std::time_t now = std::time(nullptr);
	std::strftime(timebuf, sizeof(timebuf), "%d/%m/%Y %H:%M:%S", std::localtime(&now));
	file << "[" << timebuf << "] [" << GetPriorityString(prio) << "] " << msg << '\n';
}
