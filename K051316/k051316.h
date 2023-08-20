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
	bool pin_m12 = false;
	bool pin_m6 = false;
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
	    void tickCLK(bool clk96);

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

	    bool m12_delay = false;

	    bool prev_m12 = false;
	    bool prev_m6 = true;
	    bool prev_write = false;

	    void tickInternal(bool clk);
	    void tickWrite();
	    void tickClocks();
	    void tickXCounter();
	    void tickYCounter();
	    void tickRAM();
	    void tickAddr();

	    bool ram_clk = false;
	    bool prev_ram_clk = false;

	    bool prev_ram_l_we = false;
	    bool prev_ram_r_we = false;
	    bool ram_l_we = false;
	    bool ram_r_we = false;

	    bool prev_r31 = false;
	    bool r31_q = false;

	    bool prev_e43 = false;
	    bool e43_q = false;

	    bool e37_q = false;
	    bool e34_q = false;

	    bool t114_clk = false;

	    bool prev_l16 = false;
	    bool l16_clk = false;

	    bool prev_t163 = false;
	    bool t163_clk = false;

	    uint16_t ram_addr = 0;
	    uint16_t ram_l_we_addr = 0;
	    uint16_t ram_r_we_addr = 0;

	    uint8_t ram_l_dout = 0;
	    uint8_t ram_r_dout = 0;

	    array<uint8_t, 0x400> ram_left;
	    array<uint8_t, 0x400> ram_right;

	    uint32_t x_res = 0;
	    uint32_t x_acc = 0;
	    uint32_t x_prev = 0;

	    uint32_t y_res = 0;
	    uint32_t y_acc = 0;
	    uint32_t y_prev = 0;

	    uint16_t ram_reg = 0;
	    uint16_t pre_reg = 0;
	    uint16_t ram_data = 0;
	    uint32_t render_addr = 0;

	    int oblk_delay = 0;

	    int x_reg = 0;
	    int y_reg = 0;

	    int xflip = 0;
	    int yflip = 0;

	    uint8_t reg0 = 0;
	    uint8_t reg1 = 0;
	    uint8_t reg2 = 0;
	    uint8_t reg3 = 0;
	    uint8_t reg4 = 0;
	    uint8_t reg5 = 0;
	    uint8_t reg6 = 0;
	    uint8_t reg7 = 0;
	    uint8_t reg8 = 0;
	    uint8_t reg9 = 0;
	    uint8_t regA = 0;
	    uint8_t regB = 0;
	    uint8_t regC = 0;
	    uint8_t regD = 0;
	    uint8_t regE = 0;
    };
};


#endif // BEEKONAMI_K051316