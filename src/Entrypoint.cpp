
#include <iostream>

#include "../raylib/raygui.h"

#include "TilesetData.hpp"

#include "util/Input.hpp"
#include "util/AssetLoader.hpp"

#include "regions/GuiSizeInfo.hpp"
#include "regions/Background.hpp"
#include "regions/PaletteEditor.hpp"
#include "regions/ColorPicker.hpp"
#include "regions/TileSelector.hpp"
#include "regions/ToolBar.hpp"
#include "regions/TileEditor.hpp"

int main() {

	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	InitWindow(1280, 720, "Tileset Editor");
	SetWindowMinSize(848, 480);
	SetTargetFPS(60);
	SetTraceLogLevel(TraceLogLevel::LOG_WARNING);

	if(AssetLoader::TryMakeDir("", "temp")) {
		Logger::Message("Created temp directory");
	}
	else {
		Logger::Warning("Failed to create temp directory");
	}

	TilesetData::Initialize();
	Input::Initialize();

	PaletteEditor::Initialize();
	ColorPicker colorPicker;
	TileSelector tileSelector;
	ToolBar toolBar;
	TileEditor::Initialize();

	while(!WindowShouldClose()) {
		BeginDrawing();
		ClearBackground(BLACK);

		GuiSizeInfo info = GuiSizeInfo(GetScreenWidth(), GetScreenHeight());
		
		Background::DrawLeftRegion(info.LeftRegion());
		Background::DrawCenterRegion(info.CenterRegion());
		Background::DrawRightRegion(info.RightRegion());
		
		//DrawRectangleRec(info.PaletteEditor(), GREEN);
		//DrawRectangleRec(info.ColorPicker(), BLUE);
		//DrawRectangleRec(info.TileEditor(), RED);
		//DrawRectangleRec(info.TileSelector(), YELLOW);
		DrawRectangleRec(info.MenuBar(), WHITE);
		//DrawRectangleRec(info.ToolBar(), RED);

		PaletteEditor::Inst()->Update(info.PaletteEditor());
		colorPicker.Update(info.ColorPicker());
		tileSelector.Update(info.TileSelector());
		toolBar.Update(info.ToolBar());
		TileEditor::Inst()->Update(info.TileEditor());

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

		if(Input::KeybindIsPressed("Undo")) TilesetData::UndoLatestChange();
		if(Input::KeybindIsPressed("Redo")) TilesetData::RedoLatestChange();

		EndDrawing();
	}

	CloseWindow();
}