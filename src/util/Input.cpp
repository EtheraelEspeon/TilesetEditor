#include "Input.hpp"

#include <format>

#include "AssetLoader.hpp"

void Input::Initialize() {
	if(instance != nullptr) delete instance;
	instance = new Input();
	ConfigParser::Parse();
	Logger::Message("Initialized input layer!");
}
Input* Input::instance = nullptr;

bool Input::KeybindIsPressed(std::string identifier) {
	auto keybind = TryGetKeybind(identifier);
	if(keybind == nullptr) return false;

	bool result = true;

	int numKeys = keybind->keyCombination.size();
	for(int i = 0; i < numKeys; i++) {
		auto key = Inst()->registeredKeys[keybind->keyCombination[i]];

		if(i == numKeys - 1) {
			result &= key.Pressed();
		}
		else {
			result &= key.Held();
		}

		if(!result) break;
	}

	return result;
}
bool Input::KeybindIsHeld(std::string identifier) {
	auto keybind = TryGetKeybind(identifier);
	if(keybind == nullptr) return false;

	bool result = true;

	int numKeys = keybind->keyCombination.size();
	for(int i = 0; i < numKeys; i++) {
		auto key = Inst()->registeredKeys[keybind->keyCombination[i]];

		result &= key.Held();

		if(!result) break;
	}

	return result;
}
bool Input::KeybindIsReleased(std::string identifier) {
	auto keybind = TryGetKeybind(identifier);
	if(keybind == nullptr) return false;
	
	bool allAreHeld = true;
	bool result = true;

	int numKeys = keybind->keyCombination.size();
	for(int i = 0; i < numKeys; i++) {
		auto key = Inst()->registeredKeys[keybind->keyCombination[i]];

		bool held = key.Held();
		allAreHeld &= held;
		result &= key.Released() || held;
	}
	result &= !allAreHeld;

	return result;
}

bool Input::PrimaryInteractionPressed() {
	return IsMouseButtonPressed(0);
}
bool Input::PrimaryInteractionHeld() {
	return IsMouseButtonDown(0);
}
bool Input::PrimaryInteractionReleased() {
	return IsMouseButtonReleased(0);
}

bool Input::SecondaryInteractionPressed() {
	return IsMouseButtonPressed(1);
}
bool Input::SecondaryInteractionHeld() {
	return IsMouseButtonDown(1);
}
bool Input::SecondaryInteractionReleased() {
	return IsMouseButtonReleased(1);
}

Input* Input::Inst() {
	if(instance == nullptr) {
		Logger::Error("Input singleton not initialized");
	}

	return instance;
}

Input::Keybind* Input::TryGetKeybind(std::string identifier) {
	auto itr = Inst()->keybinds.find(identifier);
	if(itr == Inst()->keybinds.end()) {
		if(!Inst()->missingKeybinds.contains(identifier)) {
			Logger::Warning(std::format("Keybind {} not found", identifier));
			Inst()->missingKeybinds.insert(identifier);
		}
		
		return nullptr;
	}

	return &(itr->second);
}

Input::Key::Key() {};
Input::Key::Key(std::function<bool()> pressed, std::function<bool()> held, std::function<bool()> released) {
	Pressed = pressed;
	Held = held;
	Released = released;
}

void Input::ConfigParser::Parse() {
	auto file = AssetLoader::LoadTextByLine("res/input.cfg");

	for(auto l : file) {{
		if(l == "") continue;

		// find indices of = and + symbols
		int assignmentIdx = -1;
		std::vector<int> keySeparators = {}; // the found indices
		for(int i = 0; i < l.size(); i++) {
			if(l[i] == '=') { // find the assignment
				if(assignmentIdx != -1) {
					Logger::Warning(std::format("Multiple assignments in input config line \"{}\"", l));
					goto failed_parsing_line;
				}
				assignmentIdx = i;
				keySeparators.push_back(i);
			}
			if(l[i] == '+') { // found a separator between two keys
				keySeparators.push_back(i);
			}
		}
		if(assignmentIdx == -1) { // if we failed to find the assignment
			Logger::Warning(std::format("No assignment in input config line \"{}\"", l));
			goto failed_parsing_line;
		}

		// split the string on the + and =, construct a keybind from that info
		Keybind kb;
		kb.identifier = l.substr(0, assignmentIdx);
		for(int i = 0; i < keySeparators.size(); i++) {
			// if this isnt the last separator, base it off the idx of the next separator
			// otherwise, set it to the end of the string
			int keycodeEnd = i < keySeparators.size() - 1 ? keySeparators[i+1] - 1 : l.size() - 1;
			
			// string name of the keycode (ex: "A", "Ctrl", "ScrollUp")
			std::string keycodeString = l.substr(keySeparators[i] + 1, keycodeEnd - keySeparators[i]);

			// register the key if it isnt already known
			if(!Inst()->registeredKeys.contains(keycodeString)) {
				Inst()->registeredKeys.insert({keycodeString, StringToKey(keycodeString)});
			}

			kb.keyCombination.push_back(keycodeString);
		}

		// add the found keybind
		//Logger::Debug(std::format("Loaded keybind \"{}\" with keycode {}", kb.identifier, l.substr(keySeparators[0] + 1)));
		Inst()->keybinds[kb.identifier] = kb;

	}failed_parsing_line:;}
}

