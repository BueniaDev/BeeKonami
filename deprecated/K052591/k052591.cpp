#include "k052591.h"
using namespace beekonami;

namespace beekonami
{
    K052591::K052591()
    {

    }

    K052591::~K052591()
    {

    }

    void K052591::init()
    {
	current_pins = {};
	iram.fill(0);
	regs.fill(0);
	reg_accum = 0;
    }

    void K052591::debugOutput()
    {
	for (int i = 0; i < 8; i++)
	{
	    cout << "R" << dec << int(i) << ": 0x" << toHex(regs.at(i), 4) << endl;
	}

	cout << "RA: 0x" << toHex(reg_a, 4) << endl;
	cout << "RB: 0x" << toHex(reg_b, 4) << endl;
	cout << "ACC: 0x" << toHex(reg_accum, 4) << endl;
	cout << "IR: 0x" << toHex(inst_reg, 9) << endl;
	cout << "iram_a/PC: 0x" << toHex(iram_addr, 2) << endl;
	cout << "ZF: " << dec << int(alu_zero) << endl;
	cout << "SF: " << dec << int(alu_sign) << endl;
	cout << "CF: " << dec << int(alu_carry) << endl;
	cout << "VF: " << dec << int(alu_overflow) << endl;
	cout << endl;
    }

    void K052591::tickCLK(bool clk)
    {
	for (int i = 0; i < 2; i++)
	{
	    tickOnce(true, clk);
	    tickOnce(false, clk);
	}
    }

    void K052591::tickOnce(bool clk48, bool clk12)
    {
	delay_edge = (prev_clk48 && !clk48);
	clk12_rise = (!prev_clk12 && clk12);
	clk12_fall = (prev_clk12 && !clk12);
	tickInternal();
	prev_clk12 = clk12;
	prev_clk48 = clk48;
    }

    void K052591::tickInternal()
    {
	// TODO: Finish implementing functionality of this IC

	tickClocks();
	tickWrites();
	tickALU();
	tickIRAMLogic();
	tickExternalIO();
	tickIRAM();
    }

    void K052591::tickClocks()
    {
	// Synchronous reset logic
	if (!current_pins.pin_rst)
	{
	    rsts = false;
	}
	else if (clk12_rise)
	{
	    rsts = true;
	}

	// Internal clock signal logic
	if (!rsts)
	{
	    clk_int = true;
	    clk6 = true;
	    clk_select = false;
	}
	else if (clk12_rise)
	{
	    if (clk6)
	    {
		clk_select = current_pins.pin_21;
	    }

	    clk6 = !clk6;
	}

	if (clk_select)
	{
	    clk_int = clk12;
	}
	else
	{
	    clk_int = clk6;
	}

	clk_int_rise = (!prev_clk_int && clk_int);
	clk_int_fall = (prev_clk_int && !clk_int);
	prev_clk_int = clk_int;

	// Program start logic
	if (!rsts)
	{
	    is_run = false;
	    prev_run = false;
	}
	else if (clk_int_rise)
	{
	    prev_run = is_run;
	    is_run = current_pins.pin_start;
	}

	is_run_long = (is_run || prev_run);
    }

    void K052591::tickWrites()
    {
	// Determine if internal RAM is being written to
	// or if program is being started
	cpu_write = (!current_pins.pin_cs && current_pins.pin_nrd && !current_pins.pin_bk && !is_run_long);
	cpu_write_edge = (prev_cpu_write && !cpu_write);

	is_set_pc = (cpu_write && testbit(current_pins.addr, 9));

	// NOTE: is_set_pc needs to be delayed for loading of entrypoint to work properly
	if (delay_edge)
	{
	    is_set_pc_delayed = is_set_pc;
	}

	set_pc_edge = (prev_set_pc && !is_set_pc);

	// Data mux
	if (prev_run)
	{
	    data_mux = current_pins.ea_data;
	}
	else
	{
	    data_mux = current_pins.data;
	}

	prev_set_pc = is_set_pc;
	prev_cpu_write = cpu_write;
    }

