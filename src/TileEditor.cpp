#include "TileEditor.hpp"

#include <algorithm>

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
	if(!pressBeganOutsideEditorRegion && PointInRec(GetMousePosition(), size)) {
		tool->Paint(activeTile, &reservedPixels, size);
	}
	// If the mouse is outside the draw region and the user presses a mouse button, deactivate painting until they release it 
	else if(IsMouseButtonPressed(0) || IsMouseButtonPressed(1)) {
		pressBeganOutsideEditorRegion = true;
	}
	// reset this as soon as the user releases both mouse buttons.
	pressBeganOutsideEditorRegion &= IsMouseButtonDown(0) || IsMouseButtonDown(1);

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
	// Not literal distance, but the value this returns monotonically increases as you stray farther from the line
	const auto DistanceToLine = [to, from](Vector2 pos) {
		Vector2 dir = {(float)(to.x - from.x), (float)(to.y - from.y)};
		return std::abs((pos.y - to.y) * dir.x - (pos.x - to.x) * dir.y);
	};

	std::vector<TilePos> linePositions = {from};

	int xDir, yDir;
	xDir = sign(to.x - from.x);
	yDir = sign(to.y - from.y);

	while(linePositions.back() != to) {
		Vector2 currentPos = {(float)(linePositions.back().x), (float)(linePositions.back().y)};

		Vector2 moveX = {currentPos.x + xDir, currentPos.y};
		Vector2 moveY = {currentPos.x, currentPos.y + yDir};
		Vector2 moveBoth = {currentPos.x + xDir, currentPos.y + yDir};	
	
		std::array<std::pair<Vector2, float>, 3> moveOptions = {
			std::pair<Vector2, float>{moveX, DistanceToLine(moveX)},
			std::pair<Vector2, float>{moveY, DistanceToLine(moveY)},
			std::pair<Vector2, float>{moveBoth, DistanceToLine(moveBoth)}
		};

		int minIdx = -1;
		for(int i = 0; i < moveOptions.size(); i++) {
			if(moveOptions[i].first.x == currentPos.x && moveOptions[i].first.y == currentPos.y) continue; // will never reach this loop when to and from are on top of eachother
			
			if(minIdx == -1) minIdx = i;
			else if(moveOptions[i].second < moveOptions[minIdx].second) minIdx = i;
		}
		linePositions.push_back(moveOptions[minIdx].first);

		if(linePositions.size() > 256) {
			Logger::Warning("Couldnt finish drawing line from (" + std::to_string(from.x) + ", " + std::to_string(from.y) + ") to (" + std::to_string(to.x) + ", " + std::to_string(to.y) + ")");
			return {};
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

TileEditor::TilePos TileEditor::TilePos::operator+(const TilePos& rhs) const {
	return {std::clamp(x + rhs.x, 0, 15), std::clamp(y + rhs.y, 0, 15)};
}

TileEditor::Tool* TileEditor::tool = nullptr;
TileEditor::ToolType TileEditor::currentTool = ToolType::Null;
void TileEditor::SetTool(ToolType toolType) {
	
	currentTool = toolType;

	if(!tool) delete tool;
	switch(toolType) {
	case(ToolType::Brush):
		tool = new Brush();
		break;
	case(ToolType::Line):
		tool = new Line();
		break;
	case(ToolType::Fill):
		tool = new Fill();
		break;
	default:
		tool = nullptr;
		Logger::Error("Tried to set invalid tool type");
	}
}
TileEditor::ToolType TileEditor::CurrentTool() { return currentTool; }

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
void TileEditor::Line::Paint(Tile* activeTile, std::set<TilePos>* reservedPixels, Rectangle tileRegion) {
	
	Vector2 mousePos = GetMousePosition();
	bool leftClick = IsMouseButtonPressed(0);
	bool rightClick = IsMouseButtonPressed(1);

	TilePos mouseTilePos = WindowPosToTilePos(mousePos, tileRegion);
	ColorIdx paintColorIdx = rightClick ? 0 : TilesetData::GetActiveColorIdx();
	Color paintColor = TilesetData::GetColor(paintColorIdx);

	if(rightClick) state = NoPoints;

	switch(state) {
	case(NoPoints): {
		// paint preview point
		DrawRectangleRec(GetPixelBounds(mouseTilePos, tileRegion), paintColor);
		reservedPixels->insert(mouseTilePos);
		// progress state
		if(leftClick) {
			startPos = mouseTilePos;
			state = OnePoint;
		}
		break;
	} case(OnePoint): {
		// paint preview line
		auto toPaint = LineBetween(mouseTilePos, startPos);
		for(auto t : toPaint) {
			DrawRectangleRec(GetPixelBounds(t, tileRegion), paintColor);
			reservedPixels->insert(t);
		}
		// progress state
		if(leftClick) for (auto t : toPaint) {
			TilesetData::GetActiveTile()->SetPixel(t.x, t.y, paintColorIdx);
			state = NoPoints;
		}
		break;
	} default:
		Logger::Warning("Line tool in invalid state. Rightclick in tile editor region to resolve");
	}
}
void TileEditor::Fill::Paint(Tile* activeTile, std::set<TilePos>* reservedPixels, Rectangle tileRegion) {

	Vector2 mousePos = GetMousePosition();
	bool leftClick = IsMouseButtonPressed(0);
	bool rightClick = IsMouseButtonPressed(1);
	bool userIsInteracting = leftClick || rightClick;

	TilePos mouseTilePos = WindowPosToTilePos(mousePos, tileRegion);
	ColorIdx paintColor = rightClick ? 0 : TilesetData::GetActiveColorIdx();

	// paint
	if(userIsInteracting) {
		ColorIdx colorToMatch = activeTile->GetPixel(mouseTilePos.x, mouseTilePos.y);
	
		std::set<TilePos> checkedTiles = {mouseTilePos};
		std::set<TilePos>* toCheck = new std::set<TilePos>({mouseTilePos});
		std::set<TilePos>* toCheckNext = new std::set<TilePos>({});
		std::vector<TilePos> fillDirections;

		if(!IsKeyDown(KEY_LEFT_CONTROL)) fillDirections = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
		else fillDirections = {{1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}, {-1, -1}, {0, -1}, {1, -1}};

		while(!toCheck->empty()) {
			for(auto p : *toCheck) {
				checkedTiles.insert(p);

				if(activeTile->GetPixel(p.x, p.y) != colorToMatch) continue;
				activeTile->SetPixel(p.x, p.y, paintColor);

				for(auto d : fillDirections) {
					auto newPosToCheck = p + d;
					if(checkedTiles.contains(newPosToCheck)) continue;

					toCheckNext->insert(newPosToCheck);
				}
			}

			delete toCheck;
			toCheck = toCheckNext;
			toCheckNext = new std::set<TilePos>();
		}

		delete toCheck;
		delete toCheckNext;
	}

	// draw brush cursor
	DrawRectangleRec(GetPixelBounds(mouseTilePos, tileRegion), TilesetData::GetColor(paintColor));
	reservedPixels->insert(mouseTilePos);
}