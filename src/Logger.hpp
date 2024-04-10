#pragma once

#include <cstdint>
#include <string>

class Logger {
	public:
	enum class Severity : uint8_t {
		// for messages while debugging, there's no reason to print these in release mode
		Debug = 0,
		// for messages saying tasks have completed, like file i/o
		Message = 1,
		// for situations where the state of the program can't be changed as intended, but isn't invalidated. i.e., failing to set color idx 0
		Warning = 2,
		// for when you have to break an invariant, like setting a non-nullable pointer to null
		Error = 3
	};

	static void Log(std::string message, Severity severity);
	static void Debug(std::string message);
	static void Message(std::string message);
	static void Warning(std::string message);
	static void Error(std::string message);

	static std::string SeverityAsString(Severity severity);
};