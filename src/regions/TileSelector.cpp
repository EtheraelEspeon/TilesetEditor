#include "TileSelector.hpp"

#include <algorithm>

#include "GuiSizeInfo.hpp"
#include "TileEditor.hpp"
#include "Background.hpp"

#include "../util/Input.hpp"
#include "../util/Logger.hpp"
#include "../util/AssetLoader.hpp"

#include "../TilesetData.hpp"

/* ---- Singleton Stuff ---- */

TileSelector* TileSelector::instance = nullptr;
void TileSelector::Initialize() {
	if(instance != nullptr) delete instance;
	instance = new TileSelector();

	instance->addTileTex = AssetLoader::LoadRaylibTexture("res/tile_selector/add_tile.png");

	int numTiles = TilesetData::NumTiles();
	for(int i = 0; i < numTiles; i++) {
		auto tile = *TilesetData::TilesBegin();
		Inst()->tileTextures.insert({tile, TileToTexture(tile)});
	}

	Inst()->activeTileItr = TilesetData::TilesBegin();
}
TileSelector* TileSelector::Inst() {
	if(instance == nullptr) Logger::Error("TileSelector uninitialized");
	return instance;
}

Tile* TileSelector::ActiveTile() {
	return *Inst()->activeTileItr;
}

/* ---- General ---- */

void TileSelector::Update(Rectangle size) {

	if(Input::KeybindIsPressed("DeleteSelectedTile") && TilesetData::NumTiles() > 1) {
		// Update active tile:
		auto itr = activeTileItr;
		itr++;
		auto rItr = std::make_reverse_iterator(itr);
		itr--;

		int offset = 0;
		
		bool foundUndeletedTile = true;

		rItr++;
		while(rItr != TilesetData::TilesREnd()) {
			offset--;
			if(!(*rItr)->deleted) {
				goto found;
			}
			rItr++;
		}

		offset = 0;
		itr++;
		while(itr != TilesetData::TilesEnd()) {
			offset++;
			if(!(*itr)->deleted) {
				goto found;
			}
			itr++;
		}

		foundUndeletedTile = false;
		found:;

		if(foundUndeletedTile) {
			TilesetData::ApplyChange(
				[](Tile* tile){tile->deleted = true;},
				[](Tile* tile){tile->deleted = false; },
				*activeTileItr
			);

			std::advance(activeTileItr, offset);
		}
	}

	GuiSetStyle(GuiControl::BUTTON, GuiControlProperty::BORDER_WIDTH, GuiSizeInfo::SelectedBorderThickness);

	GuiSetStyle(GuiControl::BUTTON, GuiControlProperty::BASE_COLOR_NORMAL, 0x00000000); // make button bodies transparent
	GuiSetStyle(GuiControl::BUTTON, GuiControlProperty::BORDER_COLOR_NORMAL, 0x00000040); // make borders transparent when unselected

	GuiSetStyle(GuiControl::BUTTON, GuiControlProperty::BASE_COLOR_FOCUSED, 0x00000000); // make the button body transparent when focused
	GuiSetStyle(GuiControl::BUTTON, GuiControlProperty::BORDER_COLOR_FOCUSED, 0x00000060); // make the button border darken when focused

	GuiSetStyle(GuiControl::BUTTON, GuiControlProperty::BASE_COLOR_PRESSED, 0x00000000); // make buttons body transparent when pressed
	GuiSetStyle(GuiControl::BUTTON, GuiControlProperty::BORDER_COLOR_PRESSED, 0x000000AA); // make the border darker when pressed

	GuiSetStyle(GuiControl::BUTTON, GuiControlProperty::BASE_COLOR_DISABLED, 0x00000000); // make the button body transparent when the color is selected
	GuiSetStyle(GuiControl::BUTTON, GuiControlProperty::BORDER_COLOR_DISABLED, 0x000000AA); // darken the border when the color is selected

	GuiSetStyle(GuiControl::BUTTON, GuiControlProperty::TEXT_COLOR_NORMAL, 0x000000FF);
	GuiSetStyle(GuiControl::BUTTON, GuiControlProperty::TEXT_COLOR_FOCUSED, 0x000000FF);
	GuiSetStyle(GuiControl::BUTTON, GuiControlProperty::TEXT_COLOR_PRESSED, 0x000000FF);
	GuiSetStyle(GuiControl::BUTTON, GuiControlProperty::TEXT_COLOR_DISABLED, 0x000000FF);

	// We're assuming the width of this viewport is an integer
	// multiple of 32, which GuiSizeInfo should take care of
	float previewSize = size.width;
	float unitHeight = previewSize + GuiSizeInfo::RightRegionPadding; // the height of one viewpoint position unit in pixels

	UpdateScroll(size, unitHeight);

	auto tileItr = TilesetData::TilesBegin();
	int drawnTiles = 0;
	while(tileItr != TilesetData::TilesEnd()) {

		if((*tileItr)->deleted) {
			tileItr++;
			continue;
		}

		Rectangle previewPos;
		previewPos.y = size.y + unitHeight * ((float)drawnTiles - viewportPos);
		previewPos.x = size.x;
		previewPos.width = previewSize;
		previewPos.height = previewSize;
		drawnTiles++;

		if(!CheckCollisionRecs(previewPos, size)) { 
			tileItr++;
			continue;
		}

		Texture tex;
		// Initialize tex
		Tile* tile = *tileItr; // Try to get the tile
		auto itr = tileTextures.find(tile);
		if(itr != tileTextures.end()) { // if the tile has a texture already, just get it
			tex = itr->second;
		}
		else { // if the tile doesn't have an associated texture
			if(tile != nullptr) { // Generate a texture for it
				tex = TileToTexture(tile);
				tileTextures.insert({tile, tex});
			}
			else { // If the tile doesn't exist
				auto img = GenImageColor(32, 32, {255, 0, 255, 255}); // create a magenta error texture
				tex = LoadTextureFromImage(img);
				UnloadImage(img);
			}
		}

		DrawTexturePro(tex, {0, 0, 32, 32}, previewPos, {0, 0}, 0, {255, 255, 255, 255});

		if(activeTileItr == tileItr) GuiDisable();
		bool pressed = GuiButton(previewPos, "");
		if(activeTileItr == tileItr) GuiEnable();
	
		if(pressed) {
			activeTileItr = tileItr;
		}

		tileItr++;
	}

	// Draw the add tile button
	GuiSetStyle(GuiControl::BUTTON, GuiControlProperty::BORDER_WIDTH, 0);

	GuiSetStyle(GuiControl::BUTTON, GuiControlProperty::BASE_COLOR_NORMAL, 0x00000000); // make button bodies transparent
	GuiSetStyle(GuiControl::BUTTON, GuiControlProperty::BASE_COLOR_FOCUSED, 0x00000040); // make the button body darken when focused
	GuiSetStyle(GuiControl::BUTTON, GuiControlProperty::BASE_COLOR_PRESSED, 0x00000080); // make buttons get darker when pressed
	GuiSetStyle(GuiControl::BUTTON, GuiControlProperty::BASE_COLOR_DISABLED, 0x00000040); // make the button body transparent when the color is selected

	Rectangle buttonPos;
	buttonPos.y = size.y + unitHeight * (TilesetData::NumTiles() - viewportPos);
	buttonPos.x = size.x;
	buttonPos.width = previewSize;
	buttonPos.height = previewSize;

	if(CheckCollisionRecs(buttonPos, size)) {
		DrawRectangleRec(buttonPos, Background::rightBody);
		if(GuiButton(buttonPos, "")) TilesetData::AddTile();
		DrawTexturePro(addTileTex, {0, 0, (float)addTileTex.width, (float)addTileTex.height}, buttonPos, {0, 0}, 0, {255, 255, 255, 255});
	}
}
void TileSelector::InvalidateTextureOf(Tile* id) {
	auto itr = Inst()->tileTextures.find(id);
	if(itr == Inst()->tileTextures.end()) return;

	Inst()->tileTextures.erase(itr);
}

