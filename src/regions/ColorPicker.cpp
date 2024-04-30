#include "ColorPicker.hpp"

#include <format>

#include "../util/Logger.hpp"
#include "../util/Input.hpp"

void ColorPicker::Update(Rectangle size) {
	
	ColorIdx activeColor = 	TilesetData::GetActiveColorIdx();
	Color pickedColor = TilesetData::GetColor(activeColor);
	
	if(Input::PrimaryInteractionPressed()) {
		colorIdxOnPress = activeColor;
		colorOnPress = pickedColor;
	}

	int barWidth = GuiGetStyle(GuiControl::COLORPICKER, GuiColorPickerProperty::HUEBAR_PADDING) +
	               GuiGetStyle(GuiControl::COLORPICKER, GuiColorPickerProperty::HUEBAR_WIDTH);
	size.width -= barWidth;

	GuiColorPicker(size, "Color Picker", &pickedColor);

	TilesetData::SetColor(activeColor, pickedColor);

	if(Input::PrimaryInteractionReleased() && colorIdxOnPress == activeColor) {
		
		uint32_t startColorInt = ColorToInt(colorOnPress);
		uint32_t pickedColorInt = ColorToInt(pickedColor);
		if(startColorInt != pickedColorInt) {
			//Logger::Debug(std::format("Color picker pushed an undo action. (Reverted {0:8x} to {1:8x})", pickedColorInt, startColorInt));
			ChangeQueue::ApplyChange(
				[idx=colorIdxOnPress, color=pickedColor ](Tile* unused){ TilesetData::SetColor(idx, color); },
				[idx=colorIdxOnPress, color=colorOnPress](Tile* unused){ TilesetData::SetColor(idx, color); },
				nullptr
			);
		}
	}
}