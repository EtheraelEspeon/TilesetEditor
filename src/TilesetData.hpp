#pragma once

#include <array>
#include <list>
#include <vector>
#include <cstdint>
#include <functional>
#include <set>

#include "../raylib/raylib.h"

typedef uint8_t ColorIdx;

struct Tile {
	Tile();

	void SetPixel(int x, int y, ColorIdx color);
	void SetPixel(uint8_t rawIdx, ColorIdx color);
	ColorIdx GetPixel(int x, int y);

private:
	std::array<ColorIdx, 16 * 16> colorData;
};

class UndoQueue {
public:
	typedef std::function<void(Tile*)> Resetter; // closure that modifies the global state (TilesetData) to undo an action

	static void PushResetter(Resetter undo, Tile* currentActiveTile);
	static void UndoLatestChange();

private:
	struct UndoAction {
		Resetter resetter;
		Tile* targetTile; // stores this separately so it can check if the tile has been deleted/moved between the construction of the closure and when the user undoes an action
	};

	static inline std::list<UndoAction> changeHistory = {}; // TODO: How do i track the memory allocated to the closures in here?
};

// Singleton
class TilesetData {
public:
	static void Initialize();

	static void DeleteTile(int tileIdx);
	static Tile* GetTile(int tileIdx);

	static Color GetColor(ColorIdx colorIdx);
	static void SetColor(ColorIdx colorIdx, Color color);
	
	static Color GetActiveColor();
	static int GetActiveColorIdx();
	static void SetActiveColor(ColorIdx colorIdx);

	static Tile* GetActiveTile();
	static void SetActiveTile(int tileIdx);

	static bool TileIsDeleted(void* tileLocation);

private:
	static TilesetData* Inst(); // checks for null before returning a pointer to the instance
	static TilesetData* inst;

	std::array<Color, 15> palette;
	std::list<Tile> tiles; // cache locality doesnt really matter for this, i'd rather be able to delete elements without invalidating pointers.
	std::set<void*> deletedTileLocations = {};

	static std::list<Tile>::iterator ItrFromTileIdx(int tileIdx);

	int activeTileIdx = 0;
	int activeColorIdx = 1;
};
