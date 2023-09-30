#ifndef BEEKONAMI_K052001_H
#define BEEKONAMI_K052001_H

#include <iostream>
#include <cstdint>
using namespace std;

namespace beekonami
{
    struct K052001Pins
    {
	uint32_t addr = 0;
	uint8_t data = 0;
	bool pin_e = false;
	bool pin_q = false;
	bool pin_nnmi = true;
	bool pin_nfirq = true;
	bool pin_nirq = true;
	bool pin_dtac = true;
	bool pin_nas = true;
	bool pin_rnw = false;
	bool pin_nres = true;
    };

    class K052001
    {
	enum K052001State
	{
	    Reset = 0,
	    Reset0,
	    Reset1,
	    FetchI1,
	    FetchI2,
	    BraDontCare,
	    LBraOffsetLow,
	    Imm16Low,
	    IndexedBase,
	    ExtendedAddrLow,
	    ExtendedDontCare,
	    AluEA,
	    DivZeroCheck,
	    Divide,
	};

	enum K052001AddrMode
	{
	    Invalid = 0,
	    Inherent,
	    Immediate,
	    Indexed,
	    Relative,
	};

	enum K052001AluOp
	{
	    None = 0,
	    Load8,
	    Add8,
	    Adc8,
	    Sub8,
	    Sbc8,
	    And8,
	    Or8,
	    Eor8,
	    Bit8,
	    Cmp8,
	};

	enum K052001Alu16Op
	{
	    None16 = 0,
	    Load16,
	    Cmp16,
	    Add16,
	    Sub16,
	    Lea16,
	};

	enum K052001Reg16
	{
	    RegInvalid = 0,
	    RegD,
	    RegX,
	    RegY,
	    RegU,
	    RegS
	};

	enum K052001IndexMode
	{
	    ModeNone = 0,
	    ModeExtended,
	    ModeNoOffs,
	};

	enum K052001IndexReg
	{
	    IdxRegNone = 0,
	    IdxRegX,
	    IdxRegY,
	    IdxRegU,
	    IdxRegS,
	    IdxRegPC
	};

	public:
	    K052001();
	    ~K052001();

	    void init();
	    void tick();
	    void tickCLK(bool clk);

	    bool isFetch()
	    {
		if (is_fetch)
		{
		    return (current_state == FetchI1);
		}
		else
		{
		    return false;
		}
	    }

	    K052001Pins &getPins()
	    {
		return current_pins;
	    }

	    uint8_t getA()
	    {
		return rega;
	    }

	    uint8_t getB()
	    {
		return regb;
	    }

	    uint8_t getCC()
	    {
		return reg_cc;
	    }

	    uint16_t getX()
	    {
		return regx;
	    }

	    uint16_t getY()
	    {
		return regy;
	    }

	    uint16_t getUSP()
	    {
		return reg_usp;
	    }

	    uint16_t getSSP()
	    {
		return reg_ssp;
	    }

	    uint16_t getPC()
	    {
		return reg_pc;
	    }

	private:
	    template<typename T>
	    bool testbit(T reg, int bit)
	    {
		return ((reg >> bit) & 0x1) ? true : false;
	    }

	    template<typename T>
	    T setbit(T reg, int bit)
	    {
		return (reg | (1 << bit));
	    }

	    template<typename T>
	    T resetbit(T reg, int bit)
	    {
		return (reg & ~(1 << bit));
	    }

	    template<typename T>
	    T changebit(T reg, int bit, bool is_set)
	    {
		return (is_set) ? setbit(reg, bit) : resetbit(reg, bit);
	    }

	    K052001Pins current_pins;

	    void tickInternal();
	    void execInst();

	    uint8_t rega = 0;
	    uint8_t rega_next = 0;

	    uint8_t regb = 0;
	    uint8_t regb_next = 0;

	    uint16_t regx = 0;
	    uint16_t regx_next = 0;

	    uint16_t regy = 0;
	    uint16_t regy_next = 0;

	    uint8_t reg_cc = 0;
	    uint8_t reg_cc_next = 0;

	    uint16_t reg_pc = 0;
	    uint16_t reg_pc_next = 0;

