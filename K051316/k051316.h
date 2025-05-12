#ifndef BEEKONAMI_K051316
#define BEEKONAMI_K051316

#include <iostream>
#include <cstdint>
#include <array>
using namespace std;

namespace beekonami
{
    struct K051316Pins
    {
	bool pin_m12 = true;
	bool pin_m6 = true;
	bool pin_iocs = true;
	bool pin_vrcs = true;
	bool pin_oblk = false;
	uint16_t addr = 0;
	uint8_t data = 0;
	uint32_t rom_addr = 0;
	bool pin_rnw = true;
	bool pin_vscn = false;
	bool pin_hscn = false;
	bool pin_vrc = false;
	bool pin_hrc = false;
    };


    class K051316
    {
	public:
	    K051316();
	    ~K051316();

	    void init();
	    void tickCLK(bool clk, bool clk12, bool clk6);
	    void tickCLKTest(bool clk);

	    K051316Pins &getPins()
	    {
		return current_pins;
	    }

	private:
	    template<typename T>
	    bool testbit(T reg, int bit)
	    {
		return ((reg >> bit) & 0x1);
	    }

	    K051316Pins current_pins;

	    bool prev_clk = false;
	    bool clk_val = false;
	    bool clk_rise = false;

	    bool clk12 = false;
	    bool clk12_rise = false;
	    bool clk12_fall = false;
	    bool prev_clk12 = false;

	    bool clk12_delay = false;

	    bool clk6 = false;
	    bool clk6_rise = false;
	    bool clk6_fall = false;
	    bool prev_clk6 = false;

	    uint8_t clk_counter = 0;

	    void tickInternal();
	    void tickClocks();
	    void tickIO();
	    void tickSync();
	    void tickRAMLogic();
	    void tickRAM();
	    void tickAddrOutput();
	    void tickCounters();
	    void tickXCounter();
	    void tickYCounter();

	    bool is_io_write = false;
	    bool prev_io_write = false;

	    bool reg_e_d7 = false;
	    bool test_en = false;
	    bool reg_e_d5 = false;
	    bool reg_e_d4 = false;
	    bool reg_e_d3 = false;
	    bool is_yflip = false;
	    bool is_xflip = false;
	    bool reg_e_d0 = false;

	    bool ram_clk = false;
	    bool ram_clk_rise = false;
	    bool ram_clk_fall = false;
	    bool prev_ram_clk = false;

	    uint16_t ram_addr = 0;
	    uint16_t ram_write_addr = 0;

	    bool ram_l_we = false;
	    bool ram_l_we_edge = false;
	    bool prev_ram_l_we = false;

	    bool ram_r_we = false;
	    bool ram_r_we_edge = false;
	    bool prev_ram_r_we = false;

	    uint8_t ram_l_dout = 0;
	    uint8_t ram_r_dout = 0;

	    array<uint8_t, 1024> ram_left;
	    array<uint8_t, 1024> ram_right;

	    // Equivalent to R31_q in Furrtek's schematics
	    bool m6_n = false;
	    bool m6_n_edge = false;
	    bool prev_m6_n = false;

	    // Equivalent to E43_q in Furrtek's schematics
	    bool is_hrc = false;
	    bool prev_hrc = false;

	    // Equivalent to E37_q in Furrtek's schematics
	    bool is_vscn = false;

	    // Equivalent to T163 in Furrtek's schematics
	    bool accum_latch = false;
	    bool accum_latch_edge = false;
	    bool prev_accum_latch = true;

	    // Equivalent to L16 in Furrtek's schematics
	    bool accum_prev_latch = false;
	    bool accum_prev_latch_edge = false;
	    bool prev_accum_prev_latch = true;

	    // Equivalent to W119 in Furrtek's schematics
	    bool is_hvblank = false;

	    // Equivalent to E34 in Furrtek's schematics
	    bool is_hvsync = false;

	    uint16_t x_start = 0;

	    uint32_t x_line_inc = 0;
	    uint32_t x_pixel_inc = 0;

	    uint32_t x_accum_a = 0;
	    uint32_t x_accum_b = 0;
	    uint32_t x_accum_out = 0;

	    uint32_t x_accum = 0;
	    uint32_t prev_x_accum = 0;

	    uint16_t y_start = 0;

	    uint32_t y_line_inc = 0;
	    uint32_t y_pixel_inc = 0;

	    uint32_t y_accum_a = 0;
	    uint32_t y_accum_b = 0;
	    uint32_t y_accum_out = 0;

	    uint32_t y_accum = 0;
	    uint32_t prev_y_accum = 0;

	    uint8_t pre_x = 0;
	    uint8_t pre_y = 0;

	    uint16_t pre_reg = 0;
	    uint16_t ram_reg = 0;

	    uint32_t tile_addr = 0;

	    uint16_t rom_read_addr = 0;

	    bool oblk_val = false;
	    bool oblk_delay = false;

    };
};

#endif // BEEKONAMI_K051316