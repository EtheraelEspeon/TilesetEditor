#pragma once

#include <array>
#include <list>
#include <vector>
#include <cstdint>
#include <functional>
#include <set>

#include "../raylib/raylib.h"

typedef uint8_t ColorIdx;

class TilesetData;
struct Tile {
	
	ColorIdx GetPixel(int x, int y) const;

private:
	Tile();

	void SetPixel(int x, int y, ColorIdx color);
	void SetPixel(uint8_t rawIdx, ColorIdx color);

	std::array<ColorIdx, 16 * 16> colorData;

	friend class TilesetData;
};

// Singleton
class TilesetData {
public:
	/// @brief Initialize the data singleton
	static void Initialize();

	/* ---- Tile Management ---- */

	/// @brief Marks the tile at tileIdx as deleted
	static void DeleteTile(int tileIdx);
	/// @return a pointer to the tile at tileIdx 
	static Tile* GetTile(int tileIdx);
	/// @return the number of (non-deleted) tiles
	static int NumTiles();
	/// @brief Pushes a tile to the end of the tile list
	static void AddTile();

	/// @return if a tile at memory location tileLocation is marked as deleted, to check for dangling pointers
	static bool TileIsDeleted(Tile* tileLocation);

	/* ---- Palette Management ---- */

	/// @return the color in the palette at idx colorIdx
	static Color GetColor(ColorIdx colorIdx);
	/// @brief sets the color at idx colorIdx to color 
	static void SetColor(ColorIdx colorIdx, Color color);

	/* ---- Undo/Redo Support ---- */

	/// @brief The color to paint a single pixel
	struct PaintData {
		PaintData(int x, int y, ColorIdx color);
		uint8_t pixelIdx;
		ColorIdx color;
	};

	/// @brief Transforms the data of the tileset
	typedef std::function<void(Tile*)> Action;

	/// @note stomps over the color member of the supplied PaintData with the old color idx
	/// @return a closure using the given paintData to undo the painting done by the redo action
	static Action UnpaintPixelsAction(std::vector<PaintData> paintData, Tile* activeTile);
	/// @return a closure using the given paintData to paint the specified pixels
	static Action PaintPixelsAction(std::vector<PaintData> paintData);

	static void ApplyChange(Action redo, Action undo, Tile* targetTile);

	static void UndoLatestChange();
	static void RedoLatestChange();

private:
	/// @brief Checks for null before returning the pointer to the current instance 
	static TilesetData* Inst();
	static TilesetData* inst;
	~TilesetData();

	/* ---- Utility ---- */
	/// @return an iterator pointing to the tile at tileIdx 
	static std::list<Tile*>::iterator ItrFromTileIdx(int tileIdx);


	/* ---- State ---- */
	std::array<Color, 15> palette;
	std::list<Tile*> tiles; // cache locality doesnt really matter for this, i'd rather be able to rearrange elements without invalidating pointers.
	std::set<Tile*> deletedTiles = {};

	/* ---- Undo/Redo Support ---- */
	struct State {
		~State();

		struct Change {
			Change() = delete;
			Change(Action traverse, Tile* targetTile);
			
			void Apply();

			/// @brief A closure that advances the Tileset's state
			Action traverse;
			/// @brief the tile targeted by this action
			Tile* targetTile;
		};

		Change* forward = nullptr;
		Change* reverse = nullptr;
		State* next = nullptr;
		State* prev = nullptr;
	};

	State* rootState = new State();
	State* currentState = rootState;
};
