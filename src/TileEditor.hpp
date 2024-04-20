#pragma once

#include <cstdint>
#include <set>

#include "../raylib/raygui.h"

#include "TilesetData.hpp"

class TileEditor {
public:
	const std::array<Color, 2> backgroundColors = {ColorFromHSV(0, 0, 0.4), ColorFromHSV(0, 0, 0.6)};

	TileEditor();

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
private:

	bool pressBeganOutsideEditorRegion = false;

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
	static TilePos WindowPosToTilePos(Vector2 windowPos, Rectangle tileRegionSize);
	static bool PointInRec(Vector2 point, Rectangle rec);
	static Rectangle GetPixelBounds(TilePos tilePosition, Rectangle tileRegionSize);

	// in tile coordinates
	static std::vector<TilePos> LineBetween(TilePos to, TilePos from);

	struct Tool {
		/// @brief Paints onto the active tile, draws to the screen, and polls user input.
		/// @param activeTile The active tile
		/// @param reservedPixels An out parameter containing tile pixels this tool painted over (like cursor previews)
		/// @return a vector of tiles to paint with the active color
		virtual void Paint(Tile* activeTile, std::set<TilePos>* reservedPixels, Rectangle tileRegion) = 0;
	};

	static Tool* tool; // Should never be null. Is initialized on object creation
	static ToolType currentTool; // as above

	struct Brush : public Tool {
		void Paint(Tile* activeTile, std::set<TilePos>* reservedPixels, Rectangle tileRegion) override;
	private:
		bool painting = false;
		std::vector<ChangeQueue::PaintData> paintLocations;
		std::set<TilePos> tilePosToBePainted;
	};
	struct Line : public Tool {
		void Paint(Tile* activeTile, std::set<TilePos>* reservedPixels, Rectangle tileRegion) override;
	private:
		enum {
			NoPoints,
			OnePoint
		} state = NoPoints;

		TilePos startPos = TilePos(0, 0);
	};
	struct Fill : public Tool {
		void Paint(Tile* activeTile, std::set<TilePos>* reservedPixels, Rectangle tileRegion) override;
	};
	struct Eyedropper : public Tool {
		void Paint(Tile* activeTile, std::set<TilePos>* reservedPixels, Rectangle tileRegion) override;
	};
};