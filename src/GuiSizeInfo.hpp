
#pragma once

#include <cmath>

#include "../raylib/raylib.h"

struct GuiSizeInfo {

	const float MenuBarHeight = 32;
	const float PercentPaletteEditorHeight = 2.0/3.0;
	const float Padding = 4;
	const int TargetNumberOfDisplayedTiles = 8;
	const int TileSelectorSpacing = 8;

	GuiSizeInfo(int windowWidth, int windowHeight);

	Rectangle TileEditor() const;
	Rectangle PaletteEditor() const;
	Rectangle ColorPicker() const;
	Rectangle TileSelector() const;
	Rectangle MenuBar() const;

private:
	Rectangle tileEditor, paletteEditor, colorPicker, tileSelector, menuBar;
};