#ifndef BEEKONAMI_K054000
#define BEEKONAMI_K054000

#include <iostream>
#include <cstdint>
#include <array>
#include <algorithm>
using namespace std;

namespace beekonami
{
    struct K054000Pins
    {
	uint8_t addr = 0;
	uint8_t data = 0;
	bool pin_io_sel = true; // P20
	bool pin_rnw = true; // P22
	bool pin_cs = false; // P26
	bool pin_nwr = true; // P27
    };

    class K054000
    {
	public:
	    K054000();
	    ~K054000();

	    void init();
	    void tickCLK(bool clk);

	    K054000Pins &getPins()
	    {
		return current_pins;
	    }

	private:
	    template<typename T>
	    bool testbit(T reg, size_t bit)
	    {
		return ((reg >> bit) & 0x1) ? true : false;
	    }

	    K054000Pins current_pins;
    };
};

#endif // BEEKONAMI_K054000