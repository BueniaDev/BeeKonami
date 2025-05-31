#ifndef BEEKONAMI_K051649
#define BEEKONAMI_K051649

#include <iostream>
#include <cstdint>
#include <array>
using namespace std;

namespace beekonami
{
    struct K051649Pins
    {
	bool pin_res = true;
	uint16_t addr = 0;
	uint8_t data = 0;
	bool pin_cs = true;
	bool pin_rd = true;
	bool pin_wr = true;
	bool pin_rom_cs = true;
	uint8_t rom_addr = 0;
	bool pin_fref = false;
	uint16_t audio_out = 0;
    };

    class K051649
    {
	public:
	    K051649();
	    ~K051649();

	    void init();
	    void reset();
	    void tickCLK(bool clk);

	    K051649Pins &getPins()
	    {
		return current_pins;
	    }

	private:
	    template<typename T>
	    bool testbit(T reg, size_t bit)
	    {
		return ((reg >> bit) & 0x1) ? true : false;
	    }

	    K051649Pins current_pins;

	    bool clk_rise = false;
	    bool clk_fall = false;
	    bool prev_clk = false;

	    void tickInternal();
	    void tickVRC();
	    void tickSCC();
	    void tickSCC_IO();
	    void tickSCC_Channels();
	    void tickSCC_FREF();
	    void tickMixer();

	    void tickSCC_Channel1();
	    void tickSCC_Channel2();
	    void tickSCC_Channel3();
	    void tickSCC_Channels45();

	    void tickSCC_Channel1Counter();
	    void tickSCC_Channel2Counter();
	    void tickSCC_Channel3Counter();

	    void tickSCC_Channel1Out();
	    void tickSCC_Channel2Out();
	    void tickSCC_Channel3Out();

	    void tickSCC_Channels45Timing();

	    bool is_cs = false;
	    bool is_cs_edge = false;
	    bool prev_cs = false;

	    bool is_read = false;
	    bool is_read_edge = false;
	    bool prev_read = false;

	    bool is_write = false;
	    bool is_write_edge = false;
	    bool prev_write = false;

	    bool is_scc_sel = false;

	    uint8_t scc_addr = 0;
	    uint8_t scc_ch1_addr = 0;
	    uint8_t scc_ch2_addr = 0;
	    uint8_t scc_ch3_addr = 0;
	    uint8_t scc_ch45_addr = 0;

	    uint8_t bank_reg[4] = {0, 0, 0, 0};

	    uint8_t scc_test_reg = 0;

	    array<uint8_t, 32> scc_ch1_ram;
	    array<uint8_t, 32> scc_ch2_ram;
	    array<uint8_t, 32> scc_ch3_ram;
	    array<uint8_t, 32> scc_ch45_ram;

	    uint16_t scc_ch1_freq = 0;
	    bool scc_ch1_fchange = false;

	    uint8_t scc_ch1_addr_cntr = 0;
	    bool scc_ch1_fcounter_ld = false;
	    bool scc_ch1_icounter_cnt = false;

	    uint8_t scc_ch1_cycles = 0;
	    uint8_t scc_ch1_fcounter_lo = 0;
	    uint8_t scc_ch1_fcounter_mid = 0;
	    uint8_t scc_ch1_fcounter_hi = 0;

	    bool scc_ch1_fcounter_lo_borrow = false;
	    bool scc_ch1_fcounter_mid_borrow = false;
	    bool scc_ch1_fcounter_hi_borrow = false;
	    bool scc_ch1_fcounter_hi_cnt = false;

	    uint8_t scc_ch1_icounter = 0;

	    uint8_t scc_ch1_data = 0;

	    bool scc_ch1_serial = false;
	    bool prev_ch1_serial = false;
	    bool scc_ch1_mul_rst = false;
	    bool prev_ch1_mul_rst = false;

	    uint8_t scc_ch1_vol = 0;
	    uint8_t scc_ch1_current_vol = 0;

	    uint8_t scc_ch1_weighted_vol = 0;
	    bool scc_ch1_weighted_vol_carry = 0;

	    bool scc_ch1_accshift_en = false;
	    bool prev_ch1_accshift_en = false;
	    bool scc_ch1_accshift_en_next = false;
	    bool prev_ch1_accshift_en_next = false;
	    uint8_t scc_ch1_accshift = 0;
	    uint8_t scc_ch1_accshift_next = 0;
	    uint8_t scc_ch1_lower = 0;

	    bool scc_ch1_mute = false;

	    uint16_t scc_ch2_freq = 0;
	    bool scc_ch2_fchange = false;

