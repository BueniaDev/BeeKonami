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
	    void tickCLK(bool clk);

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
    };
};

#endif // BEEKONAMI_K051960