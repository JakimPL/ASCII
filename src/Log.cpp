#include <string>
#include <vector>
#include <iostream>
#include <fstream>

#include "Log.hpp"

std::string Log::getCurrentDateTime()
{
	time_t now = time(0);
	struct tm tstruct;
	char buf[86];
	tstruct = *localtime(&now);
	strftime(buf, sizeof(buf), "%Y-%m-%d %X", &tstruct);
	return std::string(buf);
};

void Log::logToFile(const std::string &message, bool date)
{
	std::string filePath = LOG_FILE;
	std::string now = getCurrentDateTime();
	std::ofstream ofs(filePath.c_str(), std::ios_base::out | std::ios_base::app);
	if (date) {
		ofs << now << '\t';
	}
	ofs << message << '\n';
	ofs.close();
}

std::string Log::typeText(LogType type)
{
	std::string text;
	switch (type) {
	case LogType::NONE:
		text = "";
		break;
	case LogType::MESSAGE:
		text = "[MESSAGE] ";
		break;
	case LogType::INFO:
		text = "   [INFO] ";
		break;
	case LogType::WARNING:
		text = "[WARNING] ";
		break;
	case LogType::ERROR:
		text = "  [ERROR] ";
		break;
	}

	return text;
}

void Log::log(const std::string &message, LogType type, bool file, bool date)
{
	if (static_cast<unsigned int>(type) >= LOG_LEVEL) {
		std::string dateText = getCurrentDateTime();
		if (date) {
			std::cout << dateText << ": ";
		}
		std::cout << typeText(type) << message << "\n";
		if (file) {
			logToFile(message, date);
		}
	}
}
