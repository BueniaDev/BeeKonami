#ifndef BEEKONAMI_K052591
#define BEEKONAMI_K052591

#include <iostream>
#include <cstdint>
#include <array>
#include <algorithm>
using namespace std;

namespace beekonami
{
    struct K052591Pins
    {
	bool pin_rst = true;
	uint16_t addr = 0;
	uint8_t data = 0;
	bool pin_nrd = true;
	bool pin_bk = false;
	bool pin_start = false;
	bool pin_cs = true;
	bool pin_out0 = true;
	bool pin_ercs = true;
	bool pin_eroe = true;
	bool pin_erwe = true;
	bool pin_21 = false;
	uint16_t ea_addr = 0;
	uint8_t ea_data = 0;
    };

    class K052591
    {
	public:
	    K052591();
	    ~K052591();

	    void init();
	    void tickCLK(bool clk, bool clk12);
	    void tickCLKTest(bool clk);

	    K052591Pins &getPins()
	    {
		return current_pins;
	    }

	private:
	    template<typename T>
	    bool testbit(T reg, size_t bit)
	    {
		return ((reg >> bit) & 0x1) ? true : false;
	    }

	    K052591Pins current_pins;
    };
};

#endif // BEEKONAMI_K052591