#include "PaletteEditor.hpp"

#include <string>

#include "TilesetData.hpp"

void PaletteEditor::Update(Rectangle size){

	GuiSetStyle(GuiControl::BUTTON, GuiControlProperty::BORDER_WIDTH, 6);

	GuiSetStyle(GuiControl::BUTTON, GuiControlProperty::BASE_COLOR_NORMAL, 0x00000000); // make button bodies transparent
	GuiSetStyle(GuiControl::BUTTON, GuiControlProperty::BORDER_COLOR_NORMAL, 0x00000040); // make borders partially transparent when unselected

	GuiSetStyle(GuiControl::BUTTON, GuiControlProperty::BASE_COLOR_FOCUSED, 0x00000040); // make the button body darken when focused
	GuiSetStyle(GuiControl::BUTTON, GuiControlProperty::BORDER_COLOR_FOCUSED, 0x00000060); // make the button border darken when focused

	GuiSetStyle(GuiControl::BUTTON, GuiControlProperty::BASE_COLOR_PRESSED, 0x00000080); // make buttons get darker when pressed
	GuiSetStyle(GuiControl::BUTTON, GuiControlProperty::BORDER_COLOR_PRESSED, 0x000000AA); // make the border darker when pressed

	GuiSetStyle(GuiControl::BUTTON, GuiControlProperty::BASE_COLOR_DISABLED, 0x00000000); // make the button body transparent when the color is selected
	GuiSetStyle(GuiControl::BUTTON, GuiControlProperty::BORDER_COLOR_DISABLED, 0x000000AA); // darken the border when the color is selected

	GuiSetStyle(GuiControl::BUTTON, GuiControlProperty::TEXT_COLOR_NORMAL, 0x000000FF);
	GuiSetStyle(GuiControl::BUTTON, GuiControlProperty::TEXT_COLOR_FOCUSED, 0x000000FF);
	GuiSetStyle(GuiControl::BUTTON, GuiControlProperty::TEXT_COLOR_PRESSED, 0x000000FF);
	GuiSetStyle(GuiControl::BUTTON, GuiControlProperty::TEXT_COLOR_DISABLED, 0x000000FF);

	GuiSetStyle(GuiControl::DEFAULT, GuiDefaultProperty::TEXT_SIZE, 30);

	int hStep, vStep;
	hStep = size.width / 3;
	vStep = size.height / 5;
	
	int buttonIdx = 1;
	for(int y = 0; y < 5; y++) {
		for(int x = 0; x < 3; x++) {
			Rectangle bounds;
			bounds.x = size.x + hStep * x;
			bounds.y = size.y + vStep * y;
			bounds.width = hStep;
			bounds.height = vStep;

			DrawRectangleRec(bounds, TilesetData::GetColor(buttonIdx));

			std::string idxStr = std::to_string(buttonIdx);
			
			// make the selected color's button disabled to highlight its border
			if(buttonIdx == TilesetData::GetActiveColorIdx()) GuiDisable();
			if(GuiButton(bounds, idxStr.c_str())) TilesetData::SetActiveColor(buttonIdx);
			if(buttonIdx == TilesetData::GetActiveColorIdx()) GuiEnable();
			
			buttonIdx++;
		}
	}
}