	    uint16_t reg_ssp = 0;
	    uint16_t reg_ssp_next = 0;

	    uint16_t reg_usp = 0;
	    uint16_t reg_usp_next = 0;

	    uint16_t addr = 0;
	    uint16_t addr_next = 0;

	    uint8_t instr1 = 0;
	    uint8_t instr1_next = 0;

	    uint8_t instr2 = 0;
	    uint8_t instr2_next = 0;

	    uint8_t instr3 = 0;
	    uint8_t instr3_next = 0;

	    uint32_t temp = 0;
	    uint32_t temp_next = 0;

	    uint16_t ea = 0;
	    uint16_t ea_next = 0;

	    bool e_clk = false;
	    bool q_clk = false;

	    bool prev_e = false;
	    bool prev_q = false;

	    void latchValues();
	    void setValues();

	    int clk_phase = 0;

	    bool is_fetch = false;

	    K052001State current_state = Reset;
	    K052001State current_state_next = Reset;

	    K052001AddrMode addr_mode = Invalid;

	    K052001AluOp alu8_op = None;
	    bool is_alu8_wb = false;

	    uint8_t alu8_a = 0;
	    uint8_t alu8_b = 0;
	    uint8_t alu8_cc = 0;
	    uint8_t alu8_res = 0;

	    K052001Alu16Op alu16_op = None16;
	    bool is_alu16_wb = false;

	    uint16_t alu16_a = 0;
	    uint16_t alu16_b = 0;
	    uint8_t alu16_cc = 0;
	    uint16_t alu16_res = 0;

	    K052001Reg16 alu16_reg = RegInvalid;

	    K052001IndexMode index_mode = ModeNone;
	    K052001IndexReg index_reg = IdxRegNone;

	    uint8_t alu_cc_out = 0;

	    bool is_rega = false;

	    bool is_store8 = false;
	    bool is_store_a = false;
	    bool is_jump = false;

	    bool is_branch_cond = false;

	    bool is_alu8_op = false;
	    bool is_alu16_op = false;
	    bool is_special_imm = false;

	    bool is_idx_indirect = false;

	    uint16_t divx_dividend = 0;
	    uint16_t divx_divisor = 0;
	    uint16_t divx_quotient = 0;
	    uint8_t divx_counter = 0;

	    void setState(K052001State state)
	    {
		current_state_next = state;
	    }

	    uint16_t getAddress()
	    {
		return (addr & 0xFFFF);
	    }

	    void setAddress(uint16_t address)
	    {
		addr_next = ((addr_next & 0xFF0000) | address);
	    }

	    void incAddress()
	    {
		addr_next = ((addr_next & 0xFF0000) | ((addr + 1) & 0xFFFF));
	    }

	    uint8_t getData()
	    {
		return current_pins.data;
	    }

	    void setData(uint8_t data)
	    {
		current_pins.data = data;
	    }

	    void clearAS()
	    {
		current_pins.pin_nas = false;
	    }

	    void setAS()
	    {
		current_pins.pin_nas = true;
	    }

	    void read()
	    {
		current_pins.pin_rnw = true;
	    }

	    void write()
	    {
		current_pins.pin_rnw = false;
	    }

	    bool isSign()
	    {
		return testbit(reg_cc, 3);
	    }

	    void setSign(bool is_set)
	    {
		alu_cc_out = changebit(alu_cc_out, 3, is_set);
	    }

	    bool isZero()
	    {
		return testbit(reg_cc, 2);
	    }

	    void setZero(bool is_set)
	    {
		alu_cc_out = changebit(alu_cc_out, 2, is_set);
	    }

	    bool isOverflow()
	    {
		return testbit(reg_cc, 1);
	    }

	    void setOverflow(bool is_set)
	    {
		alu_cc_out = changebit(alu_cc_out, 1, is_set);
	    }

	    bool isCarry()
	    {
		return testbit(reg_cc, 0);
	    }

	    void setCarry(bool is_set)
	    {
		alu_cc_out = changebit(alu_cc_out, 0, is_set);
	    }

