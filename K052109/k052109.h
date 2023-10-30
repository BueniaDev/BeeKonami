#ifndef BEEKONAMI_K052109
#define BEEKONAMI_K052109

#include <iostream>
#include <cstdint>
#include <array>
using namespace std;

namespace beekonami
{
    struct K052109Pins
    {
	bool pin_m12 = false; // M12
	uint16_t addr = 0; // AB0-AB15
	uint8_t data = 0; // DB0-DB7
	bool pin_pe = false; // PE
	bool pin_pq = false; // PQ
	bool pin_rden = false; // RDEN
	bool pin_wren = false; // WREN
	bool pin_wrp = false; // WRP
	bool pin_nres = true; // RES
	bool pin_rst = false; // RST
	bool pin_vcs = true; // VCS
	bool pin_crcs = true; // CRCS
	bool pin_ben = false; // BEN
	bool pin_rmrd = false; // RMRD
	bool pin_nrd = true; // NRD
	bool pin_nfirq = true; // FIRQ
	bool pin_nirq = true; // IRQ
	bool pin_nnmi = true; // NMI
	bool pin_rcs0 = true; // RCS0
	bool pin_rcs1 = true; // RCS1
	bool pin_roe0 = true; // ROE0
	bool pin_roe1 = true; // ROE1
	bool pin_roe2 = true; // ROE2
	bool pin_rwe0 = true; // RWE0
	bool pin_rwe1 = true; // RWE1
	bool pin_rwe2 = true; // RWE2
	bool pin_hvot = false; // HVOT
	uint8_t za_scx = 0; // ZA1H, ZA2H and ZA4H
	uint8_t zb_scx = 0; // ZB1H, ZB2H and ZB4H
	uint16_t vram_addr = 0; // RA0-RA12
	uint16_t vram_data = 0; // VD0-VD15
	uint16_t rom_addr = 0; // VC0-VC10
	uint8_t rom_color = 0; // COL0-COL7
	bool pin_cab1 = false; // CAB1
	bool pin_cab2 = false; // CAB2
	bool pin_vde = false; // VDE
    };

    class K052109
    {
	public:
	    K052109();
	    ~K052109();

	    void init();
	    void tickCLK(bool clk24);

	    K052109Pins &getPins()
	    {
		return current_pins;
	    }

	    void setReg1C00(uint8_t data)
	    {
		reg_1C00 = data;
	    }

	    void setScrollControl(uint8_t data)
	    {
		return;
	    }

	    void setROMBanks(uint8_t bank1, uint8_t bank2)
	    {
		reg_1D80 = bank1;
		reg_1F00 = bank2;
	    }

	private:
	    template<typename T>
	    bool testbit(T reg, int bit)
	    {
		return ((reg >> bit) & 0x1) ? true : false;
	    }

	    K052109Pins current_pins;

	    void tickInternal(bool clk24);

	    void tickClocks();
	    void tickCounters();
	    void tickIO();
	    void tickOutput();

	    bool clk = false;
	    bool prev_clk = false;

	    int clk_counter = 0;
	    bool res_sync = false;

	    bool clk24 = false;

	    bool clk12 = false;
	    bool clk6 = false;
	    bool clk3 = false;
	    bool nclk3 = true;

	    bool prev_clk12 = false;
	    bool prev_clk6 = false;

	    bool aa38 = false;

	    bool k130 = false;
	    bool k148 = false;
	    bool k123 = false;
	    bool j140 = false;
	    bool j151 = false;

	    bool cpu_access_n = false;

	    int k77_q = 0;

	    int hcounter = 0;
	    int vcounter = 0;

	    uint8_t range_val = 0;

	    bool vram_cs0 = false;
	    bool vram_cs1 = false;
	    bool vram_cs2 = false;

	    bool vd_low_dir = false;
	    bool vd_high_dir = false;

	    bool reg_wr = false;

	    bool prev_res_sync = true;
	    bool prev_write_io = true;

	    uint8_t reg_1C00 = 0;
	    uint8_t reg_1D80 = 0;
	    uint8_t reg_1F00 = 0;

	    bool h1 = false;
	    bool prev_h1 = false;

	    bool h2 = false;
	    bool prev_h2 = false;

	    bool read_tile_num = false;
	    bool read_row_fix = false;

	    int render_addr = 0;
	    uint16_t render_rom_addr = 0;

	    int row_fix = 0;

	    int vc_mux = 0;

	    uint8_t color_attrib_a = 0;
	    uint8_t color_attrib_b = 0;

	    uint8_t color_mux = 0;

	    bool prev_hvot = false;

	    bool prev_j140 = false;
    };
};

#endif // BEEKONAMI_K052109