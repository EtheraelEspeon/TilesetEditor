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

	Vector2 mousePos = GetMousePosition();
	if(!PointInRec(mousePos, size)) return;
	/* ----- The following should only happen when the user is clicking on the tile editor ----- */

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

			bool mouseIsOnTile = PointInRec(mousePos, bounds);
			bool leftClick = IsMouseButtonDown(0) && !IsMouseButtonPressed(0);
			bool rightClick = IsMouseButtonDown(1) && !IsMouseButtonPressed(1);

			// paint
			if(leftClick || rightClick) {
				ColorIdx drawColor;
				if(leftClick)  drawColor = TilesetData::GetActiveColorIdx();
				if(rightClick) drawColor = 0;

				// draw line
				Vector2 mouseDeltaBetweenFrames = GetMouseDelta();
				Vector2 mouseTilePos = WindowPosToTilePos(mousePos, size);
				Vector2 prevMouseTilePos = WindowPosToTilePos({mousePos.x - mouseDeltaBetweenFrames.x, mousePos.y - mouseDeltaBetweenFrames.y}, size);

				auto linePositions = LineBetween(mouseTilePos, prevMouseTilePos);

				for(auto p : linePositions) {
					TilesetData::GetActiveTile()->SetPixel(p.x, p.y, drawColor);
				}
			}

			if(mouseIsOnTile) {
				// make the color preview invisible when erasing
				if(!rightClick) DrawRectangleRec(bounds, TilesetData::GetActiveColor());
			}
			else DrawRectangleRec(bounds, TilesetData::GetColor(activeTile->GetPixel(x, y)));
		}
	}

}

int TileEditor::sign(int x) {
	if(x < 0) return -1;
	if(x > 0) return 1;
	if(x == 0) return 0;
	return 0;
}
Vector2 TileEditor::WindowPosToTilePos(Vector2 windowPos, Rectangle tileRegionSize) {
	Vector2 tilePos = windowPos; // tile that the mouse is over
	tilePos.x -= tileRegionSize.x;
	tilePos.y -= tileRegionSize.y;
	tilePos.x /= (tileRegionSize.width / 16);
	tilePos.y /= (tileRegionSize.height / 16);

	if      (tilePos.x < 0)  tilePos.x = 0;
	else if (tilePos.x > 15) tilePos.x = 15;
	if      (tilePos.y < 0)  tilePos.y = 0;
	else if (tilePos.y > 15) tilePos.y = 15;

	return tilePos;
}
bool TileEditor::PointInRec(Vector2 point, Rectangle rec) {
	return point.x > rec.x && point.y > rec.y && point.x < (rec.x + rec.width) && point.y < (rec.y + rec.height);
}

std::vector<Vector2> TileEditor::LineBetween(Vector2 to, Vector2 from) {
	int dx, dy;
	dx = to.x - from.x;
	dy = to.y - from.y;
	
	std::vector<Vector2> linePositions = {from};
	
	while(dx != 0 || dy != 0) {
		auto prevLinePos = linePositions.back();
		if(std::abs(dx) == std::abs(dy)) {
			linePositions.push_back({prevLinePos.x + sign(dx), prevLinePos.y + sign(dy)});
			dx -= sign(dx);
			dy -= sign(dy);
		}
		else if(std::abs(dx) > std::abs(dy)) {
			linePositions.push_back({prevLinePos.x + sign(dx), prevLinePos.y});
			dx -= sign(dx);
		}
		else {
			linePositions.push_back({prevLinePos.x, prevLinePos.y + sign(dy)});
			dy -= sign(dy);
		}
	}

	return linePositions;
}