	    void decodeInstruction(uint8_t instr)
	    {
		int instr_hi = ((instr >> 4) & 0xF);
		int instr_lo = (instr & 0xF);

		fetchAddrMode(instr_hi, instr_lo);
		fetchALU8(instr_hi, instr_lo);
		fetchSpecialImm(instr_hi, instr_lo);
		fetchStore8(instr);
		fetchJump(instr);
	    }

	    uint32_t calcOverflow(uint16_t opa, uint16_t opb, uint32_t res)
	    {
		return (opa ^ opb ^ res ^ (res >> 1));
	    }

	    void fetchAddrMode(int instr_hi, int instr_lo)
	    {
		switch (instr_hi)
		{
		    case 0x1:
		    case 0x2:
		    {
			switch (instr_lo)
			{
			    case 0x0:
			    case 0x1:
			    case 0x4:
			    case 0x5:
			    case 0x8:
			    case 0x9:
			    case 0xC:
			    case 0xD: addr_mode = Immediate; break;
			    default: addr_mode = Indexed; break;
			}
		    }
		    break;
		    case 0x3:
		    {
			switch (instr_lo)
			{
			    case 0x2:
			    case 0x3:
			    case 0x6:
			    case 0x7:
			    case 0x9:
			    case 0xA:
			    case 0xB: addr_mode = Indexed; break;
			    default:
			    {
				addr_mode = Immediate;
			    }
			    break;
			}
		    }
		    break;
		    case 0x4:
		    {
			switch (instr_lo)
			{
			    case 0x1:
			    case 0x3:
			    case 0x5:
			    case 0x7:
			    case 0x9:
			    case 0xB:
			    case 0xD:
			    case 0xF: addr_mode = Indexed; break;
			    default:
			    {
				addr_mode = Immediate;
			    }
			    break;
			}
		    }
		    break;
		    case 0x5:
		    {
			switch (instr_lo)
			{
			    case 0x0:
			    case 0x2:
			    case 0x4:
			    case 0x6: addr_mode = Immediate; break;
			    case 0x1:
			    case 0x3:
			    case 0x5:
			    case 0x7: addr_mode = Indexed; break;
			    default:
			    {
				cout << "Unrecognized instruction 0x5 low bits of " << hex << int(instr_lo) << endl;
				throw runtime_error("BeeKonami error");
			    }
			    break;
			}
		    }
		    break;
		    case 0x6:
		    case 0x7: addr_mode = Relative; break;
		    case 0xB:
		    {
			switch (instr_lo)
			{
			    case 0x8:
			    case 0x9:
			    case 0xA:
			    case 0xB:
			    case 0xC:
			    case 0xD:
			    case 0xE:
			    case 0xF:
			    {
				cout << "Unrecognized instruction 0xB low bits of " << hex << int(instr_lo) << endl;
				throw runtime_error("BeeKonami error");
			    }
			    break;
			    default: addr_mode = Inherent; break;
			}
		    }
		    break;
		    default:
		    {
			cout << "Unrecognized instruction high bits of " << hex << int(instr_hi) << endl;
			throw runtime_error("BeeKonami error");
		    }
		    break;
		}
	    }

	    void fetchALU8(uint8_t instr)
	    {
		int instr_hi = (instr >> 4);
		int instr_lo = (instr & 0xF);
		fetchALU8(instr_hi, instr_lo);
	    }

