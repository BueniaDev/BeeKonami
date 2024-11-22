#include "k007232.h"
using namespace std;

namespace beekonami
{
    K007232::K007232()
    {

    }

    K007232::~K007232()
    {

    }

    void K007232::init()
    {
	current_pins = {};
    }

    void K007232::reset()
    {
	current_pins.pin_nres = false;
	tickCLK(true);
	tickCLK(false);
	current_pins.pin_nres = true;
    }

    void K007232::tickCLK(bool clk)
    {
	clk_rise = (!prev_clk && clk);
	clk_fall = (prev_clk && !clk);
	tickInternal();
	prev_clk = clk;
    }

    void K007232::tickInternal()
    {
	tickIO();
	tickClocks();
	tickPrescaler();
	tickCounters();
	tickReads();
    }

    void K007232::tickIO()
    {
	// AB0 is inverted externally
	uint8_t addr = ((current_pins.addr & 0xE) | !testbit(current_pins.addr, 0));

	current_pins.pin_slev = !(!current_pins.pin_dacs && (addr == 0xC));
	reg5_wr = (prev_dacs && !current_pins.pin_dacs && (addr == 0x5));
	regB_wr = (prev_dacs && !current_pins.pin_dacs && (addr == 0xB));

	ch1_pre_write = (!current_pins.pin_dacs && ((addr == 0x0) || (addr == 0x1)));
	ch2_pre_write = (!current_pins.pin_dacs && ((addr == 0x6) || (addr == 0x7)));

	if (prev_dacs && !current_pins.pin_dacs)
	{
	    switch (addr)
	    {
		case 0x0:
		{
		    ch1_prescale_data = ((ch1_prescale_data & 0xF00) | current_pins.data);
		}
		break;
		case 0x1:
		{
		    ch1_prescale_ctrl = ((current_pins.data >> 4) & 0x3);
		    ch1_prescale_data = ((ch1_prescale_data & 0xFF) | ((current_pins.data & 0xF) << 8));
		}
		break;
		case 0x2:
		{
		    ch1_counter_data = ((ch1_counter_data & 0x1FF00) | current_pins.data);
		}
		break;
		case 0x3:
		{
		    ch1_counter_data = ((ch1_counter_data & 0x100FF) | (current_pins.data << 8));
		}
		break;
		case 0x4:
		{
		    ch1_counter_data = ((ch1_counter_data & 0xFFFF) | (testbit(current_pins.data, 0) << 16));
		}
		break;
		case 0x6:
		{
		    ch2_prescale_data = ((ch2_prescale_data & 0xF00) | current_pins.data);
		}
		break;
		case 0x7:
		{
		    ch2_prescale_ctrl = ((current_pins.data >> 4) & 0x3);
		    ch2_prescale_data = ((ch2_prescale_data & 0xFF) | ((current_pins.data & 0xF) << 8));
		}
		break;
		case 0x8:
		{
		    ch2_counter_data = ((ch2_counter_data & 0x1FF00) | current_pins.data);
		}
		break;
		case 0x9:
		{
		    ch2_counter_data = ((ch2_counter_data & 0x100FF) | (current_pins.data << 8));
		}
		break;
		case 0xA:
		{
		    ch2_counter_data = ((ch2_counter_data & 0xFFFF) | (testbit(current_pins.data, 0) << 16));
		}
		break;
		case 0xD:
		{
		    is_ch2_loop = testbit(current_pins.data, 1);
		    is_ch1_loop = testbit(current_pins.data, 0);
		}
		break;
		default: break;
	    }
	}

	prev_dacs = current_pins.pin_dacs;
    }

