#ifndef BEEKONAMI_K051962
#define BEEKONAMI_K051962

#include <iostream>
#include <cstdint>
#include <array>
using namespace std;

namespace beekonami
{
    struct K051962Pins
    {
	bool pin_m12 = false; // M12
	bool pin_m6 = false; // M6
	bool pin_p1h = false; // P1H
	bool pin_nres = true; // RES
	bool pin_rst = false; // RST
	bool pin_rmrd = false; // RMRD
	bool pin_crcs = true; // CRCS
	bool pin_ben = false; // BEN
	uint8_t address = 0; // AB0-AB1
	uint8_t data = 0; // DB0-DB7
	uint32_t rom_data = 0; // VC0-VC31
	uint8_t rom_color = 0; // COL0-COL7
	uint8_t za_scx = 0; // ZA1H, ZA2H and ZA4H
	uint8_t zb_scx = 0; // ZB1H, ZB2H and ZB4H
	uint8_t fix_color = 0; // DFI0-DFI7
	bool pin_nfic = true; // NFIC
	bool pin_nvbk = true; // NVBK
	bool pin_nhbk = true; // NHBK
	bool pin_ohbk = false; // OHBK
	bool pin_nvsy = true; // NVSY
	bool pin_nhsy = true; // NHSY
	bool pin_ncsy = true; // NCSY
    };

    class K051962
    {
	public:
	    K051962();
	    ~K051962();

	    void init();
	    void tickCLK(bool clk24);

	    K051962Pins &getPins()
	    {
		return current_pins;
	    }

	private:
	    template<typename T>
	    bool testbit(T reg, int bit)
	    {
		return ((reg >> bit) & 0x1) ? true : false;
	    }

	    K051962Pins current_pins;

	    bool clk = false;
	    bool prev_clk = false;

	    void tickInternal(bool clk24);

	    int clk_counter = 0;
	    bool res_sync = false;

	    bool clk24 = false;

	    bool clk12 = false;
	    bool clk6 = false;
	    bool clk3 = false;
	    bool nclk3 = true;

	    bool prev_clk12 = false;
	    bool prev_clk6 = false;

	    bool t61 = false;

	    int hcounter = 0;
	    int vcounter = 0;

	    bool line_end = false;
	    bool z99_cout = false;
	    bool z88 = false;
	    bool z81 = false;
	    bool z80 = false;

	    bool prev_pxh4 = false;
	    bool prev_pxh3 = false;
	    bool prev_pxh1 = false;

	    bool y100 = false;
	    bool prev_y100 = false;

	    bool prev_row4 = false;

	    bool vblank = false;

	    bool y151 = false;

	    void tickClocks();
	    void tickCounters();
	    void tickSync();
	    void tickOutput();

	    bool x116 = false;

	    uint32_t fix_delay_a = 0;
	    int pixel_sel_fix = 0;
	    bool x80 = false;
	    bool prev_x80 = true;

	    int fix_color = 0;

	    bool prev_t61 = true;
    };
};


#endif // BEEKONAMI_K051962