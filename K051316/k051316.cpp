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

// BeeKonami-K051316
// Chip name: K051316 (aka. PSAC)
// Chip description: ROZ plane generator
// Used in: Various Konami arcade games (such as Ajax, Bottom of the Ninth, Over Drive, Rollergames, and Konami '88/'88 Games)
//
// BueniaDev's Notes:
// This implementation is derived from Furrtek's reverse-engineered schematics for this chip,
// which can be found at the link below:
//
// https://github.com/furrtek/SiliconRE/blob/master/Konami/051316/051316_schematics.pdf
//
// This implementation is pretty much complete, AFAIK.

#include "k051316.h"
using namespace beekonami;

namespace beekonami
{
    K051316::K051316()
    {

    }

    K051316::~K051316()
    {

    }

    void K051316::init()
    {
	current_pins = {};
	test_en = false;
	reg_e_d5 = false;
	reg_e_d4 = false;
	reg_e_d0 = false;
	clk_counter = 0;
	clk12 = false;
	clk6 = false;
    }

    void K051316::tickCLKTest(bool clk)
    {
	if (!prev_clk && clk)
	{
	    clk12 = testbit(clk_counter, 1);
	    clk6 = !testbit(clk_counter, 2);
	}

	if (prev_clk && !clk)
	{
	    clk_counter = ((clk_counter + 1) & 0x7);
	}

	tickCLK(clk, clk12, clk6);
    }

    void K051316::tickCLK(bool clk, bool clk12, bool clk6)
    {
	clk_val = clk;
	current_pins.pin_m12 = clk12;
	current_pins.pin_m6 = clk6;

	if (prev_clk && !clk)
	{
	    clk12_delay = clk12;
	}

	clk_rise = (!prev_clk && clk);

	clk12_rise = (!prev_clk12 && clk12);
	clk12_fall = (prev_clk12 && !clk12);

	clk6_rise = (!prev_clk6 && clk6);
	clk6_fall = (prev_clk6 && !clk6);

	if (clk_rise)
	{
	    tickInternal();
	}

	prev_clk12 = clk12;
	prev_clk6 = clk6;
	prev_clk = clk;
    }

    void K051316::tickInternal()
    {
	tickClocks();
	tickIO();
	tickSync();
	tickRAMLogic();
	tickRAM();
	tickAddrOutput();
	tickXCounter();
	tickYCounter();
    }

    void K051316::tickClocks()
    {
	if (clk12_rise)
	{
	    m6_n = !clk6;
	}

	m6_n_edge = (!prev_m6_n && m6_n);
	prev_m6_n = m6_n;

	ram_clk = !((reg_e_d5 || clk12) && (reg_e_d4 || clk12_delay));

	ram_clk_rise = (!prev_ram_clk && ram_clk);
	ram_clk_fall = (prev_ram_clk && !ram_clk);

	prev_ram_clk = ram_clk;
    }