    void K052591::tickIRAM()
    {
	// Clock for internal RAM is (!clk_int)
	if (clk_int_fall)
	{
	    if (iram_we)
	    {
		iram.at(iram_addr) = iram_din;
	    }

	    iram_dout = iram.at(iram_addr);
	}

	// Update instruction register
	if (clk_int_rise)
	{
	    inst_reg = iram_dout;
	}
    }

    void K052591::tickIRAMLogic()
    {
	// Determine if program in internal RAM is running
	if (!rsts)
	{
	    is_prog_start = true;
	}
	else if (set_pc_edge)
	{
	    is_prog_start = testbit(data_mux, 7);
	}

	is_reset_pc = (is_prog_start && !is_run);

	// IRAM byte counter logic
	is_iram_load = (!testbit(current_pins.addr, 9) && cpu_write);
	iram_load_rise = (!prev_iram_load && is_iram_load);
	iram_load_fall = (prev_iram_load && !is_iram_load);
	prev_iram_load = is_iram_load;

	if (is_set_pc)
	{
	    ld_inc_en = false;
	}
	else if (iram_load_fall)
	{
	    ld_inc_en = testbit(byte_cntr, 2);
	}

	trig_iram_wr = (iram_load_rise && (byte_cntr == 4));
	iram_wr_edge = (!prev_iram_wr && trig_iram_wr);
	prev_iram_wr = trig_iram_wr;

	if (trig_iram_wr)
	{
	    setIRAMDin(32, 4, data_mux);
	}

	if (iram_load_rise && (byte_cntr == 3))
	{
	    setIRAMDin(24, 8, data_mux);
	}

	if (iram_load_rise && (byte_cntr == 2))
	{
	    setIRAMDin(16, 8, data_mux);
	}

	if (iram_load_rise && (byte_cntr == 1))
	{
	    setIRAMDin(8, 8, data_mux);
	}

	is_byte_cntr_zero = !(iram_load_rise && (byte_cntr == 0));
	is_alu_high_byte = (testbit(inst_reg, 15) || testbit(inst_reg, 35) || !prev_run || clk_int_rise);
	iram_lsb_latch = (is_byte_cntr_zero && is_alu_high_byte);
	iram_lsb_edge = (!prev_iram_lsb && iram_lsb_latch);
	prev_iram_lsb = iram_lsb_latch;

	if (iram_lsb_edge)
	{
	    setIRAMDin(0, 8, data_mux);
	}

	// IRAM write enable logic
	if (iram_we || !rsts)
	{
	    iram_wr_sync = true;
	}
	else if (iram_wr_edge)
	{
	    iram_wr_sync = false;
	}

	if (!rsts)
	{
	    iram_we = false;
	}
	else if (clk_int_rise)
	{
	    iram_we = !iram_wr_sync;
	}

	// Adjust offset of byte written to internal RAM
	if ((byte_cntr == 5) || is_set_pc || !rsts)
	{
	    byte_cntr = 0;
	}
	else if (iram_load_fall)
	{
	    byte_cntr += 1;
	}

	// Compute address of next instruction
	if (clk_int_rise)
	{
	    iram_addr_next = (iram_addr + 1);
	}

	// Calculate condition flag
	uint8_t cond_index = getbits(inst_reg, 22, 2);

	switch (cond_index)
	{
	    case 0: is_cond_flag = alu_zero; break;
	    case 1: is_cond_flag = alu_carry; break;
	    case 2: is_cond_flag = alu_overflow; break;
	    case 3: is_cond_flag = alu_sign; break;
	    default: is_cond_flag = false; break;
	}

	// TODO: Implement remaming control-flow logic
	if (is_reset_pc)
	{
	    // Set IRAM address to 0 if resetting PC
	    iram_addr = 0;
	}
	else if (!prev_run)
	{
	    // Set IRAM address to load address if prev_run is false
	    iram_addr = ld_addr;
	}
	else if (testbit(inst_reg, 15))
	{
	    // Advance PC to next instruction
	    iram_addr = iram_addr_next;
	}
	else if (getbits(inst_reg, 24, 3) == 3)
	{
	    // Advance PC to next instruction
	    iram_addr = iram_addr_next;
	}
	else if (!testbit(inst_reg, 25) && (testbit(inst_reg, 26) || is_cond_flag))
	{
	    // Set IRAM address to immediate value
	    iram_addr = getbits(inst_reg, 16, 6);
	}
	else if (!testbit(inst_reg, 26) && !is_cond_flag)
	{
	    // Advance PC to next instruction
	    iram_addr = iram_addr_next;
	}
	else
	{
	    cout << "Modifying iram_addr:" << endl;
	    cout << "IR15: " << dec << int(testbit(inst_reg, 15)) << endl;
	    cout << "IR26-IR24: " << dec << int(getbits(inst_reg, 24, 3)) << endl;
	    cout << "IR25-IR24: " << dec << int(getbits(inst_reg, 24, 2)) << endl;
	    cout << "IR26: " << dec << int(testbit(inst_reg, 26)) << endl;
	    cout << "IR25: " << dec << int(testbit(inst_reg, 25)) << endl;
	    cout << "IR24: " << dec << int(testbit(inst_reg, 24)) << endl;
	    cout << "IR23-IR22: " << dec << int(getbits(inst_reg, 22, 2)) << endl;
	    cout << "Condition flag: " << dec << int(is_cond_flag) << endl;

	    throw runtime_error("BeeKonami error");
	}

	// Compute next load address
	if (!rsts)
	{
	    ld_addr = 0;
	}
	else if (cpu_write_edge)
	{
	    if (is_set_pc_delayed)
	    {
		ld_addr = (data_mux & 0x3F);
	    }
	    else if (ld_inc_en)
	    {
		ld_addr = ((ld_addr + 1) & 0x3F);
	    }
	}
    }

