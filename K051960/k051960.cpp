#include "k051960.h"
using namespace beekonami;

namespace beekonami
{
    K051960::K051960()
    {

    }

    K051960::~K051960()
    {

    }

    void K051960::init()
    {
	current_pins = {};
    }

    void K051960::tickCLK(bool clk24)
    {
	for (int i = 0; i < 2; i++)
	{
	    tickOnce(true, clk24);
	    tickOnce(false, clk24);
	}
    }

    void K051960::tickOnce(bool clk96, bool clk24)
    {
	delay_edge = (prev_clk96 && !clk96);
	clk_m24 = clk24;
	clk_rise = (!prev_clk && clk24);
	clk_fall = (prev_clk && !clk24);
	tickInternal();
	prev_clk = clk24;
	prev_clk96 = clk96;
    }

    void K051960::tickInternal()
    {
	// TODO: Implement the following:
	// Internal RAM reads
	// Remaining RAM address indexes
	// Overall sprite rendering
	// Memory reads and writes
	// Test functionality
	tickClocks();
	tickTest();
	tickObjLogic();
	tickCounters();
    }

    void K051960::tickClocks()
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
	    clk_copy_val = false;
	}
	else if (clk_rise)
	{
	    clk_copy_val = clk_copy_delay;
	    clk_copy_delay = !(clk_pq_delay[2] && !clk_pq_delay[0]);
	}

	clk_copy = (!clk_copy_val && !current_pins.pin_te0);

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
	    is_cpu_access = true;
	}
	else if (clk_fall)
	{
	    is_cpu_access = clk_m3;
	}

	if (!res_sync)
	{
	    ram_clk2 = true;
	}
	else if (clk_fall)
	{
	    ram_clk2 = !ram_clk1;
	}

	if (!res_sync)
	{
	    ram_clk1 = false;
	    ram_clk1_delay = false;
	}
	else if (clk_rise)
	{
	    ram_clk1 = ram_clk1_delay;
	    ram_clk1_delay = !clk_m6;
	}

	if (!current_pins.pin_te0)
	{
	    ram_clk = (ram_clk2 && !ram_clk1);
	}
	else
	{
	    ram_clk = clk_m24;
	}

	ram_clk_edge = (prev_ram_clk && !ram_clk);
	prev_ram_clk = ram_clk;

	if (!res_sync)
	{
	    clk_counter = 0;
	}
	else if (clk_fall)
	{
	    clk_counter = ((clk_counter + 1) % 8);
	}

	current_pins.pin_m12 = clk_m12;
	current_pins.pin_m6 = clk_m6;
	current_pins.pin_pe = clk_pe;
	current_pins.pin_pq = clk_pq;

	clk6_rise = (!prev_clk_m6 && clk_m6);
	prev_clk_m6 = clk_m6;

	clk3_rise = (!prev_clk_m3 && clk_m3);
	prev_clk_m3 = clk_m3;
    }

    void K051960::tickTest()
    {
	if (!res_sync)
	{
	    test_addr = 0x00;
	    test_data = 0x00;
	}
    }

    void K051960::tickCounters()
    {
	tickHCounter();
	tickVCounter();
	tickVBlank();
    }

    void K051960::tickHCounter()
    {
	if (!res_sync)
	{
	    hcounter_bit0 = false;
	    hcounter_lsb = 0;
	    hcounter_msb = 0;
	    hcount_lsb_carry = false;
	}
	else if (clk6_rise)
	{
	    hvin_sync = testbit(hvin_sr, 7);
	    hvin_sr = ((hvin_sr << 1) | !current_pins.pin_hvin);

	    bool hcount_lsb_en = (hcounter_bit0 || testbit(test_addr, 0));

	    hcount_lsb_carry = (hcount_lsb_en && (hcounter_lsb == 15));

	    bool hcount_msb_en = (hcount_lsb_carry || testbit(test_addr, 1));

	    is_hend = ((((hcounter_msb >> 2) & 0x3) == 3) && hcount_msb_en);

	    is_hrst = (hvin_sync || is_hend);

	    if (is_hrst)
	    {
		hcounter_msb = 1;
		hcounter_lsb = 0;
	    }
	    else
	    {
		if (hcount_msb_en)
		{
		    hcounter_msb = ((hcounter_msb + 1) % 16);
		}

		if (hcount_lsb_en)
		{
		    hcounter_lsb = ((hcounter_lsb + 1) % 16);
		}
	    }

	    hcounter_bit0 = clk_pe;
	}

	if (!res_sync)
	{
	    hrst_sr = 0;
	}
	else if (clk6_rise)
	{
	    hrst_delay6 = testbit(hrst_sr, 5);
	    hrst_delay4 = testbit(hrst_sr, 3);
	    hrst_delay1 = testbit(hrst_sr, 0);
	    hrst_sr = (((hrst_sr << 1) | is_hrst) & 0x7F);
	}

	current_pins.pin_p1h = hcounter_bit0;
	current_pins.pin_p2h = testbit(hcounter_lsb, 0);
    }

    void K051960::tickVCounter()
    {
	if (!res_sync)
	{
	    vcounter_bit0 = false;
	    vcounter_lsb = 0;
	    vcounter_msb = 0;
	    vcount_lsb_carry = false;
	    vcount_msb_carry = false;
	}
	else if (clk6_rise)
	{
	    bool vcount_lsb_en = ((vcounter_bit0 && is_hend) || testbit(test_addr, 2));

	    vcount_lsb_carry = (vcount_lsb_en && (vcounter_lsb == 15));

	    bool vcount_msb_ci = (vcount_lsb_carry || testbit(test_addr, 3));
	    bool vcount_msb_en = (vcounter_bit0 || testbit(test_addr, 5));

	    vcount_msb_carry = (vcount_msb_ci && (vcounter_msb == 15));

	    is_hvot = (vcount_msb_carry || hvin_sync);

	    if (is_hvot)
	    {
		vcounter_msb = 0x7;
		vcounter_lsb = 0xC;
	    }
	    else
	    {
		if (vcount_msb_en && vcount_msb_ci)
		{
		    vcounter_msb = ((vcounter_msb + 1) % 16);
		}

		if (vcount_lsb_en)
		{
		    vcounter_lsb = ((vcounter_lsb + 1) % 16);
		}
	    }

	    if (!hvin_sync && is_hend)
	    {
		vcounter_bit0 = !vcounter_bit0;
	    }
	}

	current_pins.pin_hvot = !is_hvot;
    }

    void K051960::tickVBlank()
    {
	bool v16 = testbit(vcounter_lsb, 3);

	if (!res_sync)
	{
	    is_vblank = false;
	}
	else if (!prev_v16 && v16)
	{
	    is_vblank = ((vcounter_msb & 0x7) == 0x7);
	}

	prev_v16 = v16;

	bool rst_clk = (is_vblank && !testbit(test_data, 2));

	if (!res_sync)
	{
	    rst_sr = 0;
	}
	else if (!prev_rst_clk && rst_clk)
	{
	    current_pins.pin_rst = testbit(rst_sr, 7);
	    rst_sr = ((rst_sr << 1) | res_sync);
	}

	prev_rst_clk = rst_clk;

	if (!res_sync)
	{
	    sprite_process_sync = false;
	}
	else if (!prev_vblank && is_vblank)
	{
	    sprite_process_sync = is_sprite_process;
	}

	prev_vblank = is_vblank;

	if (!res_sync)
	{
	    is_vblank_sync = false;
	}
	else if (clk6_rise)
	{
	    is_vblank_sync = !vblank_sync_data;
	}

	if (!res_sync)
	{
	    prev_sprite_copy = true;
	}
	else if (clk6_rise)
	{
	    prev_sprite_copy = !is_sprite_copy;
	}

	is_sprite_copy = (!is_vblank || sprite_process_sync);

	sprite_copy_start = !((obj_counter_msb_carry || !is_vblank_sync) && (is_sprite_copy || prev_sprite_copy));
	vblank_sync_data = !((is_vblank && sprite_copy_start) || testbit(test_addr, 6));
    }

    void K051960::tickObjLogic()
    {
	tickObjAttrib();
	tickObjAddr();
	tickObjRAM();
	tickObjPins();
    }

    void K051960::tickObjAttrib()
    {
	if (!res_sync)
	{
	    is_sprite_active = false;
	    sprite_priority = 0;
	}
	else if (clk3_rise)
	{
	    if (obj_attrib_counter == 0)
	    {
		is_sprite_active = testbit(current_pins.ram_data, 7);
		sprite_priority = (current_pins.ram_data & 0x7F);
	    }
	}

	ram_data_wr = (testbit(obj_counter_msb, 3) || testbit(test_addr, 6));
	obj_counter_inc = (!hcounter_bit0 && ((ram_data_wr && !is_sprite_active) || (obj_attrib_counter == 7)));

	if (current_pins.pin_te0)
	{
	    bool ram_wr_in = current_pins.pin_ram_wr;
	    uint8_t ram_test_val = (current_pins.ram_addr & 0x7);
	    ram_f_we = (ram_wr_in && (ram_test_val == 1));
	    ram_e_we = (ram_wr_in && (ram_test_val == 2));
	    ram_c_we = (ram_wr_in && (ram_test_val == 3));
	    ram_d_we = (ram_wr_in && (ram_test_val == 4));
	    ram_b_we = (ram_wr_in && (ram_test_val == 5));
	    ram_a_we = (ram_wr_in && (ram_test_val == 6));
	    ram_g_we = (ram_wr_in && (ram_test_val == 7));
	}
	else
	{
	    bool obj_ram_copy = (is_vblank_sync && (!ram_data_wr || is_sprite_active) && clk_copy);
	    ram_f_we = (obj_ram_copy && (obj_attrib_counter == 1));
	    ram_e_we = (obj_ram_copy && (obj_attrib_counter == 2));
	    ram_c_we = (obj_ram_copy && (obj_attrib_counter == 3));
	    ram_d_we = (obj_ram_copy && (obj_attrib_counter == 4));
	    ram_b_we = (obj_ram_copy && (obj_attrib_counter == 5));
	    ram_a_we = (obj_ram_copy && (obj_attrib_counter == 6));
	    ram_g_we = (obj_ram_copy && (obj_attrib_counter == 7));
	}

	if (!res_sync)
	{
	    obj_counter_lsb = 0;
	    obj_counter_msb = 0;
	    obj_counter_lsb_carry = false;
	    obj_counter_msb_carry = false;
	}
	else if (clk6_rise)
	{
	    obj_counter_lsb_carry = (obj_counter_inc && (obj_counter_lsb == 15));
	    obj_counter_msb_carry = (obj_counter_lsb_carry && (obj_counter_msb == 15));

	    if (!is_vblank_sync)
	    {
		obj_counter_lsb = 0;
		obj_counter_msb = 0;
	    }
	    else if (!hcounter_bit0)
	    {
		if (obj_counter_lsb_carry)
		{
		    obj_counter_msb = ((obj_counter_msb + 1) & 0xF);
		}

		if (obj_counter_inc)
		{
		    obj_counter_lsb = ((obj_counter_lsb + 1) & 0xF);
		}
	    }
	}

	if (!res_sync)
	{
	    obj_attrib_counter = 0;
	}
	else if (clk6_rise)
	{
	    if (!is_vblank_sync || (ram_data_wr && !is_sprite_active))
	    {
		obj_attrib_counter = 0;
	    }
	    else if (!hcounter_bit0)
	    {
		obj_attrib_counter = ((obj_attrib_counter + 1) & 0x7);
	    }
	}
    }

    /*
    void K051960::tickObjAddr()
    {
	if (!res_sync)
	{
	    l101 = false;
	}
	else if (clk6_rise)
	{
	    if (!hrst_delay6)
	    {
		l101 = false;
	    }
	}
    }
    */

    void K051960::tickObjAddr()
    {
	if (current_pins.pin_te0)
	{
	    ram_addr = ((current_pins.ram_addr >> 3) & 0x7F);
	}
	else if (!is_vblank_sync)
	{
	    // TODO: Implement remaining RAM address indexes
	    /*
	    if (clk_m3)
	    {
		ram_addr = getRAMAddr(6);
	    }
	    else if (l101)
	    {
		if (l126)
		{
		    ram_addr = getRAMAddr(2);
		}
		else
		{
		    ram_addr = getRAMAddr(3);
		}
	    }
	    else if (l126)
	    {
		ram_addr = getRAMAddr(4);
	    }
	    else
	    {
		ram_addr = getRAMAddr(5);
	    }
	    */

	    ram_addr = 0;
	}
	else if (ram_data_wr)
	{
	    ram_addr = sprite_priority;
	}
	else
	{
	    ram_addr = (((obj_counter_msb & 0x7) << 4) | obj_counter_lsb);
	}

	if (clk3_rise)
	{
	    ram_din_latch = current_pins.ram_data;
	}

	if (ram_data_wr)
	{
	    ram_din = ram_din_latch;
	}
	else
	{
	    ram_din = 0;
	}
    }

    void K051960::tickObjRAM()
    {
	if (ram_clk_edge)
	{
	    if (ram_f_we)
	    {
		// cout << "Writing value of " << hex << int(ram_din) << " to RAM F address of " << hex << int(ram_addr) << endl;
		ram_f.at(ram_addr & 0x7F) = ram_din;
	    }

	    if (ram_e_we)
	    {
		// cout << "Writing value of " << hex << int(ram_din) << " to RAM E address of " << hex << int(ram_addr) << endl;
		ram_e.at(ram_addr & 0x7F) = ram_din;
	    }

	    if (ram_c_we)
	    {
		// cout << "Writing value of " << hex << int(ram_din) << " to RAM C address of " << hex << int(ram_addr) << endl;
		ram_c.at(ram_addr & 0x7F) = ram_din;
	    }

	    if (ram_d_we)
	    {
		// cout << "Writing value of " << hex << int(ram_din) << " to RAM D address of " << hex << int(ram_addr) << endl;
		ram_d.at(ram_addr & 0x7F) = ram_din;
	    }

	    if (ram_b_we)
	    {
		// cout << "Writing value of " << hex << int(ram_din) << " to RAM B address of " << hex << int(ram_addr) << endl;
		ram_b.at(ram_addr & 0x7F) = ram_din;
	    }

	    if (ram_a_we)
	    {
		// cout << "Writing value of " << hex << int(ram_din) << " to RAM A address of " << hex << int(ram_addr) << endl;
		ram_a.at(ram_addr & 0x7F) = ram_din;
	    }

	    if (ram_g_we)
	    {
		// cout << "Writing value of " << hex << int(ram_din) << " to RAM G address of " << hex << int(ram_addr) << endl;
		ram_g.at(ram_addr & 0x7F) = ram_din;
	    }
	}
    }

    void K051960::tickObjPins()
    {
	if (is_cpu_access)
	{
	    current_pins.pin_ram_oe = current_pins.pin_nrd;
	    current_pins.ram_addr = (current_pins.addr & 0x3FF);
	}
	else
	{
	    current_pins.pin_ram_oe = false;
	    current_pins.ram_addr = (((obj_counter_msb & 0x7) << 7) | (obj_counter_lsb << 3) | obj_attrib_counter);
	}
    }
};