    void K051316::tickIO()
    {
	is_io_write = (!current_pins.pin_iocs && !current_pins.pin_rnw);

	if (!prev_io_write && is_io_write)
	{
	    uint8_t io_reg = (current_pins.addr & 0xF);
	    uint8_t data = current_pins.data;

	    switch (io_reg)
	    {
		case 0x0:
		{
		    x_start = ((x_start & 0xFF) | (data << 8));
		}
		break;
		case 0x1:
		{
		    x_start = ((x_start & 0xFF00) | data);
		}
		break;
		case 0x2:
		{
		    x_pixel_inc = ((x_pixel_inc & 0xFF) | (data << 8));

		    if (testbit(data, 7))
		    {
			x_pixel_inc |= 0xFF0000;
		    }
		}
		break;
		case 0x3:
		{
		    x_pixel_inc = ((x_pixel_inc & 0xFFFF00) | data);
		}
		break;
		case 0x4:
		{
		    x_line_inc = ((x_line_inc & 0xFF) | (data << 8));

		    if (testbit(data, 7))
		    {
			x_line_inc |= 0xFF0000;
		    }
		}
		break;
		case 0x5:
		{
		    x_line_inc = ((x_line_inc & 0xFFFF00) | data);
		}
		break;
		case 0x6:
		{
		    y_start = ((y_start & 0xFF) | (data << 8));
		}
		break;
		case 0x7:
		{
		    y_start = ((y_start & 0xFF00) | data);
		}
		break;
		case 0x8:
		{
		    y_pixel_inc = ((y_pixel_inc & 0xFF) | (data << 8));

		    if (testbit(data, 7))
		    {
			y_pixel_inc |= 0xFF0000;
		    }
		}
		break;
		case 0x9:
		{
		    y_pixel_inc = ((y_pixel_inc & 0xFFFF00) | data);
		}
		break;
		case 0xA:
		{
		    y_line_inc = ((y_line_inc & 0xFF) | (data << 8));

		    if (testbit(data, 7))
		    {
			y_line_inc |= 0xFF0000;
		    }
		}
		break;
		case 0xB:
		{
		    y_line_inc = ((y_line_inc & 0xFFFF00) | data);
		}
		break;
		case 0xC:
		{
		    rom_read_addr = ((rom_read_addr & 0x1F00) | data);
		}
		break;
		case 0xD:
		{
		    rom_read_addr = ((rom_read_addr & 0xFF) | ((data & 0x1F) << 8));
		}
		break;
		case 0xE:
		{
		    reg_e_d7 = testbit(data, 7);
		    test_en = testbit(data, 6);
		    reg_e_d5 = testbit(data, 5);
		    reg_e_d4 = testbit(data, 4);
		    reg_e_d3 = testbit(data, 3);
		    is_yflip = testbit(data, 2);
		    is_xflip = testbit(data, 1);
		    reg_e_d0 = testbit(data, 0);
		}
		break;
	    }
	}

	prev_io_write = is_io_write;
    }

    void K051316::tickSync()
    {
	if (clk6_fall)
	{
	    is_hrc = !current_pins.pin_hrc;

	    if (!prev_hrc && is_hrc)
	    {
		is_vscn = current_pins.pin_vscn;
	    }

	    prev_hrc = is_hrc;
	}

	accum_latch = ((!(current_pins.pin_vrc || is_vscn) || is_hrc) && (!(is_vscn && current_pins.pin_hscn) || !clk6));
	accum_latch_edge = (!prev_accum_latch && accum_latch);
	prev_accum_latch = accum_latch;

	accum_prev_latch = (is_hrc || (!current_pins.pin_vrc && !is_vscn));
	accum_prev_latch_edge = (!prev_accum_prev_latch && accum_prev_latch);
	prev_accum_prev_latch = accum_prev_latch;

	is_hvblank = (!is_hrc && current_pins.pin_vrc);
	is_hvsync = (!is_hrc && is_vscn);
    }

    void K051316::tickRAMLogic()
    {
	if (m6_n && !reg_e_d7)
	{
	    uint8_t x_addr = ((x_accum >> 15) & 0x1F);
	    uint8_t y_addr = ((y_accum >> 15) & 0x1F);
	    ram_addr = ((y_addr << 5) | x_addr);
	}
	else
	{
	    ram_addr = (current_pins.addr & 0x3FF);
	}

	bool is_ram_write = (!current_pins.pin_rnw && !current_pins.pin_vrcs && !clk6 && ram_clk);

	ram_l_we = (is_ram_write && testbit(current_pins.addr, 10));
	ram_r_we = (is_ram_write && !testbit(current_pins.addr, 10));

	ram_l_we_edge = (!prev_ram_l_we && ram_l_we);
	ram_r_we_edge = (!prev_ram_r_we && ram_r_we);

	prev_ram_l_we = ram_l_we;
	prev_ram_r_we = ram_r_we;
    }

    void K051316::tickRAM()
    {
	if (ram_clk_rise)
	{
	    ram_write_addr = ram_addr;
	    ram_l_dout = ram_left.at(ram_addr);
	    ram_r_dout = ram_right.at(ram_addr);
	}

	if (ram_l_we_edge)
	{
	    ram_left.at(ram_write_addr) = current_pins.data;
	}

	if (ram_r_we_edge)
	{
	    ram_right.at(ram_write_addr) = current_pins.data;
	}
    }

