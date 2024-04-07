#pragma once

#include <array>
#include <list>
#include <vector>
#include <cstdint>

#include "../raylib/raylib.h"

typedef uint8_t ColorIdx;

struct Tile {
	Tile();

	void SetPixel(int x, int y, ColorIdx paletteIdx);
	ColorIdx GetPixel(int x, int y);

	void CloseCurrentChangeFrame();
	void RevertChangesInFrame();

private:
	struct Change {
		int idxChanged;
		ColorIdx prevColor;

		bool operator== (const Change& rhs);
		bool operator!= (const Change& rhs);
	};

	std::array<ColorIdx, 16 * 16> colorData;

	std::vector<int> changeFrameBeginnings = {0};
	std::vector<Change> changeHistory;
};

// Singleton
class TilesetData {
public:
	static void Initialize();

	static void DeleteTile(int tileIdx);
	static Tile* GetTile(int tileIdx);

	static Color GetColor(int paletteIdx);
	static void SetColor(int paletteIdx, Color color);
	
	static Color GetActiveColor();
	static int GetActiveColorIdx();
	static void SetActiveColor(int colorIdx);

	static Tile* GetActiveTile();
	static void SetActiveTile(int tileIdx);

private:
	static TilesetData* Inst(); // checks for null before returning a pointer to the instance
	static TilesetData* inst;

	std::array<Color, 15> palette;
	std::list<Tile> tiles; // cache locality doesnt really matter for this, i'd rather be able to delete elements without invalidating pointers.

	static std::list<Tile>::iterator ItrFromTileIdx(int tileIdx);

	int activeTileIdx = 0;
	int activeColorIdx = 1;
};