    void K007232::tickClocks()
    {
	if (!current_pins.pin_nres)
	{
	    e74 = false;
	}
	else if (clk_fall)
	{
	    e74 = f74;
	}

	if (!current_pins.pin_nres)
	{
	    f74 = false;
	}
	else if (clk_rise)
	{
	    f74 = !e74;
	}

	current_pins.pin_ne = f74;
	current_pins.pin_nq = e74;

	f74_rise = (!prev_f74 && f74);

	if (!current_pins.pin_nres)
	{
	    clk_d4 = false;
	}
	else if (f74_rise)
	{
	    clk_d4 = !clk_d4;
	}

	is_sel_ch2 = clk_d4;

	clk_d4_rise = (!prev_clk_d4 && clk_d4);
	clk_d4_fall = (prev_clk_d4 && !clk_d4);

	clk_d1024 = testbit(clk_div, 7);
	clk_d1024_rise = (!prev_clk_d1024 && clk_d1024);

	if (!current_pins.pin_nres)
	{
	    clk_div = 0;
	}
	else if (clk_d4_rise)
	{
	    clk_div += 1;
	}

	prev_f74 = f74;
	prev_clk_d4 = clk_d4;
	prev_clk_d1024 = clk_d1024;

	bool clk_pre_rise = testbit(ch1_prescale_ctrl, 0) ? clk_d1024_rise : clk_d4_rise;

	if (testbit(ch1_prescale_ctrl, 1))
	{
	    current_pins.pin_ck2m = clk_d1024;
	}
	else
	{
	    current_pins.pin_ck2m = (clk_div_10 == 0xF);
	}

	if (!current_pins.pin_nres)
	{
	    clk_div_10 = 0;
	}
	else if (clk_pre_rise)
	{
	    if (clk_div_10 == 0xF)
	    {
		clk_div_10 = 0x9;
	    }
	    else
	    {
		clk_div_10 = ((clk_div_10 + 1) % 16);
	    }
	}
    }

    void K007232::tickPrescaler()
    {
	if (ch1_pre_write)
	{
	    ch1_reset_pre = true;
	}
	else if (f74_rise)
	{
	    ch1_reset_pre = false;
	}

	if (ch2_pre_write)
	{
	    ch2_reset_pre = true;
	}
	else if (f74_rise)
	{
	    ch2_reset_pre = false;
	}

	ch1_low_carry = (clk_d4 && (ch1_prescale_cntr_low == 0xF));
	ch1_mid_carry = (ch1_low_carry && (ch1_prescale_cntr_mid == 0xF));

	if (testbit(ch1_prescale_ctrl, 1))
	{
	    ch1_high_cin = clk_d4;
	}
	else
	{
	    ch1_high_cin = ch1_mid_carry;
	}

	ch1_high_carry = (ch1_high_cin && (ch1_prescale_cntr_high == 0xF));

	if (testbit(ch1_prescale_ctrl, 0))
	{
	    ch1_prescale_carry = ch1_mid_carry;
	}
	else
	{
	    ch1_prescale_carry = ch1_high_carry;
	}

	ch1_prescale_load = (ch1_reset_pre || ch1_prescale_carry);

	if (f74_rise)
	{
	    if (ch1_prescale_load)
	    {
		ch1_prescale_cntr_high = ((ch1_prescale_data >> 8) & 0xF);
		ch1_prescale_cntr_mid = ((ch1_prescale_data >> 4) & 0xF);
		ch1_prescale_cntr_low = (ch1_prescale_data & 0xF);
	    }
	    else
	    {
		if (ch1_high_cin)
		{
		    ch1_prescale_cntr_high = ((ch1_prescale_cntr_high + 1) & 0xF);
		}

		if (ch1_low_carry)
		{
		    ch1_prescale_cntr_mid = ((ch1_prescale_cntr_mid + 1) & 0xF);
		}

		if (clk_d4)
		{
		    ch1_prescale_cntr_low = ((ch1_prescale_cntr_low + 1) & 0xF);
		}
	    }
	}

	ch2_low_carry = (!clk_d4 && (ch2_prescale_cntr_low == 0xF));
	ch2_mid_carry = (ch2_low_carry && (ch2_prescale_cntr_mid == 0xF));

	if (testbit(ch2_prescale_ctrl, 1))
	{
	    ch2_high_cin = !clk_d4;
	}
	else
	{
	    ch2_high_cin = ch2_mid_carry;
	}

	ch2_high_carry = (ch2_high_cin && (ch2_prescale_cntr_high == 0xF));

	if (testbit(ch2_prescale_ctrl, 0))
	{
	    ch2_prescale_carry = ch2_mid_carry;
	}
	else
	{
	    ch2_prescale_carry = ch2_high_carry;
	}

	ch2_prescale_load = (ch2_reset_pre || ch2_prescale_carry);

	if (f74_rise)
	{
	    if (ch2_prescale_load)
	    {
		ch2_prescale_cntr_high = ((ch2_prescale_data >> 8) & 0xF);
		ch2_prescale_cntr_mid = ((ch2_prescale_data >> 4) & 0xF);
		ch2_prescale_cntr_low = (ch2_prescale_data & 0xF);
	    }
	    else
	    {
		if (ch2_high_cin)
		{
		    ch2_prescale_cntr_high = ((ch2_prescale_cntr_high + 1) & 0xF);
		}

		if (ch2_low_carry)
		{
		    ch2_prescale_cntr_mid = ((ch2_prescale_cntr_mid + 1) & 0xF);
		}

		if (clk_d4)
		{
		    ch2_prescale_cntr_low = ((ch2_prescale_cntr_low + 1) & 0xF);
		}
	    } 
	}
    }

