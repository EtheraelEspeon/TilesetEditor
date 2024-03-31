#pragma once

#include <cstdint>

#include "../raylib/raygui.h"

class PaletteEditor {
public:
	void Update(Rectangle size);

private:
	uint8_t selectedPaletteIdx = 1; // ranges 1-15
};