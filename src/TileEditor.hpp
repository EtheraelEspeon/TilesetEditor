#pragma once

#include <cstdint>

#include "../raylib/raygui.h"

#include "TilesetData.hpp"

class TileEditor {
public:
	const std::array<Color, 2> backgroundColors = {ColorFromHSV(0, 0, 0.4), ColorFromHSV(0, 0, 0.6)};

	void Update(Rectangle size);

private:
	int sign(int x);
	Vector2 WindowPosToTilePos(Vector2 windowPos, Rectangle tileRegionSize);
	bool PointInRec(Vector2 point, Rectangle rec);

	// in tile coordinates
	std::vector<Vector2> LineBetween(Vector2 to, Vector2 from);
};