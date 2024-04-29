#pragma once

#include "../../raylib/raygui.h"

#include "../TilesetData.hpp"

class ColorPicker {
public:
	void Update(Rectangle size);

private:
	Color colorOnPress;
	ColorIdx colorIdxOnPress;
};