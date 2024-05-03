#include "TileEditor.hpp"

#include <algorithm>
#include <format>
#include <cmath>

#include "PaletteEditor.hpp"

#include "../util/Logger.hpp"
#include "../util/Input.hpp"

TileEditor* TileEditor::instance = nullptr;
void TileEditor::Initialize() {
	if(instance != nullptr) delete instance;
	instance = new TileEditor();

	Inst()->SetTool(ToolType::Brush);
}
TileEditor* TileEditor::Inst() {
	if(instance == nullptr) Logger::Error("TileEditor uninitialized");
	return instance;
}

void TileEditor::Update(Rectangle size) {

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

			DrawRectangleRec(bounds, BackgroundColors[colorIdx]);
		}
	}

	Tile* activeTile = TilesetData::GetTile(activeTileIdx);

	// If the mouse is outside the draw region and the user presses a mouse button, deactivate painting until they release it 
	if(!CheckCollisionPointRec(GetMousePosition(), size) && (Input::PrimaryInteractionPressed() || Input::SecondaryInteractionPressed())) { // TODO: Make a wrapper for mouse position in Input
		pressBeganOutsideEditorRegion = true;
	}

	// paint
	std::set<TilePos> reservedPixels = {}; // pixels that wont be drawn when the sprite is drawn
	if(!pressBeganOutsideEditorRegion) {
		tool->Paint(activeTile, &reservedPixels, size);
	}
	
	// reset this as soon as the user releases both mouse buttons.
	pressBeganOutsideEditorRegion &= Input::PrimaryInteractionHeld() || Input::SecondaryInteractionHeld();

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
TileEditor::TilePos TileEditor::WindowPosToTilePos(Vector2 windowPos, Rectangle editorRegion) {
	Vector2 tilePos = windowPos; // tile that the mouse is over
	tilePos.x -= editorRegion.x;
	tilePos.y -= editorRegion.y;
	tilePos.x /= (editorRegion.width / 16);
	tilePos.y /= (editorRegion.height / 16);

	if      (tilePos.x < 0)  tilePos.x = 0;
	else if (tilePos.x > 15) tilePos.x = 15;
	if      (tilePos.y < 0)  tilePos.y = 0;
	else if (tilePos.y > 15) tilePos.y = 15;

	return tilePos;
}
Rectangle TileEditor::GetPixelBounds(TilePos tilePos, Rectangle editorRegion) {
	
	// floor values
	tilePos.x = (int)tilePos.x;
	tilePos.y = (int)tilePos.y;

	int step = editorRegion.width / 16;
	
	Rectangle bounds;
	bounds.x = tilePos.x * step + editorRegion.x;
	bounds.y = tilePos.y * step + editorRegion.y;
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
std::vector<TileEditor::TilePos> TileEditor::CircleAround(TilePos center, float radius) {
	int integerRadius = std::ceil(radius);
	int minX, minY, maxX, maxY;
	minX = std::clamp(center.x - integerRadius, 0, 15);
	maxX = std::clamp(center.x + integerRadius, 0, 15);
	minY = std::clamp(center.y - integerRadius, 0, 15);
	maxY = std::clamp(center.y + integerRadius, 0, 15);

	std::vector<TilePos> out = {};
	for(int y = minY; y <= maxY; y++) {
		for(int x = minX; x <= maxX; x++) {
			float relX = x - center.x;
			float relY = y - center.y;

			if(relX * relX + relY * relY <= radius * radius) {
				out.push_back({x, y});
			}
		}
	}

	return out;
}

void TileEditor::SetTool(ToolType toolType) {

	Tool* newTool = nullptr;

	switch(toolType) {
	case(ToolType::Brush):
		newTool = new Brush();
		break;
	case(ToolType::Line):
		newTool = new Line();
		break;
	case(ToolType::Fill):
		newTool = new Fill();
		break;
	case(ToolType::Eyedropper):
		newTool = new Eyedropper();
		break;
	default:
		break;
	}

	if(newTool == nullptr) {
		Logger::Warning(std::format("Tried to set invalid tool type {}", (int)toolType));
		return;
	}

	if(Inst()->tool != nullptr) delete Inst()->tool;
	Inst()->tool = newTool;
	Inst()->currentTool = toolType;
}
TileEditor::ToolType TileEditor::CurrentTool() { return Inst()->currentTool; }

TileEditor::TilePos::TilePos(Vector2 v) {
	x = v.x;
	y = v.y;
}
TileEditor::TilePos::TilePos(int x, int y) {
	this->x = x;
	this->y = y;
}
TileEditor::TilePos::TilePos(int rawIdx) {
	y = rawIdx / 16;
	x = rawIdx - y * 16;
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

void TileEditor::Brush::Paint(Tile* activeTile, std::set<TilePos>* reservedPixels, Rectangle editorRegion) {

	if(Input::KeybindIsPressed("GrowBrush"))   radius += 0.5;
	if(Input::KeybindIsPressed("ShrinkBrush")) radius -= 0.5;
	radius = std::clamp<float>(radius, 0, 16);

	Vector2 mousePos = GetMousePosition(); // TODO: Make a wrapper for mouse position in Input
	bool leftClick = Input::PrimaryInteractionHeld() && !Input::PrimaryInteractionPressed();
	bool rightClick = Input::SecondaryInteractionHeld() && !Input::SecondaryInteractionPressed();
	bool userIsInteracting = leftClick || rightClick;
	bool mouseInRegion = CheckCollisionPointRec(mousePos, editorRegion);

	auto mouseTilePositions = CircleAround(WindowPosToTilePos(mousePos, editorRegion), radius);
	ColorIdx cursorColor = rightClick ? 0 : PaletteEditor::ActiveColorIdx();

	// paint
	if(userIsInteracting && mouseInRegion) {

		painting = true;

		Vector2 mouseDeltaBetweenFrames = GetMouseDelta(); // TODO: Make a wrapper for this in Input
		auto prevMouseTilePositions = CircleAround(WindowPosToTilePos({mousePos.x - mouseDeltaBetweenFrames.x, mousePos.y - mouseDeltaBetweenFrames.y}, editorRegion), radius);

		for(int i = 0; i < mouseTilePositions.size(); i++) {
			// paint line
			auto linePositions = LineBetween(mouseTilePositions[i], prevMouseTilePositions[i]);
			for(auto p : linePositions) {
			
			auto newPaintData = TilesetData::PaintData(p.x, p.y, cursorColor);

			if(tileIndicesToBePainted.contains(newPaintData.pixelIdx)) continue;

			tileIndicesToBePainted.insert(newPaintData.pixelIdx);
			paintLocations.push_back(newPaintData);
			}
		}
	}
	else if(painting) { // if the user stops interacting and was previously painting
		painting = false;

		if(!paintLocations.empty()) {
			TilesetData::ApplyChange(
				TilesetData::PaintPixelsAction(paintLocations),
				TilesetData::UnpaintPixelsAction(paintLocations, activeTile),
				activeTile
			);
		}

		tileIndicesToBePainted.clear();
		paintLocations.clear();
	}

	// draw brush cursor and painted locations
	if(mouseInRegion) {

		for(auto p : mouseTilePositions) {
			DrawRectangleRec(GetPixelBounds(p, editorRegion), TilesetData::GetColor(cursorColor));
			reservedPixels->insert(p);
		}
		for(auto p : paintLocations) {
			DrawRectangleRec(GetPixelBounds(TilePos(p.pixelIdx), editorRegion), TilesetData::GetColor(cursorColor));
			reservedPixels->insert(TilePos(p.pixelIdx));
		}
	}
}
void TileEditor::Line::Paint(Tile* activeTile, std::set<TilePos>* reservedPixels, Rectangle editorRegion) {
	
	Vector2 mousePos = GetMousePosition(); // TODO: Make a wrapper for mouse position in Input
	if(!CheckCollisionPointRec(mousePos, editorRegion)) return;

	bool leftClick = Input::PrimaryInteractionPressed();
	bool rightClick = Input::SecondaryInteractionPressed();

	TilePos mouseTilePos = WindowPosToTilePos(mousePos, editorRegion);
	ColorIdx paintColorIdx = rightClick ? 0 : PaletteEditor::ActiveColorIdx();
	Color paintColor = TilesetData::GetColor(paintColorIdx);

	if(rightClick) state = NoPoints;

	switch(state) {
	case(NoPoints): {
		// paint preview point
		DrawRectangleRec(GetPixelBounds(mouseTilePos, editorRegion), paintColor);
		reservedPixels->insert(mouseTilePos);
		// progress state
		if(leftClick) {
			startPos = mouseTilePos;
			state = OnePoint;
		}
		break;
	} case(OnePoint): {
		auto linePoints = LineBetween(mouseTilePos, startPos);
		// display preview line
		for(auto t : linePoints) {
			DrawRectangleRec(GetPixelBounds(t, editorRegion), paintColor);
			reservedPixels->insert(t);
		}
		// progress state
		if(leftClick) {
			
			std::vector<TilesetData::PaintData> toPaint = {};

			for (auto t : linePoints) {
				toPaint.push_back(TilesetData::PaintData(t.x, t.y, paintColorIdx));
			}
			state = NoPoints;

			TilesetData::ApplyChange(
				TilesetData::PaintPixelsAction(toPaint),
				TilesetData::UnpaintPixelsAction(toPaint, activeTile),
				activeTile
			);
		}
		break;
	} default:
		Logger::Warning("Line tool in invalid state. Rightclick in tile editor region to resolve");
	}
}
void TileEditor::Fill::Paint(Tile* activeTile, std::set<TilePos>* reservedPixels, Rectangle editorRegion) {

	Vector2 mousePos = GetMousePosition(); // TODO: Make a wrapper for mouse position in Input
	if(!CheckCollisionPointRec(mousePos, editorRegion)) return;

	bool leftClick = Input::PrimaryInteractionPressed();
	bool rightClick = Input::SecondaryInteractionPressed();
	bool userIsInteracting = leftClick || rightClick;

	TilePos mouseTilePos = WindowPosToTilePos(mousePos, editorRegion);
	ColorIdx paintColor = rightClick ? 0 : PaletteEditor::ActiveColorIdx();

	// paint
	if(userIsInteracting) {
		ColorIdx colorToMatch = activeTile->GetPixel(mouseTilePos.x, mouseTilePos.y);
	
		std::set<TilePos> checkedTiles = {mouseTilePos};
		std::set<TilePos>* toCheck = new std::set<TilePos>({mouseTilePos});
		std::set<TilePos>* toCheckNext = new std::set<TilePos>({});
		std::vector<TilePos> fillDirections;

		if(Input::KeybindIsHeld("SwapFillMode")) fillDirections = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
		else fillDirections = {{1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}, {-1, -1}, {0, -1}, {1, -1}};

		std::vector<TilesetData::PaintData> toPaint = {};

		while(!toCheck->empty()) {
			for(auto p : *toCheck) {
				checkedTiles.insert(p);

				if(activeTile->GetPixel(p.x, p.y) != colorToMatch) continue;
				toPaint.push_back(TilesetData::PaintData(p.x, p.y, paintColor));

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

		TilesetData::ApplyChange(
			TilesetData::PaintPixelsAction(toPaint),
			TilesetData::UnpaintPixelsAction(toPaint, activeTile),
			activeTile
		);
	}

	// draw brush cursor
	DrawRectangleRec(GetPixelBounds(mouseTilePos, editorRegion), TilesetData::GetColor(paintColor));
	reservedPixels->insert(mouseTilePos);
}
void TileEditor::Eyedropper::Paint(Tile* activeTile, std::set<TilePos>* reservedPixels, Rectangle editorRegion) {
	// TODO: Make a wrapper for mouse position in Input
	if(!CheckCollisionPointRec(GetMousePosition(), editorRegion)) return;

	bool leftClick = Input::PrimaryInteractionPressed();
	
	if(leftClick) {
		TilePos mousePos = WindowPosToTilePos(GetMousePosition(), editorRegion);
		ColorIdx newColor = activeTile->GetPixel(mousePos.x, mousePos.y);
		
		if(newColor != 0) PaletteEditor::SetActiveColorIdx(newColor);
	}
	

}
