#include "k051962.h"
using namespace beekonami;
using namespace std;

namespace beekonami
{
    K051962::K051962()
    {

    }

    K051962::~K051962()
    {

    }

    void K051962::init()
    {
	current_pins = {};
    }

    void K051962::tickCLK(bool clk24)
    {
	tickInternal(clk24);
    }

    void K051962::tickInternal(bool clk24)
    {
	clk = clk24;

	tickClocks();
	tickCounters();
	tickSync();
	tickOutput();

	prev_t61 = t61;
	prev_clk = clk24;
	prev_clk12 = clk12;
	prev_clk6 = clk6;
    }

    void K051962::tickClocks()
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

	current_pins.pin_m12 = clk12;
	current_pins.pin_m6 = clk6;

	if (!res_sync)
	{
	    t61 = true;
	}
	else if (!prev_clk && clk)
	{
	    t61 = !clk6;
	}
    }

    void K051962::tickSync()
    {
	if (!res_sync)
	{
	    x116 = false;
	}
	else if (!prev_pxh1 && testbit(hcounter, 1))
	{
	    x116 = testbit(hcounter, 2);
	}

	pixel_sel_fix = (7 - (((x116 << 2) | (!testbit(hcounter, 1) << 1) | testbit(hcounter, 0))));
	pixel_sel_la = (7 - (current_pins.za_scx & 0x7));
	pixel_sel_lb = (7 - (current_pins.zb_scx & 0x7));

	z99_cout = (((hcounter >> 1) & 0xF) == 0xF);
	line_end = (z99_cout && (testbit(hcounter, 8) && testbit(hcounter, 7)));

	if (!res_sync)
	{
	    current_pins.pin_ohbk = false;
	}
	else if (!prev_clk6 && clk6)
	{
	    current_pins.pin_ohbk = (!line_end && (current_pins.pin_ohbk || (z99_cout && testbit(hcounter, 6))));
	}

	if (!res_sync)
	{
	    y100 = false;
	}
	else if (!prev_clk6 && clk6)
	{
	    y100 = (testbit(hcounter, 0) && testbit(hcounter, 3) && !(testbit(hcounter, 2) || testbit(hcounter, 1)));
	}

	if (!res_sync)
	{
	    current_pins.pin_nhbk = false;
	}
	else if (!prev_y100 && y100)
	{
	    current_pins.pin_nhbk = current_pins.pin_ohbk;
	}

	if (!res_sync)
	{
	    vblank = false;
	}
	else if (!prev_row4 && testbit(vcounter, 4))
	{
	    vblank = (testbit(vcounter, 7) && testbit(vcounter, 6) && testbit(vcounter, 5));
	}

	current_pins.pin_nvbk = !vblank;

	prev_y100 = y100;
	prev_row4 = testbit(vcounter, 4);
	prev_pxh1 = testbit(hcounter, 1);
    }

    void K051962::tickCounters()
    {
	if (!res_sync)
	{
	    hcounter = 0;
	    vcounter = 0;
	}
	else if (!prev_clk6 && clk6)
	{
	    hcounter += 1;

	    if (hcounter == 0x1A0)
	    {
		hcounter = 0x20;
		vcounter += 1;

		if (vcounter == 0x200)
		{
		    vcounter = 0xF8;
		}
	    }
	}
    }

    void K051962::tickOutput()
    {
	l77 = !(testbit(current_pins.zb_scx, 2) && testbit(current_pins.zb_scx, 1) && testbit(current_pins.zb_scx, 0));
	t19 = !(testbit(current_pins.za_scx, 2) && testbit(current_pins.za_scx, 1) && testbit(current_pins.za_scx, 0));
	x80 = !(!testbit(hcounter, 2) && !testbit(hcounter, 1) && testbit(hcounter, 0));
	x78 = !(testbit(hcounter, 2) && !testbit(hcounter, 1) && testbit(hcounter, 0));
	v154 = !(testbit(hcounter, 2) && testbit(hcounter, 1) && testbit(hcounter, 0));

	if (!prev_clk6 && clk6)
	{
	    if (!prev_x78 && x78)
	    {
		la_pal_delay_a = current_pins.rom_color;
		la_delay_a = current_pins.rom_data;
	    }

	    if (!prev_v154 && v154)
	    {
		la_delay_b = la_delay_a;
		la_pal_delay_b = la_pal_delay_a;

		lb_pal_delay_a = current_pins.rom_color;
		lb_delay_a = current_pins.rom_data;
	    }

	    if (!prev_t19 && t19)
	    {
		current_pins.layer_a_color = ((current_pins.layer_a_color & 0xFF) | ((la_pal_delay_b & 0xF) << 8));
		la_pal_delay_c = ((la_pal_delay_b >> 4) & 0xF);
		la_delay_c = la_delay_b;
	    }

	    if (!prev_l77 && l77)
	    {
		current_pins.layer_b_color = ((current_pins.layer_b_color & 0xFF) | ((lb_pal_delay_a & 0xF) << 8));
		lb_pal_delay_b = ((lb_pal_delay_a >> 4) & 0xF);
		lb_delay_b = lb_delay_a;
	    }	    

	    if (!prev_x80 && x80)
	    {
		lf_pal_delay_a = ((current_pins.rom_color >> 4) & 0xF);
		fix_delay_a = current_pins.rom_data;
	    }

	    prev_l77 = l77;
	    prev_t19 = t19;
	    prev_v154 = v154;
	    prev_x78 = x78;
	    prev_x80 = x80;
	}

	fix_color = 0;
	la_color = 0;
	lb_color = 0;

	for (int pixel = 0; pixel < 4; pixel++)
	{
	    int bit = ((pixel * 8) + pixel_sel_fix);
	    fix_color |= (testbit(fix_delay_a, bit) << pixel);
	}

	for (int pixel = 0; pixel < 4; pixel++)
	{
	    int bit = ((pixel * 8) + pixel_sel_la);
	    la_color |= (testbit(la_delay_c, bit) << pixel);
	}

	for (int pixel = 0; pixel < 4; pixel++)
	{
	    int bit = ((pixel * 8) + pixel_sel_lb);
	    lb_color |= (testbit(lb_delay_b, bit) << pixel);
	}

	if (!t61)
	{
	    current_pins.layer_b_color = ((current_pins.layer_b_color & 0xF00) | ((lb_color & 0xF) | (lb_pal_delay_b << 4)));
	    current_pins.layer_a_color = ((current_pins.layer_a_color & 0xF00) | ((la_color & 0xF) | (la_pal_delay_c << 4)));
	    current_pins.fix_color = ((fix_color & 0xF) | (lf_pal_delay_a << 4));
	}

	current_pins.pin_nfic = (fix_color != 0);
    }
};