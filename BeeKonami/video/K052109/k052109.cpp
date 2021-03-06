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

// BeeKonami-K052109
// Chip name: K052109
// Chip description: Plane address generator
//
// BueniaDev's Notes:
// This implementation is inspired by MAME's implementation and derived from Furrtek's reverse-engineered schematics for this chip,
// both of which can be found at the links below:
//
// MAME's implementation:
// https://github.com/mamedev/mame/blob/master/src/mame/video/k052109.cpp
//
// Furrtek's schematics:
// https://github.com/furrtek/VGChips/tree/master/Konami/052109
//
// The following features still need to be improved:
// Screen flipping
// 
// The following features are currently unimplemented:
// IRQ-related functionality

#include "k052109.h"
using namespace beekonami::video;

namespace beekonami::video
{
    K052109::K052109()
    {

    }

    K052109::~K052109()
    {

    }

    uint8_t K052109::fetch_rom(uint32_t addr)
    {
	if (gfx_rom.empty())
	{
	    return 0;
	}

	addr %= gfx_rom.size();
	return gfx_rom.at(addr);
    }

    void K052109::init()
    {
	reset();
    }

    void K052109::set_gfx_rom(vector<uint8_t> rom)
    {
	gfx_rom = vector<uint8_t>(rom.begin(), rom.end());
    }

    void K052109::set_tile_read_cb(tilereadfunc cb)
    {
	tile_read = cb;
    }

    void K052109::set_irq_cb(irqfunc cb)
    {
	irq_handler = cb;
    }

    void K052109::reset()
    {
	is_rm_rd = false;
	is_irq_enabled = false;
	vram.fill(0);
	reg_1C00 = 0;
	reg_1D80 = 0;
	reg_1F00 = 0;
	is_scy_enable.fill(false);
	is_scx_enable.fill(false);
	is_scx_interval.fill(false);
	is_flip_screen = false;
	is_flip_y_enable = false;
	gfx_rom_bank = 0;
    }

    bool K052109::irq_enabled()
    {
	return is_irq_enabled;
    }

    bool K052109::get_rmrd_line()
    {
	return is_rm_rd;
    }

    void K052109::vblank(bool line)
    {
	if (is_irq_enabled && line)
	{
	    if (irq_handler)
	    {
		irq_handler(true);
	    }
	}
    }

    gfxaddr K052109::render(int layer_num)
    {
	if ((layer_num < 0) || (layer_num >= 3))
	{
	    throw out_of_range("Invalid layer number");
	}

	gfxaddr tilemap;

	switch (layer_num)
	{
	    case 0: tilemap = render_fixed(); break;
	    case 1: tilemap = render_layer_a(); break;
	    case 2: tilemap = render_layer_b(); break;
	    default: tilemap.fill(0); break;
	}

	return tilemap;
    }

    gfxaddr K052109::render_fixed()
    {
	gfxaddr fixed_tilemap;
	fixed_tilemap.fill(0);

	for (int col = 0; col < 64; col++)
	{
	    for (int row = 0; row < 32; row++)
	    {
		for (int pixel = 0; pixel < 64; pixel++)
		{
		    int pixely = (pixel / 8);
		    int pixelx = (pixel % 8);
		    uint32_t offs = ((row * 64) + col);
		    uint16_t vram_data = ((vram[offs] << 8) | vram[0x2000 + offs]);
		    uint8_t vram_lsb = (vram_data & 0xFF);
		    uint8_t color_attrib = (vram_data >> 8);

		    bool is_flipy = (is_flip_y_enable && testbit(color_attrib, 1));

		    int rom_bank = testbit(color_attrib, 3) ? reg_1F00 : reg_1D80;

		    if (testbit(color_attrib, 2))
		    {
			rom_bank >>= 4;
		    }
		    else
		    {
			rom_bank &= 0xF;
		    }

		    if (!testbit(reg_1C00, 5))
		    {
			color_attrib = ((color_attrib & 0xF3) | ((rom_bank & 0x3) << 2));
		    }

		    int cab_pins = ((rom_bank >> 2) & 0x3);

		    uint32_t tile_offs = ((row * 64) + col);
		    int pixel_offs = ((tile_offs * 64) + pixel);
		    int py = pixely;
		    int px = pixelx;

		    if (is_flipy)
		    {
			py = (7 - py);
		    }

		    fixed_tilemap.at(pixel_offs) = ((px << 21) | (cab_pins << 19) | (color_attrib << 11) | (vram_lsb << 3) | py);
		}
	    }
	}

	return fixed_tilemap;
    }

