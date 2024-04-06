#include "TilesetData.hpp"

#include "Logger.hpp"

Tile::Tile() {
	idxData.fill(0);
}

void Tile::SetPixel(int x, int y, uint8_t paletteIdx) { idxData[y * 16 + x] = paletteIdx; }
uint8_t Tile::GetPixel(int x, int y) { return idxData[y * 16 + x]; }

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

void TilesetData::SetColor(int paletteIdx, Color color) {
	if(paletteIdx == 0) {
		Logger::Warning("Tried to set index zero of palette");
		return;
	}

	Inst()->palette[paletteIdx - 1] = color;
}
Color TilesetData::GetColor(int paletteIdx) {
	if(paletteIdx == 0) return {0, 0, 0, 0};
	return Inst()->palette[paletteIdx - 1];
}

Color TilesetData::GetActiveColor() {
	return GetColor(Inst()->activeColorIdx);
}
int TilesetData::GetActiveColorIdx() {
	return Inst()->activeColorIdx;
}
void TilesetData::SetActiveColor(int colorIdx) {
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