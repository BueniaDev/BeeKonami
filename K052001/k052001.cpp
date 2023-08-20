#include "k052001.h"
using namespace beekonami;

namespace beekonami
{
    K052001::K052001()
    {

    }

    K052001::~K052001()
    {

    }

    void K052001::init()
    {
	current_pins = {};
    }

    void K052001::tick()
    {
	tickCLK(true);
	tickCLK(false);
    }

    void K052001::latchValues()
    {
	addr = addr_next;
	current_state = current_state_next;

	rega = rega_next;
	regb = regb_next;
	reg_cc = reg_cc_next;

	regx = regx_next;
	regy = regy_next;

	reg_pc = reg_pc_next;

	instr1 = instr1_next;
	instr2 = instr2_next;
	instr3 = instr3_next;

	temp = temp_next;
	ea = ea_next;

	reg_ssp = reg_ssp_next;
	reg_usp = reg_usp_next;
    }

    void K052001::setValues()
    {
	current_pins.addr = addr_next;
	addr_next = 0xFFFF;
	current_state_next = current_state;

	instr1_next = instr1;
	instr2_next = instr2;
	instr3_next = instr3;

	alu8_a = 0;
	alu8_b = 0;
	alu8_cc = 0;
	alu8_op = None;

	alu16_a = 0;
	alu16_b = 0;
	alu16_cc = 0;
	alu16_op = None16;

	rega_next = rega;
	regb_next = regb;
	reg_cc_next = reg_cc;

	regx_next = regx;
	regy_next = regy;

	reg_ssp_next = reg_ssp;
	reg_usp_next = reg_usp;

	reg_pc_next = reg_pc;
	temp_next = temp;

	ea_next = ea;
    }

    void K052001::tickCLK(bool clk)
    {
	static bool prev_clk = false;

	if (prev_clk && !clk)
	{
	    switch (clk_phase)
	    {
		case 0: current_pins.pin_e = false; break;
		case 1: current_pins.pin_q = true; break;
		case 2: current_pins.pin_e = true; break;
		case 3: current_pins.pin_q = false; break;
	    }

	    if (current_pins.pin_dtac)
	    {
		clk_phase += 1;

		if (clk_phase == 4)
		{
		    clk_phase = 0;
		}
	    }
	}

	tickInternal();

	prev_clk = clk;
    }

    void K052001::tickInternal()
    {
	is_fetch = (prev_e && !current_pins.pin_e);
	if (prev_e && !current_pins.pin_e)
	{
	    if (!current_pins.pin_nres)
	    {
		current_state = Reset;
	    }
	    else
	    {
		latchValues();
	    }
	}

	setValues();
	execInst();

	prev_q = current_pins.pin_q;
	prev_e = current_pins.pin_e;
    }

    void K052001::execInst()
    {
	#include "instr.inl"
    }
};