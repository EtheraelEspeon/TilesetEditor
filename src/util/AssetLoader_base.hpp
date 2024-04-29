#pragma once

#include <functional>
#include <vector>

#include "../../raylib/raylib.h"
#include "Logger.hpp"

namespace AssetLoader {
	/// @brief Checks if a file exists, logs the result, then returns the contents if possible
	/// @tparam T The type to return
	/// @param path The path to the target file
	/// @param onSuccess A lambda responsible for loading the file from the path after it's known to exist
	/// @param onFail A lambda responsible for giving some kind of null value if the file doesn't exist
	template<typename T>
	T TryLoadFile(std::string path, std::function<T (std::string)> onSuccess, std::function<T (std::string)> onFail);
	
	bool TryMakeDir(std::string path, std::string dirName);
}

#include "AssetLoader.tpp"