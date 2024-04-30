#pragma once

#include "../../raylib/raygui.h"

class TileSelector {
	static constexpr float Timestep = 1.0/60.0;
	static constexpr float ScrollSpeed = 8;
public:
	void Update(Rectangle size);
private:
	float scrollTarget = 0;
	float targetViewportPos = 0; // starts at 0, scrolling down increases the value, making the list entries move up
	float viewportPos = 0;
 
	void UpdateScroll(Rectangle region, float unitHeight);
};