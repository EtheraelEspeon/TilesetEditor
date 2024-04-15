
#include "GuiSizeInfo.hpp"

GuiSizeInfo::GuiSizeInfo(int windowWidth, int windowHeight) {
	/* --- set left region size --- */
	leftRegion.x = 0;
	leftRegion.y = 0;
	leftRegion.width = windowWidth * PercentLeftRegionWidth;
	leftRegion.width = (leftRegion.width - 2 * LeftRegionPadding) / 3 * 3 + 2 * LeftRegionPadding; // ensure an integer multiple of 3 for the palette editor later
	leftRegion.height = windowHeight;

	/* --- set right region size --- */
	rightRegion.y = 0;
	rightRegion.width = windowWidth * PercentRightRegionWidth; // TODO: Will need to round to an integer multiple of something here later, once tile selector is implemented
	rightRegion.height = windowHeight;
	rightRegion.x = windowWidth - rightRegion.width;

	/* --- set center region size --- */
	centerRegion.x = leftRegion.width;
	centerRegion.y = 0;
	centerRegion.width = windowWidth - leftRegion.width - rightRegion.width;
	centerRegion.height = windowHeight;

	/* --- set Palette Editor size --- */
	paletteEditor.x = LeftRegionPadding;
	paletteEditor.y = LeftRegionPadding;
	paletteEditor.width = leftRegion.width - 2 * LeftRegionPadding;
	paletteEditor.height = paletteEditor.width / 3 * 5;

	/* --- set Tool Bar size --- */
	toolBar.x = LeftRegionPadding;
	toolBar.y = paletteEditor.y + paletteEditor.height + LeftRegionPadding;
	toolBar.height = ToolButtonSize;
	toolBar.width = leftRegion.width - 2 * LeftRegionPadding;

	/* --- set Color Picker size --- */
	colorPicker.x = LeftRegionPadding;
	colorPicker.y = toolBar.y + toolBar.height + LeftRegionPadding;
	colorPicker.height = windowHeight - colorPicker.y - LeftRegionPadding;
	colorPicker.width = paletteEditor.width;

	/* --- set Tile Selector size --- */
	tileSelector = rightRegion;

	/* --- set Menu Bar size --- */
	menuBar.width = centerRegion.width;
	menuBar.height = MenuBarHeight;
	menuBar.x = leftRegion.width;
	menuBar.y = 0;

	/* --- set Tile Editor size --- */
	int allocatedHeightTE = centerRegion.height - menuBar.height;
	int allocatedWidthTE = centerRegion.width - 2 * CenterRegionPadding;
	int maxMultiplesOf32 = std::min(allocatedWidthTE / 32, allocatedHeightTE / 32); // 32 for background checkerboard
	
	tileEditor.width = maxMultiplesOf32 * 32;
	tileEditor.height = tileEditor.width;
	tileEditor.y = MenuBarHeight + (allocatedHeightTE - tileEditor.height) / 2;
	tileEditor.x = leftRegion.width + CenterRegionPadding + (allocatedWidthTE - tileEditor.width) / 2;
}

Rectangle GuiSizeInfo::LeftRegion()   const { return leftRegion; }
Rectangle GuiSizeInfo::CenterRegion() const { return centerRegion; }
Rectangle GuiSizeInfo::RightRegion()  const { return rightRegion; }

Rectangle GuiSizeInfo::TileEditor()    const { return tileEditor; }
Rectangle GuiSizeInfo::PaletteEditor() const { return paletteEditor; }
Rectangle GuiSizeInfo::ToolBar()       const { return toolBar; }
Rectangle GuiSizeInfo::ColorPicker()   const { return colorPicker; }
Rectangle GuiSizeInfo::TileSelector()  const { return tileSelector; }
Rectangle GuiSizeInfo::MenuBar()       const { return menuBar; }