	    void fetchALU8(int instr_hi, int instr_lo)
	    {
		fetchRegA(instr_hi, instr_lo);

		is_alu8_wb = true;
		switch (instr_hi)
		{
		    case 0x1:
		    {
			switch (instr_lo)
			{
			    case 0x0:
			    case 0x1:
			    case 0x2:
			    case 0x3: alu8_op = Load8; break;
			    case 0x4:
			    case 0x5:
			    case 0x6:
			    case 0x7: alu8_op = Add8; break;
			    case 0x8:
			    case 0x9:
			    case 0xA:
			    case 0xB: alu8_op = Adc8; break;
			    case 0xC:
			    case 0xD:
			    case 0xE:
			    case 0xF: alu8_op = Sub8; break;
			    default: alu8_op = None; break; // This shouldn't happen
			}
		    }
		    break;
		    case 0x2:
		    {
			switch (instr_lo)
			{
			    case 0x0:
			    case 0x1:
			    case 0x2:
			    case 0x3: alu8_op = Sbc8; break;
			    case 0x4:
			    case 0x5:
			    case 0x6:
			    case 0x7: alu8_op = And8; break;
			    case 0x8:
			    case 0x9:
			    case 0xA:
			    case 0xB: alu8_op = Bit8; break;
			    case 0xC:
			    case 0xD:
			    case 0xE:
			    case 0xF: alu8_op = Eor8; break;
			    default: alu8_op = None; break; // This shouldn't happen
			}
		    }
		    break;
		    case 0x3:
		    {
			switch (instr_lo)
			{
			    case 0x0:
			    case 0x1:
			    case 0x2:
			    case 0x3: alu8_op = Or8; break;
			    case 0x4:
			    case 0x5:
			    case 0x6:
			    case 0x7: alu8_op = Cmp8; is_alu8_wb = false; break;
			    default: alu8_op = None; break;
			}
		    }
		    break;
		    case 0x4:
		    case 0x5:
		    case 0x6:
		    case 0x7: alu8_op = None; break;
		    case 0xB:
		    {
			switch (instr_lo)
			{
			    case 0x8:
			    case 0x9:
			    case 0xA:
			    case 0xB:
			    case 0xC:
			    case 0xD:
			    case 0xE:
			    case 0xF:
			    {
				cout << "Unrecognized instruction 0xB ALU 8-bit op low bits of " << hex << int(instr_lo) << endl;
				throw runtime_error("BeeKonami error");
			    }
			    break;
			    default: alu8_op = None; break;
			}
		    }
		    break;
		    default:
		    {
			cout << "Unrecognized instruction ALU 8-bit op high bits of " << hex << int(instr_hi) << endl;
			throw runtime_error("BeeKonami error");
		    }
		    break;
		}

		is_alu8_op = (alu8_op != None);
	    }

	    void fetchALU16Reg(int instr_hi, int instr_lo)
	    {
		switch (instr_hi)
		{
		    case 0x0:
		    case 0x1:
		    case 0x2:
		    case 0x3: 
		    case 0x6: 
		    case 0x7: alu16_reg = RegInvalid; break;
		    case 0x4:
		    {
			switch (instr_lo)
			{
			    case 0x0:
			    case 0x1:
			    case 0xA:
			    case 0xB: alu16_reg = RegD; break;
			    case 0x2:
			    case 0x3:
			    case 0xC:
			    case 0xD: alu16_reg = RegX; break;
			    case 0x4:
			    case 0x5:
			    case 0xE:
			    case 0xF: alu16_reg = RegY; break;
			    case 0x6:
			    case 0x7: alu16_reg = RegU; break;
			    case 0x8:
			    case 0x9: alu16_reg = RegS; break;
			    default: alu16_reg = RegInvalid; break;
			}
		    }
		    break;
		    case 0x5:
		    {
			switch (instr_lo)
			{
			    case 0x0:
			    case 0x1: alu16_reg = RegU; break;
			    case 0x2:
			    case 0x3: alu16_reg = RegS; break;
			    case 0x4:
			    case 0x5:
			    case 0x6:
			    case 0x7: alu16_reg = RegD; break;
			    default:
			    {
				cout << "Unrecognized ALU 16-bit register instruction 0x5 low bits of " << hex << int(instr_lo) << endl;
				throw runtime_error("BeeKonami error");
			    }
			    break;
			}
		    }
		    break;
		    default:
		    {
			cout << "Unrecognized ALU 16-bit register instruction high bits of " << hex << int(instr_lo) << endl;
			throw runtime_error("BeeKonami error");
		    }
		    break;
		}
	    }

	    void fetchALU16(uint8_t instr)
	    {
		int instr_hi = (instr >> 4);
		int instr_lo = (instr & 0xF);
		fetchALU16(instr_hi, instr_lo);
	    }

