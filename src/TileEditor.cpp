#include "TileEditor.hpp"

#include "Logger.hpp"

void TileEditor::Update(Rectangle size) {
	
	uint64_t activeColor = ColorToInt(TilesetData::GetActiveColor());
	
	GuiSetStyle(GuiControl::BUTTON, GuiControlProperty::BASE_COLOR_NORMAL, 0x00000000);
	GuiSetStyle(GuiControl::BUTTON, GuiControlProperty::BASE_COLOR_FOCUSED, activeColor);
	GuiSetStyle(GuiControl::BUTTON, GuiControlProperty::BASE_COLOR_PRESSED, activeColor);

	GuiSetStyle(GuiControl::BUTTON, GuiControlProperty::BORDER_COLOR_NORMAL, 0x00000000);
	GuiSetStyle(GuiControl::BUTTON, GuiControlProperty::BORDER_COLOR_FOCUSED, 0x00000000);
	GuiSetStyle(GuiControl::BUTTON, GuiControlProperty::BORDER_COLOR_PRESSED, 0x00000000);
	GuiSetStyle(GuiControl::BUTTON, GuiControlProperty::BORDER_WIDTH, 4);

	int hStep = size.width / 16;
	int vStep = size.height / 16;
	int hStepHalf = hStep / 2;
	int vStepHalf = vStep / 2;
	
	for(int y = 0; y < 32; y++) {
		for(int x = 0; x < 32; x++) {
			Rectangle bounds;
			bounds.x = size.x + hStepHalf * x;
			bounds.y = size.y + vStepHalf * y;
			bounds.width = hStepHalf;
			bounds.height = vStepHalf;

			int colorIdx = (x + y % 2) % 2;

			DrawRectangleRec(bounds, backgroundColors[colorIdx]);
		}
	}

	for(int y = 0; y < 16; y++) {
		for(int x = 0; x < 16; x++) {
			Tile* activeTile = TilesetData::GetActiveTile();
			int colorIdx = activeTile->GetPixel(x, y);
			Color color = TilesetData::GetColor(colorIdx);

			Rectangle bounds;
			bounds.x = size.x + hStep * x;
			bounds.y = size.y + vStep * y;
			bounds.width = hStep;
			bounds.height = vStep;

			DrawRectangleRec(bounds, color);

			if(GuiButton(bounds, "")) activeTile->SetPixel(x, y, TilesetData::GetActiveColorIdx());
		}
	}


}
