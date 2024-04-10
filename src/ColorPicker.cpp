#include "ColorPicker.hpp"

#include "TilesetData.hpp"

void ColorPicker::Update(Rectangle size) {
	ColorIdx activeColor = 	TilesetData::GetActiveColorIdx();
	Color pickedColor = TilesetData::GetColor(activeColor);

	int barWidth = GuiGetStyle(GuiControl::COLORPICKER, GuiColorPickerProperty::HUEBAR_PADDING) +
	               GuiGetStyle(GuiControl::COLORPICKER, GuiColorPickerProperty::HUEBAR_WIDTH);
	size.width -= barWidth;

	GuiColorPicker(size, "Color Picker", &pickedColor);

	TilesetData::SetColor(activeColor, pickedColor);
}