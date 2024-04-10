
#pragma once

#include <cmath>

#include "../raylib/raylib.h"

struct GuiSizeInfo {

	const float MenuBarHeight = 32;
	const float PercentLeftRegionWidth = 12.0/32.0; // Percent of the screen's width occupied by the left region
	const float PercentRightRegionWidth = 8.0/32.0; // as above for the right region
	const float Padding = 4;
	const int TargetNumberOfDisplayedTiles = 8;
	const int TileSelectorSpacing = 8;

	GuiSizeInfo(int windowWidth, int windowHeight);

	Rectangle LeftRegion() const;
	Rectangle CenterRegion() const;
	Rectangle RightRegion() const;

	Rectangle TileEditor() const;
	Rectangle PaletteEditor() const;
	Rectangle ColorPicker() const;
	Rectangle TileSelector() const;
	Rectangle MenuBar() const;

private:
	Rectangle leftRegion, centerRegion, rightRegion;
	Rectangle tileEditor, paletteEditor, colorPicker, tileSelector, menuBar;
};