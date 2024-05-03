#include "TilesetData.hpp"

#include <format>

#include "util/Logger.hpp"

/* ---- Tile ---- */

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

/* ---- Singleton Stuff ---- */

TilesetData* TilesetData::inst = nullptr;
void TilesetData::Initialize() {
	if(!inst) delete inst;
	inst = new TilesetData();

	for(int i = 0; i < 15; i++) {
		SetColor(i + 1, ColorFromHSV((float)i * 360.0 / 15.0, 0.8, 1));
	}

	Inst()->tiles.push_back(new Tile());

	Logger::Message("Initialized new tileset!");	
}
TilesetData* TilesetData::Inst() {
	if(!inst) Logger::Error("TilesetData unintialized");
	return inst;
}
TilesetData::~TilesetData() {
	delete rootState;

	for(auto t : tiles) {
		delete t;
	}
	for(auto t : deletedTiles) {
		delete t;
	}
}

/* ---- Tile Management ---- */

Tile* TilesetData::GetTile(int tileIdx) {
	return *ItrFromTileIdx(tileIdx);
}
int TilesetData::NumTiles() {
	return Inst()->tiles.size();
}
void TilesetData::AddTile() {
	Inst()->tiles.push_back(new Tile());
}

bool TilesetData::TileIsDeleted(Tile* tileLocation) {
	return Inst()->deletedTiles.contains(tileLocation);
}

/* ---- Palette Management ---- */

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

/* ---- Utility ---- */
std::list<Tile*>::iterator TilesetData::ItrFromTileIdx(int tileIdx) {
	auto itr = inst->tiles.begin();
	std::advance(itr, tileIdx);
	return itr;
}

/* ---- Undo/Redo Support ---- */

TilesetData::Action TilesetData::UnpaintPixelsAction(std::vector<PaintData> paintData, Tile* activeTile) {
	for(auto& p : paintData) {
		p.color = activeTile->colorData[p.pixelIdx];
	}

	return [paintData=paintData](Tile* tile){
		for(int i = paintData.size() - 1; i >= 0; i--) {
			tile->SetPixel(paintData[i].pixelIdx, paintData[i].color);
		}
	};
}
TilesetData::Action TilesetData::PaintPixelsAction(std::vector<PaintData> paintData) {
	return [paintData=paintData](Tile* tile){
		for(int i = 0; i < paintData.size(); i++) {
			tile->SetPixel(paintData[i].pixelIdx, paintData[i].color);
		}
	};
}

TilesetData::PaintData::PaintData(int x, int y, ColorIdx color) {
	pixelIdx = 16 * y + x;
	this->color = color;
}

TilesetData::State::~State() {
	if(prev != nullptr) {
		delete reverse;
	}
	if(next != nullptr) {
		delete forward;
		delete next;
	}
	//Logger::Debug("Deleted state node");
}

TilesetData::State::Change::Change(Action traverse, Tile* targetTile) {
	this->traverse = traverse;
	this->targetTile = targetTile;
}
void TilesetData::State::Change::Apply() {
	if(TileIsDeleted(targetTile)) {
		Logger::Warning("No mechanism for undeleting tiles");
	}
	
	traverse(targetTile);
}

void TilesetData::ApplyChange(Action redo, Action undo, Tile* targetTile) {
	auto inst = Inst();
	
	auto nextState = new State();
	nextState->reverse = new State::Change(undo, targetTile);
	nextState->prev = inst->currentState;

	if(inst->currentState->next != nullptr) {
		//Logger::Debug("Deleting undone changes");
		delete inst->currentState->next;
	} 

	inst->currentState->forward = new State::Change(redo, targetTile);
	inst->currentState->next = nextState;

	inst->currentState->forward->Apply();
	inst->currentState = nextState;
}

void TilesetData::UndoLatestChange() {
	if(inst->currentState->prev == nullptr) {
		//Logger::Debug("Tried to undo at root of tree");
		return;
	}

	inst->currentState->reverse->Apply();
	inst->currentState = inst->currentState->prev;
}
void TilesetData::RedoLatestChange() {
	if(inst->currentState->next == nullptr) {
		//Logger::Debug("Tried to redo at end of tree");
		return;
	}

	inst->currentState->forward->Apply();
	inst->currentState = inst->currentState->next;
}

