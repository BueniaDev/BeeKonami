/*
    This file is part of the BeeKonami engine.
    Copyright (C) 2025 BueniaDev.

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

// BeeKonami-K053260 (WIP)
// Chip name: K053260
// Chip description: 4-channel PCM/KADPCM sound generator
// Used in: Various Konami arcade games (such as The Simpsons, Punkshot, Trigon/Lightning Fighters, Vendetta, Over Drive, and Asterix)
//
// BueniaDev's Notes:
// This implementation is derived from Furrtek's reverse-engineered schematics for this chip,
// which can be found at the link below:
//
// https://github.com/furrtek/SiliconRE/blob/master/Konami/053260/k053260_schematics.PDF
//
// This implementation is still a massive WIP, and lots of features are completely unimplemented.
// As such, expect a bunch of features to be completely missing.
// However, work is being done to implement this chip, so don't lose hope here!

#include "k053260.h"
using namespace beekonami;
using namespace std;

namespace beekonami
{
    K053260::K053260()
    {
    }

    K053260::~K053260()
    {
    }

    void K053260::init()
    {
	current_pins = {};
    }

    void K053260::reset()
    {
	current_pins.pin_res = false;
	tickCLK(true);
	tickCLK(false);
	current_pins.pin_res = true;
    }

    void K053260::tickCLK(bool clk)
    {
	clk_val = clk;
	clk_rise = (!prev_clk && clk);
	clk_fall = (prev_clk && !clk);
	if (prev_clk != clk)
	{
	    tickInternal();
	}

	prev_clk = clk;
    }

    void K053260::tickInternal()
    {
	// TODO: Implement the remainder of this IC
	tickReset();
	tickClocks();
	tickMainWrites();
	tickSubWrites();
	tickYMIn();
	tickMix();
	tickYMOut();
    }

    void K053260::tickReset()
    {
	if (!current_pins.pin_res)
	{
	    res_sync = false;
	    res_sr = 0x7;
	}
	else if (clk_rise)
	{
	    res_sync = testbit(res_sr, 2);
	    res_sr = ((res_sr << 1) & 0x7);
	    res_sr |= !(!prev_stbi && current_pins.pin_stbi);

	    prev_stbi = current_pins.pin_stbi;
	}
    }

    void K053260::tickClocks()
    {
	if (clk_rise)
	{
	    ym_load = !clk32;
	}

	if (!res_sync)
	{
	    nclk2 = false;
	    clk4 = true;
	    clk8 = false;
	    nclk8 = true;
	    clk16 = true;
	    nclk16 = false;
	    clk32 = true;
	    nclk32 = false;
	    nclk64 = true;
	    clk64 = false;
	}
	else if (clk_rise)
	{
	    bool ae104 = !(clk4 && !nclk2);
	    bool ad17 = !(ae104 || nclk8);
	    bool af26 = !(ad17 && nclk16);
	    bool aj19 = !(af26 || nclk32);

	    nclk64 = !(!nclk64 ^ aj19);
	    clk64 = !nclk64;

	    clk32 = !(clk32 ^ af26);
	    nclk32 = !clk32;

	    clk16 = !(!clk16 ^ ad17);
	    nclk16 = !clk16;

	    nclk8 = !(nclk8 ^ ae104);
	    clk8 = !nclk8;

	    clk4 = !(!clk4 ^ !nclk2);
	    nclk2 = !nclk2;
	}

	nclk2_edge = (!prev_nclk2 && nclk2);
	prev_nclk2 = nclk2;

	ym_load_edge = (!prev_ym_load && ym_load);
	prev_ym_load = ym_load;

	current_pins.pin_ne = nclk2;

	current_pins.pin_sy = nclk2;
	current_pins.pin_sh1 = !(nclk64 || nclk32);
	current_pins.pin_sh2 = !(clk64 || nclk32);
    }


    void K053260::tickMainWrites()
    {
	main_data_dir = (!current_pins.pin_main_cs && current_pins.pin_main_rw);

	if (!main_data_dir)
	{
	    main_data_in = current_pins.main_data;
	}

	if (prev_main_cs && !current_pins.pin_main_cs)
	{
	    uint8_t data = main_data_in;
	    if (current_pins.pin_main_rw)
	    {
		if (current_pins.pin_main_a0)
		{
		    main_data_out = sub_to_main_b;
		}
		else
		{
		    main_data_out = sub_to_main_a;
		}
	    }
	    else
	    {
		if (current_pins.pin_main_a0)
		{
		    main_to_sub_b = data;
		}
		else
		{
		    main_to_sub_a = data;
		}
	    }
	}

	prev_main_cs = current_pins.pin_main_cs;

	if (main_data_dir)
	{
	    current_pins.main_data = main_data_out;
	}
    }

    void K053260::tickSubWrites()
    {
	if (clk_rise)
	{
	    is_ab0_flip = next_ab0_flip;
	}

	sub_data_dir = (!current_pins.pin_sub_cs && current_pins.pin_sub_rw);

	if (!sub_data_dir)
	{
	    sub_data_in = current_pins.sub_data;
	}

	if (prev_sub_cs && !current_pins.pin_sub_cs)
	{
	    uint8_t addr = (current_pins.sub_addr & 0x3F);
	    uint8_t data = sub_data_in;

	    if (is_ab0_flip)
	    {
		addr = ((addr & 0x3E) | !testbit(addr, 0));
	    }

	    if (addr < 0x08)
	    {
		uint8_t latch_addr = (addr & 0x3);
		if (current_pins.pin_sub_rw)
		{
		    switch (latch_addr)
		    {
			case 0x00: sub_data_out = main_to_sub_a; break;
			case 0x01: sub_data_out = main_to_sub_b; break;
			default: sub_data_out = 0x00; break;
		    }
		}
		else
		{
		    switch (latch_addr)
		    {
			case 0x02: sub_to_main_a = data; break;
			case 0x03: sub_to_main_b = data; break;
			default: break;
		    }
		}
	    }
	    else if (addr < 0x28)
	    {
		uint8_t ch_offs = (addr - 0x8);
		uint8_t ch_num = ((ch_offs >> 3) & 0x3);
		uint8_t ch_reg = (ch_offs & 0x7);

		cout << "Writing value of " << hex << int(data) << " to K053260 PCM channel #" << dec << int(ch_num) << " register of " << hex << int(ch_reg) << endl;
	    }
	    else if (addr < 0x30)
	    {
		if (current_pins.pin_sub_rw)
		{
		    switch (addr)
		    {
			case 0x29:
			{
			    cout << "Reading from K053260 voice status register" << endl;
			    sub_data_out = 0x00;
			}
			break;
			case 0x2E:
			{
			    cout << "Reading from K053260 sample ROM readback register" << endl;
			    sub_data_out = 0x00;
			}
			break;
			default: sub_data_out = 0x00; break;
		    }
		}
		else
		{
		    switch (addr)
		    {
			case 0x28:
			case 0x2A:
			case 0x2B:
			case 0x2C:
			case 0x2D:
			{
			    cout << "Writing value of " << hex << int(data) << " to K053260 register of 0x" << hex << int(addr) << endl;
			}
			break;
			case 0x2F:
			{
			    cout << "Writing value of " << hex << int(data) << " to K053260 register of 0x2f" << endl;
			    enable_output = testbit(data, 1);
			    enable_aux1 = testbit(data, 2);
			    enable_aux2 = testbit(data, 3);
			    next_ab0_flip = testbit(data, 4);
			}
			break;
			default: break;
		    }
		}
	    }
	}

	prev_sub_cs = current_pins.pin_sub_cs;

	if (sub_data_dir)
	{
	    current_pins.sub_data = sub_data_out;
	}
    }

    void K053260::tickYMIn()
    {
	if (nclk2_edge)
	{
	    aux1_sr = ((aux1_sr >> 1) | (current_pins.pin_aux1 << 13));
	    aux2_sr = ((aux2_sr >> 1) | (current_pins.pin_aux2 << 13));
	}

	if (!current_pins.pin_res)
	{
	    aux1_reg = 0;
	    aux2_reg = 0;
	}
	else if (ym_load_edge)
	{
	    aux1_reg = (aux1_sr & 0x1FFF);
	    aux2_reg = (aux2_sr & 0x1FFF);
	}

	ym_sample = decodeYM();

	if (clk_rise)
	{
	    uint8_t ym_mux_reg = ((nclk16 << 1) | clk32);

	    for (int i = 0; i < 4; i++)
	    {
		bool ym_mux = (nclk64) ? false : (ym_mux_reg == i);
		ym_mux_edge[i] = (!prev_ym_mux[i] && ym_mux);
		prev_ym_mux[i] = ym_mux;
	    }
	}

	if (!enable_aux1)
	{
	    ym_reg[0] = 0;
	    ym_reg[2] = 0;
	}
	else
	{
	    if (ym_mux_edge[0])
	    {
		ym_reg[0] = ym_sample;
	    }

	    if (ym_mux_edge[2])
	    {
		ym_reg[2] = ym_sample;
	    }
	}

	if (!enable_aux2)
	{
	    ym_reg[1] = 0;
	    ym_reg[3] = 0;
	}
	else
	{
	    if (ym_mux_edge[1])
	    {
		ym_reg[1] = ym_sample;
	    }

	    if (ym_mux_edge[3])
	    {
		ym_reg[3] = ym_sample;
	    }
	}

	if (clk4)
	{
	    ym_mux_val = 0;
	}
	else
	{
	    uint8_t ym_mux_index = ((clk32 << 1) | clk8);
	    ym_mux_val = ym_reg[ym_mux_index];
	}
    }

    void K053260::tickMix()
    {
	if (clk_rise)
	{
	    mix_reset = (!clk8 && !nclk64 && !nclk2 && !clk4 && !nclk16);
	}

	uint32_t mix_res = 0;

	if (clk64)
	{
	    mix_res = (nclk16) ? ym_mux_val : 0;

	    if (testbit(mix_res, 15))
	    {
		mix_res |= 0x70000;
	    }
	}

	uint32_t mix_add = ((mix_val + mix_res) & 0x7FFFF);

	if (clk_rise && mix_reset)
	{
	    mix_val = 0;
	}
	else if (clk_fall && !nclk2)
	{
	    mix_val = mix_add;
	}

	uint8_t sat_msb = ((mix_val >> 15) & 0xF);

	int16_t sample_mix = 0;

	if ((sat_msb >= 0x8) && (sat_msb <= 0xE))
	{
	    sample_mix = 0;
	}
	else if ((sat_msb >= 0x1) && (sat_msb <= 0x7))
	{
	    sample_mix = 0x7FFF;
	}
	else
	{
	    sample_mix = (mix_val & 0x7FFF);
	}

	if (clk_rise)
	{
	    sample_set = (nclk64 || nclk16);
	}

	if (prev_sample_set && !sample_set)
	{
	    uint16_t mask = testbit(mix_val, 18) ? 0xFFFF : 0;
	    final_sample = (sample_mix ^ mask);
	}

	prev_sample_set = sample_set;
    }

    uint16_t K053260::decodeYM()
    {
	uint16_t ym_val = (clk32) ? aux2_reg : aux1_reg;
	int exp = ((ym_val >> 10) & 0x7);
	bool sign = testbit(ym_val, 9);
	uint16_t mant = (ym_val & 0x1FF);
	uint16_t mask = 0;

	if (!sign)
	{
	    mask = 0xFFFF;
	}

	if (exp == 0)
	{
	    return mask;
	}

	mant ^= (mask & 0x1FF);
	return (((mant << exp) >> 1) ^ mask);
    }

    void K053260::tickYMOut()
    {
	if (clk_rise)
	{
	    ym_out_load = (clk32 && clk4 && clk8 && nclk16);
	}

	ym_out_tick = (clk_rise || nclk2);

	if (!prev_ym_out_tick && ym_out_tick)
	{
	    if (ym_out_counter < 3)
	    {
		out_so = testbit(ym_out_sample, 0);
	    }
	    else
	    {
		out_so = testbit(ym_out_sample, (ym_out_counter - 3));
	    }

	    if (ym_out_load)
	    {
		ym_out_counter = 0;
		ym_out_sample = encodeYM();
	    }
	    else
	    {
		ym_out_counter += 1;
	    }
	}

	prev_ym_out_tick = ym_out_tick;

	current_pins.pin_so = out_so;
    }

    uint16_t K053260::encodeYM()
    {
	if (!enable_output)
	{
	    return 0x600;
	}

	uint8_t sample_msb = ((final_sample >> 9) & 0x3F);

	bool sign = testbit(final_sample, 15);
	int exponent = 1;

	if (testbit(sample_msb, 5))
	{
	    exponent = 7;
	}
	else if (testbit(sample_msb, 4))
	{
	    exponent = 6;
	}
	else if (testbit(sample_msb, 3))
	{
	    exponent = 5;
	}
	else if (testbit(sample_msb, 2))
	{
	    exponent = 4;
	}
	else if (testbit(sample_msb, 1))
	{
	    exponent = 3;
	}
	else if (testbit(sample_msb, 0))
	{
	    exponent = 2;
	}
	else
	{
	    exponent = 1;
	}

	uint16_t mantissa = ((final_sample >> (exponent - 1)) & 0x1FF);

	if (sign)
	{
	    mantissa ^= 0x1FF;
	}

	return ((exponent << 10) | (!sign << 9) | mantissa);
    }
};