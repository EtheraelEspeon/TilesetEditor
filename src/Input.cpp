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
		auto key = keybind->keyCombination[i];

		if(i == numKeys - 1) {
			result &= IsKeyPressed(key);
		}
		else {
			result &= IsKeyDown(key);
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
		auto key = keybind->keyCombination[i];

		result &= IsKeyDown(key);

		if(!result) break;
	}

	return result;
}
bool Input::KeybindIsReleased(std::string identifier) {
	auto keybind = TryGetKeybind(identifier);
	if(keybind == nullptr) return false;
	
	bool result = true;

	int numKeys = keybind->keyCombination.size();
	for(int i = 0; i < numKeys; i++) {
		auto key = keybind->keyCombination[i];

		result &= IsKeyReleased(key);

		if(!result) break;
	}

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

void Input::ConfigParser::Parse() {
	auto file = AssetLoader::LoadTextByLine("res/input.cfg");

	for(auto l : file) {{
		if(l == "") continue;

		// find indices = and +
		int assignmentIdx = -1;
		std::vector<int> keySeparators = {};
		for(int i = 0; i < l.size(); i++) {
			if(l[i] == '=') {
				if(assignmentIdx != -1) {
					Logger::Warning(std::format("Multiple assignments in input config line \"{}\"", l));
					goto failed_parsing_line;
				}
				assignmentIdx = i;
				keySeparators.push_back(i);
			}
			if(l[i] == '+') {
				keySeparators.push_back(i);
			}
		}
		if(assignmentIdx == -1) {
			Logger::Warning(std::format("No assignment in input config line \"{}\"", l));
			goto failed_parsing_line;
		}

		// split the string on the + and =, construct a keybind from that info
		Keybind kb;
		kb.identifier = l.substr(0, assignmentIdx);
		for(int i = 0; i < keySeparators.size(); i++) {
			int keycodeEnd = i < keySeparators.size() - 1 ? keySeparators[i+1] - 1 : l.size() - 1;
			std::string keycodeString = l.substr(keySeparators[i] + 1, keycodeEnd - keySeparators[i]);

			kb.keyCombination.push_back(StringToKeycode(keycodeString));
		}

		// add the found keybind
		Logger::Debug(std::format("Loaded keybind \"{}\" with keycode {}", kb.identifier, l.substr(keySeparators[0] + 1)));
		Inst()->keybinds[kb.identifier] = kb;

	}failed_parsing_line:;}
}
KeyboardKey Input::ConfigParser::StringToKeycode(std::string id) {
	id = TextToLower(id.c_str());

	if(id == "1") return KEY_ONE;
	if(id == "2") return KEY_TWO;
	if(id == "3") return KEY_THREE;
	if(id == "4") return KEY_FOUR;
	if(id == "5") return KEY_FIVE;
	if(id == "6") return KEY_SIX;
	if(id == "7") return KEY_SEVEN;
	if(id == "8") return KEY_EIGHT;
	if(id == "9") return KEY_NINE;
	if(id == "0") return KEY_ZERO;

	if(id == "a") return KEY_A;
	if(id == "b") return KEY_B;
	if(id == "c") return KEY_C;
	if(id == "d") return KEY_D;
	if(id == "e") return KEY_E;
	if(id == "f") return KEY_F;
	if(id == "g") return KEY_G;
	if(id == "h") return KEY_H;
	if(id == "i") return KEY_I;
	if(id == "j") return KEY_J;
	if(id == "k") return KEY_K;
	if(id == "l") return KEY_L;
	if(id == "m") return KEY_M;
	if(id == "n") return KEY_N;
	if(id == "o") return KEY_O;
	if(id == "p") return KEY_P;
	if(id == "q") return KEY_Q;
	if(id == "r") return KEY_R;
	if(id == "s") return KEY_S;
	if(id == "t") return KEY_T;
	if(id == "u") return KEY_U;
	if(id == "v") return KEY_V;
	if(id == "w") return KEY_W;
	if(id == "x") return KEY_X;
	if(id == "y") return KEY_Y;
	if(id == "z") return KEY_Z;

	if(id == "ctrl")  return KEY_LEFT_CONTROL;
	if(id == "alt")   return KEY_LEFT_ALT;
	if(id == "shift") return KEY_LEFT_SHIFT;

	Logger::Warning(std::format("Could not parse key name \"{}\"", id));
	return KEY_NULL;
}
