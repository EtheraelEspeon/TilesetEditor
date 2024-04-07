#include "TileEditor.hpp"

#include "Logger.hpp"

void TileEditor::Update(Rectangle size) {
	// check hotkeys
	if(IsKeyDown(KEY_LEFT_CONTROL)) {
		if(IsKeyPressed(KEY_Z)) {
			TilesetData::GetActiveTile()->RevertChangesInFrame();
		}
	}

	// delete old changes on the rising edge of a new erase or draw action
	if(IsMouseButtonPressed(0) || IsMouseButtonPressed(1)) {
		TilesetData::GetActiveTile()->CloseCurrentChangeFrame();
	}

	int hStep = size.width / 16;
	int vStep = size.height / 16;
	int hStepHalf = hStep / 2;
	int vStepHalf = vStep / 2;
	
	// draw background checkerboard
	for(int y = 0; y < 32; y++) {
		for(int x = 0; x < 32; x++) {
			Rectangle bounds;
			bounds.x = size.x + hStepHalf * x;
			bounds.y = size.y + vStepHalf * y;
			bounds.width = hStepHalf;
			bounds.height = vStepHalf;

			int colorIdx = (x + y % 2) % 2;

			DrawRectangleRec(bounds, backgroundColors[colorIdx]);
		}
	}

	// draw sprite and paint
	for(int y = 0; y < 16; y++) {
		for(int x = 0; x < 16; x++) {
			Tile* activeTile = TilesetData::GetActiveTile();

			Rectangle bounds;
			bounds.x = size.x + hStep * x;
			bounds.y = size.y + vStep * y;
			bounds.width = hStep;
			bounds.height = vStep;

			Vector2 mousePos = GetMousePosition();
			bool mouseIsOnTile = mousePos.x >= bounds.x && mousePos.x <= bounds.x + bounds.width && mousePos.y >= bounds.y && mousePos.y <= bounds.y + bounds.height;
			bool leftClick = IsMouseButtonDown(0);
			bool rightClick = IsMouseButtonDown(1);

			if(mouseIsOnTile && leftClick) activeTile->SetPixel(x, y, TilesetData::GetActiveColorIdx());
			if(mouseIsOnTile && rightClick) activeTile->SetPixel(x, y, 0); 

			if(mouseIsOnTile) {
				// make the color preview invisible when erasing
				if(!rightClick) DrawRectangleRec(bounds, TilesetData::GetActiveColor());
			}
			else DrawRectangleRec(bounds, TilesetData::GetColor(activeTile->GetPixel(x, y)));
		}
	}

}
