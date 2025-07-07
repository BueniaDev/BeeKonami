#include "k007782.h"
using namespace beekonami;

namespace beekonami
{
    K007782::K007782()
    {

    }

    K007782::~K007782()
    {

    }

    void K007782::init()
    {
	current_pins = {};
    }

    void K007782::reset()
    {
	current_pins.pin_exres = false;
	tickCLK(true);
	tickCLK(false);
	current_pins.pin_exres = true;
    }

    void K007782::tickCLK(bool clk)
    {
	clk_rise = (!prev_clk && clk);
	clk_fall = (prev_clk && !clk);

	if (prev_clk != clk)
	{
	    tickInternal();
	}

	prev_clk = clk;
    }

    void K007782::tickInternal()
    {
	// TODO: Implement the remainder of this IC
	tickReset();
	tickClocks();
	tickHorizontalSignals();
    }

    void K007782::tickClocks()
    {
	if (!exres_sync)
	{
	    cclk = false;
	}
	else if (clk_rise)
	{
	    cclk = !cclk;
	}

	current_pins.pin_cclk = cclk;
    }

    void K007782::tickReset()
    {
	// TODO: Implement CRES pin

	if (!current_pins.pin_exres)
	{
	    exres_sync = false;
	}
	else if (clk_rise)
	{
	    exres_sync = true;
	}

	current_pins.pin_clr = exres_sync;
    }

    void K007782::tickHorizontalSignals()
    {
	// TODO: Implement remaining horizontal signals logic

	if (!exres_sync)
	{
	    clk2_rise = false;
	    clk2_counter = 0;
	    prev_clk2_counter = start_val;
	}
	else if (clk_fall)
	{
	    if (prev_clk2_counter != clk2_counter)
	    {
		clk2_rise = ((prev_clk2_counter == 2) && (clk2_counter == 0));
	    }

	    prev_clk2_counter = clk2_counter;
	    clk2_counter = ((clk2_counter + 1) % 3);
	}

	if (!exres_sync)
	{
	    hcounter = 0;
	    prev_hcounter = start_val;

	    p1h = false;
	    p2h = false;
	    p4h = false;
	}
	else if (clk2_rise)
	{
	    if (prev_hcounter != hcounter)
	    {
		p1h = testbit(hcounter, 0);
		p2h = testbit(hcounter, 1);
		p4h = testbit(hcounter, 2);
	    }

	    prev_hcounter = hcounter;

	    hcounter += 1;

	    if (current_pins.pin_tes1)
	    {
		if (hcounter == 2)
		{
		    hcounter = 0;
		}
	    }
	    else
	    {
		if (hcounter == 512)
		{
		    hcounter = 128;
		}
	    }
	}

	current_pins.pin_x1s = p1h;
	current_pins.pin_p1h = p1h;
	current_pins.pin_p2h = p2h;
	current_pins.pin_p4h = p4h;
    }
};