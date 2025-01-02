#include "k052109.h"
using namespace beekonami;

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

    void K052109::tickCLK(bool clk)
    {
	clk_rise = (!prev_clk && clk);
	clk_fall = (prev_clk && !clk);
	tickInternal();
	prev_clk = clk;
    }

    void K052109::tickInternal()
    {
	// TODO: Implement the remainder of this IC
	tickClocks();
    }

    void K052109::tickClocks()
    {
	if (!current_pins.pin_nres)
	{
	    res_sync = false;
	}
	else if (clk_rise)
	{
	    res_sync = true;
	}

	if (!res_sync)
	{
	    clk_m12 = true;
	    clk_m6 = true;
	    clk_m3 = true;
	    clk_counter = 0;
	}
	else if (clk_rise)
	{
	    clk_m12 = !testbit(clk_counter, 0);
	    clk_m6 = !testbit(clk_counter, 1);
	    clk_m3 = !testbit(clk_counter, 2);
	}

	if (!res_sync)
	{
	    clk_pe = false;
	    clk_pe_delay = false;
	}
	else if (clk_rise)
	{
	    clk_pe = clk_pe_delay;
	    clk_pe_delay = clk_m3;
	}

	if (!res_sync)
	{
	    clk_pq = false;
	    clk_pq_delay.fill(false);
	}
	else if (clk_rise)
	{
	    clk_pq = clk_pq_delay[2];
	    clk_pq_delay[2] = clk_pq_delay[1];
	    clk_pq_delay[1] = clk_pq_delay[0];
	    clk_pq_delay[0] = !clk_m3;
	}

	if (!res_sync)
	{
	    clk_counter = 0;
	}
	else if (clk_fall)
	{
	    clk_counter = ((clk_counter + 1) % 8);
	}

	current_pins.pin_m12 = clk_m12;
	current_pins.pin_pe = clk_pe;
	current_pins.pin_pq = clk_pq;
    }
};