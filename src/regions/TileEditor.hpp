#pragma once

#include <cstdint>
#include <set>

#include "../../raylib/raygui.h"

#include "../TilesetData.hpp"

class TileEditor {
public:
	static constexpr std::array<Color, 2> BackgroundColors = {(Color){102, 102, 102, 255}, (Color){153, 153, 153, 255}};

	static void Initialize();

	void Update(Rectangle size);

	enum class ToolType {
		Null,
		Brush,
		Line,
		Fill,
		Eyedropper
	};
	static void SetTool(ToolType tool);
	static ToolType CurrentTool();

	static TileEditor* Inst();
private:

	/* ---- Utils ---- */

	struct TilePos {
		TilePos(Vector2 v);
		TilePos(int x, int y);
		TilePos(int rawIdx);

		int x, y;

		bool operator==(const TilePos& rhs) const;
		bool operator!=(const TilePos& rhs) const;
		bool operator>(const TilePos& rhs) const;
		bool operator<(const TilePos& rhs) const;

		TilePos operator+(const TilePos& rhs) const;
	};

	static int sign(int x);
	static TilePos WindowPosToTilePos(Vector2 windowPos, Rectangle editorRegion);
	static Rectangle GetPixelBounds(TilePos tilePosition, Rectangle editorRegion);

	/* ---- Draw Shapes ----*/

	static std::vector<TilePos> LineBetween(TilePos to, TilePos from);
	static std::vector<TilePos> CircleAround(TilePos center, float radius);

	/* ---- Tools ---- */

	struct Tool {
		/// @brief Paints onto the active tile, draws to the screen, and polls user input.
		/// @param activeTile The active tile
		/// @param reservedPixels An out parameter containing tile pixels this tool painted over (like cursor previews)
		/// @return a vector of tiles to paint with the active color
		virtual void Paint(Tile* activeTile, std::set<TilePos>* reservedPixels, Rectangle editorRegion) = 0;
	};

	struct Brush : public Tool {
		void Paint(Tile* activeTile, std::set<TilePos>* reservedPixels, Rectangle editorRegion) override;
	private:
		bool painting = false;
		std::vector<TilesetData::PaintData> paintLocations;
		std::set<uint8_t> tileIndicesToBePainted;
		float radius = 0;
	};
	struct Line : public Tool {
		void Paint(Tile* activeTile, std::set<TilePos>* reservedPixels, Rectangle editorRegion) override;
	private:
		enum {
			NoPoints,
			OnePoint
		} state = NoPoints;

		TilePos startPos = TilePos(0, 0);
	};
	struct Fill : public Tool {
		void Paint(Tile* activeTile, std::set<TilePos>* reservedPixels, Rectangle editorRegion) override;
	};
	struct Eyedropper : public Tool {
		void Paint(Tile* activeTile, std::set<TilePos>* reservedPixels, Rectangle editorRegion) override;
	};

	/* ---- State ---- */

	static TileEditor* instance;

	bool pressBeganOutsideEditorRegion = false;

	Tool* tool = nullptr; // Should never be null. Is initialized on object creation
	ToolType currentTool = ToolType::Null; // as above
};