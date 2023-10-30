#include "k051316.h"
using namespace beekonami;
using namespace std;

namespace beekonami
{
    K051316::K051316()
    {

    }

    K051316::~K051316()
    {

    }

    void K051316::init()
    {
	current_pins = {};
	ram_left.fill(0);
	ram_right.fill(0);
    }

    void K051316::tickCLK(bool clk96)
    {
	tickInternal(clk96);
    }

    void K051316::tickInternal(bool clk)
    {
	if (prev_clk && !clk)
	{
	    m12_delay = current_pins.pin_m12;
	}

	tickWrite();
	tickClocks();
	tickRAM();
	tickAddr();
	tickXCounter();
	tickYCounter();

	prev_ram_l_we = ram_l_we;
	prev_ram_r_we = ram_r_we;
	prev_ram_clk = ram_clk;
	prev_t163 = t163_clk;
	prev_l16 = l16_clk;
	prev_e43 = e43_q;
	prev_r31 = r31_q;
	prev_m12 = current_pins.pin_m12;
	prev_m6 = current_pins.pin_m6;
	prev_clk = clk;
    }

    void K051316::tickClocks()
    {
	if (!prev_m12 && current_pins.pin_m12)
	{
	    r31_q = !current_pins.pin_m6;
	}

	if (prev_m6 && !current_pins.pin_m6)
	{
	    e43_q = !current_pins.pin_hrc;
	}

	if (!prev_e43 && e43_q)
	{
	    e37_q = current_pins.pin_vscn;
	}

	e34_q = !(!e43_q && e37_q);

	l16_clk = !((current_pins.pin_vrc || e37_q) && !e43_q);

	t114_clk = !(current_pins.pin_vrc && !e43_q);

	bool t65 = !(current_pins.pin_vrc || e37_q);
	bool f9 = !(current_pins.pin_hscn && e37_q);

	t163_clk = ((e43_q || t65) && (f9 || !current_pins.pin_m6));

	ram_clk = !((testbit(regE, 5) || current_pins.pin_m12) && (testbit(regE, 4) || m12_delay));

	bool r11_clk = !(current_pins.pin_rnw || current_pins.pin_vrcs || current_pins.pin_m6 || !ram_clk);

	ram_l_we = (r11_clk && testbit(current_pins.addr, 10));
	ram_r_we = (r11_clk && !testbit(current_pins.addr, 10));

	if (r31_q && !testbit(regE, 7))
	{
	    int x_val = ((x_acc >> 15) & 0x1F);
	    int y_val = ((y_acc >> 15) & 0x1F);
	    ram_addr = ((y_val << 5) | x_val);
	}
	else
	{
	    ram_addr = (current_pins.addr & 0x3FF);
	}
    }

    void K051316::tickXCounter()
    {
	uint8_t x_start_lsb = 0;
	uint8_t x_start_hsb = 0;
	uint8_t x_start_msb = 0;

	uint8_t x_add_lsb = 0;
	uint8_t x_add_hsb = 0;
	uint8_t x_add_msb = 0;

	if (!t114_clk)
	{
	    x_start_lsb = 0;
	    x_start_hsb = reg1;
	    x_start_msb = reg0;

	    x_add_lsb = 0;
	    x_add_hsb = 0;
	    x_add_msb = 0;
	}
	else
	{
	    x_start_lsb = (l16_clk) ? (x_acc & 0xFF) : (x_prev & 0xFF);
	    x_start_hsb = (l16_clk) ? ((x_acc >> 8) & 0xFF) : ((x_prev >> 8) & 0xFF);
	    x_start_msb = (l16_clk) ? ((x_acc >> 16) & 0xFF) : ((x_prev >> 16) & 0xFF);

	    x_add_lsb = (e34_q) ? reg3 : reg5;
	    x_add_hsb = (e34_q) ? reg2 : reg4;
	    uint8_t sign_val = (e34_q) ? reg2 : reg4;
	    x_add_msb = testbit(sign_val, 7) ? 0xFF : 0x00;
	}

	uint32_t x_start = ((x_start_msb << 16) | (x_start_hsb << 8) | x_start_lsb);
	uint32_t x_add = ((x_add_msb << 16) | (x_add_hsb << 8) | x_add_lsb);

	if (!prev_l16 && l16_clk)
	{
	    x_prev = x_res;
	}

	if (!prev_t163 && t163_clk)
	{
	    x_acc = x_res;
	}

	x_res = ((x_start + x_add) & 0xFFFFFF);
    }

