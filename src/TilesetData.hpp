#pragma once

#include <array>
#include <cstdint>

#include "../raylib/raylib.h"

struct Tile {
	std::array<uint8_t, 16 * 16> idxData;
	void SetPixel(int x, int y, uint8_t paletteIdx);
	uint8_t GetPixel(int x, int y);
};

namespace data {
	extern std::array<Color, 15> palette;
}