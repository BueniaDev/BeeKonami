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
	    array<uint8_t, 32> rgb8_table = 
	    {
		0x00, 0x06, 0x0F, 0x16, 0x1F, 0x25, 0x2E, 0x35,
		0x40, 0x47, 0x50, 0x57, 0x5F, 0x66, 0x6F, 0x76,
    		0x88, 0x8F, 0x98, 0x9F, 0xA7, 0xAE, 0xB7, 0xBE,
		0xC9, 0xD0, 0xD9, 0xDF, 0xE8, 0xEF, 0xF8, 0xFF
	    };
    };
};

#endif // BEEKONAMI_K052535