    gfxaddr K052109::render_layer_a()
    {
	gfxaddr layer_a_tilemap;
	layer_a_tilemap.fill(0);

	for (int col = 0; col < 64; col++)
	{
	    for (int row = 0; row < 32; row++)
	    {
		for (int pixel = 0; pixel < 64; pixel++)
		{
		    int pixely = (pixel / 8);
		    int pixelx = (pixel % 8);

		    uint32_t scrollx_offs = 0;

		    uint32_t scrolly_offs = 0;

		    if (is_scx_interval[0])
		    {
			scrollx_offs = (row * 8);
		    }

		    if (is_scx_enable[0])
		    {
			scrollx_offs += pixelx;
		    }

		    uint32_t scrollx_addr = (0x1A00 + (scrollx_offs * 2));

		    uint16_t scrollx_value = (vram[scrollx_addr] | (testbit(vram[scrollx_addr + 1], 0) << 8));

		    if (is_scy_enable[0])
		    {
			scrolly_offs = ((col + testbit(scrollx_value, 2)) & 0x3F);
		    }
		    else
		    {
			scrolly_offs = 0x0C;
		    }

		    uint8_t scrolly_value = vram[(0x1800 + scrolly_offs)];

		    int column = ((col * 8) + pixelx);
		    column = ((column + scrollx_value) % 512);

		    int line = ((row * 8) + pixely);
		    line = ((line + scrolly_value) % 256);

		    uint32_t scroll_row = (line >> 3);
		    int scrolly_fine = (line & 0x7);

		    uint32_t scroll_col = (column >> 3);
		    int scrollx_fine = (column & 0x7);

		    uint32_t offs = ((scroll_row * 64) + scroll_col);
		    uint16_t vram_data = ((vram[0x800 + offs] << 8) | vram[0x2800 + offs]);
		    uint8_t vram_lsb = (vram_data & 0xFF);
		    uint8_t color_attrib = (vram_data >> 8);

		    bool is_flipy = (is_flip_y_enable && testbit(color_attrib, 1));

		    int rom_bank = testbit(color_attrib, 3) ? reg_1F00 : reg_1D80;

		    if (testbit(color_attrib, 2))
		    {
			rom_bank >>= 4;
		    }
		    else
		    {
			rom_bank &= 0xF;
		    }

		    if (!testbit(reg_1C00, 5))
		    {
			color_attrib = ((color_attrib & 0xF3) | ((rom_bank & 0x3) << 2));
		    }

		    int cab_pins = ((rom_bank >> 2) & 0x3);

		    uint32_t tile_offs = ((row * 64) + col);
		    int pixel_offs = ((tile_offs * 64) + pixel);
		    int py = scrolly_fine;
		    int px = scrollx_fine;

		    if (is_flipy)
		    {
			py = (7 - py);
		    }

		    layer_a_tilemap.at(pixel_offs) = ((px << 21) | (cab_pins << 19) | (color_attrib << 11) | (vram_lsb << 3) | py);
		}
	    }
	}

	return layer_a_tilemap;
    }