    void K007232::tickCounters()
    {
	if (clk_d4_rise)
	{
	    current_pins.cha_output = (current_pins.pcm_data & 0x7F);
	}

	if (clk_d4_fall)
	{
	    current_pins.chb_output = (current_pins.pcm_data & 0x7F);
	}

	if (clk_d4_rise)
	{
	    is_ch1_stop = testbit(current_pins.pcm_data, 7);
	}

	if (clk_d4_fall)
	{
	    is_ch2_stop = testbit(current_pins.pcm_data, 7);
	}

	if (is_sel_ch2)
	{
	    current_pins.pcm_addr = ((ch2_counter_high << 12) | (ch2_counter_mid2 << 8) | (ch2_counter_mid1 << 4) | ch2_counter_low);
	}
	else
	{
	    current_pins.pcm_addr = ((ch1_counter_high << 12) | (ch1_counter_mid2 << 8) | (ch1_counter_mid1 << 4) | ch1_counter_low);
	}

	if (ch1_cntr_reset)
	{
	    ch1_counter_high = 0;
	    ch1_counter_mid2 = 0;
	    ch1_counter_mid1 = 0;
	    ch1_counter_low = 0;
	}
	else if (f74_rise)
	{
	    if (ch1_cntr_reload)
	    {
		ch1_counter_high = ((ch1_counter_data >> 12) & 0x1F);
		ch1_counter_mid2 = ((ch1_counter_data >> 8) & 0xF);
		ch1_counter_mid1 = ((ch1_counter_data >> 4) & 0xF);
		ch1_counter_low = (ch1_counter_data & 0xF);
	    }
	    else
	    {
		if (ch1_prescale_carry)
		{
		    if (testbit(ch1_prescale_ctrl, 1))
		    {
			ch1_counter_high = ((ch1_counter_high + 1) & 0x1F);
			ch1_counter_mid2 = ((ch1_counter_mid2 + 1) & 0xF);
			ch1_counter_mid1 = ((ch1_counter_mid1 + 1) & 0xF);
		    }
		    else
		    {
			if (ch1_counter_low == 0xF)
			{
			    if (ch1_counter_mid1 == 0xF)
			    {
				if (ch1_counter_mid2 == 0xF)
				{
				    ch1_counter_high = ((ch1_counter_high + 1) & 0x1F);
				}

				ch1_counter_mid2 = ((ch1_counter_mid2 + 1) & 0xF);
			    }

			    ch1_counter_mid1 = ((ch1_counter_mid1 + 1) & 0xF);
			}
		    }

		    ch1_counter_low = ((ch1_counter_low + 1) & 0xF);
		}
	    }
	}

	if (ch2_cntr_reset)
	{
	    ch2_counter_high = 0;
	    ch2_counter_mid2 = 0;
	    ch2_counter_mid1 = 0;
	    ch2_counter_low = 0;
	}
	else if (f74_rise)
	{
	    if (ch2_cntr_reload)
	    {
		ch2_counter_high = ((ch2_counter_data >> 12) & 0x1F);
		ch2_counter_mid2 = ((ch2_counter_data >> 8) & 0xF);
		ch2_counter_mid1 = ((ch2_counter_data >> 4) & 0xF);
		ch2_counter_low = (ch2_counter_data & 0xF);
	    }
	    else
	    {
		if (ch2_prescale_carry)
		{
		    if (testbit(ch2_prescale_ctrl, 1))
		    {
			ch2_counter_high = ((ch2_counter_high + 1) & 0x1F);
			ch2_counter_mid2 = ((ch2_counter_mid2 + 1) & 0xF);
			ch2_counter_mid1 = ((ch2_counter_mid1 + 1) & 0xF);
		    }
		    else
		    {
			if (ch2_counter_low == 0xF)
			{
			    if (ch2_counter_mid1 == 0xF)
			    {
				if (ch2_counter_mid2 == 0xF)
				{
				    ch2_counter_high = ((ch2_counter_high + 1) & 0x1F);
				}

				ch2_counter_mid2 = ((ch2_counter_mid2 + 1) & 0xF);
			    }

			    ch2_counter_mid1 = ((ch2_counter_mid1 + 1) & 0xF);
			}
		    }

		    ch2_counter_low = ((ch2_counter_low + 1) & 0xF);
		}
	    }
	}

	if (!current_pins.pin_nres)
	{
	    ch1_trigger = true;
	}
	else if (reg5_wr)
	{
	    ch1_trigger = false;
	}
	else if (f74_rise)
	{
	    ch1_trigger = true;
	}

	if (!current_pins.pin_nres)
	{
	    ch2_trigger = true;
	}
	else if (regB_wr)
	{
	    ch2_trigger = false;
	}
	else if (f74_rise)
	{
	    ch2_trigger = true;
	}

	if (!current_pins.pin_nres)
	{
	    ch1_cntr_reset = true;
	}
	else if (ch1_trigger)
	{
	    ch1_cntr_reset = (ch1_cntr_reset || (is_ch1_stop && !is_ch1_loop));
	}
	else
	{
	    ch1_cntr_reset = false;
	}

	if (!current_pins.pin_nres)
	{
	    ch2_cntr_reset = true;
	}
	else if (ch2_trigger)
	{
	    ch2_cntr_reset = (ch2_cntr_reset || (is_ch2_stop && !is_ch2_loop));
	}
	else
	{
	    ch2_cntr_reset = false;
	}

	if (!ch1_trigger)
	{
	    ch1_cntr_reload = true;
	}
	else
	{
	    ch1_cntr_reload = (is_ch1_stop && is_ch1_loop);
	}

	if (!ch2_trigger)
	{
	    ch2_cntr_reload = true;
	}
	else
	{
	    ch2_cntr_reload = (is_ch2_stop && is_ch2_loop);
	}
    }

    void K007232::tickReads()
    {
	if (!current_pins.pin_nrd && !current_pins.pin_nrcs && !current_pins.pin_ne)
	{
	    current_pins.data = current_pins.pcm_data;
	}

	if (current_pins.pin_nrd && !current_pins.pin_nrcs && !current_pins.pin_ne)
	{
	    current_pins.pcm_data = current_pins.data;
	}
    }
};