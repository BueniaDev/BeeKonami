read();
setAS();

switch (current_state)
{
    case Reset:
    {
	setAddress(0xFFFF);

	read();
	clearAS();

	setState(Reset0);
    }
    break;
    case Reset0:
    {
	setAddress(0xFFFE);
	reg_pc_next = ((reg_pc_next & 0xFF) | (getData() << 8));
	clearAS();
	setState(Reset1);
    }
    break;
    case Reset1:
    {
	setAddress(0xFFFF);
	reg_pc_next = ((reg_pc_next & 0xFF00) | getData());
	clearAS();
	setState(FetchI1);
    }
    break;
    case FetchI1:
    {
	// TODO: Implement interrupts

	if (false)
	{
	}
	else
	{
	    setAddress(reg_pc);
	    reg_pc_next = (reg_pc + 1);
	    read();
	    instr1 = getData();

	    clearAS();
	    setState(FetchI2);
	}
    }
    break;
    case FetchI2:
    {
	incAddress();
	reg_pc_next = (reg_pc + 1);
	instr2_next = getData();

	clearAS();
	setState(FetchI1);

	decodeInstruction(instr1);

	switch (addr_mode)
	{
	    case Inherent:
	    {
		switch (instr1)
		{
		    case 0xB5:
		    {
			setAS();
			setState(DivZeroCheck);
			reg_pc_next = reg_pc;
		    }
		    break;
		    default:
		    {
			cout << "Unrecognized instruction of " << hex << int(instr1) << endl;
			throw runtime_error("Botnami error");
		    }
		    break;
		}
	    }
	    break;
	    case Indexed:
	    {
		clearAS();
		setState(IndexedBase);
	    }
	    break;
	    case Immediate:
	    {
		if (is_special_imm)
		{
		    switch (instr1)
		    {
			default:
			{
			    cout << "Unrecognized special immediate instruction of " << hex << int(instr1) << endl;
			    throw runtime_error("BeeKonami error");
			}
			break;
		    }
		}
		else if (is_alu8_op)
		{
		    if (is_rega)
		    {
			alu8_a = rega;
		    }
		    else
		    {
			alu8_a = regb;
		    }

		    alu8_b = instr2_next;
		    alu8_cc = reg_cc;

		    runALU8();

		    reg_cc_next = alu_cc_out;

		    if (is_alu8_wb)
		    {
			if (is_rega)
			{
			    rega_next = alu8_res;
			}
			else
			{
			    regb_next = alu8_res;
			}
		    }

		    clearAS();
		    setState(FetchI1);
		}
		else
		{
		    fetchALU16(instr1);

		    if (is_alu16_op)
		    {
			clearAS();
			setState(Imm16Low);
		    }
		}
	    }
	    break;
	    case Relative:
	    {
		int instr_lo = (instr1 & 0xF);

		if ((instr1 == 0xAB) || ((instr_lo >= 0x8) && (instr_lo <= 0xF)))
		{
		    clearAS();
		    setState(LBraOffsetLow);
		}
		else
		{
		    clearAS();
		    setState(BraDontCare);
		}
	    }
	    break;
	    default:
	    {
		cout << "Unrecognized addressing mode of " << dec << int(addr_mode) << endl;
		throw runtime_error("BeeKonami error");
	    }
	    break;
	}
    }
    break;
    case Imm16Low:
    {
	setAddress(reg_pc);
	reg_pc_next = (reg_pc + 1);

	alu16_cc = reg_cc;
	alu16_b = ((instr2 << 8) | getData());
	fetchALU16(instr1);

	switch (alu16_reg)
	{
	    case RegD: alu16_a = ((rega << 8) | regb); break;
	    case RegX: alu16_a = regx; break;
	    case RegY: alu16_a = regy; break;
	    case RegU: alu16_a = reg_usp; break;
	    case RegS: alu16_a = reg_ssp; break;
	    default:
	    {
		cout << "Unrecognized ALU16 register of " << dec << int(alu16_reg) << endl;
		throw runtime_error("BeeKonami error");
	    }
	    break;
	}

	runALU16();

	reg_cc_next = alu_cc_out;

	if (is_alu16_wb)
	{
	    switch (alu16_reg)
	    {
		case RegD:
		{
		    rega_next = (alu16_res >> 8);
		    regb_next = (alu16_res & 0xFF);
		}
		break;
		case RegX: regx_next = alu16_res; break;
		case RegY: regy_next = alu16_res; break;
		case RegS: reg_ssp_next = alu16_res; break;
		case RegU: reg_usp_next = alu16_res; break;
		default:
		{
		    cout << "Unrecognized ALU16 writeback register of " << dec << int(alu16_reg) << endl;
		    throw runtime_error("BeeKonami error");
		}
		break;
	    }
	}

	clearAS();
	setState(FetchI1);
    }
    break;
    case BraDontCare:
    {
	setAddress(0xFFFF);
	temp_next = reg_pc;

	fetchBranch(instr1);

	if (is_branch_cond)
	{
	    reg_pc_next = (reg_pc + int8_t(instr2));

	    if (instr1 == 0xAA)
	    {
		cout << "BSR" << endl;
		throw runtime_error("BeeKonami error");
	    }
	    else
	    {
		clearAS();
		setState(FetchI1);
	    }
	}
	else
	{
	    clearAS();
	    setState(FetchI1);
	}
    }
    break;
    case IndexedBase:
    {
	setAddress(reg_pc);
	instr3_next = getData();
	decodeIndexed(instr2);

	switch (index_reg)
	{
	    case IdxRegX: alu16_a = regx; break;
	    default:
	    {
		cout << "Unrecognized indexed register of " << dec << int(index_reg) << endl;
		throw runtime_error("Botnami error");
	    }
	    break;
	}

	alu16_op = Add16;

	switch (index_mode)
	{
	    case ModeNoOffs:
	    {
		switch (index_reg)
		{
		    case IdxRegX: ea_next = regx; break;
		    default:
		    {
			cout << "Unrecognized no-offset indexed register of " << dec << int(index_reg) << endl;
			throw runtime_error("Botnami error");
		    }
		    break;
		}

		if (is_idx_indirect)
		{
		    cout << "Index indirect instruction" << endl;
		    throw runtime_error("BeeKonami error");
		}
		else if (is_jump)
		{
		    cout << "Jump instruction" << endl;
		    throw runtime_error("BeeKonami error");
		}
		else
		{
		    setAS();
		    setState(AluEA);
		}
	    }
	    break;
	    default:
	    {
		cout << "Unrecognized indexed mode of " << dec << int(index_mode) << endl;
		throw runtime_error("BeeKonami error");
	    }
	    break;
	}
    }
    break;
    case AluEA:
    {
	fetchALU16(instr1);

	if (is_alu8_op)
	{
	    cout << "ALU 8-bit opcode" << endl;
	    throw runtime_error("BeeKonami error");
	}
	else if (is_store8)
	{
	    setAddress(ea);
	    write();

	    alu8_op = Load8;
	    alu8_a = 0;
	    alu8_cc = reg_cc;

	    if (is_store_a)
	    {
		setData(rega);
		alu8_a = rega;
	    }
	    else
	    {
		setData(regb);
		alu8_a = regb;
	    }

	    runALU8();
	    reg_cc_next = alu_cc_out;

	    clearAS();
	    setState(FetchI1);
	}
	else if (alu16_op == Load16)
	{
	    cout << "Load 16-bit opcode" << endl;
	    throw runtime_error("BeeKonami error");
	}
	else if ((is_alu16_op && (alu16_op != Load16)) && ((instr1 < 0x08) || (instr1 > 0x0B)))
	{
	    cout << "ALU 16-bit opcode" << endl;
	    throw runtime_error("BeeKonami error");
	}
	else
	{
	    switch (instr1)
	    {
		default:
		{
		    cout << "Unrecognized instruction of " << hex << int(instr1) << endl;
		    throw runtime_error("Botnami error");
		}
		break;
	    }
	}
    }
    break;
    case DivZeroCheck:
    {
	setAddress(0xFFFF);
	setAS();

	// Check for division by zero
	if (regb == 0)
	{
	    // Divide by zero, not sure what happens here
	    auto prev_alu_out = alu_cc_out;
	    setZero(true);
	    setCarry(false);
	    reg_cc_next = alu_cc_out;
	    alu_cc_out = prev_alu_out;
	    clearAS();
	    setState(FetchI1);
	}
	else
	{
	    // Set up registers for divide state
	    temp_next = regx_next;
	    regx_next = regb;
	    regb_next = 16;
	    setState(Divide);
	}
    }
    break;
    case Divide:
    {
	setAddress(0xFFFF);

	// The below division routine was based on the algorithm from the link below:
	// https://www.mattmillman.com/mcs-48-the-quest-for-16-bit-division-on-the-8-bit-cpu-which-cant-divide-anything/

	// Register usage:
	// temp = dividend
	// x = divisor
	// b = counter

	if (regb != 0)
	{
	    bool is_force = testbit(temp, 23);

	    temp_next = ((temp << 1) & 0xFFFFFF);

	    uint8_t msb = (temp_next >> 16);
	    uint8_t divisor = uint8_t(regx);

	    if ((msb >= divisor) || is_force)
	    {
		temp_next = ((((msb - divisor) << 16) | (temp_next & 0xFFFF)) + 1);
	    }

	    regb_next = (regb - 1);
	}
	else
	{
	    regx_next = (temp & 0xFFFF);
	    regb_next = (temp >> 16);
	    auto prev_alu_out = alu_cc_out;
	    setZero(regx_next == 0);
	    setCarry(testbit(temp, 7));

	    reg_cc_next = alu_cc_out;
	    alu_cc_out = prev_alu_out;
	    clearAS();
	    setState(FetchI1);
	}
    }
    break;
    default:
    {
	cout << "Unimplemented state of " << dec << int(current_state) << endl;
	throw runtime_error("BeeKonami error");
    }
    break;
}