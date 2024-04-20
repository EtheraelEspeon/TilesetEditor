#include "AssetLoader.hpp"

template<typename T>
T TryLoadFile(std::string path, std::function<T (std::string)> onSuccess, std::function<T (std::string)> onFail) {
	bool fileExists = std::filesystem::exists(path);
	if(fileExists) {
		Logger::Message("Loaded file \"" + path + "\" successfully!");
		return onSuccess(path);
	}
	else {
		Logger::Warning("Failed to find file \"" + path + "\"");
		return onFail(path);
	}
}