#include "PaletteEditor.hpp"

#include <string>
#include <format>

#include "../TilesetData.hpp"

#include "../util/Logger.hpp"
#include "../util/Input.hpp"

PaletteEditor* PaletteEditor::instance = nullptr;
void PaletteEditor::Initialize() {
	if(instance != nullptr) delete instance;
	instance = new PaletteEditor();
}
PaletteEditor* PaletteEditor::Inst() {
	if(instance == nullptr) Logger::Error("Palette Editor uninitialized");
	return instance;
}

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
			if(buttonIdx == activeColor) GuiDisable();
			if(GuiButton(bounds, idxStr.c_str())) activeColor = buttonIdx;
			if(buttonIdx == activeColor) GuiEnable();
			
			buttonIdx++;
		}
	}

	if(Input::KeybindIsPressed("CopyColor")) {
		Color c = ActiveColor();
			uint32_t colorInt = ColorToInt(c);
			std::string colorHexCode = std::format("{0:8x}", colorInt);
			
			SetClipboardText(colorHexCode.c_str());
			Logger::Debug("Copied " + colorHexCode);
	}
	if(Input::KeybindIsPressed("PasteColor")) {
		std::string colorHexCode = GetClipboardText();

		if (colorHexCode.size() != 8)
		{
			Logger::Debug("Tried to paste \"" + colorHexCode + "\" and failed to parse. Size != 8");
			return;
		}

		uint32_t colorInt = 0;
		int multiplier = 1;
		for (int i = 7; i >= 0; i--)
		{
			char c = colorHexCode[i];

			// numeral [0, 9]
			if (c >= 48 && c <= 57)
			{
				colorInt += (c - 48) * multiplier;
			}
			// capital letter [A, F]
			else if (c >= 65 && c <= 70)
			{
				colorInt += (c - 65 + 10) * multiplier;
			}
			// lowercase letter [a, f]
			else if (c >= 97 && c <= 102)
			{
				colorInt += (c - 97 + 10) * multiplier;
			}
			else
			{
				Logger::Debug("Failed to parse character \"" + std::string{c} + "\" in hex code \"" + colorHexCode + "\"");
				return;
			}

			multiplier *= 16;
		}

		TilesetData::SetColor(activeColor, GetColor(colorInt));
		Logger::Debug("Pasted " + colorHexCode + " (" + std::format("{0:8x}", colorInt) + ")");
	}
}

void PaletteEditor::SetActiveColorIdx(ColorIdx idx) {
	Inst()->activeColor = idx;
}
ColorIdx PaletteEditor::ActiveColorIdx() {
	return Inst()->activeColor;
}
Color PaletteEditor::ActiveColor() {
	return TilesetData::GetColor(Inst()->activeColor);
}