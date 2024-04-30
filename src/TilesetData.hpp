#pragma once

#include <array>
#include <list>
#include <vector>
#include <cstdint>
#include <functional>
#include <set>

#include "../raylib/raylib.h"

typedef uint8_t ColorIdx;

class ChangeQueue;
struct Tile {
	Tile();

	ColorIdx GetPixel(int x, int y);

private:

	void SetPixel(int x, int y, ColorIdx color);
	void SetPixel(uint8_t rawIdx, ColorIdx color);

	std::array<ColorIdx, 16 * 16> colorData;

	friend class ChangeQueue;
};

class ChangeQueue {
public:
	/// @brief The color to paint a single pixel
	struct PaintData {
		PaintData(int x, int y, ColorIdx color);
		uint8_t pixelIdx;
		ColorIdx color;
	};

	typedef std::function<void(Tile*)> RedoAction;
	typedef std::function<void(Tile*)> UndoAction;

	/// @note stomps over the color member of the supplied PaintData with the old color idx
	/// @return a closure using the given paintData to undo the painting done by the redo action
	static UndoAction UnpaintPixelsAction(std::vector<PaintData> paintData, Tile* activeTile);
	/// @return a closure using the given paintData to paint the specified pixels
	static RedoAction PaintPixelsAction(std::vector<PaintData> paintData);

	static void ApplyChange(RedoAction redo, UndoAction undo, Tile* targetTile);

	static void UndoLatestChange();
	static void RedoLatestChange();

private:
	struct Change {

		~Change();

		/// @brief Performs this change.
		/// @note Applied when this becomes the latest change.
		RedoAction redo;
		/// @brief Resets the tileset's state that of the previous change
		/// @note Applied before switching to the previous change
		UndoAction undo;
		/// @brief The tile that was active when this change was originally applied
		Tile* targetTile;

		Change* next = nullptr;
		Change* prev = nullptr;
	};

	static inline Change* latestChange = nullptr; // TODO: How do i track the memory allocated to the closures in this tree?
	static inline bool firstChangeIsUndone = false;
};

// Singleton
class TilesetData {
public:
	static void Initialize();

	static void DeleteTile(int tileIdx);
	static Tile* GetTile(int tileIdx);
	static int NumTiles();
	static void AddTile();

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
