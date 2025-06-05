#include "k052535.h"
using namespace beekonami;

namespace beekonami
{
    K052535::K052535()
    {

    }

    K052535::~K052535()
    {

    }

    void K052535::init()
    {
	current_pins = {};
	copy(rgb8_table.begin(), rgb8_table.end(), rgb_table.begin());
    }

    void K052535::setRGBTable(array<uint8_t, 32> col_table)
    {
	copy(col_table.begin(), col_table.end(), rgb_table.begin());
    }

    void K052535::tickCLK(bool clk)
    {
	if (prev_clk != clk)
	{
	    tickInternal();
	}

	prev_clk = clk;
    }

    void K052535::tickInternal()
    {
	if (!current_pins.pin_blank_n)
	{
	    current_pins.color_out = 0;
	}
	else
	{
	    current_pins.color_out = rgb_table.at(current_pins.data & 0x1F);
	}
    }
};