	    void fetchALU16(int instr_hi, int instr_lo)
	    {
		is_alu16_wb = true;
		switch (instr_hi)
		{
		    case 0x0:
		    case 0x1:
		    case 0x2:
		    case 0x3:
		    case 0x6:
		    case 0x7:
		    {
			alu16_op = None16;
		    }
		    break;
		    case 0x4:
		    {
			if (instr_lo < 0xA)
			{
			    alu16_op = Load16;
			}
			else
			{
			    alu16_op = Cmp16;
			    is_alu16_wb = false;
			}
		    }
		    break;
		    case 0x5:
		    {
			switch (instr_lo)
			{
			    case 0x0:
			    case 0x1:
			    case 0x2:
			    case 0x3:
			    {
				alu16_op = Cmp16;
				is_alu16_wb = false;
			    }
			    break;
			    case 0x4:
			    case 0x5:
			    {
				alu16_op = Add16;
			    }
			    break;
			    case 0x6:
			    case 0x7:
			    {
				alu16_op = Sub16;
			    }
			    break;
			    default:
			    {
				cout << "Unrecognized instruction ALU 16-bit op 0x5 low bits of " << hex << int(instr_lo) << endl;
				throw runtime_error("BeeKonami error");
			    }
			    break;
			}
		    }
		    break;
		    default:
		    {
			cout << "Unrecognized instruction ALU 16-bit op high bits of " << hex << int(instr_hi) << endl;
			throw runtime_error("BeeKonami error");
		    }
		    break;
		}

		fetchALU16Reg(instr_hi, instr_lo);

		is_alu16_op = (alu16_op != None16);
	    }

	    void fetchSpecialImm(int instr_hi, int instr_lo)
	    {
		switch (instr_hi)
		{
		    case 0x1:
		    case 0x2: is_special_imm = false; break;
		    case 0x3:
		    {
			switch (instr_lo)
			{
			    case 0x8:
			    case 0xC:
			    case 0xD:
			    case 0xE:
			    case 0xF: is_special_imm = true; break;
			    default: is_special_imm = false; break;
			}
		    }
		    break;
		    case 0x4:
		    case 0x5:
		    case 0x6:
		    case 0x7: is_special_imm = false; break;
		    case 0xB:
		    {
			switch (instr_lo)
			{
			    case 0x8:
			    case 0x9:
			    case 0xA:
			    case 0xB:
			    case 0xC:
			    case 0xD:
			    case 0xE:
			    case 0xF:
			    {
				cout << "Unrecognized instruction 0xB special imm low bits of " << hex << int(instr_lo) << endl;
				throw runtime_error("BeeKonami error");
			    }
			    break;
			    default: is_special_imm = false; break;
			}
		    }
		    break;
		    default:
		    {
			cout << "Unrecognized instruction special imm high bits of " << hex << int(instr_hi) << endl;
			throw runtime_error("BeeKonami error");
		    }
		    break;
		}
	    }

	    void fetchBranch(uint8_t instr)
	    {
		int cond = (((instr & 0x7) << 1) | testbit(instr, 4));

		if ((instr == 0xAA) || (instr == 0xAB))
		{
		    is_branch_cond = true;
		}
		else
		{
		    switch (cond)
		    {
			case 0x0: is_branch_cond = true; break; // AL
			case 0x1: is_branch_cond = false; break; // NV
			case 0x4: is_branch_cond = !isCarry(); break; // CC
			case 0x5: is_branch_cond = isCarry(); break; // CS
			case 0x6: is_branch_cond = !isZero(); break; // NE
			case 0x7: is_branch_cond = isZero(); break; // EQ
			case 0xF: is_branch_cond = ((isSign() != isOverflow()) || isZero()); break; // LE
			default:
			{
			    cout << "Unrecognized branch condition of " << hex << int(cond) << endl;
			    throw runtime_error("Botnami error");
			}
			break;
		    }
		}
	    }

	    void fetchStore8(uint8_t instr)
	    {
		if ((instr == 0x3A) || (instr == 0x3B))
		{
		    is_store8 = true;
		    is_store_a = (instr == 0x3A);
		}
		else
		{
		    is_store8 = false;
		    is_store_a = false;
		}
	    }

	    void fetchJump(uint8_t instr)
	    {
		if (instr == 0xA8)
		{
		    is_jump = true;
		}
		else
		{
		    is_jump = false;
		}
	    }

