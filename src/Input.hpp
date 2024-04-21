#pragma once

#include <string>
#include <vector>
#include <map>
#include <set>

#include "../raylib/raylib.h"

// Singleton
class Input {
public:
	static void Initialize();

	struct Keybind {
		std::string identifier;
		std::vector<KeyboardKey> keyCombination;
	};

	static bool KeybindIsPressed(std::string identifier);
	static bool KeybindIsHeld(std::string identifier);
	static bool KeybindIsReleased(std::string identifier);

	static bool PrimaryInteractionPressed();
	static bool PrimaryInteractionHeld();
	static bool PrimaryInteractionReleased();

	static bool SecondaryInteractionPressed();
	static bool SecondaryInteractionHeld();
	static bool SecondaryInteractionReleased();
private:
	class ConfigParser {
	public:
		/// @brief Modifies the state of the Input singleton
		static void Parse();

		static KeyboardKey StringToKeycode(std::string id);
	};

	static Input* instance;
	static Input* Inst();

	static Keybind* TryGetKeybind(std::string identifier);
	std::set<std::string> missingKeybinds = {};

	std::map<std::string, Keybind> keybinds = {};

	friend class ConfigParser;
};