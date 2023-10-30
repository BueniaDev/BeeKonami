#include "k052109.h"
using namespace beekonami;
using namespace std;

namespace beekonami
{
    K052109::K052109()
    {

    }

    K052109::~K052109()
    {

    }

    void K052109::init()
    {
	current_pins = {};
    }

    void K052109::tickCLK(bool clk24)
    {
	tickInternal(clk24);
    }

    void K052109::tickInternal(bool clk24)
    {
	clk = clk24;
	tickClocks();
	tickIO();
	tickCounters();
	tickOutput();

	prev_j140 = j140;
	prev_clk = clk24;
	prev_clk12 = clk12;
	prev_clk6 = clk6;
    }

    void K052109::tickClocks()
    {
	if (!current_pins.pin_nres)
	{
	    res_sync = false;
	}
	else if (!prev_clk && clk)
	{
	    res_sync = true;
	}

	if (!res_sync)
	{
	    clk_counter = 0;
	    clk12 = true;
	    clk6 = true;
	    clk3 = true;
	}
	else if (!prev_clk && clk)
	{
	    clk12 = !testbit(clk_counter, 0);
	    clk6 = !testbit(clk_counter, 1);
	    clk3 = !testbit(clk_counter, 2);

	    clk_counter = ((clk_counter + 1) % 8);
	}

	if (!res_sync)
	{
	    k123 = false;
	}
	else if (!prev_clk && clk)
	{
	    k123 = !clk3;
	}

	if (!res_sync)
	{
	    cpu_access_n = false;
	}
	else if (prev_clk && !clk)
	{
	    cpu_access_n = clk3;
	}

	if (!res_sync)
	{
	    k130 = false;
	    current_pins.pin_pq = false;
	    k148 = false;
	    k123 = false;
	}
	else if (!prev_clk && clk)
	{
	    k130 = k148;
	    current_pins.pin_pq = k148;
	    k148 = k123;
	    k123 = !clk3;
	}

	if (!res_sync)
	{
	    k77_q = 0;
	}
	else if (!prev_clk && clk)
	{
	    bool bit3 = (current_pins.pin_nrd && k123);
	    bool bit2 = !(current_pins.pin_nrd && clk3 && !k123);
	    bool bit1 = !(current_pins.pin_nrd && !k123 && k130);
	    bool bit0 = clk3;
	    k77_q = ((bit3 << 3) | (bit2 << 2) | (bit1 << 1) | bit0);
	}

	current_pins.pin_rden = testbit(k77_q, 3);
	current_pins.pin_wren = testbit(k77_q, 2);
	current_pins.pin_wrp = testbit(k77_q, 1);
	current_pins.pin_pe = testbit(k77_q, 0);

	if (!res_sync)
	{
	    j140 = false;
	}
	else if (prev_clk12 && !clk12)
	{
	    j140 = clk6;
	}

	j151 = (j140 && testbit(reg_1C00, 6));
    }

    void K052109::tickCounters()
    {
	prev_h1 = h1;
	h1 = testbit(hcounter, 0);

	prev_h2 = h2;
	h2 = testbit(hcounter, 1);

	read_tile_num = (prev_h1 && !h1);
	read_row_fix = (!prev_h2 && h2);

	if (!res_sync)
	{
	    hcounter = 0;
	    vcounter = 0;
	    current_pins.pin_hvot = true;
	}
	else if (!prev_clk6 && clk6)
	{
	    current_pins.pin_hvot = true;
	    hcounter += 1;

	    if (hcounter == 0x1A0)
	    {
		hcounter = 0x20;
		vcounter += 1;

		if (vcounter == 0x200)
		{
		    current_pins.pin_hvot = false;
		    vcounter = 0xF8;
		}
	    }
	}
    }