    void K051316::tickAddrOutput()
    {
	if (m6_n_edge)
	{
	    pre_reg = ram_reg;
	}

	if (ram_clk_fall)
	{
	    ram_reg = ((ram_l_dout << 8) | ram_r_dout);
	}

	if (clk6_fall)
	{
	    uint8_t x_oblk = ((x_accum >> 20) & 0xF);
	    uint8_t y_oblk = ((y_accum >> 20) & 0xF);
	    oblk_delay = !oblk_val;
	    oblk_val = ((x_oblk == 0) && (y_oblk == 0));

	    uint8_t xflip = pre_x;
	    uint8_t yflip = pre_y;

	    if (is_xflip && testbit(ram_reg, 14))
	    {
		xflip ^= 0xF;
	    }

	    if (is_yflip && testbit(ram_reg, 15))
	    {
		yflip ^= 0xF;
	    }

	    tile_addr = ((pre_reg << 8) | (yflip << 4) | xflip);
	}

	if (clk6_fall)
	{
	    pre_x = ((x_accum >> 11) & 0xF);
	    pre_y = ((y_accum >> 11) & 0xF);
	}

	if (test_en)
	{
	    current_pins.rom_addr = (reg_e_d0) ? x_accum : y_accum;
	}
	else
	{
	    current_pins.rom_addr = (reg_e_d0) ? tile_addr : ((rom_read_addr << 11) | current_pins.addr);
	}

	current_pins.pin_oblk = (reg_e_d3) ? !ram_clk : oblk_delay;
    }

    void K051316::tickCounters()
    {
	if (accum_latch_edge)
	{
	    x_accum = x_accum_out;
	    y_accum = y_accum_out;
	}

	if (accum_prev_latch_edge)
	{
	    prev_x_accum = x_accum_out;
	    prev_y_accum = y_accum_out;
	}

	if (is_hvblank)
	{
	    x_accum_a = 0;
	    x_accum_b = (x_start << 8);
	    y_accum_a = 0;
	    y_accum_b = (y_start << 8);
	}
	else
	{
	    x_accum_a = (is_hvsync) ? x_line_inc : x_pixel_inc;
	    x_accum_b = (accum_prev_latch) ? x_accum : prev_x_accum;
	    y_accum_a = (is_hvsync) ? y_line_inc : y_pixel_inc;
	    y_accum_b = (accum_prev_latch) ? y_accum : prev_y_accum;
	}

	x_accum_out = ((x_accum_a + x_accum_b) & 0xFFFFFF);
	y_accum_out = ((y_accum_a + y_accum_b) & 0xFFFFFF);
    }

    void K051316::tickXCounter()
    {
	if (accum_latch_edge)
	{
	    x_accum = x_accum_out;
	}

	if (accum_prev_latch_edge)
	{
	    prev_x_accum = x_accum_out;
	}

	if (is_hvblank)
	{
	    x_accum_a = 0;
	    x_accum_b = (x_start << 8);
	}
	else
	{
	    x_accum_a = (is_hvsync) ? x_line_inc : x_pixel_inc;
	    x_accum_b = (accum_prev_latch) ? x_accum : prev_x_accum;
	}

	x_accum_out = ((x_accum_a + x_accum_b) & 0xFFFFFF);
    }

    void K051316::tickYCounter()
    {
	if (accum_latch_edge)
	{
	    y_accum = y_accum_out;
	}

	if (accum_prev_latch_edge)
	{
	    prev_y_accum = y_accum_out;
	}

	if (is_hvblank)
	{
	    y_accum_a = 0;
	    y_accum_b = (y_start << 8);
	}
	else
	{
	    y_accum_a = (is_hvsync) ? y_line_inc : y_pixel_inc;
	    y_accum_b = (accum_prev_latch) ? y_accum : prev_y_accum;
	}

	y_accum_out = ((y_accum_a + y_accum_b) & 0xFFFFFF);
    }
};