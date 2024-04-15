#include "ToolBar.hpp"

#include <filesystem>

#include "Logger.hpp"
#include "GuiSizeInfo.hpp"
#include "Background.hpp"

std::vector<ToolBar::Tool> ToolBar::tools = {
	Tool(TileEditor::ToolType::Brush, "res/toolbar/brush.png"),
	Tool(TileEditor::ToolType::Line, "res/toolbar/line.png"),
	Tool(TileEditor::ToolType::Fill, "res/toolbar/fill.png")
};
ToolBar::Tool::Tool(TileEditor::ToolType type, std::string iconPath) {
	this->type = type;
	texturePath = iconPath;
	
}
Texture2D ToolBar::Tool::Texture() {
	if(!textureInitialized) {
		bool textureExists = std::filesystem::exists(texturePath);
		if(textureExists) {
			Logger::Message("Loaded texture \"" + texturePath + "\" successfully!");
			tex = LoadTexture(texturePath.c_str());
		}
		else {
			Logger::Warning("Failed to find texture\"" + texturePath + "\"");
			tex = LoadTextureFromImage(GenImageColor(16, 16, {255, 0, 255, 255}));
		}
		
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
			TileEditor::SetTool(tools[i].type);
		}
		if(thisToolIsSelected) GuiEnable();
		
		// Draw the icon on top of the darkened button
		DrawTexturePro(tools[i].Texture(), {0, 0, 16, 16}, bounds, {0, 0}, 0, {255, 255, 255, 255});
	}
}