#ifndef BEEKONAMI_K007232_H
#define BEEKONAMI_K007232_H

#include <iostream>
#include <cstdint>
using namespace std;

namespace beekonami
{
    struct K007232Pins
    {
	bool pin_nres = true; // NRES
	bool pin_dacs = true; // DACS
	bool pin_nrd = true; // NRD
	bool pin_nrcs = true; // NRCS
	bool pin_slev = true; // SLEV
	bool pin_ck2m = false; // CK2M
	bool pin_nq = false; // NQ
	bool pin_ne = false; // NE
	uint8_t addr = 0; // AB0-AB3
	uint8_t data = 0; // DB0-DB7
	uint32_t pcm_addr = 0; // SA0-SA17
	uint8_t pcm_data = 0; // RAM0-RAM7
	uint8_t cha_output = 0; // ASD0-ASD6
	uint8_t chb_output = 0; // BSD0-BSD6
    };

    class K007232
    {
	public:
	    K007232();
	    ~K007232();

	    void init();
	    void reset();
	    void tickCLK(bool clk);

	    K007232Pins &getPins()
	    {
		return current_pins;
	    }

	private:
	    template<typename T>
	    bool testbit(T reg, int bit)
	    {
		return ((reg >> bit) & 0x1) ? true : false;
	    }

	    K007232Pins current_pins;

	    bool clk_rise = false;
	    bool clk_fall = false;
	    bool prev_clk = false;

	    void tickInternal();
	    void tickIO();
	    void tickClocks();
	    void tickPrescaler();
	    void tickCounters();
	    void tickReads();

	    bool prev_dacs = true;

	    bool reg5_wr = false;
	    bool regB_wr = false;

	    bool e74 = false;
	    bool f74 = false;

	    bool clk_d4 = false;
	    bool clk_d4_rise = false;
	    bool clk_d4_fall = false;
	    bool prev_clk_d4 = false;
	    uint8_t clk_div = 0;

	    uint8_t clk_div_10 = 0;

	    bool clk_d1024 = false;
	    bool clk_d1024_rise = false;
	    bool prev_clk_d1024 = false;

	    bool f74_rise = false;
	    bool prev_f74 = false;

	    bool ram_dir = false;
	    bool data_dir = false;

	    bool is_sel_ch2 = false;

	    bool ch1_pre_write = false;
	    bool ch2_pre_write = false;

	    bool ch1_reset_pre = false;
	    bool ch2_reset_pre = false;

	    uint16_t ch1_prescale_data = 0;
	    uint8_t ch1_prescale_cntr_low = 0;
	    uint8_t ch1_prescale_cntr_mid = 0;
	    uint8_t ch1_prescale_cntr_high = 0;
	    uint8_t ch1_prescale_ctrl = 0;

	    bool ch1_low_carry = false;
	    bool ch1_mid_carry = false;
	    bool ch1_high_cin = false;
	    bool ch1_high_carry = false;

	    bool ch1_prescale_carry = false;
	    bool ch1_prescale_load = false;

	    bool ch1_cntr_reset = false;
	    bool ch1_cntr_reload = false;
	    bool ch1_trigger = false;
	    bool is_ch1_stop = false;
	    bool is_ch1_loop = false;
	    uint8_t ch1_counter_low = 0;
	    uint8_t ch1_counter_mid1 = 0;
	    uint8_t ch1_counter_mid2 = 0;
	    uint8_t ch1_counter_high = 0;
	    uint32_t ch1_counter_data = 0;

	    uint16_t ch2_prescale_data = 0;
	    uint8_t ch2_prescale_cntr_low = 0;
	    uint8_t ch2_prescale_cntr_mid = 0;
	    uint8_t ch2_prescale_cntr_high = 0;
	    uint8_t ch2_prescale_ctrl = 0;

	    bool ch2_low_carry = false;
	    bool ch2_mid_carry = false;
	    bool ch2_high_cin = false;
	    bool ch2_high_carry = false;

	    bool ch2_prescale_carry = false;
	    bool ch2_prescale_load = false;

	    bool ch2_cntr_reset = false;
	    bool ch2_cntr_reload = false;
	    bool ch2_trigger = false;
	    bool is_ch2_stop = false;
	    bool is_ch2_loop = false;
	    uint8_t ch2_counter_low = 0;
	    uint8_t ch2_counter_mid1 = 0;
	    uint8_t ch2_counter_mid2 = 0;
	    uint8_t ch2_counter_high = 0;
	    uint32_t ch2_counter_data = 0;
    };
};


#endif // BEEKONAMI_K007232_H