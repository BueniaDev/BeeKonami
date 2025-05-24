#ifndef BEEKONAMI_K053251
#define BEEKONAMI_K053251

#include <iostream>
#include <cstdint>
using namespace std;

namespace beekonami
{
    struct K053251Pins
    {
	uint8_t addr = 0;
	uint8_t data = 0;
	bool pin_cs = true;
	bool pin_sel = false;
	bool pin_sdi0 = false;
	bool pin_sdi1 = false;
	uint8_t prior_in0 = 0;
	uint8_t prior_in1 = 0;
	uint8_t prior_in2 = 0;
	uint16_t color_in0 = 0;
	uint16_t color_in1 = 0;
	uint16_t color_in2 = 0;
	uint8_t color_in3 = 0;
	uint8_t color_in4 = 0;
	uint16_t color_out = 0;
	bool pin_sdo0 = false;
	bool pin_sdo1 = false;
	bool pin_brit = false;
	bool pin_ncol = true;
    };

    class K053251
    {
	public:
	    K053251();
	    ~K053251();

	    void init();
	    void tickCLK(bool clk);

	    K053251Pins &getPins()
	    {
		return current_pins;
	    }

	private:
	    template<typename T>
	    bool testbit(T reg, size_t bit)
	    {
		return ((reg >> bit) & 0x1) ? true : false;
	    }

	    K053251Pins current_pins;

	    bool clk_rise = false;
	    bool clk_fall = false;
	    bool prev_clk = false;

	    void tickInternal();

	    void tickIO();

	    bool is_write = false;
	    bool prev_write = false;
    };
};

#endif // BEEKONAMI_K053251