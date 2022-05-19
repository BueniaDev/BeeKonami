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

// BeeKonami-K051960 (WIP)
// Chip name: K051960
// Chip description: Sprite address generator
//
// BueniaDev's Notes:
// This implementation is derived from both MAME's implementation and Furrtek's reverse-engineered schematics for this chip,
// which can be found at the links below:
//
// MAME's implementation:
// https://github.com/mamedev/mame/blob/master/src/mame/video/k051960.cpp
//
// Furrtek's reverse-engineered schematics:
// https://github.com/furrtek/VGChips/tree/master/Konami/051960
//
// NOTICE: This implementation is a huge WIP, and lots of features are currently unimplemented.
// In addition, please take note that the core API is in heavy flux at this precise moment.
// However, work is being done on all of those fronts, so don't lose hope here!

#include "k051960.h"
using namespace beekonami::video;

namespace beekonami::video
{
    K051960::K051960()
    {

    }

    K051960::~K051960()
    {

    }

    void K051960::init()
    {
	obj_ram.fill(0);
	is_rmrd = false;
    }

    uint8_t K051960::read(uint16_t addr)
    {
	// Initial attempt at K051960 reads
	// TODO: Implement sprite ROM and GFX reads
	uint8_t data = 0;
	addr &= 0x7FF;

	if (addr >= 0x400)
	{
	    // FIXME: Does this specific behavior even match the schematics?
	    if (!is_rmrd)
	    {
		data = obj_ram.at(addr & 0x3FF);
	    }
	    else
	    {
		cout << "Reading from K051960 ROM address of " << hex << int(addr) << endl;
	    }
	}
	else
	{
	    cout << "Reading from K051960 address of " << hex << int(addr) << endl;
	}

	return data;
    }

    void K051960::write(uint16_t addr, uint8_t data)
    {
	addr &= 0x7FF;

	if (addr >= 0x400)
	{
	    obj_ram.at(addr & 0x3FF) = data;
	}
	else
	{
	    switch (addr)
	    {
		case 0:
		{
		    cout << "Writing value of " << hex << int(data) << " to K051960 register of 0" << endl;
		    is_rmrd = testbit(data, 5);
		}
		break;
		default:
		{
		    if (addr <= 7)
		    {
			cout << "Writing value of " << hex << int(data) << " to K051960 register of " << hex << int(addr) << endl;
		    }
		}
		break;
	    }
	}
    }
};