    void K052591::tickALU()
    {
	// TODO: Implement remainder of ALU

	// Optimization: Don't run ALU if internal program is not currently running
	if (!is_run_long)
	{
	    alu_pa = 0;
	    alu_pb = 0;
	    alu_output = 0;
	    return;
	}

	// Select between immediate or RAM value
	is_alu_a_imm = (testbit(inst_reg, 35) && testbit(inst_reg, 15));

	if (is_alu_a_imm)
	{
	    alu_input_a = getbits(inst_reg, 16, 13);

	    if (testbit(inst_reg, 28))
	    {
		alu_input_a |= 0xE000;
	    }
	}
	else
	{
	    alu_input_a = (((iram_din & 0xFF) << 8) | data_mux);
	}

	if (testbit(inst_reg, 35) && !testbit(inst_reg, 15))
	{
	    // Clear high byte of ALU input A
	    alu_input_a &= 0xFF;
	}

	// Calculate register A
	if (clk_int)
	{
	    reg_a = regs.at(getbits(inst_reg, 9, 3));
	}

	// Calculate register B
	if (clk_int)
	{
	    reg_b = regs.at(getbits(inst_reg, 12, 3));
	}

	// Determine appropriate value for ir[2:0]
	if (clk_int_rise)
	{
	    is_ir1_high = (getbits(inst_reg, 32, 2) == 3);
	}

	ir_2_0 = getbits(inst_reg, 0, 3);

	if (!testbit(inst_reg, 1) && (!testbit(reg_accum, 0) && is_ir1_high))
	{
	    ir_2_0 = setbit(ir_2_0, 1);
	}

	// Determine appropriate value for ir[5:3]
	if (clk_int_rise)
	{
	    is_ir3_high = ((getbits(inst_reg, 32, 2) == 1) && !testbit(alu_output, 15));
	}

	ir_5_3 = getbits(inst_reg, 3, 3);

	if (!testbit(inst_reg, 3) && is_ir3_high)
	{
	    ir_5_3 = setbit(ir_5_3, 0);
	}

	ir_8_6 = getbits(inst_reg, 6, 3);

	// Select ALU operand A based on ir[2:0]
	switch (ir_2_0)
	{
	    case 0: alu_pa = reg_a; break;
	    case 1: alu_pa = reg_a; break;
	    case 2: alu_pa = 0; break;
	    case 3: alu_pa = 0; break;
	    case 4: alu_pa = 0; break;
	    case 5: alu_pa = alu_input_a; break;
	    case 6: alu_pa = alu_input_a; break;
	    case 7: alu_pa = alu_input_a; break;
	    default: alu_pa = 0; break;
	}

	// Select ALU operand B based on ir[2:0]
	switch (ir_2_0)
	{
	    case 0: alu_pb = reg_accum; break;
	    case 1: alu_pb = reg_b; break;
	    case 2: alu_pb = reg_accum; break;
	    case 3: alu_pb = reg_b; break;
	    case 4: alu_pb = reg_a; break;
	    case 5: alu_pb = reg_a; break;
	    case 6: alu_pb = reg_accum; break;
	    case 7: alu_pb = 0; break;
	}

	alu_carry_in = false;

	if (getbits(inst_reg, 32, 2) == 1)
	{
	    alu_carry_in = testbit(ir_5_3, 0);
	}
	else
	{
	    alu_carry_in = (!testbit(inst_reg, 15) && testbit(inst_reg, 34));
	}

	// TODO: Implement arithmetic operations

	switch (ir_5_3)
	{
	    case 0:
	    {
		uint32_t alu_res = (alu_pa + alu_pb + alu_carry_in);
		alu_carry = testbit(alu_res, 16);
		alu_output = uint16_t(alu_res);
	    }
	    break;
	    case 1:
	    {
		uint32_t alu_res = (uint16_t(~alu_pa) + alu_pb + alu_carry_in);
		alu_carry = testbit(alu_res, 16);
		alu_output = uint16_t(alu_res);
	    }
	    break;
	    case 3:
	    {
		alu_output = (alu_pa | alu_pb);
		alu_carry = (alu_output < (15 + alu_carry_in));
	    }
	    break;
	    case 4:
	    {
		alu_output = (alu_pa & alu_pb);
		alu_carry = ((alu_output + alu_carry_in) > 0);
	    }
	    break;
	    case 5:
	    {
		alu_output = (uint16_t(~alu_pa) & alu_pb);
		alu_carry = ((alu_output + alu_carry_in) > 0);
	    }
	    break;
	    case 6:
	    {
		alu_output = (alu_pa ^ alu_pb);
		alu_carry = ((alu_pa + alu_carry_in) > alu_pb);
	    }
	    break;
	    case 7:
	    {
		alu_output = ~(alu_pa ^ alu_pb);
		alu_carry = ((alu_pa + alu_pb) < (15 + alu_carry_in));
	    }
	    break;
	    default:
	    {
		cout << "Unrecognized ALU operation of " << dec << int(ir_5_3) << endl;
		cout << "Input A: 0x" << toHex(alu_pa, 4) << endl;
		cout << "Input B: 0x" << toHex(alu_pb, 4) << endl;
		cout << "Carry input: " << dec << int(alu_carry_in) << endl;
		debugOutput();
		throw runtime_error("BeeKonami error");
	    }
	    break;
	}

	if (ir_5_3 >= 3)
	{
	    alu_overflow = alu_carry;
	}
	else
	{
	    uint32_t carry_res = (alu_output ^ alu_pa ^ alu_pb);
	    alu_overflow = (testbit(carry_res, 15) != testbit(carry_res, 16));
	}

	// Calculate ALU zero and sign flags
	alu_zero = (alu_output == 0);
	alu_sign = testbit(alu_output, 15);

	switch (getbits(inst_reg, 7, 2))
	{
	    case 0: reg_wr_mux = alu_output; break;
	    case 1: reg_wr_mux = alu_output; break;
	    case 3:
	    {
		reg_wr_mux = (alu_output << 1);

		if (!testbit(inst_reg, 33))
		{
		    cout << "Rotate left occured" << endl;
		    throw runtime_error("BeeKonami error");
		}
	    }
	    break;
	    default:
	    {
		cout << "Unrecognized register shift type of " << dec << int(getbits(inst_reg, 7, 2)) << endl;
		throw runtime_error("BeeKonami error");
	    }
	    break;
	}

	if (!clk_int && (getbits(inst_reg, 7, 2) != 0))
	{
	    regs.at(getbits(inst_reg, 12, 3)) = reg_wr_mux;
	}

	is_accum_update = ((ir_8_6 == 0) || (ir_8_6 == 4) || (ir_8_6 == 6));
	is_accum_wr = (clk_int || !is_accum_update);
	accum_wr_edge = (!prev_accum_wr && is_accum_wr);
	prev_accum_wr = is_accum_wr;

	if (accum_wr_edge)
	{
	    switch (getbits(inst_reg, 7, 2))
	    {
		case 0: reg_accum = alu_output; break;
		case 1: reg_accum = alu_output; break;
		default:
		{
		    cout << "Unrecognized accumulator register shift type of " << dec << int(getbits(inst_reg, 7, 2)) << endl;
		    throw runtime_error("BeeKonami error");
		}
		break;
	    }
	}
    }

