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
	bool pin_nres = true;
	bool pin_rst = true;
	uint16_t addr = 0;
	uint8_t data = 0;
	bool pin_m12 = false;
	bool pin_pe = false;
	bool pin_pq = false;
	bool pin_rden = false;
	bool pin_wren = false;
	bool pin_wrp = false;
	bool pin_firq = true;
	bool pin_irq = true;
	bool pin_nmi = true;
	bool pin_nrd = true;
	bool pin_vcs = true;
	bool pin_ben = false;
	bool pin_crcs = true;
	bool pin_rmrd = false;
	bool pin_hvot = false;
	bool pin_za1h = false;
	bool pin_za2h = false;
	bool pin_za4h = false;
	bool pin_zb1h = false;
	bool pin_zb2h = false;
	bool pin_zb4h = false;
	bool pin_rcs0 = true;
	bool pin_rcs1 = true;
	bool pin_roe0 = true;
	bool pin_roe1 = true;
	bool pin_roe2 = true;
	bool pin_rwe0 = true;
	bool pin_rwe1 = true;
	bool pin_rwe2 = true;
	uint16_t vram_addr = 0;
	uint16_t vram_data = 0;
	uint16_t rom_addr = 0;
	uint8_t rom_color = 0;
	bool pin_cab1 = false;
	bool pin_cab2 = false;
	bool pin_vde = false;
	bool pin_test = false;
    };

    class K052109
    {
	public:
	    K052109();
	    ~K052109();

	    void init();
	    void tickCLK(bool clk);

	    K052109Pins &getPins()
	    {
		return current_pins;
	    }

	private:
	    template<typename T>
	    bool testbit(T reg, size_t bit)
	    {
		return ((reg >> bit) & 0x1) ? true : false;
	    }

	    K052109Pins current_pins;

	    bool clk_rise = false;
	    bool clk_fall = false;
	    bool prev_clk = false;

	    void tickInternal();
    };
};

#endif // BEEKONAMI_K052109