	    void fetchRegA(int instr_hi, int instr_lo)
	    {
		switch (instr_hi)
		{
		    case 0x1:
		    case 0x2:
		    case 0x3:
		    {
			is_rega = !testbit(instr_lo, 0);
		    }
		    break;
		    case 0x4:
		    case 0x5:
		    case 0x6:
		    case 0x7: is_rega = false; break;
		    case 0xB:
		    {
			switch (instr_lo)
			{
			    case 0x8:
			    case 0x9:
			    case 0xA:
			    case 0xB:
			    case 0xC:
			    case 0xD:
			    case 0xE:
			    case 0xF:
			    {
				cout << "Unrecognized instruction 0xB reg A imm op low bits of " << hex << int(instr_lo) << endl;
				throw runtime_error("BeeKonami error");
			    }
			    break;
			    default: is_rega = false; break;
			}
		    }
		    break;
		    default:
		    {
			cout << "Unrecognized instruction reg A imm high bits of " << hex << int(instr_hi) << endl;
			throw runtime_error("BeeKonami error");
		    }
		    break;
		}
	    }

	    K052001IndexReg getIndexReg(uint8_t instr)
	    {
		switch ((instr & 0x70))
		{
		    case 0x20: return IdxRegX; break;
		    case 0x30: return IdxRegY; break;
		    case 0x50: return IdxRegU; break;
		    case 0x60: return IdxRegS; break;
		    case 0x70: return IdxRegPC; break;
		    default: return IdxRegNone; break;
		}
	    }

	    void decodeIndexed(uint8_t instr)
	    {
		uint8_t indexed = (instr & 0xF7);
		switch (indexed)
		{
		    case 0x07:
		    {
			index_mode = ModeExtended;
			index_reg = IdxRegNone;
		    }
		    break;
		    case 0x26:
		    case 0x36:
		    case 0x46:
		    case 0x56:
		    case 0x66:
		    case 0x76:
		    {
			index_mode = ModeNoOffs;
			index_reg = getIndexReg(instr);
		    }
		    break;
		    default:
		    {
			cout << "Unrecognized indexed mode byte of " << hex << int(indexed) << endl;
			throw runtime_error("BeeKonami error");
		    }
		    break;
		}

		is_idx_indirect = testbit(instr, 3);
	    }

	    void runALU8()
	    {
		alu_cc_out = alu8_cc;

		switch (alu8_op)
		{
		    case Load8:
		    {
			alu8_res = alu8_b;
			setOverflow(false);
		    }
		    break;
		    case Cmp8:
		    {
			uint16_t res16 = (alu8_a - alu8_b);
			alu8_res = (res16 & 0xFF);
			setCarry(testbit(res16, 8));

			uint32_t overflow_res = calcOverflow(alu8_a, alu8_b, alu8_res);
			setOverflow(testbit(overflow_res, 7));
		    }
		    break;
		    case And8:
		    {
			alu8_res = (alu8_a & alu8_b);
			setOverflow(false);
		    }
		    break;
		    default:
		    {
			cout << "Unrecognized ALU 8-bit opcode of " << hex << int(alu8_op) << endl;
			throw runtime_error("BeeKonami error");
		    }
		    break;
		}

		setSign(testbit(alu8_res, 7));
		setZero(alu8_res == 0);
	    }

	    void runALU16()
	    {
		alu_cc_out = alu16_cc;

		switch (alu16_op)
		{
		    case Load16:
		    {
			alu16_res = alu16_b;
			setOverflow(false);
		    }
		    break;
		    case Cmp16:
		    {
			uint32_t res32 = (alu16_a - alu16_b);
			alu16_res = (res32 & 0xFFFF);
			setCarry(testbit(res32, 16));

			uint32_t overflow_res = calcOverflow(alu16_a, alu16_b, alu16_res);
			setOverflow(testbit(overflow_res, 15));
		    }
		    break;
		    default:
		    {
			cout << "Unrecognized ALU 16-bit opcode of " << hex << int(alu16_op) << endl;
			throw runtime_error("BeeKonami error");
		    }
		    break;
		}

		setZero(alu16_res == 0);

		if (alu16_op != Lea16)
		{
		    setSign(testbit(alu16_res, 15));
		}
	    }
    };
};

#endif // BEEKONAMI_K052001_H