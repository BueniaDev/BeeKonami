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

// BeeKonami-K053251
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
// The following features are currently unimplemented:
// BRIT/NCOL output
// Shadow selection

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
	layer_inputs.fill(0);
	priority_inputs.fill(0x3F);
	is_priority_enabled.fill(false);
	palette_index.fill(0);
	calc_priority();
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
		calc_priority();
	    }
	    break;
	    case 0x6: shadow_priorities[0] = data; break;
	    case 0x7: shadow_priorities[1] = data; break;
	    case 0x8: shadow_priorities[2] = data; break;
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
	    case 0xB:
	    {
		is_transparent[0] = testbit(data, 0);
		is_transparent[1] = testbit(data, 1);
		is_transparent[2] = testbit(data, 2);
		is_transparent[3] = testbit(data, 3);
		is_transparent[4] = testbit(data, 4);
		is_prior_swap = testbit(data, 5);
	    }
	    break;
	    case 0xC:
	    {
		is_priority_enabled[0] = testbit(data, 0);
		is_priority_enabled[1] = testbit(data, 1);
		is_priority_enabled[2] = testbit(data, 2);
		calc_priority();
	    }
	    break;
	    default:
	    {
		cout << "Writing value of " << hex << int(data) << " to K053251 register " << dec << int(reg) << endl;
	    }
	    break;
	}
    }

    void K053251::set_priority(int layer, uint8_t data)
    {
	if (!inRangeEx(layer, 0, 2))
	{
	    stringstream ss;
	    ss << "Invalid layer of " << dec << layer;
	    throw out_of_range(ss.str());
	}

	priority_inputs.at(layer) = (data & 0x3F);
	calc_priority();
    }

    int K053251::get_priority(K053251Priority index)
    {
	int priority = 0x3F;
	switch (index)
	{
	    case CI0:
	    case CI1:
	    case CI2:
	    {
		if (is_priority_enabled.at(index))
		{
		    priority = layer_priorities.at(index);
		}
		else
		{
		    priority = priority_inputs.at(index);
		}
	    }
	    break;
	    case CI3:
	    case CI4:
	    {
		priority = layer_priorities.at(index);
	    }
	    break;
	}

	return priority;
    }

    void K053251::set_input(int layer, uint16_t data)
    {
	if (!inRangeEx(layer, 0, 4))
	{
	    stringstream ss;
	    ss << "Invalid layer of " << dec << layer;
	    throw out_of_range(ss.str());
	}

	layer_inputs.at(layer) = data;
    }

    uint16_t K053251::get_output(K053251Priority layer)
    {
	return layer_inputs.at(layer);
    }

    int K053251::get_palette_index(K053251Priority layer)
    {
	int pal_index = 0;
	switch (layer)
	{
	    case CI0:
	    case CI1:
	    case CI2:
	    {
		pal_index = (32 * palette_index.at(layer));
	    }
	    break;
	    case CI3:
	    case CI4:
	    {
		pal_index = (16 * palette_index.at(layer));
	    }
	    break;
	    default: break;
	}

	return pal_index;
    }

    K053251Priority K053251::get_top_layer()
    {
	K053251Priority layer;

	for (int i = 4; i >= 0; i--)
	{
	    layer = layer_order.at(i);
	    uint8_t layer_input = layer_inputs.at(layer);

	    if (!is_transparent.at(layer))
	    {
		layer_input &= 0xF;
	    }

	    if (layer_input != 0)
	    {
		break;
	    }
	}

	return layer;
    }

    void K053251::set_shadow(int data)
    {
	shadow_reg = (data & 0x3);
    }

    int K053251::get_shadow()
    {
	int priority = get_priority(get_top_layer());
	int shadow_priority = get_shadow_priority();

	if (priority < shadow_priority)
	{
	    return shadow_reg;
	}

	return 0;
    }

    int K053251::get_shadow_priority()
    {
	int data = 0x3F;

	switch (shadow_reg)
	{
	    case 0: data = 0x3F; break;
	    case 1: data = shadow_priorities[0]; break;
	    case 2: data = shadow_priorities[1]; break;
	    case 3: data = shadow_priorities[2]; break;
	    default: data = 0x3F; break;
	}

	return data;
    }

    void K053251::calc_priority()
    {
	int prior0 = get_priority(K053251Priority::CI0);
	int prior1 = get_priority(K053251Priority::CI1);
	int prior2 = get_priority(K053251Priority::CI2);
	int prior3 = get_priority(K053251Priority::CI3);
	int prior4 = get_priority(K053251Priority::CI4);

	layer_order = {CI0, CI1, CI2, CI3, CI4};
	array<int, 5> priorities = {prior0, prior1, prior2, prior3, prior4};

	// TODO: Verify that this logic matches the schematics
	if ((prior1 < prior0) && is_prior_swap)
	{
	    swap(priorities.at(0), priorities.at(1));
	    swap(layer_order.at(0), layer_order.at(1));
	}

	for (int i = 0; i < 5; i++)
	{
	    for (int j = (i + 1); j < 5; j++)
	    {
		auto &layer0 = layer_order.at(i);
		auto &layer1 = layer_order.at(j);

		int &priora = priorities.at(i);
		int &priorb = priorities.at(j);

		if (priora <= priorb)
		{
		    swap(priora, priorb);
		    swap(layer0, layer1);
		}
	    }
	}
    }
}