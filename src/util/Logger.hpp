#pragma once

#include <cstdint>
#include <string>

namespace Logger {
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

	void Log(std::string message, Severity severity);
	void Debug(std::string message);
	void Message(std::string message);
	void Warning(std::string message);
	void Error(std::string message);

	std::string SeverityAsString(Severity severity);
};