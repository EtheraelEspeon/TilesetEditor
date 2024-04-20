#include "AssetLoader.hpp"

#include <filesystem>
#include <fstream>

Texture2D AssetLoader::LoadTexture(std::string path) {
	return TryLoadFile<Texture2D>(
		path,
		[](std::string path) { return LoadTexture(path); },
		[](std::string path) { return LoadTextureFromImage(GenImageColor(16, 16, {255, 0, 255, 255})); }
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