    void K052109::tickIO()
    {
	aa38 = (hcounter < 0x60);
	bool e34 = !(current_pins.pin_vcs || current_pins.pin_rmrd);

	range_val = 0x3F;

	if (e34)
	{
	    int addr_msb = ((current_pins.addr >> 13) & 0x7);

	    if (addr_msb < 6)
	    {
		range_val &= ~(1 << addr_msb);
	    }
	}

	int range0 = ((range_val >> 2) & 0xF);
	int range1 = (range_val & 0xF);
	int range2 = ((range_val >> 1) & 0xF);

	int cfg = (~reg_1C00 & 0x3);

	vram_cs0 = testbit(~range0, cfg);
	vram_cs1 = testbit(~range1, cfg);
	vram_cs2 = testbit(~range2, cfg);

	current_pins.pin_rwe0 = (current_pins.pin_wrp || vram_cs0);
	current_pins.pin_rwe1 = (current_pins.pin_wrp || vram_cs1);
	current_pins.pin_rwe2 = (current_pins.pin_wrp || vram_cs2);

	if (cpu_access_n)
	{
	    current_pins.pin_roe0 = current_pins.pin_rden;
	    current_pins.pin_roe1 = current_pins.pin_rden;
	    current_pins.pin_roe2 = current_pins.pin_rden;
	    current_pins.pin_rcs0 = vram_cs0;
	    current_pins.pin_rcs1 = vram_cs1;
	}
	else
	{
	    current_pins.pin_roe0 = !j140;
	    current_pins.pin_roe1 = j151;
	    current_pins.pin_roe2 = true;
	    current_pins.pin_rcs0 = false;
	    current_pins.pin_rcs1 = false;
	}

	uint16_t scroll_ram_a = 0;

	uint8_t row_msb = ((vcounter >> 3) & 0x1F);
	uint8_t pxhf = ((hcounter >> 3) & 0x3F);

	// TODO: Implement screen flipping
	uint8_t flip_adder = pxhf;

	if (aa38)
	{
	    scroll_ram_a = (0x0200 | (row_msb << 4) | testbit(hcounter, 3));
	}
	else
	{
	    scroll_ram_a = flip_adder;
	}

	render_addr = ((hcounter >> 1) & 0x3);

	if (!cpu_access_n)
	{
	    current_pins.vram_addr = (current_pins.addr & 0x1FFF);
	}
	else
	{
	    switch (render_addr)
	    {
		case 0: current_pins.vram_addr = (0x1800 + scroll_ram_a); break;
		case 3: current_pins.vram_addr = ((row_msb << 6) + pxhf); break;
		default: current_pins.vram_addr = 0; break;
	    }
	}
    }

    void K052109::tickOutput()
    {
	int row = vcounter;
	if (!res_sync)
	{
	    row_fix = 0;
	}
	else if (!prev_h2 && h2)
	{
	    row_fix = (row & 0x7);
	}

	switch (render_addr)
	{
	    case 0:
	    case 1: vc_mux = row_fix; break;
	    default: vc_mux = 0; break;
	}

	if (read_tile_num)
	{
	    render_rom_addr = ((render_rom_addr & 0x7) | ((current_pins.vram_data & 0xFF) << 3));
	}

	if (cpu_access_n)
	{
	    render_rom_addr = ((render_rom_addr & 0x7F8) | (vc_mux & 0x7));
	}

	if (current_pins.pin_rmrd)
	{
	    current_pins.rom_addr = 0;
	}
	else
	{
	    current_pins.rom_addr = (render_rom_addr & 0x7FF);
	}

	if (prev_h1 && !h1)
	{
	    color_attrib_a = (current_pins.vram_data >> 8);
	}

	if (prev_j140 && !j140)
	{
	    color_attrib_b = (current_pins.vram_data >> 8);
	}

	bool f30 = (clk6 && testbit(reg_1C00, 6) && !h1);

	color_mux = (f30) ? color_attrib_b : color_attrib_a;

	int color_addr = ((color_mux >> 2) & 0x3);

	int col_cab_mux = 0;

	switch (color_addr)
	{
	    case 0: col_cab_mux = (reg_1D80 & 0xF); break;
	    case 1: col_cab_mux = (reg_1D80 >> 4); break;
	    case 2: col_cab_mux = (reg_1F00 & 0xF); break;
	    case 3: col_cab_mux = (reg_1F00 >> 4); break;
	}

	if (current_pins.pin_rmrd)
	{
	    current_pins.rom_color = 0;
	}
	else
	{
	    uint8_t color = color_mux;

	    if (!testbit(reg_1C00, 6))
	    {
		color = ((color & 0xF3) | ((col_cab_mux & 0x3) << 2));
	    }

	    current_pins.rom_color = color;
	}

	current_pins.pin_cab1 = testbit(col_cab_mux, 2);
	current_pins.pin_cab2 = testbit(col_cab_mux, 3);
    }
}