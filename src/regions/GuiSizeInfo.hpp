
#pragma once

#include <cmath>

#include "../../raylib/raylib.h"

struct GuiSizeInfo {

	constexpr static float PercentLeftRegionWidth = 6.0/32.0; // Percent of the screen's width occupied by the left region
	constexpr static float PercentRightRegionWidth = 8.0/32.0; // as above for the right region
	constexpr static int TargetNumberOfDisplayedTiles = 8;
	constexpr static int ToolButtonSize = 48; 
	constexpr static int MenuBarHeight = 32;
	constexpr static int TileSelectorSpacing = 8;
	constexpr static int LeftRegionPadding = 16;
	constexpr static int CenterRegionPadding = 4;
	constexpr static int RightRegionPadding = 16;

	GuiSizeInfo(int windowWidth, int windowHeight);

	Rectangle LeftRegion() const;
	Rectangle CenterRegion() const;
	Rectangle RightRegion() const;

	Rectangle TileEditor() const;
	Rectangle PaletteEditor() const;
	Rectangle ToolBar() const;
	Rectangle ColorPicker() const;
	Rectangle TileSelector() const;
	Rectangle MenuBar() const;

private:
	Rectangle leftRegion, centerRegion, rightRegion;
	Rectangle tileEditor, paletteEditor, toolBar, colorPicker, tileSelector, menuBar;
};