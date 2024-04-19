#include "ColorPicker.hpp"

#include <format>

#include "Logger.hpp"

void ColorPicker::Update(Rectangle size) {
	
	ColorIdx activeColor = 	TilesetData::GetActiveColorIdx();
	Color pickedColor = TilesetData::GetColor(activeColor);
	
	if(IsMouseButtonPressed(0)) {
		colorIdxOnPress = activeColor;
		colorOnPress = pickedColor;
	}

	int barWidth = GuiGetStyle(GuiControl::COLORPICKER, GuiColorPickerProperty::HUEBAR_PADDING) +
	               GuiGetStyle(GuiControl::COLORPICKER, GuiColorPickerProperty::HUEBAR_WIDTH);
	size.width -= barWidth;

	GuiColorPicker(size, "Color Picker", &pickedColor);

	TilesetData::SetColor(activeColor, pickedColor);

	if(IsMouseButtonReleased(0) && colorIdxOnPress == activeColor) {
		
		uint32_t startColorInt = ColorToInt(colorOnPress);
		uint32_t pickedColorInt = ColorToInt(pickedColor);
		if(startColorInt != pickedColorInt) {
			Logger::Debug(std::format("Color picker pushed an undo action. (Reverted {0:8x} to {1:8x})", pickedColorInt, startColorInt));
			UndoQueue::PushResetter([oldColor=colorOnPress, oldColorIdx=activeColor](Tile* unused){
				TilesetData::SetColor(oldColorIdx, oldColor);
			}, nullptr);
		}
	}
}