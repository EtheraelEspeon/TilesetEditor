#include "TilesetData.hpp"

void Tile::SetPixel(int x, int y, uint8_t paletteIdx) { idxData[y * 16 + x] = paletteIdx; }
uint8_t Tile::GetPixel(int x, int y) { return idxData[y * 16 + x]; }

namespace data {
	std::array<Color, 15> palette = {
	GetColor(0xffd19dff),
	GetColor(0xaeb5bdff),
	GetColor(0x4d80c9ff),
	GetColor(0x054494ff),
	GetColor(0x511e43ff),
	GetColor(0x100820ff),
	GetColor(0x823e2cff),
	GetColor(0xe93841ff),
	GetColor(0xf1892dff),
	GetColor(0xffe947ff),
	GetColor(0xffa9a9ff),
	GetColor(0xeb6c82ff),
	GetColor(0x7d3ebfff),
	GetColor(0x1e8a4cff),
	GetColor(0x5ae150ff)
	};
}