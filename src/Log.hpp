#ifndef LOG_H
#define LOG_H

#define dRED        "\x1b[0;31;40m"
#define dGREEN      "\x1b[0;32;40m"
#define dYELLOW     "\x1b[0;33;40m"
#define dBLUE       "\x1b[0;34;40m"
#define dMAGENTA    "\x1b[0;35;40m"
#define dCYAN       "\x1b[0;36;40m"
#define dWHITE      "\x1b[0;37;40m"

#define lRED		"\033[1;31m"
#define lGREEN		"\033[1;32m"
#define lYELLOW		"\033[1;33m"
#define lBLUE		"\033[1;34m"
#define lMAGENTA	"\033[1;35m"
#define lCYAN		"\033[1;36m"
#define lWHITE		"\033[1;37m"

#define RESET 	    "\x1b[0;7;0m"

#define LOG_FILE    "ascii.log"

#define _LogNone(msg)    Log::log((msg), LogType::NONE)
#define _LogMessage(msg) Log::log((msg), LogType::MESSAGE)
#define _LogInfo(msg)    Log::log((msg), LogType::INFO)
#define _LogWarning(msg) Log::log((msg), LogType::WARNING)
#define _LogError(msg)   Log::log((msg), LogType::ERROR)

#define PRINT(string)    std::cout << string << std::endl
#define STR(text)        std::string(text)

#include <string>

enum class LogType {
	NONE,
	MESSAGE,
	INFO,
	WARNING,
	ERROR,
};

class Log
{
private:
	static std::string getCurrentDateTime();
	static void logToFile(const std::string &message, bool date = true);
public:
	static std::string typeText(LogType type);
	static void log(const std::string &message, LogType type = LogType::NONE, bool file = false, bool date = true);
};

#endif
