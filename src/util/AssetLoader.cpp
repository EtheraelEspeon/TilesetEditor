#include "AssetLoader.hpp"

#include <filesystem>
#include <fstream>
#include <vector>
#include <format>

#include "../../raylib/raylib.h"

bool AssetLoader::TryMakeDir(std::string path, std::string dirName) {
	bool createdDir = std::filesystem::create_directories(path + "/" + dirName);
	bool dirExists = std::filesystem::exists(path + "/" + dirName);

	return (!createdDir && dirExists) || (createdDir && dirExists); // catches the case where we didn't create the dir because it already existed
}

Texture2D AssetLoader::LoadRaylibTexture(std::string path) {
	return TryLoadFile<Texture2D>(
		path,
		[](std::string path) { return LoadTexture(path.c_str()); },
		[](std::string path) {
			auto img = GenImageColor(16, 16, {255, 0, 255, 255});
			auto tex = LoadTextureFromImage(img);
			UnloadImage(img);
			return tex;
		}
	);
}
std::vector<std::string> AssetLoader::LoadTextByLine(std::string path) {
	return TryLoadFile<std::vector<std::string>>(
		path,
		[](std::string path) {
			std::ifstream reader;
			reader.open(path);

			std::string temp;
			std::vector<std::string> out;
			while(reader.good() && !reader.eof()) {
				std::getline(reader, temp);
				out.push_back(temp);
			}

			reader.close();
			return out;
		},
		[](std::string path) {
			return std::vector<std::string>{};
		}
	);
}
