#ifndef BEEKONAMI_K052535
#define BEEKONAMI_K052535

#include <iostream>
#include <cstdint>
#include <array>
#include <algorithm>
using namespace std;

namespace beekonami
{
    struct K052535Pins
    {
	bool pin_blank_n = true;
	uint8_t data = 0; // D4 - D0 (5-bits)
	// Output signal is not analog here (obviously!)
	// so instead we output an 8-bit RGB value
	uint8_t color_out = 0;
    };

    class K052535
    {
	public:
	    K052535();
	    ~K052535();

	    void init();
	    void setRGBTable(array<uint8_t, 32> col_table);
	    void tickCLK(bool clk);

	    array<uint8_t, 32> getDefaultRGBTable()
	    {
		return rgb8_table;
	    }

	    K052535Pins &getPins()
	    {
		return current_pins;
	    }

	private:
	    K052535Pins current_pins;

	    bool prev_clk = false;
	    void tickInternal();

	    array<uint8_t, 32> rgb_table = {{0}};

	    // Default table for 5-bit to 8-bit RGB conversion
	    // (taken from custom MAME-derived netlist implementation (see "utils" folder))
	    array<uint8_t, 32> rgb8_table = 
	    {
		0x04, 0x09, 0x10, 0x16, 0x1E, 0x24, 0x2C, 0x32,
		0x3D, 0x43, 0x4C, 0x52, 0x5B, 0x61, 0x69, 0x70,
		0x87, 0x8D, 0x96, 0x9D, 0xA6, 0xAC, 0xB5, 0xBC,
		0xC8, 0xCF, 0xD8, 0xDF, 0xE8, 0xEF, 0xF8, 0xFF
	    };
    };
};

#endif // BEEKONAMI_K052535