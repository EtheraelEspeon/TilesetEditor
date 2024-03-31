
#include "GuiSizeInfo.hpp"

GuiSizeInfo::GuiSizeInfo(int windowWidth, int windowHeight) {
	
	/* --- set Palette Editor size --- */
	paletteEditor.x = 0;
	paletteEditor.y = 0;
	paletteEditor.height = windowHeight * PercentPaletteEditorHeight / 5 * 5; // ensure an integer multiple of 5, as the palette is 5 tiles tall
	paletteEditor.width = paletteEditor.height / 5 * 3;

	/* --- set Color Picker size --- */
	colorPicker.x = 0;
	colorPicker.y = paletteEditor.height + Padding;
	colorPicker.height = windowHeight - colorPicker.y;
	colorPicker.width = paletteEditor.width;

	/* --- set Tile Selector size --- */
	float maxPixelsPerTile = (windowHeight - (TargetNumberOfDisplayedTiles - 1) * TileSelectorSpacing) / (float)TargetNumberOfDisplayedTiles; // exact amount of pixels
	int pixelsPerTile = (int)maxPixelsPerTile / 16 * 16; // round down to nearest multiple of 16 pixels;
	
	tileSelector.width = pixelsPerTile + 2 * TileSelectorSpacing;
	tileSelector.height = windowHeight;
	tileSelector.x = windowWidth - tileSelector.width;
	tileSelector.y = 0;

	/* --- set Tile Editor size --- */
	int allocatedHeightTE = windowHeight - MenuBarHeight;
	int allocatedWidthTE = tileSelector.x - paletteEditor.width - 2 * Padding; // wrong
	int maxMultiplesOf16 = std::min(allocatedWidthTE / 16, allocatedHeightTE / 16);
	
	tileEditor.width = maxMultiplesOf16 * 16;
	tileEditor.height = tileEditor.width;
	tileEditor.y = MenuBarHeight + (allocatedHeightTE - tileEditor.height) / 2;
	tileEditor.x = paletteEditor.width + Padding + (allocatedWidthTE - tileEditor.width) / 2;

	/* --- set Menu Bar size --- */
	menuBar.width = tileSelector.x - paletteEditor.width;
	menuBar.height = MenuBarHeight;
	menuBar.x = paletteEditor.width;
	menuBar.y = 0;
}

Rectangle GuiSizeInfo::TileEditor()    const { return tileEditor; }
Rectangle GuiSizeInfo::PaletteEditor() const { return paletteEditor; }
Rectangle GuiSizeInfo::ColorPicker()   const { return colorPicker; }
Rectangle GuiSizeInfo::TileSelector()  const { return tileSelector; }
Rectangle GuiSizeInfo::MenuBar()       const { return menuBar; }