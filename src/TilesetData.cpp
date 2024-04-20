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

ChangeQueue::PaintData::PaintData(int x, int y, ColorIdx color) {
	pixelIdx = 16 * y + x;
	this->color = color;
}

ChangeQueue::UndoAction ChangeQueue::UnpaintPixelsAction(std::vector<PaintData> paintData, Tile* activeTile) {
	for(auto& p : paintData) {
		p.color = activeTile->colorData[p.pixelIdx];
	}

	return [paintData=paintData](Tile* tile){
		for(int i = paintData.size() - 1; i >= 0; i--) {
			tile->SetPixel(paintData[i].pixelIdx, paintData[i].color);
		}
	};
}
ChangeQueue::RedoAction ChangeQueue::PaintPixelsAction(std::vector<PaintData> paintData) {
	return [paintData=paintData](Tile* tile){
		for(int i = 0; i < paintData.size(); i++) {
			tile->SetPixel(paintData[i].pixelIdx, paintData[i].color);
		}
	};
}

ChangeQueue::Change::~Change() {
	if(next != nullptr) delete next;
	Logger::Debug("Deleted change");
}
void ChangeQueue::ApplyChange(RedoAction redo, UndoAction undo, Tile* activeTile) {
	if(firstChangeIsUndone == true) {
		delete latestChange;
		latestChange = nullptr;
		firstChangeIsUndone = false;
	}
	
	if(latestChange == nullptr) {
		latestChange = new Change();
		Logger::Debug("Initialized change history tree");
	}
	else {
		if(latestChange->next != nullptr) delete latestChange->next; 
		latestChange->next = new Change();
		latestChange->next->prev = latestChange;
		latestChange = latestChange->next;
		Logger::Debug("Appended new change to history tree");
	}

	latestChange->redo = redo;
	latestChange->undo = undo;
	latestChange->targetTile = activeTile;
	
	latestChange->redo(latestChange->targetTile);
};
void ChangeQueue::UndoLatestChange() {

	if(latestChange == nullptr || firstChangeIsUndone) {
		Logger::Debug("Attempted to undo while at the beginning of history");
		return;
	}

	if(TilesetData::TileIsDeleted(latestChange->targetTile)) {
		Logger::Debug("Attempted to undo an action on a deleted tile, recurring");
		latestChange = latestChange->prev;
		UndoLatestChange(); // This might overflow the stack? Seems unlikely.
	}
	else {
		latestChange->undo(latestChange->targetTile);
		if(latestChange->prev == nullptr) firstChangeIsUndone = true;
		else latestChange = latestChange->prev;
	}
}
void ChangeQueue::RedoLatestChange() {

	if(latestChange->next == nullptr && !firstChangeIsUndone) {
		Logger::Debug("Attempted to redo when end of history");
		return;
	}
	
	if(!firstChangeIsUndone) latestChange = latestChange->next;
	else firstChangeIsUndone = false;

	if(TilesetData::TileIsDeleted(latestChange->targetTile)) {
		Logger::Debug("Attempted to redo an action on a deleted tile, recurring");
		latestChange = latestChange->next;
		RedoLatestChange(); // This might overflow the stack? Seems unlikely.
	}
	else latestChange->redo(latestChange->targetTile);
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