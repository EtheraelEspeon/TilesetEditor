
#include <iostream>

#include "../raylib/raygui.h"

#include "GuiSizeInfo.hpp"

#include "PaletteEditor.hpp"

int main() {
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	InitWindow(1280, 720, "Tileset Editor");
	SetWindowMinSize(640, 360);
	SetTargetFPS(60);

	GuiSetStyle(GuiControl::BUTTON, GuiControlProperty::BORDER_WIDTH, 6);

	GuiSetStyle(GuiControl::BUTTON, GuiControlProperty::BASE_COLOR_NORMAL, 0x00000000); // make button bodies transparent
	GuiSetStyle(GuiControl::BUTTON, GuiControlProperty::BORDER_COLOR_NORMAL, 0x00000040); // make borders partially transparent when unselected
	
	GuiSetStyle(GuiControl::BUTTON, GuiControlProperty::BASE_COLOR_FOCUSED, 0x00000040); // make the button body darken when focused
	GuiSetStyle(GuiControl::BUTTON, GuiControlProperty::BORDER_COLOR_FOCUSED, 0x00000060); // make the button border darken when focused

	GuiSetStyle(GuiControl::BUTTON, GuiControlProperty::BASE_COLOR_PRESSED, 0x00000080); // make buttons get darker when pressed
	GuiSetStyle(GuiControl::BUTTON, GuiControlProperty::BORDER_COLOR_PRESSED, 0x000000AA); // make the border darker when pressed

	GuiSetStyle(GuiControl::BUTTON, GuiControlProperty::TEXT_COLOR_NORMAL, 0x000000FF);
	GuiSetStyle(GuiControl::BUTTON, GuiControlProperty::TEXT_COLOR_FOCUSED, 0x000000FF);
	GuiSetStyle(GuiControl::BUTTON, GuiControlProperty::TEXT_COLOR_PRESSED, 0x000000FF);

	GuiSetStyle(GuiControl::DEFAULT, GuiDefaultProperty::TEXT_SIZE, 30);

	PaletteEditor paletteEditor;

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
}