	    uint8_t scc_ch2_addr_cntr = 0;
	    bool scc_ch2_fcounter_ld = false;
	    bool scc_ch2_icounter_cnt = false;

	    uint8_t scc_ch2_cycles = 0;
	    uint8_t scc_ch2_fcounter_lo = 0;
	    uint8_t scc_ch2_fcounter_mid = 0;
	    uint8_t scc_ch2_fcounter_hi = 0;

	    bool scc_ch2_fcounter_lo_borrow = false;
	    bool scc_ch2_fcounter_mid_borrow = false;
	    bool scc_ch2_fcounter_hi_borrow = false;
	    bool scc_ch2_fcounter_hi_cnt = false;

	    uint8_t scc_ch2_icounter = 0;

	    uint8_t scc_ch2_data = 0;

	    bool scc_ch2_serial = false;
	    bool prev_ch2_serial = false;
	    bool scc_ch2_mul_rst = false;
	    bool prev_ch2_mul_rst = false;

	    uint8_t scc_ch2_vol = 0;
	    uint8_t scc_ch2_current_vol = 0;

	    uint8_t scc_ch2_weighted_vol = 0;
	    bool scc_ch2_weighted_vol_carry = 0;

	    bool scc_ch2_accshift_en = false;
	    bool prev_ch2_accshift_en = false;
	    bool scc_ch2_accshift_en_next = false;
	    bool prev_ch2_accshift_en_next = false;
	    uint8_t scc_ch2_accshift = 0;
	    uint8_t scc_ch2_accshift_next = 0;
	    uint8_t scc_ch2_lower = 0;

	    bool scc_ch2_mute = false;

	    uint16_t scc_ch3_freq = 0;
	    bool scc_ch3_fchange = false;

	    uint8_t scc_ch3_addr_cntr = 0;
	    bool scc_ch3_fcounter_ld = false;
	    bool scc_ch3_icounter_cnt = false;

	    uint8_t scc_ch3_cycles = 0;
	    uint8_t scc_ch3_fcounter_lo = 0;
	    uint8_t scc_ch3_fcounter_mid = 0;
	    uint8_t scc_ch3_fcounter_hi = 0;

	    bool scc_ch3_fcounter_lo_borrow = false;
	    bool scc_ch3_fcounter_mid_borrow = false;
	    bool scc_ch3_fcounter_hi_borrow = false;
	    bool scc_ch3_fcounter_hi_cnt = false;

	    uint8_t scc_ch3_icounter = 0;

	    uint8_t scc_ch3_data = 0;

	    bool scc_ch3_serial = false;
	    bool prev_ch3_serial = false;
	    bool scc_ch3_mul_rst = false;
	    bool prev_ch3_mul_rst = false;

	    uint8_t scc_ch3_vol = 0;
	    uint8_t scc_ch3_current_vol = 0;

	    uint8_t scc_ch3_weighted_vol = 0;
	    bool scc_ch3_weighted_vol_carry = 0;

	    bool scc_ch3_accshift_en = false;
	    bool prev_ch3_accshift_en = false;
	    bool scc_ch3_accshift_en_next = false;
	    bool prev_ch3_accshift_en_next = false;
	    uint8_t scc_ch3_accshift = 0;
	    uint8_t scc_ch3_accshift_next = 0;
	    uint8_t scc_ch3_lower = 0;

	    bool scc_ch3_mute = false;

	    // Channel 4
	    uint16_t scc_ch4_freq = 0;
	    bool scc_ch4_fchange = false;

	    uint8_t scc_ch4_addr_cntr = 0;
	    bool scc_ch4_fcounter_ld = false;
	    bool scc_ch4_icounter_cnt = false;

	    // Channel 5
	    uint16_t scc_ch5_freq = 0;
	    bool scc_ch5_fchange = false;

	    uint8_t scc_ch5_addr_cntr = 0;
	    bool scc_ch5_fcounter_ld = false;
	    bool scc_ch5_icounter_cnt = false;

	    // Channels 4 and 5
	    uint8_t scc_ch45_data = 0;
	    uint8_t scc_ch45_addrsel = 0;

	    uint8_t ch45_counter = 0;
	    bool ch45_clock = false;
	    bool prev_ch45_clock = false;

	    uint8_t getNextAccshift(uint8_t accshift, uint8_t weighted_vol, bool weighted_vol_carry);

	    int8_t scc_ch1_out = 0;
	    int8_t scc_ch2_out = 0;
	    int8_t scc_ch3_out = 0;
	    int8_t scc_ch4_out = 0;
	    int8_t scc_ch5_out = 0;
    };
};

#endif // BEEKONAMI_K01649