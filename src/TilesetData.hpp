#pragma once

#include <array>
#include <list>
#include <cstdint>

#include "../raylib/raylib.h"

struct Tile {
	Tile();

	std::array<uint8_t, 16 * 16> idxData;
	void SetPixel(int x, int y, uint8_t paletteIdx);
	uint8_t GetPixel(int x, int y);
};

// Singleton
class TilesetData {
public:
	static void Initialize();

	static void DeleteTile(int tileIdx);
	static Tile* GetTile(int tileIdx);

	static Color GetColor(int paletteIdx);
	static void SetColor(int paletteIdx, Color color);

private:
	static TilesetData* inst;

	std::array<Color, 15> palette;
	std::list<Tile> tiles; // cache locality doesnt really matter for this, i'd rather be able to delete elements without invalidating pointers.

	static std::list<Tile>::iterator ItrFromTileIdx(int tileIdx);
};
