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

// BeeKonami-K054000
// Chip name: K054000
// Chip description: Collision detection / protection device
//
// BueniaDev's Notes:
// This implementation is inspired by MAME's implementation and derived from Furrtek's reverse-engineered schematics for this chip,
// both of which can be found at the links below:
//
// MAME's implementation:
// https://github.com/mamedev/mame/blob/master/src/mame/konami/k054000.cpp
//
// Furrtek's schematics:
// https://github.com/furrtek/VGChips/tree/master/Konami/054000

#include "k054000.h"
using namespace beekonami::machine;

namespace beekonami::machine
{
    K054000::K054000()
    {

    }

    K054000::~K054000()
    {

    }

    void K054000::init()
    {
	acx_raw.fill(0);
	acy_raw.fill(0);
	bcx_raw.fill(0);
	bcy_raw.fill(0);

	acx_reg = 0;
	acy_reg = 0;
	bcx_reg = 0;
	bcy_reg = 0;

	aax_reg = 1;
	aay_reg = 1;
	bax_reg = 1;
	bay_reg = 1;
    }

    int32_t K054000::convert_result(array<uint8_t, 4> buffer)
    {
	int32_t res = ((buffer[0] << 16) | (buffer[1] << 8) | buffer[2]);
	return res;
    }

    int32_t K054000::convert_result_delta(array<uint8_t, 4> buffer)
    {
	int32_t res = ((buffer[0] << 16) | (buffer[1] << 8) | buffer[2]);

	if (testbit(buffer[3], 7))
	{
	    res -= (0x100 - buffer[3]);
	}
	else
	{
	    res += buffer[3];
	}

	return res;
    }

    bool K054000::axis_check(uint32_t ac, uint32_t bc, uint32_t aa, uint32_t ba)
    {
	bool res = false;
	int32_t sub = (ac - bc);

	if ((sub > 511) || (sub <= -1024))
	{
	    res = true;
	}

	if ((abs(sub) & 0x1FF) > ((aa + ba) & 0x1FF))
	{
	    res = true;
	}

	return res;
    }

    uint8_t K054000::readReg(int reg)
    {
	uint8_t data = 0;
	reg &= 0x1F;

	if (reg == 0x18)
	{
	    bool axis_x = axis_check(acx_reg, bcx_reg, aax_reg, bax_reg);
	    bool axis_y = axis_check(acy_reg, bcy_reg, aay_reg, bay_reg);
	    data = (axis_x || axis_y) ? 0x01 : 0x00;
	}

	return data;
    }

    void K054000::writeReg(int reg, uint8_t data)
    {
	reg &= 0x1F;
	switch (reg)
	{
	    case 0x01:
	    case 0x02:
	    case 0x03:
	    case 0x04:
	    {
		acx_raw.at(reg - 0x01) = data;
		acx_reg = convert_result_delta(acx_raw);
	    }
	    break;
	    case 0x06: aax_reg = data; break;
	    case 0x07: aay_reg = data; break;
	    case 0x09:
	    case 0x0A:
	    case 0x0B:
	    case 0x0C:
	    {
		acy_raw.at(reg - 0x09) = data;
		acy_reg = convert_result_delta(acy_raw);
	    }
	    break;
	    case 0x0E: bax_reg = data; break;
	    case 0x0F: bay_reg = data; break;
	    case 0x11:
	    case 0x12:
	    case 0x13:
	    {
		bcy_raw.at(reg - 0x11) = data;
		bcy_reg = convert_result(bcy_raw);
	    }
	    break;
	    case 0x15:
	    case 0x16:
	    case 0x17:
	    {
		bcx_raw.at(reg - 0x15) = data;
		bcx_reg = convert_result(bcx_raw);
	    }
	    break;
	}
    }
};