
#include <iostream>

#include "../raylib/raygui.h"

#include "GuiSizeInfo.hpp"

int main() {
	std::cout << "Gay\n";

	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	InitWindow(1280, 720, "Yippee");
	SetWindowMinSize(640, 360);
	SetTargetFPS(60);

	while(!WindowShouldClose()) {
		BeginDrawing();
		ClearBackground(BLACK);

		GuiSizeInfo info = GuiSizeInfo(GetScreenWidth(), GetScreenHeight());
		DrawRectangleRec(info.PaletteEditor(), GREEN);
		DrawRectangleRec(info.ColorPicker(), BLUE);
		DrawRectangleRec(info.TileEditor(), RED);
		DrawRectangleRec(info.TileSelector(), YELLOW);
		DrawRectangleRec(info.MenuBar(), WHITE);

		EndDrawing();
	}
}