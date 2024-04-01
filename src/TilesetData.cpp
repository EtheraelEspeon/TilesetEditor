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
		SetColor(i, ColorFromHSV((float)i * 360.0 / 15.0, 0.8, 1));
	}

	inst->tiles.push_back(Tile());

	Logger::Message("Initialized new tileset!");	
}

void TilesetData::DeleteTile(int tileIdx) {
	inst->tiles.erase(ItrFromTileIdx(tileIdx));
}
Tile* TilesetData::GetTile(int tileIdx) {
	return &(*ItrFromTileIdx(tileIdx));
}

void TilesetData::SetColor(int paletteIdx, Color color) {
	if(inst == nullptr) Logger::Error("TilesetData unintialized");
	inst->palette[paletteIdx] = color;
}
Color TilesetData::GetColor(int paletteIdx) {
	if(inst == nullptr) Logger::Error("TilesetData unintialized");
	return inst->palette[paletteIdx];
}

std::list<Tile>::iterator TilesetData::ItrFromTileIdx(int tileIdx) {
	if(inst == nullptr) Logger::Error("TilesetData unintialized");
	auto itr = inst->tiles.begin();
	std::advance(itr, tileIdx);
	return itr;
}