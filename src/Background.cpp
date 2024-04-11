#include "Background.hpp"

#include "GuiSizeInfo.hpp"

namespace Background {

	const Color leftBorder = ColorFromHSV(0, 0, 0.8);
	const Color leftBody = ColorFromHSV(0, 0, 0.65);
	const Color centerBorder = ColorFromHSV(0, 0, 0);
	const Color centerBody = ColorFromHSV(0, 0, 0);
	const Color rightBorder = ColorFromHSV(0, 0, 0.8);
	const Color rightBody = ColorFromHSV(0, 0, 0.65);

	void DrawLeftRegion(Rectangle size) {
		int halfPadding = GuiSizeInfo::LeftRegionPadding / 2;
		DrawRectangleRec(size, leftBorder);
		DrawRectangle(size.x + halfPadding, size.y + halfPadding,
		              size.width - 2 * halfPadding, size.height - 2 * halfPadding,
					  leftBody);
	}
	void DrawCenterRegion(Rectangle size) {
		int halfPadding = GuiSizeInfo::CenterRegionPadding / 2;
		DrawRectangleRec(size, centerBorder);
		DrawRectangle(size.x + halfPadding, size.y + halfPadding,
		              size.width - 2 * halfPadding, size.height - 2 * halfPadding,
					  centerBody);
	}
	void DrawRightRegion(Rectangle size) {
		int halfPadding = GuiSizeInfo::RightRegionPadding / 2;
		DrawRectangleRec(size, rightBorder);
		DrawRectangle(size.x + halfPadding, size.y + halfPadding,
		              size.width - 2 * halfPadding, size.height - 2 * halfPadding,
					  rightBody);
	}
}