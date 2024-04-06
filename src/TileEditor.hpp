#pragma once

#include <cstdint>

#include "../raylib/raygui.h"

#include "TilesetData.hpp"

class TileEditor {
public:
	const std::array<Color, 2> backgroundColors = {ColorFromHSV(0, 0, 0.4), ColorFromHSV(0, 0, 0.6)};

	void Update(Rectangle size);
};