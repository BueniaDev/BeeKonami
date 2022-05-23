/*
    This file is part of the BeeKonami engine.
    Copyright (C) 2022 BueniaDev.

    BeeKonami is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    BeeKonami is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with BeeKonami.  If not, see <https://www.gnu.org/licenses/>.
*/

// BeeKonami-K051962
// Chip name: K051962
// Chip description: Plane data processor
//
// BueniaDev's Notes:
// This implementation is inspired by MAME's implementation and derived from Furrtek's reverse-engineered schematics for this chip,
// both of which can be found at the links below:
//
// MAME's implementation:
// https://github.com/mamedev/mame/blob/master/src/mame/video/k052109.cpp
//
// Furrtek's reverse-engineered schematics:
// https://github.com/furrtek/VGChips/tree/master/Konami/051962
//
// The following features are currently unimplemented:
// Screen flipping

#include "k051962.h"
using namespace beekonami::video;

namespace beekonami::video
{
    K051962::K051962()
    {

    }

    K051962::~K051962()
    {

    }

    uint8_t K051962::fetch_tile_rom(size_t addr)
    {
	return (addr < gfx_rom.size()) ? gfx_rom.at(addr) : 0x00;
    }

    int K051962::decode_tile(int tile_number, int ypos, int xpos)
    {
	int tile_offs = ((tile_number * 32) + (ypos * 4));

	int value = 0;

	for (int pixel = 3; pixel >= 0; pixel--)
	{
	    uint8_t tile_value = fetch_tile_rom(tile_offs + pixel);
	    value |= (testbit(tile_value, (7 - xpos)) << pixel);
	}

	return value;
    }

    void K051962::init()
    {
	return;
    }

    void K051962::write(uint8_t data)
    {
	cout << "Writing value of " << hex << int(data) << " to K059162 register" << endl;
	is_flip_screen = testbit(data, 0);
	is_flipx_enable = testbit(data, 1);
    }

    void K051962::set_gfx_rom(vector<uint8_t> tile_rom)
    {
	gfx_rom = vector<uint8_t>(tile_rom.begin(), tile_rom.end());
    }

    void K051962::set_tile_callback(tilefunc cb)
    {
	tile_callback = cb;
    }

    tilebuffer K051962::render(int layer, gfxaddr tile_addr)
    {
	if ((layer < 0) || (layer >= 3))
	{
	    throw out_of_range("Invalid layer number");
	}

	tilebuffer tilemap;
	tilemap.fill(0);

	for (uint32_t index = 0; index < tile_addr.size(); index++)
	{
	    uint32_t tilemap_addr = tile_addr[index];

	    uint8_t tile_code = ((tilemap_addr >> 3) & 0xFF);
	    uint8_t color_attrib = ((tilemap_addr >> 11) & 0xFF);
	    int cab_pins = ((tilemap_addr >> 19) & 0x3);
	    int pixelx = ((tilemap_addr >> 21) & 0x7);
	    int pixely = (tilemap_addr & 0x7);

	    bool is_flipx = (is_flipx_enable && testbit(color_attrib, 0));

	    uint32_t tile_addr = 0;

	    if (tile_callback)
	    {
		tile_addr = tile_callback(tile_code, color_attrib, cab_pins);
	    }

	    uint32_t tile_index = (index / 64);
	    int pixel_index = (index % 64);

	    int ycoord = (tile_index / 64);
	    int xcoord = (tile_index % 64);

	    int py = (pixel_index / 8);
	    int px = (pixel_index % 8);

	    if (is_flipx)
	    {
		pixelx = (7 - pixelx);
	    }

	    int pixel_color = decode_tile(tile_addr, pixely, pixelx);

	    int ypos = ((ycoord * 8) + py);
	    int xpos = ((xcoord * 8) + px);

	    uint32_t pixel_offs = (xpos + (ypos * 512));

	    int pixel_addr = ((pixel_color & 0xF) | (color_attrib & 0xF0));

	    if (layer != 0)
	    {
		pixel_addr |= ((color_attrib & 0xF) << 8);
	    }

	    tilemap.at(pixel_offs) = pixel_addr;
	}

	return tilemap;
    }
}