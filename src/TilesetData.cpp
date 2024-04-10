#include "TilesetData.hpp"

#include "Logger.hpp"

Tile::Tile() {
	colorData.fill(0);
}

void Tile::SetPixel(int x, int y, ColorIdx color) {
	int idx = y * 16 + x;

	Change change = {idx, colorData[idx]};

	if(color != colorData[idx]) changeHistory.push_back(change); // prevent meaningless changes from being logged

	colorData[idx] = color;
}
ColorIdx Tile::GetPixel(int x, int y) { return colorData[y * 16 + x]; }

void Tile::CloseCurrentChangeFrame() {
	changeFrameBeginnings.push_back(changeHistory.size());
}
void Tile::RevertChangesInFrame() {

	if(changeFrameBeginnings.empty()) {
		Logger::Debug("Tried to undo changes while tile had no change frames");
		return;
	}

	// revert changes
	for(int i = changeHistory.size() - 1; i >= changeFrameBeginnings.back(); i--) {
		Change change = changeHistory[i];
		colorData[change.idxChanged] = change.prevColor;
		
		int y = change.idxChanged / 16;
		int x = change.idxChanged - y * 16;
		Logger::Debug("Reverted pixel (" + std::to_string(x) + ", " + std::to_string(y) + ") to color " + std::to_string(change.prevColor));
	}

	changeHistory.erase(changeHistory.begin() + changeFrameBeginnings.back(), changeHistory.end());
	changeFrameBeginnings.pop_back();
}

bool Tile::Change::operator==(const Tile::Change& rhs) { return this->idxChanged == rhs.idxChanged && this->prevColor == rhs.prevColor; }
bool Tile::Change::operator!=(const Tile::Change& rhs) { return !(*this == rhs); }

TilesetData* TilesetData::inst = nullptr;
void TilesetData::Initialize() {
	if(!inst) delete inst;
	inst = new TilesetData();

	for(int i = 0; i < 15; i++) {
		SetColor(i + 1, ColorFromHSV((float)i * 360.0 / 15.0, 0.8, 1));
	}

	Inst()->tiles.push_back(Tile());

	Logger::Message("Initialized new tileset!");	
}

void TilesetData::DeleteTile(int tileIdx) {
	Inst()->tiles.erase(ItrFromTileIdx(tileIdx));
}
Tile* TilesetData::GetTile(int tileIdx) {
	return &(*ItrFromTileIdx(tileIdx));
}

void TilesetData::SetColor(ColorIdx colorIdx, Color color) {
	if(colorIdx == 0) {
		Logger::Warning("Tried to set index zero of palette");
		return;
	}

	Inst()->palette[colorIdx - 1] = color;
}
Color TilesetData::GetColor(ColorIdx colorIdx) {
	if(colorIdx == 0) return {0, 0, 0, 0};
	return Inst()->palette[colorIdx - 1];
}

Color TilesetData::GetActiveColor() {
	return GetColor(Inst()->activeColorIdx);
}
int TilesetData::GetActiveColorIdx() {
	return Inst()->activeColorIdx;
}
void TilesetData::SetActiveColor(ColorIdx colorIdx) {
	Inst()->activeColorIdx = colorIdx;
}

Tile* TilesetData::GetActiveTile() {
	return GetTile(Inst()->activeTileIdx);
}
void TilesetData::SetActiveTile(int tileIdx) {
	Inst()->activeTileIdx = tileIdx;
}

TilesetData* TilesetData::Inst() {
	if(!inst) Logger::Error("TilesetData unintialized");
	return inst;
}

std::list<Tile>::iterator TilesetData::ItrFromTileIdx(int tileIdx) {
	auto itr = inst->tiles.begin();
	std::advance(itr, tileIdx);
	return itr;
}