Input::Key Input::ConfigParser::StringToKey(std::string id) {
	id = TextToLower(id.c_str());
	KeyboardKey keycode = StringToKeycode(id);

	if(keycode != KEY_NULL) {
		return Key(
			[keycode](){ return IsKeyPressed(keycode);  },
			[keycode](){ return IsKeyDown(keycode);     },
			[keycode](){ return IsKeyReleased(keycode); }
		);
	}
	else {
		if(id == "scrollup") {
			return Key(
				[](){ return GetMouseWheelMove() > 0; },
				[](){ return false; }, // doesn't make sense to
				[](){ return false; }  // hold or release a scroll
			);
		}
		if(id == "scrolldown") {
			return Key(
				[](){ return GetMouseWheelMove() < 0; },
				[](){ return false; }, // doesn't make sense to
				[](){ return false; }  // hold or release a scroll
			);
		}
	}

	Logger::Warning(std::format("Could not parse key name \"{}\"", id));
	return Key(
		[](){ return false; },
		[](){ return false; },
		[](){ return false; } 
	);
}
KeyboardKey Input::ConfigParser::StringToKeycode(std::string id) {
	id = TextToLower(id.c_str());

	auto itr = Keycodes.find(id);
	if(itr != Keycodes.end()) return itr->second;

	return KEY_NULL;
}

// Keys have to be lowercase, TextToLower is called on the key names given in input.cfg
const std::map<std::string, KeyboardKey> Input::ConfigParser::Keycodes = {
	{ "f1" , KEY_F1  },
	{ "f2" , KEY_F2  },
	{ "f3" , KEY_F3  },
	{ "f4" , KEY_F4  },
	{ "f5" , KEY_F5  },
	{ "f6" , KEY_F6  },
	{ "f7" , KEY_F7  },
	{ "f8" , KEY_F8  },
	{ "f9" , KEY_F9  },
	{ "f10", KEY_F10 },
	{ "f11", KEY_F11 },
	{ "f12", KEY_F12 },

	{ "numpad1", KEY_KP_1},
	{ "numpad2", KEY_KP_2},
	{ "numpad3", KEY_KP_3},
	{ "numpad4", KEY_KP_4},
	{ "numpad5", KEY_KP_5},
	{ "numpad6", KEY_KP_6},
	{ "numpad7", KEY_KP_7},
	{ "numpad8", KEY_KP_8},
	{ "numpad9", KEY_KP_9},
	{ "numpad0", KEY_KP_0},
	
	{ "numlock", KEY_NUM_LOCK },
	{ "numpadplus", KEY_KP_ADD },
	{ "numpadminus", KEY_KP_SUBTRACT },
	{ "numpadmultiply", KEY_KP_MULTIPLY},
	{ "numpadslash", KEY_KP_DIVIDE },
	{ "numpaddot", KEY_KP_DECIMAL },
	{ "numpadenter", KEY_KP_ENTER },

	{ "1", KEY_ONE   },
	{ "2", KEY_TWO   },
	{ "3", KEY_THREE },
	{ "4", KEY_FOUR  },
	{ "5", KEY_FIVE  },
	{ "6", KEY_SIX   },
	{ "7", KEY_SEVEN },
	{ "8", KEY_EIGHT },
	{ "9", KEY_NINE  },
	{ "0", KEY_ZERO  },

	{ "a", KEY_A },
	{ "b", KEY_B },
	{ "c", KEY_C },
	{ "d", KEY_D },
	{ "e", KEY_E },
	{ "f", KEY_F },
	{ "g", KEY_G },
	{ "h", KEY_H },
	{ "i", KEY_I },
	{ "j", KEY_J },
	{ "k", KEY_K },
	{ "l", KEY_L },
	{ "m", KEY_M },
	{ "n", KEY_N },
	{ "o", KEY_O },
	{ "p", KEY_P },
	{ "q", KEY_Q },
	{ "r", KEY_R },
	{ "s", KEY_S },
	{ "t", KEY_T },
	{ "u", KEY_U },
	{ "v", KEY_V },
	{ "w", KEY_W },
	{ "x", KEY_X },
	{ "y", KEY_Y },
	{ "z", KEY_Z },

	{ "ctrl", KEY_LEFT_CONTROL },
	{ "alt" , KEY_LEFT_ALT },
	{ "shift", KEY_LEFT_SHIFT },

	{ "delete", KEY_DELETE }
};
