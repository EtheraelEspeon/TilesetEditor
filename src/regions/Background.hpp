#pragma once

#include "../../raylib/raygui.h"

namespace Background {

	extern const Color leftBorder, leftBody;
	extern const Color centerBorder, centerBody;
	extern const Color rightBorder, rightBody;

	void DrawLeftRegion(Rectangle size);
	void DrawCenterRegion(Rectangle size);
	void DrawRightRegion(Rectangle size);
}