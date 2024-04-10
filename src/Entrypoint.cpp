
#include <iostream>

#include "../raylib/raygui.h"

#include "TilesetData.hpp"
#include "GuiSizeInfo.hpp"

#include "PaletteEditor.hpp"
#include "ColorPicker.hpp"
#include "TileEditor.hpp"

int main() {
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	InitWindow(1280, 720, "Tileset Editor");
	SetWindowMinSize(640, 360);
	SetTargetFPS(60);

	TilesetData::Initialize();

	PaletteEditor paletteEditor;
	ColorPicker colorPicker;
	TileEditor tileEditor;

	while(!WindowShouldClose()) {
		BeginDrawing();
		ClearBackground(BLACK);

		GuiSizeInfo info = GuiSizeInfo(GetScreenWidth(), GetScreenHeight());
		DrawRectangleRec(info.PaletteEditor(), GREEN);
		DrawRectangleRec(info.ColorPicker(), BLUE);
		DrawRectangleRec(info.TileEditor(), RED);
		DrawRectangleRec(info.TileSelector(), YELLOW);
		DrawRectangleRec(info.MenuBar(), WHITE);

		paletteEditor.Update(info.PaletteEditor());
		colorPicker.Update(info.ColorPicker());
		// tileSelector
		tileEditor.Update(info.TileEditor());
		// toolBar

		/*
		Draw Order:
			Palette Editor (update active palette idx in internal state)
			Color Picker (set color of active palette idx)
			Tile Selector (update active tile in internal state)
			Tile Editor (paint tile using state of Palette Editor and Tile Selector)
			Menu Bar (perform random actions using the state of the active tile)

		Tile data is stored externally, is modified by Tile Editor,
		and a pointer into that data is given to the tile editor by the Tile Selector.
		
		*/

		EndDrawing();
	}

	CloseWindow();
}