void TileSelector::UpdateScroll(Rectangle region, float unitHeight) {
	Vector2 mousePos = GetMousePosition(); // TODO: Make a wrapper for this
	if(CheckCollisionPointRec(mousePos, region)) {
		if(Input::KeybindIsPressed("ScrollTileEditorUp"))   targetViewportPos--;
		if(Input::KeybindIsPressed("ScrollTileEditorDown")) targetViewportPos++;
	}
	float maxY = TilesetData::NumTiles() - 1;

	if(targetViewportPos < 0) {
		float dydt = -targetViewportPos;
		targetViewportPos += dydt * Timestep * ScrollSpeed * 3;

		if(targetViewportPos > 0) targetViewportPos = 0;
	}
	if(targetViewportPos > maxY) {
		float dydt = maxY - targetViewportPos;
		targetViewportPos += dydt * Timestep * ScrollSpeed * 3;

		if(targetViewportPos < maxY) targetViewportPos = maxY;
	}

	if(viewportPos != targetViewportPos) {
		float dydt = targetViewportPos - viewportPos;
		viewportPos += dydt * Timestep * ScrollSpeed;

		float singlePixelInUnits = 1 / unitHeight;
		if(std::abs(viewportPos - targetViewportPos) <= singlePixelInUnits) viewportPos = targetViewportPos;
	}
}
Texture TileSelector::TileToTexture(const Tile* tile) {
	Image img = GenImageChecked(32, 32, 1, 1, TileEditor::BackgroundColors[0], TileEditor::BackgroundColors[1]);
	for(int y = 0; y < 16; y++) {
		for(int x = 0; x < 16; x++) {
			ColorIdx colorIdx = tile->GetPixel(x, y);
			if(colorIdx == 0) continue;

			Color color = TilesetData::GetColor(colorIdx);
			ImageDrawRectangle(&img, x * 2, y * 2, 2, 2, color);
		}
	}

	auto tex = LoadTextureFromImage(img);
	UnloadImage(img);
	return tex;
}