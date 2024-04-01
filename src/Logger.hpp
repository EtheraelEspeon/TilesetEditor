#pragma once

#include <cstdint>
#include <string>

class Logger {
	public:
	enum class Severity : uint8_t {
		Debug = 0,
		Message = 1,
		Warning = 2,
		Error = 3
	};

	static void Log(std::string message, Severity severity);
	static void Debug(std::string message);
	static void Message(std::string message);
	static void Warning(std::string message);
	static void Error(std::string message);

	static std::string SeverityAsString(Severity severity);
};