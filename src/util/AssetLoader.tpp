#pragma once

#include <string>
#include <functional>
#include <filesystem>
#include <format>

#ifdef WINDOWS
#include <libloaderapi.h>
#endif

template<typename T>
T AssetLoader::TryLoadFile(std::string path, std::function<T (std::string)> onSuccess, std::function<T (std::string)> onFail) {
	
	// prepend the directory of the executable to the path
	#ifdef WINDOWS
	{
		LPSTR executablePathCStr = new CHAR[1024]; // it BETTER be shorter than this
		GetModuleFileNameA(NULL, executablePathCStr, 1024);

		std::string executableDir = std::string(executablePathCStr);
		delete[] executablePathCStr;
		
		// remove "TilesetEditor.exe" from the end
		for(int i = executableDir.size() - 1; i >= 0; i--) {
			if(executableDir[i] == '\\' || executableDir[i] == '/') {
				executableDir = executableDir.substr(0, i + 1);
				break;
			}
		}

		path = executableDir + path;
	}
	#else
	{
		Logger::Warning("Can't get executable directory on linux yet");
	}
	#endif

	bool fileExists = std::filesystem::exists(path);
	if(fileExists) {
		Logger::Message(std::format("Loaded file \"{}\" successfully!", path));
		return onSuccess(path);
	}
	else {
		Logger::Warning(std::format("Failed to find file \"{}\"", path));
		return onFail(path);
	}
}