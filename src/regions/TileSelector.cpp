#include "TileSelector.hpp"

#include <algorithm>

#include "GuiSizeInfo.hpp"

#include "../util/Input.hpp"

#include "../TilesetData.hpp"

void TileSelector::Update(Rectangle size) {
	// We're assuming the width of this viewport is an integer
	// multiple of 16, which GuiSizeInfo should take care of
	float previewSize = size.width;
	float unitHeight = previewSize + GuiSizeInfo::RightRegionPadding; // the height of one viewpoint position unit in pixels

	UpdateScroll(size, unitHeight);

	while(TilesetData::NumTiles() < 10) TilesetData::AddTile();

	std::vector<Rectangle> tilePreviewPositions;
	tilePreviewPositions.resize(TilesetData::NumTiles());
	for(int i = 0; i < tilePreviewPositions.size(); i++) {
		Rectangle previewPos;
		previewPos.y = size.y + unitHeight * ((float)i - viewportPos);
		previewPos.x = size.x;
		previewPos.width = previewSize;
		previewPos.height = previewSize;

		tilePreviewPositions[i] = previewPos;
		float temp_mul = ((float)i / (float)TilesetData::NumTiles());
		DrawRectangleRec(previewPos, {(uint8_t)(255.0 * temp_mul), 0, (uint8_t)(255.0 * temp_mul), 255});
	}
}

void TileSelector::UpdateScroll(Rectangle region, float unitHeight) {
	Vector2 mousePos = GetMousePosition(); // TODO: Make a wrapper for this
	if(CheckCollisionPointRec(mousePos, region)) {
		if(Input::KeybindIsPressed("ScrollTileEditorUp"))   targetViewportPos--;
		if(Input::KeybindIsPressed("ScrollTileEditorDown")) targetViewportPos++;
	}
	targetViewportPos = std::clamp<float>(targetViewportPos, 0, TilesetData::NumTiles() - 1);

	if(viewportPos != targetViewportPos) {
		float dydt = targetViewportPos - viewportPos;
		viewportPos += dydt * Timestep * ScrollSpeed;

		float singlePixelInUnits = 1 / unitHeight;
		if(std::abs(viewportPos - targetViewportPos) <= singlePixelInUnits) viewportPos = targetViewportPos;
	}
}