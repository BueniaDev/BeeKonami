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
	x80 = !(!testbit(hcounter, 2) && !testbit(hcounter, 1) && testbit(hcounter, 0));

	if (!prev_clk6 && clk6)
	{
	    if (!prev_x80 && x80)
	    {
		fix_delay_a = current_pins.rom_data;
	    }
	}

	fix_color = 0;

	for (int pixel = 0; pixel < 4; pixel++)
	{
	    int bit = ((pixel * 8) + pixel_sel_fix);
	    fix_color |= (testbit(fix_delay_a, bit) << pixel);
	}

	if (!t61)
	{
	    current_pins.fix_color = (fix_color & 0xF);
	}

	current_pins.pin_nfic = (fix_color != 0);
	prev_x80 = x80;
    }
};