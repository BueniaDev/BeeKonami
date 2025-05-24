#include "k053251.h"
using namespace beekonami;

namespace beekonami
{
    K053251::K053251()
    {

    }

    K053251::~K053251()
    {

    }

    void K053251::init()
    {
	current_pins = {};
    }

    void K053251::tickCLK(bool clk)
    {
	clk_rise = (!prev_clk && clk);
	clk_fall = (prev_clk && !clk);

	if (prev_clk != clk)
	{
	    tickInternal();
	}

	prev_clk = clk;
    }

    void K053251::tickInternal()
    {
	// TODO: Implement remainder of this IC
	tickIO();
    }

    void K053251::tickIO()
    {
	is_write = !current_pins.pin_cs;

	if (!prev_write && is_write)
	{
	    uint8_t addr = (current_pins.addr & 0xF);
	    uint8_t data = (current_pins.data & 0x3F);

	    cout << "Writing value of " << hex << int(data) << " to K053251 address of " << hex << int(addr) << endl;

	    switch (addr)
	    {
		default: break;
	    }
	}

	prev_write = is_write;
    }
};