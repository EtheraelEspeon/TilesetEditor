#include "PaletteEditor.hpp"

#include <string>

#include "TilesetData.hpp"

void PaletteEditor::Update(Rectangle size){
	int hStep, vStep;
	hStep = size.width / 3;
	vStep = size.height / 5;
	
	int idx = 1;
	for(int y = 0; y < 5; y++) {
		for(int x = 0; x < 3; x++) {
			Rectangle bounds;
			bounds.x = size.x + hStep * x;
			bounds.y = size.y + vStep * y;
			bounds.width = hStep;
			bounds.height = vStep;

			DrawRectangleRec(bounds, data::palette[idx - 1]);

			std::string idxStr = std::to_string(idx);
			if(GuiButton(bounds, idxStr.c_str())) selectedPaletteIdx = idx;
			
			idx++;
		}
	}
}