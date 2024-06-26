#pragma once

#include <string>

#include "../../raylib/raygui.h"

#include "TileEditor.hpp"

class ToolBar {
public:
	void Update(Rectangle size);

private:

	struct Tool {
		Tool(TileEditor::ToolType type, std::string iconPath);

		TileEditor::ToolType type;
		Texture2D Texture();

	private:
		Texture2D tex;
		std::string texturePath;
		bool textureInitialized = false;
	};
	static std::vector<Tool> tools;

	bool DrawToolButton(Tool tool, int xPos);

	TileEditor::ToolType prevToolType = TileEditor::ToolType::Null; // for hotkeys that switch the current tool while held

	void HardSwitchTool(TileEditor::ToolType toolType);
	void SoftSwitchTool(TileEditor::ToolType toolType);
	void RevertSoftSwitch(TileEditor::ToolType expectedToolType);
};