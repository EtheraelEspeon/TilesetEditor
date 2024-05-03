#pragma once

#include <cstdint>

#include "../../raylib/raygui.h"

#include "../TilesetData.hpp"

class PaletteEditor {
public:
	static void Initialize();

	void Update(Rectangle size);

	static void SetActiveColorIdx(ColorIdx idx);
	static ColorIdx ActiveColorIdx();
	static Color ActiveColor();

	static PaletteEditor* Inst();
private:
	static PaletteEditor* instance;

	ColorIdx activeColor = 1;
};