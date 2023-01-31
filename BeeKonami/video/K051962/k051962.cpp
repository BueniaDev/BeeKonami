/*
    This file is part of the BeeKonami engine.
    Copyright (C) 2023 BueniaDev.

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

// BeeKonami-K051962 (WIP)
// Chip name: K051962
// Chip description: Plane data processor
//
// BueniaDev's Notes:
// This implementation is derived from both MAME's implementation and Furrtek's reverse-engineered schematics for this chip,
// which can be found at the links below:
//
// MAME's implementation:
// https://github.com/mamedev/mame/blob/master/src/mame/konami/k052109.cpp
//
// Furrtek's schematics:
// https://github.com/furrtek/VGChips/tree/master/Konami/051962
//
// NOTICE: This implementation is a huge WIP, and lots of features are currently unimplemented.
// In addition, please take note that the core API is in heavy flux at this precise moment.
// However, work is being done on all of those fronts, so don't lose hope here!

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

    void K051962::reset()
    {
	return;
    }

    void K051962::setTileAddr(k052109gfx &addr)
    {
	tile_addr = addr;
    }

    void K051962::render()
    {
	for (int layer = 0; layer < 3; layer++)
	{
	    for (int xpos = 0; xpos < 512; xpos++)
	    {
		for (int ypos = 0; ypos < 256; ypos++)
		{
		    auto &addr = tile_addr.at(layer).at(xpos, ypos);

		    int py = addr.line;
		    int px = addr.pixelx;

		    uint32_t tile_addr = makeAddr(addr);
		    uint8_t color_attrib = makeColor(addr.color);

		    int tile_offs = ((tile_addr * 32) + (py * 4));

		    int pixel_color = 0;

		    for (int pixel = 3; pixel >= 0; pixel--)
		    {
			uint8_t tile_value = readTileROM(tile_offs + pixel);
			pixel_color |= (((tile_value >> (7 - px)) & 0x1) << pixel);
		    }

		    int pixel_addr = (pixel_color | (color_attrib & 0xF0));

		    if (layer != 0)
		    {
			pixel_addr |= ((color_attrib & 0xF) << 8);
		    }

		    gfx_addr.at(layer).at(xpos, ypos) = pixel_addr;
		}
	    }
	}
    }
};