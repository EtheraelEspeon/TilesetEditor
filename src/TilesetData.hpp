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
	/// @brief Initialize the data singleton
	static void Initialize();

	/// @brief Marks the tile at tileIdx as deleted
	static void DeleteTile(int tileIdx);
	/// @return a pointer to the tile at tileIdx 
	static Tile* GetTile(int tileIdx);
	/// @return the number of (non-deleted) tiles
	static int NumTiles();
	/// @brief Pushes a tile to the end of the tile list
	static void AddTile();

	/// @return the color in the palette at idx colorIdx
	static Color GetColor(ColorIdx colorIdx);
	/// @brief sets the color at idx colorIdx to color 
	static void SetColor(ColorIdx colorIdx, Color color);

	/// @return if a tile at memory location tileLocation is marked as deleted, to check for dangling pointers
	static bool TileIsDeleted(void* tileLocation);

private:
	/// @brief Checks for null before returning the pointer to the current instance 
	static TilesetData* Inst();
	static TilesetData* inst;

	std::array<Color, 15> palette;
	std::list<Tile*> tiles; // cache locality doesnt really matter for this, i'd rather be able to rearrange elements without invalidating pointers.
	std::set<void*> deletedTileLocations = {};

	/// @return an iterator pointing to the tile at tileIdx 
	static std::list<Tile*>::iterator ItrFromTileIdx(int tileIdx);

	int activeTileIdx = 0;  // >= 0
	int activeColorIdx = 1; // [1, 15]
};
