#ifndef BEEKONAMI_K053260_H
#define BEEKONAMI_K053260_H

#include <iostream>
#include <fstream>
#include <cstdint>
#include <array>
using namespace std;

namespace beekonami
{
    struct K053260Pins
    {
	bool pin_res = true; // RES
	bool pin_ne = false; // NE
	bool pin_nq = false; // NQ
	bool pin_sub_cs = true; // CS
	bool pin_sub_rw = true; // RW
	uint8_t sub_addr = 0; // AB0-AB5
	uint8_t sub_data = 0; // DB0-DB7
	bool pin_st1 = false; // ST1
	bool pin_st2 = false; // ST2
	bool pin_stbi = false; // STBI
	bool pin_aux1 = false; // AUX1
	bool pin_aux2 = false; // AUX2
	bool pin_sy = false; // SY
	bool pin_sh1 = false; // SH1
	bool pin_sh2 = false; // SH2
	bool pin_so = false; // SO
	bool pin_main_cs = true; // MCS
	bool pin_main_rw = true; // MRW
	bool pin_main_a0 = false; // MA0
	uint8_t main_data = 0; // MDB0-MDB7
	bool pin_rwp = false; // RWP
	uint32_t rom_addr = 0; // RA0-RA20
	uint8_t rom_data = 0; // RD0-RD7
	bool pin_tim2 = false; // TIM2
    };

    class K053260
    {
	public:
	    K053260();
	    ~K053260();

	    void init();
	    void reset();
	    void tickCLK(bool clk);

	    K053260Pins &getPins()
	    {
		return current_pins;
	    }

	private:
	    K053260Pins current_pins;

	    template<typename T>
	    bool testbit(T reg, size_t bit)
	    {
		return ((reg >> bit) & 0x1) ? true : false;
	    }

	    bool clk_val = false;
	    bool prev_clk = false;
	    bool clk_rise = false;
	    bool clk_fall = false;

	    void tickInternal();

	    void tickReset();
	    void tickClocks();
	    void tickMainWrites();
	    void tickSubWrites();
	    void tickYMIn();
	    void tickMix();
	    void tickYMOut();

	    bool res_sync = false;

	    bool nclk2 = false;
	    bool clk4 = false;
	    bool clk8 = false;
	    bool nclk8 = false;
	    bool clk16 = false;
	    bool nclk16 = false;
	    bool clk32 = false;
	    bool nclk32 = false;
	    bool nclk64 = false;
	    bool clk64 = false;

	    bool nclk2_edge = false;
	    bool prev_nclk2 = false;

	    bool ym_load = false;
	    bool ym_load_edge = false;
	    bool prev_ym_load = false;

	    uint8_t res_sr = 0;

	    bool prev_stbi = false;

	    uint16_t aux1_reg = 0;
	    uint16_t aux2_reg = 0;

	    uint16_t aux1_sr = 0;
	    uint16_t aux2_sr = 0;

	    bool enable_output = false;
	    bool enable_aux2 = false;
	    bool enable_aux1 = false;

	    bool mix_reset = false;

	    uint16_t decodeYM();
	    uint16_t encodeYM();

	    bool ym_mux_edge[4] = {false, false, false, false};
	    bool prev_ym_mux[4] = {false, false, false, false};
	    uint16_t ym_reg[4] = {0, 0, 0, 0};

	    uint16_t ym_mux_val = 0;
	    uint16_t ym_sample = 0;

	    bool sample_set = false;
	    bool prev_sample_set = false;

	    uint32_t mix_val = 0;

	    int16_t final_sample = 0;

	    bool ym_out_load = false;
	    bool ym_out_tick = false;
	    bool prev_ym_out_tick = false;
	    uint16_t ym_out_sample = 0;
	    bool out_so = false;

	    uint8_t ym_out_counter = 0;

	    bool prev_main_cs = true;
	    bool prev_sub_cs = true;

	    bool is_ab0_flip = false;
	    bool next_ab0_flip = false;

	    uint8_t main_data_out = 0;
	    uint8_t main_data_in = 0;
	    bool main_data_dir = false;

	    uint8_t sub_data_out = 0;
	    uint8_t sub_data_in = 0;
	    bool sub_data_dir = false;

	    uint8_t sub_to_main_a = 0;
	    uint8_t sub_to_main_b = 0;

	    uint8_t main_to_sub_a = 0;
	    uint8_t main_to_sub_b = 0;
    };
};

#endif // BEEKONAMI_K053260_H