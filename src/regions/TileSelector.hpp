#pragma once

#include <map>

#include "../../raylib/raygui.h"

#include "../TilesetData.hpp"

class TileSelector {
public:
	static constexpr float Timestep = 1.0/60.0;
	static constexpr float ScrollSpeed = 8;
	
	static void Initialize();
	static TileSelector* Inst();

	static Tile* ActiveTile();

	void Update(Rectangle size);

	static void InvalidateTextureOf(Tile* id);
private:
	static TileSelector* instance;

	/* ---- State ---- */

	Texture2D addTileTex;

	float scrollTarget = 0;
	float targetViewportPos = 0; // starts at 0, scrolling down increases the value, making the list entries move up
	float viewportPos = 0;

	std::list<Tile*>::iterator activeTileItr;

	std::map<Tile*, Texture> tileTextures = {};

	void UpdateScroll(Rectangle region, float unitHeight);

	/* ---- Utility ---- */

	static Texture TileToTexture(const Tile* tile);
};