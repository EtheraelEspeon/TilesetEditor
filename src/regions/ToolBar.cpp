#include "ToolBar.hpp"

#include <filesystem>

#include "GuiSizeInfo.hpp"
#include "Background.hpp"

#include "../util/AssetLoader.hpp"
#include "../util/Logger.hpp"
#include "../util/Input.hpp"

std::vector<ToolBar::Tool> ToolBar::tools = {
	Tool(TileEditor::ToolType::Eyedropper, "res/toolbar/eyedropper.png"),
	Tool(TileEditor::ToolType::Line, "res/toolbar/line.png"),
	Tool(TileEditor::ToolType::Fill, "res/toolbar/fill.png"),
	Tool(TileEditor::ToolType::Brush, "res/toolbar/brush.png")
};
ToolBar::Tool::Tool(TileEditor::ToolType type, std::string iconPath) {
	this->type = type;
	texturePath = iconPath;
}
Texture2D ToolBar::Tool::Texture() {
	if(!textureInitialized) {
		tex = AssetLoader::LoadRaylibTexture(texturePath);
		textureInitialized = true;
	}

	return tex;
}

void ToolBar::Update(Rectangle size) {

	GuiSetStyle(GuiControl::BUTTON, GuiControlProperty::BORDER_WIDTH, 0);

	GuiSetStyle(GuiControl::BUTTON, GuiControlProperty::BASE_COLOR_NORMAL, 0x00000000); // make button bodies transparent
	GuiSetStyle(GuiControl::BUTTON, GuiControlProperty::BASE_COLOR_FOCUSED, 0x00000040); // make the button body darken when focused
	GuiSetStyle(GuiControl::BUTTON, GuiControlProperty::BASE_COLOR_PRESSED, 0x00000080); // make buttons get darker when pressed
	GuiSetStyle(GuiControl::BUTTON, GuiControlProperty::BASE_COLOR_DISABLED, 0x00000040); // make the button body transparent when the color is selected

	/* ---- check for hotkeys to switch tools ---- */ 

	// permanent switches
	if(Input::KeybindIsPressed("SetTool_Brush")) HardSwitchTool(TileEditor::ToolType::Brush);
	if(Input::KeybindIsPressed("SetTool_Fill"))  HardSwitchTool(TileEditor::ToolType::Fill);
	// soft switches
	if(Input::KeybindIsPressed("SwapTool_Line"))       SoftSwitchTool(TileEditor::ToolType::Line);
	if(Input::KeybindIsPressed("SwapTool_Eyedropper")) SoftSwitchTool(TileEditor::ToolType::Eyedropper);

	bool shouldRevertSoftSwitch = Input::KeybindIsReleased("SwapTool_Line") || Input::KeybindIsReleased("SwapTool_Eyedropper");
	if(shouldRevertSoftSwitch) RevertSoftSwitch();

	/* ---- draw the buttons and poll input ---- */

	int totalWidthOfAllButtons = GuiSizeInfo::ToolButtonSize * tools.size();
	int totalSpaceBetweenButtons = (size.width - totalWidthOfAllButtons) / (tools.size() - 1);
	
	for(int i = 0; i < tools.size(); i++) {
		
		Rectangle bounds;
		bounds.x = size.x + (totalSpaceBetweenButtons + GuiSizeInfo::ToolButtonSize) * i;
		bounds.y = size.y;
		bounds.height = GuiSizeInfo::ToolButtonSize;
		bounds.width = bounds.height;

		DrawRectangleRec(bounds, Background::leftBorder);
		
		bool thisToolIsSelected = TileEditor::CurrentTool() == tools[i].type;

		if(thisToolIsSelected) GuiDisable();
		if(GuiButton(bounds, "")) {
			HardSwitchTool(tools[i].type);
		}
		if(thisToolIsSelected) GuiEnable();
		
		// Draw the icon on top of the darkened button
		DrawTexturePro(tools[i].Texture(), {0, 0, 16, 16}, bounds, {0, 0}, 0, {255, 255, 255, 255});
	}
}

void ToolBar::HardSwitchTool(TileEditor::ToolType toolType) {
	prevToolType = TileEditor::ToolType::Null;
	TileEditor::SetTool(toolType);
}
void ToolBar::SoftSwitchTool(TileEditor::ToolType toolType) {
	prevToolType = TileEditor::CurrentTool();
	TileEditor::SetTool(toolType);
}
void ToolBar::RevertSoftSwitch() {
	TileEditor::SetTool(prevToolType);
	prevToolType = TileEditor::ToolType::Null;
}