#pragma once

#include <string>
#include <vector>

#include "../../raylib/raylib.h"
#include "AssetLoader_base.hpp"

/*
*  To separate codebase-specific declarations from the AssetLoader boilerplate
*/

namespace AssetLoader {
	Texture2D LoadRaylibTexture(std::string path);
	std::vector<std::string> LoadTextByLine(std::string path);
};