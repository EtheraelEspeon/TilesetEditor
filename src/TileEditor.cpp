#include "TileEditor.hpp"

#include "Logger.hpp"

TileEditor::TileEditor() {
	SetTool(ToolType::Brush);
}

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

	Tile* activeTile = TilesetData::GetActiveTile();

	// paint
	std::set<TilePos> reservedPixels = {}; // pixels that wont be drawn when the sprite is drawn
	if(PointInRec(GetMousePosition(), size)) {
		tool->Paint(activeTile, &reservedPixels, size);
	}

	// draw sprite
	for(int y = 0; y < 16; y++) {
		for(int x = 0; x < 16; x++) {
			
			if(reservedPixels.contains({x, y})) continue; 
			
			Color color = TilesetData::GetColor(activeTile->GetPixel(x, y));

			Rectangle bounds;
			bounds.x = size.x + hStep * x;
			bounds.y = size.y + vStep * y;
			bounds.width = hStep;
			bounds.height = vStep;

			DrawRectangleRec(bounds, color);
		}
	}

}

int TileEditor::sign(int x) {
	if(x < 0) return -1;
	if(x > 0) return 1;
	if(x == 0) return 0;
	return 0;
}
TileEditor::TilePos TileEditor::WindowPosToTilePos(Vector2 windowPos, Rectangle tileRegionSize) {
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
Rectangle TileEditor::GetPixelBounds(TilePos tilePos, Rectangle tileRegionSize) {
	
	// floor values
	tilePos.x = (int)tilePos.x;
	tilePos.y = (int)tilePos.y;

	int step = tileRegionSize.width / 16;
	
	Rectangle bounds;
	bounds.x = tilePos.x * step + tileRegionSize.x;
	bounds.y = tilePos.y * step + tileRegionSize.y;
	bounds.width = step;
	bounds.height = step;

	return bounds;
}

std::vector<TileEditor::TilePos> TileEditor::LineBetween(TilePos to, TilePos from) {
	int dx, dy;
	dx = to.x - from.x;
	dy = to.y - from.y;

	std::vector<TilePos> linePositions = {from};
	
	while(dx != 0 || dy != 0) {
		auto prevLinePos = linePositions.back();
		if(std::abs(dx) == std::abs(dy)) {
			linePositions.push_back(TilePos{prevLinePos.x + sign(dx), prevLinePos.y + sign(dy)});
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

TileEditor::TilePos::TilePos(Vector2 v) {
	x = v.x;
	y = v.y;
}
TileEditor::TilePos::TilePos(int x, int y) {
	this->x = x;
	this->y = y;
}

bool TileEditor::TilePos::operator==(const TilePos& rhs) const {
	return x == rhs.x && y == rhs.y;
}
bool TileEditor::TilePos::operator!=(const TilePos& rhs) const {
	return !(*this == rhs);
}
bool TileEditor::TilePos::operator>(const TilePos& rhs) const {
	return x > rhs.x || (x == rhs.x && y > rhs.y);
}
bool TileEditor::TilePos::operator<(const TilePos& rhs) const {
	return !(*this > rhs) && (*this != rhs);
}

void TileEditor::SetTool(ToolType toolType) {
	if(!tool) delete tool;
	switch(toolType) {
		case(ToolType::Brush):
			tool = new Brush();
			break;
		default:
			tool = nullptr;
			Logger::Error("Tried to set invalid tool type");
	}
}

void TileEditor::Brush::Paint(Tile* activeTile, std::set<TilePos>* reservedPixels, Rectangle tileRegion) {
	
	Vector2 mousePos = GetMousePosition();
	bool leftClick = IsMouseButtonDown(0) && !IsMouseButtonPressed(0);
	bool rightClick = IsMouseButtonDown(1) && !IsMouseButtonPressed(1);
	bool userIsInteracting = leftClick || rightClick;

	TilePos mouseTilePos = WindowPosToTilePos(mousePos, tileRegion);
	ColorIdx cursorColor = rightClick ? 0 : TilesetData::GetActiveColorIdx();

	// paint
	if(userIsInteracting) {

		Vector2 mouseDeltaBetweenFrames = GetMouseDelta();
		TilePos prevMouseTilePos = WindowPosToTilePos({mousePos.x - mouseDeltaBetweenFrames.x, mousePos.y - mouseDeltaBetweenFrames.y}, tileRegion);

		// paint line
		auto linePositions = LineBetween(mouseTilePos, prevMouseTilePos);
		for(auto p : linePositions) {
			TilesetData::GetActiveTile()->SetPixel(p.x, p.y, cursorColor);
		}
	}

	// draw brush cursor
	DrawRectangleRec(GetPixelBounds(mouseTilePos, tileRegion), TilesetData::GetColor(cursorColor));
	reservedPixels->insert(mouseTilePos);
}