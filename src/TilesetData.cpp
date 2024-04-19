#include "TilesetData.hpp"

#include <format>

#include "Logger.hpp"

Tile::Tile() {
	colorData.fill(0);
}

void Tile::SetPixel(int x, int y, ColorIdx color) {
	int idx = y * 16 + x;
	SetPixel(idx, color);
}
void Tile::SetPixel(uint8_t rawIdx, ColorIdx color) {
	colorData[rawIdx] = color;
	//Logger::Debug(std::format("[{}] -> {}", rawIdx, color));
}
ColorIdx Tile::GetPixel(int x, int y) { return colorData[y * 16 + x]; }

void UndoQueue::PushResetter(UndoQueue::Resetter undo, Tile* currentActiveTile) {
	changeHistory.push_back({undo, currentActiveTile});
};
void UndoQueue::UndoLatestChange() {

	if(changeHistory.empty()) {
		Logger::Debug("Attempted to undo when no changes were in queue");
		return;
	}

	UndoAction latestChange = changeHistory.back();
	changeHistory.pop_back();
	
	if(TilesetData::TileIsDeleted(latestChange.targetTile)) {
		Logger::Debug("Attempted to undo an action on a deleted tile, recurring");
		UndoLatestChange(); // This might overflow the stack? Seems unlikely.
	}
	else latestChange.resetter(latestChange.targetTile);
}

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
	auto itr = ItrFromTileIdx(tileIdx);
	
	Inst()->deletedTileLocations.insert(&(*itr));
	Inst()->tiles.erase(itr);
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

bool TilesetData::TileIsDeleted(void* tileLocation) {
	return Inst()->deletedTileLocations.contains(tileLocation);
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