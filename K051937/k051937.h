#ifndef BEEKONAMI_K051937
#define BEEKONAMI_K051937

#include <iostream>
#include <cstdint>
#include <array>
using namespace std;

namespace beekonami
{
    struct K051937Pins
    {
	bool pin_nres = true;
	bool pin_obcs = true;
	bool pin_nrd = true;
	uint16_t addr = 0;
	uint8_t data = 0;
	bool pin_m12 = false;
	bool pin_m6 = false;
	bool pin_p1h = false;
	bool pin_p2h = false;
	bool pin_hvin = false;
	bool pin_hvot = false;
	bool pin_ncsy = true;
	bool pin_nvsy = true;
	bool pin_nhsy = true;
	bool pin_ncbk = true;
	bool pin_nvbk = true;
	bool pin_nhbk = true;
	bool pin_ohf = false;
	bool pin_oreg = false;
	bool pin_hend = false;
	bool pin_lach = false;
	bool pin_cary = false;
	uint16_t obj_xpos = 0;
	uint8_t obj_attrib = 0;
	uint32_t rom_data = 0;
	uint16_t obj_color = 0;
	bool pin_shad = false;
	bool pin_nco0 = true;
	bool pin_pcof = false;
	bool pin_ca0w = false;
	bool pin_ca1w = false;
	bool pin_ca2w = false;
	bool pin_ca3w = false;
	bool pin_raoe = false;
	bool pin_te1 = false;
	bool pin_te0 = false;
    };

    class K051937
    {
	public:
	    K051937();
	    ~K051937();

	    void init();
	    void tickCLK(bool clk);

	    K051937Pins &getPins()
	    {
		return current_pins;
	    }

	private:
	    template<typename T>
	    bool testbit(T reg, size_t bit)
	    {
		return ((reg >> bit) & 0x1) ? true : false;
	    }

	    K051937Pins current_pins;
    };
};

#endif // BEEKONAMI_K051937