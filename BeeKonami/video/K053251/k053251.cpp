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

// BeeKonami-K053251 (WIP)
// Chip name: K053251
// Chip description: Video priority encoder
//
// BueniaDev's Notes:
// This implementation is derived from both MAME's implementation and Furrtek's reverse-engineered schematics for this chip,
// which can be found at the links below:
//
// MAME's implementation:
// https://github.com/mamedev/mame/blob/master/src/mame/video/k053251.cpp
//
// Furrtek's schematics:
// https://github.com/furrtek/VGChips/tree/master/Konami/053251
//
// NOTICE: This implementation is a huge WIP, and lots of features are currently unimplemented.
// In addition, please take note that the core API is in heavy flux at this precise moment.
// However, work is being done on all of those fronts, so don't lose hope here!

#include "k053251.h"
using namespace beekonami::video;

namespace beekonami::video
{
    K053251::K053251()
    {

    }

    K053251::~K053251()
    {

    }

    void K053251::init()
    {
	layer_priorities.fill(0x3F);
	priority_inputs.fill(0x3F);
	is_priority_enabled.fill(false);
	palette_index.fill(0);
    }

    void K053251::write(int reg, uint8_t data)
    {
	reg &= 0xF;
	data &= 0x3F;

	switch (reg)
	{
	    case 0x0:
	    case 0x1:
	    case 0x2:
	    case 0x3:
	    case 0x4:
	    {
		layer_priorities[reg] = data;
	    }
	    break;
	    case 0x9:
	    {
		palette_index[0] = (data & 0x3);
		palette_index[1] = ((data >> 2) & 0x3);
		palette_index[2] = ((data >> 4) & 0x3);
	    }
	    break;
	    case 0xA:
	    {
		palette_index[3] = (data & 0x7);
		palette_index[4] = ((data >> 3) & 0x7);
	    }
	    break;
	    case 0xC:
	    {
		is_priority_enabled[0] = testbit(data, 0);
		is_priority_enabled[1] = testbit(data, 1);
		is_priority_enabled[2] = testbit(data, 2);
	    }
	    break;
	    default:
	    {
		cout << "Writing value of " << hex << int(data) << " to K053251 register " << dec << int(reg) << endl;
	    }
	    break;
	}
    }

    void K053251::set_priorities(int layer, uint8_t data)
    {
	if (!inRangeEx(layer, 0, 2))
	{
	    stringstream ss;
	    ss << "Invalid layer of " << dec << layer;
	    throw out_of_range(ss.str());
	}

	priority_inputs.at(layer) = (data & 0x3F);
    }
}