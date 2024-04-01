#include "Logger.hpp"

#include <iostream>

void Logger::Log(std::string message, Severity severity) {
	std::cout << SeverityAsString(severity) << " | " << message << "\n";
}

void Logger::Debug(std::string message) {
	Log(message, Severity::Debug);
}
void Logger::Message(std::string message) {
	Log(message, Severity::Message);
}
void Logger::Warning(std::string message) {
	Log(message, Severity::Warning);
}
void Logger::Error(std::string message) {
	Log(message, Severity::Error);
}

std::string Logger::SeverityAsString(Severity severity) {
	switch(severity) {
		case(Severity::Debug): return "Debug";
		case(Severity::Message): return "Message";
		case(Severity::Warning): return "Warning";
		case(Severity::Error): return "Error";
		default: return "Severity";
	}
}