    gfxaddr K052109::render_layer_b()
    {
	gfxaddr layer_b_tilemap;
	layer_b_tilemap.fill(0);

	for (int col = 0; col < 64; col++)
	{
	    for (int row = 0; row < 32; row++)
	    {
		for (int pixel = 0; pixel < 64; pixel++)
		{
		    int pixely = (pixel / 8);
		    int pixelx = (pixel % 8);

		    uint32_t scrollx_offs = 0;
		    uint32_t scrolly_offs = 0;

		    if (is_scx_interval[1])
		    {
			scrollx_offs = (row * 8);
		    }

		    if (is_scx_enable[1])
		    {
			scrollx_offs += pixelx;
		    }

		    uint32_t scrollx_addr = (0x3A00 + (scrollx_offs * 2));

		    uint16_t scrollx_value = (vram[scrollx_addr] | (testbit(vram[scrollx_addr + 1], 0) << 8));

		    if (is_scy_enable[1])
		    {
			scrolly_offs = ((col + testbit(scrollx_value, 2)) & 0x3F);
		    }
		    else
		    {
			scrolly_offs = 0x0C;
		    }

		    uint8_t scrolly_value = vram[(0x3800 + scrolly_offs)];

		    int column = ((col * 8) + pixelx);
		    column = ((column + scrollx_value) % 512);

		    uint32_t scroll_col = (column >> 3);
		    int scrollx_fine = (column & 0x7);

		    int line = ((row * 8) + pixely);
		    line = ((line + scrolly_value) % 256);

		    uint32_t scroll_row = (line >> 3);
		    int scrolly_fine = (line & 0x7);

		    uint32_t offs = ((scroll_row * 64) + scroll_col);
		    uint16_t vram_data = ((vram[0x1000 + offs] << 8) | vram[0x3000 + offs]);
		    uint8_t vram_lsb = (vram_data & 0xFF);
		    uint8_t color_attrib = (vram_data >> 8);

		    bool is_flipy = (is_flip_y_enable && testbit(color_attrib, 1));

		    int rom_bank = testbit(color_attrib, 3) ? reg_1F00 : reg_1D80;

		    if (testbit(color_attrib, 2))
		    {
			rom_bank >>= 4;
		    }
		    else
		    {
			rom_bank &= 0xF;
		    }

		    if (!testbit(reg_1C00, 5))
		    {
			color_attrib = ((color_attrib & 0xF3) | ((rom_bank & 0x3) << 2));
		    }

		    int cab_pins = ((rom_bank >> 2) & 0x3);

		    uint32_t tile_offs = ((row * 64) + col);
		    int pixel_offs = ((tile_offs * 64) + pixel);
		    int py = scrolly_fine;
		    int px = scrollx_fine;

		    if (is_flipy)
		    {
			py = (7 - py);
		    }

		    layer_b_tilemap.at(pixel_offs) = ((px << 21) | (cab_pins << 19) | (color_attrib << 11) | (vram_lsb << 3) | py);
		}
	    }
	}

	return layer_b_tilemap;
    }

    void K052109::set_rmrd_line(bool line)
    {
	is_rm_rd = line;
    }

    uint8_t K052109::read(uint16_t addr)
    {
	uint8_t data = 0;

	if (!is_rm_rd)
	{
	    data = vram.at(addr & 0x3FFF);
	}
	else
	{
	    // GFX ROM address calculation
	    // (based on MAME's implementation, improved upon 
	    // using Furrtek's schematics)
	    uint8_t tile_number = ((addr & 0x1FFF) >> 5);
	    uint8_t color_attrib = gfx_rom_bank;
	    int cab_pins = 0;

	    int rom_bank = testbit(color_attrib, 3) ? reg_1F00 : reg_1D80;

	    if (testbit(color_attrib, 2))
	    {
		rom_bank >>= 4;
	    }
	    else
	    {
		rom_bank &= 0xF;
	    }

	    cab_pins = ((rom_bank >> 2) & 0x3);

	    uint32_t gfx_addr = (tile_number | (color_attrib << 8));

	    if (tile_read)
	    {
		gfx_addr = tile_read(tile_number, color_attrib, cab_pins);
	    }

	    uint32_t rom_addr = ((gfx_addr << 5) + (addr & 0x1F));
	    data = fetch_rom(rom_addr);
	}

	return data;
    }

    bool K052109::write(uint16_t addr, uint8_t data)
    {
	addr &= 0x3FFF;
	vram.at(addr) = data;

	bool is_reg1E80_write = false;

	switch (addr & 0x1FFF)
	{
	    case 0x1C00:
	    {
 		reg_1C00 = data;
	    }
	    break;
	    case 0x1C80:
	    {
		is_scx_enable[0] = testbit(data, 0);
		is_scx_interval[0] = testbit(data, 1);
		is_scy_enable[0] = testbit(data, 2);
		is_scx_enable[1] = testbit(data, 3);
		is_scx_interval[1] = testbit(data, 4);
		is_scy_enable[1] = testbit(data, 5);
	    }
	    break;
	    case 0x1D00:
	    {
		// cout << "Writing value of " << hex << int(data) << " to K052109 register of 1d00" << endl;
		is_irq_enabled = testbit(data, 2);

		if (!is_irq_enabled)
		{
		    if (irq_handler)
		    {
			irq_handler(false);
		    }
		}
	    }
	    break;
	    case 0x1D80: reg_1D80 = data; break;
	    case 0x1E00: gfx_rom_bank = data; break;
	    case 0x1E80:
	    {
		is_flip_y_enable = testbit(data, 2);
		is_flip_screen = testbit(data, 0);

		/*
		if (is_flip_screen)
		{
		    cout << "Screen is flipped" << endl;
		}
		else
		{
		    cout << "Screen is normal" << endl;
		}
		*/

		is_reg1E80_write = true;
	    }
	    break;
	    case 0x1F00: reg_1F00 = data; break;
	    default: break;
	}

	return is_reg1E80_write;
    }
};