    void K052591::tickExternalIO()
    {
	// /OUT0 pin logic
	if (!prev_run)
	{
	    current_pins.pin_out0 = true;
	}
	else if (clk_int_fall)
	{
	    if (testbit(inst_reg, 15) && !testbit(inst_reg, 34))
	    {
		current_pins.pin_out0 = testbit(inst_reg, 16);
	    }
	}

	// /ERCS pin logic
	is_access_eram = (!current_pins.pin_cs && current_pins.pin_bk && !is_run_long);

	if (is_access_eram || (prev_run && is_run))
	{
	    current_pins.pin_ercs = false;
	}
	else
	{
	    current_pins.pin_ercs = true;
	}

	// /ERWE pin logic
	eram_clk = (testbit(inst_reg, 15) || clk_int);
	eram_clk_edge = (!prev_eram_clk && eram_clk);
	prev_eram_clk = eram_clk;

	if (!is_run)
	{
	    is_eram_read = false;
	}
	else if (eram_clk_edge)
	{
	    is_eram_read = !testbit(inst_reg, 28);
	}

	if (!is_run)
	{
	    is_eram_write = false;
	}
	else if (eram_clk_edge)
	{
	    is_eram_write = !testbit(inst_reg, 27);
	}

	if (!rsts)
	{
	    is_eram_clk_write = true;
	}
	else if (clk12_fall)
	{
	    is_eram_clk_write = clk6;
	}

	if (!is_run_long)
	{
	    current_pins.pin_eroe = current_pins.pin_nrd;
	}
	else if (is_eram_read && !testbit(inst_reg, 29))
	{
	    current_pins.pin_eroe = false;
	}
	else
	{
	    current_pins.pin_eroe = true;
	}

	if (!is_run_long)
	{
	    current_pins.pin_erwe = !current_pins.pin_nrd;
	}
	else if (is_eram_write && !testbit(inst_reg, 29))
	{
	    if (clk_select)
	    {
		current_pins.pin_erwe = !clk12;
	    }
	    else
	    {
		current_pins.pin_erwe = is_eram_clk_write;
	    }
	}
	else
	{
	    current_pins.pin_erwe = true;
	}

	// Calculate ext_data direction
	if (is_eram_write && !testbit(inst_reg, 29))
	{
	    ext_dir = true;
	}
	else if (is_access_eram && current_pins.pin_nrd)
	{
	    ext_dir = true;
	}
	else
	{
	    ext_dir = false;
	}

	ext_addr_clk = ((getbits(inst_reg, 30, 2) != 2) || clk_int);
	ext_addr_edge = (!prev_ext_addr && ext_addr_clk);
	prev_ext_addr = ext_addr_clk;

	ext_data_clk = (testbit(inst_reg, 31) || clk_int);
	ext_data_edge = (!prev_ext_data && ext_data_clk);
	prev_ext_data = ext_data_clk;

	if (ext_addr_edge)
	{
	    ext_addr = (ext_value & 0x1FFF);
	}

	if (ext_data_edge)
	{
	    if (testbit(inst_reg, 30))
	    {
		ext_data = uint8_t(ext_value >> 8);
	    }
	    else
	    {
		ext_data = uint8_t(ext_value);
	    }
	}

	if (getbits(inst_reg, 6, 3) == 2)
	{
	    ext_value = reg_a;
	}
	else
	{
	    ext_value = alu_output;
	}

	if (!is_run)
	{
	    current_pins.ea_addr = (current_pins.addr & 0x1FFF);
	}
	else
	{
	    current_pins.ea_addr = ext_addr;
	}

	if (!prev_run)
	{
	    ext_data_out = current_pins.data;
	}
	else
	{
	    ext_data_out = ext_data;
	}

	if (ext_dir)
	{
	    current_pins.ea_data = ext_data_out;
	}
    }
};