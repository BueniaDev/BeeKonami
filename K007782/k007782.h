#ifndef BEEKONAMI_K007782
#define BEEKONAMI_K007782

#include <iostream>
#include <cstdint>
#include <array>
using namespace std;

namespace beekonami
{
    struct K007782Pins
    {
	bool pin_cclk = false;
	bool pin_clr = true;
	bool pin_exres = true;
	bool pin_cres = true;
	bool pin_clk2 = false;
	bool pin_hflp = false;
	bool pin_tim = false;
	bool pin_tim1 = false;
	bool pin_tim2 = false;
	bool pin_tim3 = false;
	bool pin_hsy = true;
	bool pin_cas = true;
	bool pin_ras = true;
	bool pin_wr = true;
	bool pin_x1s = false;
	bool pin_p1h = false;
	bool pin_p2h = false;
	bool pin_p4h = false;
	bool pin_e8h = false;
	bool pin_e16h = false;
	bool pin_e32h = false;
	bool pin_e64h = false;
	bool pin_e128h = false;
	bool pin_e256h = false;
	bool pin_vflp = false;
	bool pin_e1v = false;
	bool pin_e2v = false;
	bool pin_e4v = false;
	bool pin_e8v = false;
	bool pin_e16v = false;
	bool pin_e32v = false;
	bool pin_e64v = false;
	bool pin_e128v = false;
	bool pin_vsy = true;
	bool pin_vcen = false;
	bool pin_p256 = false;
	bool pin_osc = true;
	bool pin_fbrf = false;
	bool pin_syld = false;
	bool pin_vblk = false;
	bool pin_bfc = true;
	bool pin_csy = false;
	bool pin_cbk = true;
	bool pin_dma = false;
	bool pin_dmac = true;
	bool pin_dmin = false;
	bool pin_db0 = false;
	bool pin_obin = false;
	bool pin_dmc = true;
	bool pin_ocra = true;
	bool pin_vcra = true;
	bool pin_ocro = true;
	bool pin_orad = true;
	bool pin_svdt = true;
	bool pin_ordt = true;
	bool pin_tes2 = false;
	bool pin_tes1 = false;
    };

    class K007782
    {
	public:
	    K007782();
	    ~K007782();

	    void init();
	    void reset();
	    void tickCLK(bool clk);

	    K007782Pins &getPins()
	    {
		return current_pins;
	    }

	private:
	    template<typename T>
	    bool testbit(T reg, size_t bit)
	    {
		return ((reg >> bit) & 0x1) ? true : false;
	    }

	    static constexpr uint8_t start_val = uint8_t(-1);

	    K007782Pins current_pins;

	    bool clk_rise = false;
	    bool clk_fall = false;
	    bool prev_clk = false;

	    void tickInternal();
	    void tickReset();
	    void tickClocks();
	    void tickHorizontalSignals();

	    bool exres_sync = false;
	    bool cclk = false;

	    bool clk2_rise = false;
	    uint8_t clk2_counter = 0;
	    uint8_t prev_clk2_counter = start_val;

	    uint16_t hcounter = 0;
	    uint16_t prev_hcounter = 0;

	    bool p1h = false;
	    bool p2h = false;
	    bool p4h = false;
    };
};

#endif // BEEKONAMI_K007782