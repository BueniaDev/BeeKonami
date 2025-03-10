#ifndef BEEKONAMI_K051960
#define BEEKONAMI_K051960

#include <iostream>
#include <cstdint>
#include <array>
using namespace std;

namespace beekonami
{
    struct K051960Pins
    {
	bool pin_nres = true;
	bool pin_rst = true;
	bool pin_irq = true;
	bool pin_firq = true;
	bool pin_nmi = true;
	bool pin_obcs = true;
	bool pin_nrd = true;
	bool pin_rden = false;
	bool pin_wren = false;
	bool pin_wrp = false;
	uint16_t addr = 0;
	uint8_t data = 0;
	bool pin_m12 = false;
	bool pin_m6 = false;
	bool pin_pe = false;
	bool pin_pq = false;
	bool pin_p1h = false;
	bool pin_p2h = false;
	bool pin_hvin = true;
	bool pin_hvot = true;
	bool pin_ohf = false;
	bool pin_oreg = false;
	bool pin_hend = false;
	bool pin_lach = false;
	bool pin_cary = false;
	uint16_t obj_xpos = 0;
	uint8_t obj_attrib = 0;
	uint32_t rom_addr = 0;
	bool pin_ram_wr = true;
	bool pin_ram_oe = true;
	uint16_t ram_addr = 0;
	uint8_t ram_data = 0;
	bool pin_te2 = false;
	bool pin_te1 = false;
	bool pin_te0 = false;
    };

    class K051960
    {
	public:
	    K051960();
	    ~K051960();

	    void init();
	    void tickCLK(bool clk24);

	    K051960Pins &getPins()
	    {
		return current_pins;
	    }

	private:
	    template<typename T>
	    bool testbit(T reg, int bit)
	    {
		return ((reg >> bit) & 0x1) ? true : false;
	    }

	    K051960Pins current_pins;

	    void tickOnce(bool clk96, bool clk24);
	    void tickInternal(bool clk);
	    void tickInternal();
	    void tickClocks();
	    void tickCounters();
	    void tickObjLogic();
	    void tickTest();

	    void tickHCounter();
	    void tickVCounter();
	    void tickVBlank();

	    void tickObjAttrib();
	    void tickObjAddr();
	    void tickObjPins();
	    void tickObjRAM();

	    bool delay_edge = false;
	    bool prev_clk96 = false;

	    uint8_t clk_counter = 0;

	    bool res_sync = true;
	    bool next_res_sync = true;

	    bool clk_rise = false;
	    bool clk_fall = false;
	    bool prev_clk = false;

	    bool clk6_rise = false;
	    bool prev_clk_m6 = false;

	    bool clk3_rise = false;
	    bool prev_clk_m3 = false;

	    bool prev_v16 = false;

	    bool clk_m24 = false;
	    bool clk_m12 = false;
	    bool clk_m6 = false;
	    bool clk_m3 = false;
	    bool clk_pe = false;
	    bool clk_pq = false;

	    bool clk_copy = false;
	    bool clk_copy_val = false;
	    bool clk_copy_delay = false;

	    bool clk_pe_delay = false;
	    array<bool, 3> clk_pq_delay = {{false}};

	    bool ram_clk1 = false;
	    bool ram_clk1_delay = false;
	    bool ram_clk2 = false;
	    bool ram_clk = false;

	    bool ram_clk_edge = false;
	    bool prev_ram_clk = false;

	    bool is_cpu_access = false;

	    bool hcounter_bit0 = false;
	    uint8_t hcounter_lsb = 0;
	    uint8_t hcounter_msb = 0;

	    bool vcounter_bit0 = false;
	    uint8_t vcounter_lsb = 0;
	    uint8_t vcounter_msb = 0;

	    bool is_hvot = false;

	    uint8_t test_addr = 0;
	    uint8_t test_data = 0;

	    uint8_t hvin_sr = 0;

	    bool is_hend = false;
	    bool is_hrst = false;

	    uint8_t hrst_sr = 0;
	    bool hrst_delay1 = false;
	    bool hrst_delay4 = false;
	    bool hrst_delay6 = false;

	    bool is_hcount_en = false;

	    bool hcount_lsb_carry = false;
	    bool vcount_lsb_carry = false;
	    bool vcount_msb_carry = false;

	    bool hvin_sync = false;

	    bool is_vblank = false;
	    bool prev_vblank = false;
	    bool vblank_set = false;

	    uint8_t rst_sr = 0;

	    bool is_vblank_sync = false;

	    bool prev_rst_clk = false;

	    bool is_sprite_process = false;
	    bool sprite_process_sync = false;

	    bool prev_sprite_copy = false;
	    bool is_sprite_copy = false;

	    bool sprite_copy_start = false;
	    bool vblank_sync_data = false;

	    uint8_t obj_attrib_counter = 0;

	    uint8_t obj_counter_lsb = 0;
	    uint8_t obj_counter_msb = 0;

	    bool obj_counter_lsb_carry = false;
	    bool obj_counter_msb_carry = false;

	    bool is_sprite_active = false;
	    uint8_t sprite_priority = 0;

	    bool ram_data_wr = false;

	    bool obj_counter_inc = false;

	    bool l101 = false;
	    bool l126 = false;

	    uint8_t ram_addr = 0;
	    uint8_t ram_din = 0;
	    uint8_t ram_din_latch = 0;

	    // Object attribute 1
	    bool ram_f_we = false;
	    uint8_t ram_f_dout = 0;
	    array<uint8_t, 128> ram_f = {{0}};

	    // Object attribute 2
	    bool ram_e_we = false;
	    uint8_t ram_e_dout = 0;
	    array<uint8_t, 128> ram_e = {{0}};

	    // Object attribute 3
	    bool ram_c_we = false;
	    uint8_t ram_c_dout = 0;
	    array<uint8_t, 128> ram_c = {{0}};

	    // Object attribute 4
	    bool ram_d_we = false;
	    uint8_t ram_d_dout = 0;
	    array<uint8_t, 128> ram_d = {{0}};

	    // Object attribute 5
	    bool ram_b_we = false;
	    uint8_t ram_b_dout = 0;
	    array<uint8_t, 128> ram_b = {{0}};

	    // Object attribute 6
	    bool ram_a_we = false;
	    uint8_t ram_a_dout = 0;
	    array<uint8_t, 128> ram_a = {{0}};

	    // Object attribute 7
	    bool ram_g_we = false;
	    uint8_t ram_g_dout = 0;
	    array<uint8_t, 128> ram_g = {{0}};
    };
};

#endif // BEEKONAMI_K051960