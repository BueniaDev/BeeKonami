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
	    void debugOutput();
	    void tickCLK(bool clk);

	    array<uint64_t, 64> getIRAM()
	    {
		return iram;
	    }

	    K052591Pins &getPins()
	    {
		return current_pins;
	    }

	    bool isDebugValid()
	    {
		return (is_run && clk_int);
	    }

	    uint16_t getReg(int reg_num) const
	    {
		if ((reg_num < 0) || (reg_num >= 8))
		{
		    cout << "Invalid register number of " << dec << int(reg_num) << endl;
		    throw runtime_error("BeeKonami error");
		    return 0;
		}

		return regs.at(reg_num);
	    }

	    uint16_t getRegA() const
	    {
		return reg_a;
	    }

	    uint16_t getRegB() const
	    {
		return reg_b;
	    }

	    uint16_t getRegAccum() const
	    {
		return reg_accum;
	    }

	    uint16_t getPC() const
	    {
		return iram_addr;
	    }

	private:
	    template<typename T>
	    bool testbit(T reg, int bit)
	    {
		bit = max<int>(bit, 0);
		return ((reg >> bit) & 0x1) ? true : false;
	    }

	    template<typename T>
	    T setbit(T reg, int bit)
	    {
		bit = max<int>(bit, 0);
		return (reg | (1 << bit));
	    }

	    template<typename T>
	    T resetbit(T reg, int bit)
	    {
		bit = max<int>(bit, 0);
		return (reg & ~(1 << bit));
	    }

	    template<typename T>
	    T changebit(T reg, int bit, bool is_set)
	    {
		if (is_set)
		{
		    return setbit(reg, bit);
		}
		else
		{
		    return resetbit(reg, bit);
		}
	    }

	    size_t getmask(int num_bits)
	    {
		num_bits = max<int>(num_bits, 0);
		return ((1 << num_bits) - 1);
	    }

	    template<typename T>
	    T getbits(T val, int start, int length = 1)
	    {
		start = max<int>(start, 0);
		return ((val >> start) & getmask(length));
	    }

	    template<typename T>
	    string toHex(T value, int precision = 0, char pad_char = '0')
	    {
		string buffer;

		// Create mask to clear the upper 4 bits after shifting right,
		// in case T is a signed type
		T mask = 1;
		mask <<= ((sizeof(T) * 8) - 4);
		mask -= 1;

		size_t size = 0;

		do
		{
		    uint32_t val = (value & 0xF);
		    char val_ch = (val < 10) ? ('0' + val) : ('a' + (val - 10));
		    buffer.push_back(val_ch);
		    value = ((value >> 4) & mask);
		    size += 1;
		} while ((value != 0));

		buffer.resize(size);

		if (precision != 0)
		{
		    buffer.resize(precision, pad_char);
		}

		string hex_str(buffer.size(), 0);
		copy(buffer.rbegin(), buffer.rend(), hex_str.begin());
		return hex_str;
	    }

	    K052591Pins current_pins;

	    void tickOnce(bool clk48, bool clk12);
	    void tickInternal();

	    void tickClocks();
	    void tickWrites();
	    void tickALU();
	    void tickIRAM();
	    void tickIRAMLogic();
	    void tickExternalIO();

	    bool delay_edge = false;
	    bool prev_clk48 = false;

	    bool clk12 = false;
	    bool clk12_rise = false;
	    bool clk12_fall = false;
	    bool prev_clk12 = false;

	    bool clk6 = false;
	    bool clk_select = false;

	    bool clk_int = false;
	    bool clk_int_rise = false;
	    bool clk_int_fall = false;
	    bool prev_clk_int = false;

	    bool rsts = false;

	    bool is_run = false;
	    bool prev_run = false;

	    bool is_run_long = false;

	    bool cpu_write = false;
	    bool prev_cpu_write = false;
	    bool cpu_write_edge = false;

	    bool is_set_pc = false;
	    bool is_set_pc_delayed = false;
	    bool set_pc_edge = false;
	    bool prev_set_pc = false;

	    bool is_reset_pc = false;

	    bool trig_iram_wr = false;
	    bool prev_iram_wr = false;
	    bool iram_wr_edge = false;

	    uint8_t data_mux = 0;

	    uint64_t inst_reg = 0;
	    bool is_branch_cond = false;

	    uint8_t ld_addr = 0;

	    uint8_t byte_cntr = 0;

	    bool is_iram_load = false;
	    bool iram_load_rise = false;
	    bool iram_load_fall = false;
	    bool prev_iram_load = false;

	    bool is_prog_start = false;

	    bool iram_lsb_latch = false;
	    bool prev_iram_lsb = true;
	    bool iram_lsb_edge = false;

	    bool is_byte_cntr_zero = false;
	    bool is_alu_high_byte = false;

	    bool is_alu_a_imm = false;
	    uint16_t alu_input_a = 0;

	    uint16_t alu_pa = 0;
	    uint16_t alu_pb = 0;
	    uint16_t alu_output = 0;
	    bool alu_zero = false;
	    bool alu_carry = false;
	    bool alu_overflow = false;
	    bool alu_sign = false;

	    bool alu_carry_in = false;

	    uint16_t reg_wr_mux = 0;

	    uint8_t ir_8_6 = 0;
	    uint8_t ir_5_3 = 0;
	    uint8_t ir_2_0 = 0;

	    bool is_ir1_high = false;
	    bool is_ir3_high = false;

	    uint16_t reg_accum = 0;
	    uint16_t reg_a = 0;
	    uint16_t reg_b = 0;
	    array<uint16_t, 8> regs;

	    bool is_accum_update = false;
	    bool is_accum_wr = false;
	    bool accum_wr_edge = false;
	    bool prev_accum_wr = false;

	    bool ld_inc_en = false;

	    bool iram_wr_sync = false;

	    uint8_t iram_addr_next = 0;
	    bool is_cond_flag = false;

	    bool iram_we = false;
	    uint8_t iram_addr = 0;
	    uint64_t iram_din = 0;
	    uint64_t iram_dout = 0;
	    array<uint64_t, 64> iram;

	    void setIRAMDin(int start, int num_bits, uint8_t data)
	    {
		start = clamp<int>(start, 0, 35);
		num_bits = clamp<int>(num_bits, 1, 8);

		if ((start + num_bits) > 36)
		{
		    num_bits = (36 - start);
		}

		uint64_t mask = (getmask(num_bits) << start);
		iram_din = ((iram_din & ~mask) | ((uint64_t(data) << start) & mask));
	    }

	    bool is_access_eram = false;

	    bool is_eram_read = false;
	    bool is_eram_write = false;

	    bool eram_clk = false;
	    bool eram_clk_edge = false;
	    bool prev_eram_clk = false;

	    bool is_eram_clk_write = false;

	    bool ext_set_addr = false;
	    bool ext_addr_clk = false;
	    bool ext_addr_edge = false;
	    bool prev_ext_addr = false;

	    bool ext_data_clk = false;
	    bool ext_data_edge = false;
	    bool prev_ext_data = false;

	    uint16_t ext_value = 0;
	    uint16_t ext_addr = 0;
	    uint8_t ext_data = 0;

	    uint8_t ext_data_out = 0;

	    bool ext_dir = false;
    };
};

#endif // BEEKONAMI_K052591