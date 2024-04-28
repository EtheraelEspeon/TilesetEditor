#pragma once

#include <string>
#include <vector>
#include <map>
#include <set>
#include <functional>

#include "../raylib/raylib.h"

// Singleton
class Input {
	struct Key;
public:
	static void Initialize();

	struct Keybind {
		std::string identifier;
		std::vector<Key> keyCombination;
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
	// Holds closures for getting the state of a key
	struct Key {
		Key(std::function<bool()> pressed, std::function<bool()> held, std::function<bool()> released);
		std::function<bool()> Pressed; // no touchie these pls, c++ doesn't have readonly
		std::function<bool()> Held;
		std::function<bool()> Released;
	};
	class ConfigParser {
	public:
		/// @brief Modifies the state of the Input singleton
		static void Parse();

		static Key StringToKey(std::string id);
		static KeyboardKey StringToKeycode(std::string id);
	};

	static Input* instance;
	static Input* Inst();

	static Keybind* TryGetKeybind(std::string identifier);
	std::set<std::string> missingKeybinds = {};

	std::map<std::string, Keybind> keybinds = {};

	friend class ConfigParser;
};