    void K051316::tickYCounter()
    {
	uint8_t y_start_lsb = 0;
	uint8_t y_start_hsb = 0;
	uint8_t y_start_msb = 0;

	uint8_t y_add_lsb = 0;
	uint8_t y_add_hsb = 0;
	uint8_t y_add_msb = 0;

	if (!t114_clk)
	{
	    y_start_lsb = 0;
	    y_start_hsb = reg7;
	    y_start_msb = reg6;

	    y_add_lsb = 0;
	    y_add_hsb = 0;
	    y_add_msb = 0;
	}
	else
	{
	    y_start_lsb = (l16_clk) ? (y_acc & 0xFF) : (y_prev & 0xFF);
	    y_start_hsb = (l16_clk) ? ((y_acc >> 8) & 0xFF) : ((y_prev >> 8) & 0xFF);
	    y_start_msb = (l16_clk) ? ((y_acc >> 16) & 0xFF) : ((y_prev >> 16) & 0xFF);

	    y_add_lsb = (e34_q) ? reg9 : regB;
	    y_add_hsb = (e34_q) ? reg8 : regA;
	    uint8_t sign_val = (e34_q) ? reg8 : regA;
	    y_add_msb = testbit(sign_val, 7) ? 0xFF : 0x00;
	}

	uint32_t y_start = ((y_start_msb << 16) | (y_start_hsb << 8) | y_start_lsb);
	uint32_t y_add = ((y_add_msb << 16) | (y_add_hsb << 8) | y_add_lsb);

	if (!prev_l16 && l16_clk)
	{
	    y_prev = y_res;
	}

	if (!prev_t163 && t163_clk)
	{
	    y_acc = y_res;
	}

	y_res = ((y_start + y_add) & 0xFFFFFF);
    }

    void K051316::tickRAM()
    {
	if (!prev_ram_clk && ram_clk)
	{
	    ram_l_we_addr = ram_addr;
	    ram_r_we_addr = ram_addr;

	    ram_l_dout = ram_left.at(ram_addr);
	    ram_r_dout = ram_right.at(ram_addr);
	}

	if (!prev_ram_l_we && ram_l_we)
	{
	    ram_left.at(ram_l_we_addr) = current_pins.data;
	}

	if (!prev_ram_r_we && ram_r_we)
	{
	    ram_right.at(ram_r_we_addr) = current_pins.data;
	}

	bool db_dir = !(!current_pins.pin_vrcs && current_pins.pin_rnw);

	if (!db_dir)
	{
	    uint8_t ram_val = testbit(current_pins.addr, 10) ? ram_l_dout : ram_r_dout;
	    current_pins.data = ram_val;
	}
    }

    void K051316::tickAddr()
    {
	if (prev_m6 && !current_pins.pin_m6)
	{
	    int x_oblk = ((x_acc >> 20) & 0xF);
	    int y_oblk = ((y_acc >> 20) & 0xF);

	    bool is_oblk = !((x_oblk == 0) && (y_oblk == 0));

	    oblk_delay = ((testbit(oblk_delay, 0) << 1) | !is_oblk);

	    xflip = x_reg;
	    yflip = y_reg;

	    if (testbit(regE, 2) && testbit(ram_reg, 15))
	    {
		yflip = (0xF - yflip);
	    }

	    if (testbit(regE, 1) && testbit(ram_reg, 14))
	    {
		xflip = (0xF - xflip);
	    }

	    x_reg = ((x_acc >> 11) & 0xF);
	    y_reg = ((y_acc >> 11) & 0xF);

	    render_addr = ((pre_reg << 8) | (yflip << 4) | xflip);
	}

	if (prev_ram_clk && !ram_clk)
	{
	    ram_reg = ((ram_l_dout << 8) | ram_r_dout);
	}

	if (!prev_r31 && r31_q)
	{
	    pre_reg = ram_reg;
	}

	if (prev_r31 && !r31_q)
	{
	    ram_data = ram_reg;
	}


	uint32_t rom_addr = 0;

	if (!testbit(regE, 6))
	{
	    if (!testbit(regE, 0))
	    {
		rom_addr = ((regC << 19) | (regD << 11) | current_pins.addr);
		if (testbit(regE, 7))
		{
		    uint8_t ram_val = testbit(current_pins.addr, 10) ? ram_l_dout : ram_r_dout;
		    rom_addr = ((rom_addr & 0xFFFF) | (ram_val << 16));
		}
	    }
	    else
	    {
		rom_addr = render_addr;
	    }
	}
	else
	{
	    rom_addr = testbit(regE, 0) ? x_acc : y_acc;
	}

	current_pins.rom_addr = rom_addr;

	bool is_oblk = false;

	if (testbit(regE, 3))
	{
	    is_oblk = !ram_clk;
	}
	else
	{
	    is_oblk = !testbit(oblk_delay, 1);
	}

	current_pins.pin_oblk = is_oblk;
    }

    void K051316::tickWrite()
    {
	bool io_write = !(current_pins.pin_iocs || current_pins.pin_rnw);

	if (!prev_write && io_write)
	{
	    switch (current_pins.addr & 0xF)
	    {
		case 0x0: reg0 = current_pins.data; break;
		case 0x1: reg1 = current_pins.data; break;
		case 0x2: reg2 = current_pins.data; break;
		case 0x3: reg3 = current_pins.data; break;
		case 0x4: reg4 = current_pins.data; break;
		case 0x5: reg5 = current_pins.data; break;
		case 0x6: reg6 = current_pins.data; break;
		case 0x7: reg7 = current_pins.data; break;
		case 0x8: reg8 = current_pins.data; break;
		case 0x9: reg9 = current_pins.data; break;
		case 0xA: regA = current_pins.data; break;
		case 0xB: regB = current_pins.data; break;
		case 0xC: regC = current_pins.data; break;
		case 0xD: regD = current_pins.data; break;
		case 0xE: regE = current_pins.data; break;
	    }
	}

	prev_write = io_write;
    }
};