/*
    This file is part of the BeeKonami engine.
    Copyright (C) 2025 BueniaDev.

    BeeKonami is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    BeeKonami is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with BeeKonami.  If not, see <https://www.gnu.org/licenses/>.
*/

// BeeKonami-K051649 (WIP)
// Chip name: K051649 (SCC)
// Chip description: Wavetable sound chip
//
// BueniaDev's Notes:
//
// This implementation is derived from Raki's reverse-engineered schematics
// for this IC, which can be found here:
// https://github.com/ika-musume/IKASCC/blob/main/docs/K051649.pdf
// 
// The following features are currently unimplemented:
//
// FREF output pin
// Channels 4 and 5 (including channel RAM reads)
//
// However, work is being done on ALL of those fronts, so don't lose hope here!


#include "k051649.h"
using namespace beekonami;
using namespace std;

namespace beekonami
{
    K051649::K051649()
    {

    }

    K051649::~K051649()
    {

    }

    void K051649::init()
    {
	current_pins = {};

	scc_ch1_ram.fill(0);
	scc_ch2_ram.fill(0);
	scc_ch3_ram.fill(0);
    }

    void K051649::reset()
    {
	current_pins.pin_res = false;
	tickCLK(true);
	tickCLK(false);
	current_pins.pin_res = true;
    }

    void K051649::tickCLK(bool clk)
    {
	clk_rise = (!prev_clk && clk);
	clk_fall = (prev_clk && !clk);
	tickInternal();
	prev_clk = clk;
    }

    void K051649::tickInternal()
    {
	// TODO: Implement FREF pin and channels 4-5
	tickVRC();
	tickSCC();
	tickMixer();
    }

    void K051649::tickVRC()
    {
	is_cs = !current_pins.pin_cs;
	is_cs_edge = (!prev_cs && is_cs);
	prev_cs = is_cs;

	is_read = (!current_pins.pin_cs && !current_pins.pin_rd);
	is_write = (!current_pins.pin_cs && !current_pins.pin_wr);
	uint16_t addr = (current_pins.addr & 0xF8FF);
	uint8_t write_reg = ((addr >> 11) & 0x1F);

	is_scc_sel = ((write_reg == 0x13) && (bank_reg[2] == 0x3F));
	scc_addr = uint8_t(addr);

	if (!prev_write && is_write)
	{
	    switch (write_reg)
	    {
		case 0x0A: bank_reg[0] = (current_pins.data & 0x3F); break;
		case 0x0E: bank_reg[1] = (current_pins.data & 0x3F); break;
		case 0x12: bank_reg[2] = (current_pins.data & 0x3F); break;
		case 0x16: bank_reg[3] = (current_pins.data & 0x3F); break;
		default: break;
	    } 
	}

	is_write_edge = (!prev_write && is_write);
	is_read_edge = (!prev_read && is_read);

	prev_write = is_write;
	prev_read = is_read;

	current_pins.pin_rom_cs = !is_read;

	// Equivalent to ((!testbit(addr, 14) << 1) | testbit(addr, 13))
	uint8_t rom_index = (((addr >> 13) & 0x3) ^ 0x2);
	current_pins.rom_addr = bank_reg[rom_index];
    }

    void K051649::tickSCC()
    {
	tickSCC_IO();
	tickSCC_Out();
	tickSCC_Channels();
    }

    void K051649::tickSCC_IO()
    {
	scc_ch1_addr = (is_cs && is_scc_sel && (scc_addr < 0x20)) ? (scc_addr & 0x1F) : scc_ch1_addr_cntr;
	scc_ch2_addr = (is_cs && is_scc_sel && ((scc_addr >= 0x20) && (scc_addr < 0x40))) ? (scc_addr & 0x1F) : scc_ch2_addr_cntr;
	scc_ch3_addr = (is_cs && is_scc_sel && ((scc_addr >= 0x40) && (scc_addr < 0x60))) ? (scc_addr & 0x1F) : scc_ch3_addr_cntr;

	bool is_freq_range = ((scc_addr >= 0x80) && (scc_addr < 0xA0));
	uint8_t scc_freq_addr = (scc_addr & 0xF);
	scc_ch1_fchange = (is_write_edge && is_scc_sel && is_freq_range && ((scc_freq_addr == 0) || (scc_freq_addr == 1)));
	scc_ch2_fchange = (is_write_edge && is_scc_sel && is_freq_range && ((scc_freq_addr == 2) || (scc_freq_addr == 3)));
	scc_ch3_fchange = (is_write_edge && is_scc_sel && is_freq_range && ((scc_freq_addr == 4) || (scc_freq_addr == 5)));

	if (is_write_edge && is_scc_sel)
	{
	    if ((scc_addr < 0x20) && !testbit(scc_test_reg, 6))
	    {
		scc_ch1_ram[scc_ch1_addr] = current_pins.data;
	    }
	    else if (((scc_addr >= 0x20) && (scc_addr < 0x40)) && !testbit(scc_test_reg, 6))
	    {
		scc_ch2_ram[scc_ch2_addr] = current_pins.data;
	    }
	    else if (((scc_addr >= 0x40) && (scc_addr < 0x60)) && !testbit(scc_test_reg, 6))
	    {
		scc_ch3_ram[scc_ch3_addr] = current_pins.data;
	    }
	    else if (((scc_addr >= 0x60) && (scc_addr < 0x80)) && ((scc_test_reg >> 6) == 0))
	    {
		cout << "Writing value of " << hex << int(current_pins.data) << " to SCC ch4/ch5 RAM" << endl;
	    }
	    else if ((scc_addr >= 0x80) && (scc_addr < 0xA0))
	    {
		uint8_t scc_reg_addr = (scc_addr & 0xF);

		switch (scc_reg_addr)
		{
		    case 0x0:
		    {
			scc_ch1_freq = ((scc_ch1_freq & 0xF00) | current_pins.data);
		    }
		    break;
		    case 0x1:
		    {
			scc_ch1_freq = ((scc_ch1_freq & 0xFF) | ((current_pins.data & 0xF) << 8));
		    }
		    break;
		    case 0x2:
		    {
			scc_ch2_freq = ((scc_ch2_freq & 0xF00) | current_pins.data);
		    }
		    break;
		    case 0x3:
		    {
			scc_ch2_freq = ((scc_ch2_freq & 0xFF) | ((current_pins.data & 0xF) << 8));
		    }
		    break;
		    case 0x4:
		    {
			scc_ch3_freq = ((scc_ch3_freq & 0xF00) | current_pins.data);
		    }
		    break;
		    case 0x5:
		    {
			scc_ch3_freq = ((scc_ch3_freq & 0xFF) | ((current_pins.data & 0xF) << 8));
		    }
		    break;
		    case 0xA:
		    {
			scc_ch1_vol = (current_pins.data & 0xF);
		    }
		    break;
		    case 0xB:
		    {
			scc_ch2_vol = (current_pins.data & 0xF);
		    }
		    break;
		    case 0xC:
		    {
			scc_ch3_vol = (current_pins.data & 0xF);
		    }
		    break;
		    case 0xF:
		    {
			scc_ch3_mute = testbit(current_pins.data, 2);
			scc_ch2_mute = testbit(current_pins.data, 1);
			scc_ch1_mute = testbit(current_pins.data, 0);
		    }
		    break;
		}
	    }
	    else if (scc_addr >= 0xE0)
	    {
		scc_test_reg = current_pins.data;
	    }
	}

	scc_ch1_data = (is_write && is_scc_sel && (scc_addr < 0x20) && !testbit(scc_test_reg, 6)) ? current_pins.data : scc_ch1_ram[scc_ch1_addr];
	scc_ch1_fcounter_ld = !(scc_ch1_fchange || scc_ch1_icounter_cnt);

	scc_ch2_data = (is_write && is_scc_sel && ((scc_addr >= 0x20) && (scc_addr < 0x40)) && !testbit(scc_test_reg, 6)) ? current_pins.data : scc_ch2_ram[scc_ch2_addr];
	scc_ch2_fcounter_ld = !(scc_ch2_fchange || scc_ch2_icounter_cnt);

	scc_ch3_data = (is_write && is_scc_sel && ((scc_addr >= 0x40) && (scc_addr < 0x60)) && !testbit(scc_test_reg, 6)) ? current_pins.data : scc_ch3_ram[scc_ch3_addr];
	scc_ch3_fcounter_ld = !(scc_ch3_fchange || scc_ch3_icounter_cnt);

	if (is_read_edge && is_scc_sel && (scc_addr < 0x80))
	{
	    if (scc_addr < 0x20)
	    {
		current_pins.data = scc_ch1_data;
	    }
	    else if (scc_addr < 0x40)
	    {
		current_pins.data = scc_ch2_data;
	    }
	    else if (scc_addr < 0x60)
	    {
		current_pins.data = scc_ch3_data;
	    }
	    else
	    {
		current_pins.data = 0x00;
	    }
	}
    }

    void K051649::tickSCC_Channels()
    {
	if (clk_rise)
	{
	    if (!scc_ch1_fcounter_ld)
	    {
		scc_ch1_cycles = 0;
	    }
	    else
	    {
		scc_ch1_cycles = ((scc_ch1_cycles + 1) & 0xF);
	    }

	    scc_ch1_fcounter_lo_borrow = (scc_ch1_fcounter_lo == 0);
	    scc_ch1_fcounter_mid_borrow = (scc_ch1_fcounter_mid == 0);
	    scc_ch1_fcounter_hi_borrow = (scc_ch1_fcounter_hi == 0);

	    scc_ch1_fcounter_hi_cnt = (testbit(scc_test_reg, 0) ? true : (scc_ch1_fcounter_lo_borrow && scc_ch1_fcounter_mid_borrow));

	    if (!scc_ch1_fcounter_ld)
	    {
		scc_ch1_fcounter_lo = (scc_ch1_freq & 0xF);
		scc_ch1_fcounter_mid = ((scc_ch1_freq >> 4) & 0xF);
		scc_ch1_fcounter_hi = ((scc_ch1_freq >> 8) & 0xF);
	    }
	    else
	    {
		if (scc_ch1_fcounter_hi_cnt)
		{
		    scc_ch1_fcounter_hi = ((scc_ch1_fcounter_hi - 1) & 0xF);
		}

		if (scc_ch1_fcounter_lo_borrow)
		{
		    scc_ch1_fcounter_mid = ((scc_ch1_fcounter_mid - 1) & 0xF);
		}

		scc_ch1_fcounter_lo = ((scc_ch1_fcounter_lo - 1) & 0xF);
	    }

	    scc_ch1_icounter_cnt = testbit(scc_test_reg, 1) ? (scc_ch1_fcounter_lo_borrow && scc_ch1_fcounter_mid_borrow) : (scc_ch1_fcounter_hi_cnt && scc_ch1_fcounter_hi_borrow);

	    if (testbit(scc_test_reg, 5) && scc_ch1_fchange)
	    {
		scc_ch1_icounter = 0x1F;
	    }
	    else if (scc_ch1_icounter_cnt)
	    {
		scc_ch1_icounter = ((scc_ch1_icounter - 1) & 0x1F);
	    }
	}

	scc_ch1_addr_cntr = (~scc_ch1_icounter & 0x1F);

	if (clk_rise)
	{
	    if (!scc_ch2_fcounter_ld)
	    {
		scc_ch2_cycles = 0;
	    }
	    else
	    {
		scc_ch2_cycles = ((scc_ch2_cycles + 1) & 0xF);
	    }

	    scc_ch2_fcounter_lo_borrow = (scc_ch2_fcounter_lo == 0);
	    scc_ch2_fcounter_mid_borrow = (scc_ch2_fcounter_mid == 0);
	    scc_ch2_fcounter_hi_borrow = (scc_ch2_fcounter_hi == 0);

	    scc_ch2_fcounter_hi_cnt = (testbit(scc_test_reg, 0) ? true : (scc_ch2_fcounter_lo_borrow && scc_ch2_fcounter_mid_borrow));

	    if (!scc_ch2_fcounter_ld)
	    {
		scc_ch2_fcounter_lo = (scc_ch2_freq & 0xF);
		scc_ch2_fcounter_mid = ((scc_ch2_freq >> 4) & 0xF);
		scc_ch2_fcounter_hi = ((scc_ch2_freq >> 8) & 0xF);
	    }
	    else
	    {
		if (scc_ch2_fcounter_hi_cnt)
		{
		    scc_ch2_fcounter_hi = ((scc_ch2_fcounter_hi - 1) & 0xF);
		}

		if (scc_ch2_fcounter_lo_borrow)
		{
		    scc_ch2_fcounter_mid = ((scc_ch2_fcounter_mid - 1) & 0xF);
		}

		scc_ch2_fcounter_lo = ((scc_ch2_fcounter_lo - 1) & 0xF);
	    }

	    scc_ch2_icounter_cnt = testbit(scc_test_reg, 1) ? (scc_ch2_fcounter_lo_borrow && scc_ch2_fcounter_mid_borrow) : (scc_ch2_fcounter_hi_cnt && scc_ch2_fcounter_hi_borrow);

	    if (testbit(scc_test_reg, 5) && scc_ch2_fchange)
	    {
		scc_ch2_icounter = 0x1F;
	    }
	    else if (scc_ch2_icounter_cnt)
	    {
		scc_ch2_icounter = ((scc_ch2_icounter - 1) & 0x1F);
	    }
	}

	scc_ch2_addr_cntr = (~scc_ch2_icounter & 0x1F);

	if (clk_rise)
	{
	    if (!scc_ch3_fcounter_ld)
	    {
		scc_ch3_cycles = 0;
	    }
	    else
	    {
		scc_ch3_cycles = ((scc_ch3_cycles + 1) & 0xF);
	    }

	    scc_ch3_fcounter_lo_borrow = (scc_ch3_fcounter_lo == 0);
	    scc_ch3_fcounter_mid_borrow = (scc_ch3_fcounter_mid == 0);
	    scc_ch3_fcounter_hi_borrow = (scc_ch3_fcounter_hi == 0);

	    scc_ch3_fcounter_hi_cnt = (testbit(scc_test_reg, 0) ? true : (scc_ch3_fcounter_lo_borrow && scc_ch3_fcounter_mid_borrow));

	    if (!scc_ch3_fcounter_ld)
	    {
		scc_ch3_fcounter_lo = (scc_ch3_freq & 0xF);
		scc_ch3_fcounter_mid = ((scc_ch3_freq >> 4) & 0xF);
		scc_ch3_fcounter_hi = ((scc_ch3_freq >> 8) & 0xF);
	    }
	    else
	    {
		if (scc_ch3_fcounter_hi_cnt)
		{
		    scc_ch3_fcounter_hi = ((scc_ch3_fcounter_hi - 1) & 0xF);
		}

		if (scc_ch3_fcounter_lo_borrow)
		{
		    scc_ch3_fcounter_mid = ((scc_ch3_fcounter_mid - 1) & 0xF);
		}

		scc_ch3_fcounter_lo = ((scc_ch3_fcounter_lo - 1) & 0xF);
	    }

	    scc_ch3_icounter_cnt = testbit(scc_test_reg, 1) ? (scc_ch3_fcounter_lo_borrow && scc_ch3_fcounter_mid_borrow) : (scc_ch3_fcounter_hi_cnt && scc_ch3_fcounter_hi_borrow);

	    if (testbit(scc_test_reg, 5) && scc_ch3_fchange)
	    {
		scc_ch3_icounter = 0x1F;
	    }
	    else if (scc_ch3_icounter_cnt)
	    {
		scc_ch3_icounter = ((scc_ch3_icounter - 1) & 0x1F);
	    }
	}

	scc_ch3_addr_cntr = (~scc_ch3_icounter & 0x1F);
    }

    void K051649::tickSCC_Out()
    {
	if (!current_pins.pin_res)
	{
	    scc_ch1_mul_rst = true;
	}
	else if (clk_rise)
	{
	    scc_ch1_mul_rst = scc_ch1_fcounter_ld;
	}

	if (!scc_ch1_mul_rst)
	{
	    prev_ch1_serial = false;
	}
	else if (clk_rise)
	{
	    prev_ch1_serial = scc_ch1_serial;
	}

	if (clk_rise)
	{
	    scc_ch1_serial = testbit(scc_ch1_data, (scc_ch1_cycles & 0x7));
	}

	if (!current_pins.pin_res)
	{
	    scc_ch1_current_vol = 0;
	}
	else if (!prev_ch1_mul_rst && scc_ch1_mul_rst)
	{
	    scc_ch1_current_vol = scc_ch1_vol;
	}

	if (prev_ch1_serial == scc_ch1_serial)
	{
	    scc_ch1_weighted_vol = 0;
	    scc_ch1_weighted_vol_carry = false;
	}
	else
	{
	    uint8_t ch1_vol_mask = (scc_ch1_serial) ? 0xF : 0;
	    scc_ch1_weighted_vol = ((scc_ch1_serial << 4) | (scc_ch1_current_vol ^ ch1_vol_mask));
	    scc_ch1_weighted_vol_carry = scc_ch1_serial;
	}

	if (!scc_ch1_mul_rst)
	{
	    scc_ch1_accshift = 0;
	}
	else if (clk_rise)
	{
	    scc_ch1_accshift = scc_ch1_accshift_next;
	}

	scc_ch1_accshift_next = getNextAccshift(scc_ch1_accshift, scc_ch1_weighted_vol, scc_ch1_weighted_vol_carry);

	if (!scc_ch1_mul_rst)
	{
	    scc_ch1_lower = 0;
	}
	else if (clk_rise)
	{
	    scc_ch1_lower = ((testbit(scc_ch1_accshift, 0) << 2) | ((scc_ch1_lower >> 1) & 0x3));
	}

	if (!scc_ch1_mul_rst)
	{
	    scc_ch1_accshift_en_next = false;
	}
	else if (clk_fall)
	{
	    scc_ch1_accshift_en_next = (testbit(scc_ch1_cycles, 3) && testbit(scc_ch1_cycles, 0));
	}

	if (!scc_ch1_mul_rst)
	{
	    scc_ch1_accshift_en = false;
	}
	else if (!prev_ch1_accshift_en_next && scc_ch1_accshift_en_next)
	{
	    scc_ch1_accshift_en = true;
	}

	if (!scc_ch1_mute)
	{
	    scc_ch1_out = 0;
	}
	else if (!prev_ch1_accshift_en && scc_ch1_accshift_en)
	{
	    uint8_t sound_out = ((scc_ch1_accshift << 3) | scc_ch1_lower);
	    scc_ch1_out = sound_out;
	}

	prev_ch1_accshift_en = scc_ch1_accshift_en;
	prev_ch1_accshift_en_next = scc_ch1_accshift_en_next;
	prev_ch1_mul_rst = scc_ch1_mul_rst;

	if (!current_pins.pin_res)
	{
	    scc_ch2_mul_rst = true;
	}
	else if (clk_rise)
	{
	    scc_ch2_mul_rst = scc_ch2_fcounter_ld;
	}

	if (!scc_ch2_mul_rst)
	{
	    prev_ch2_serial = false;
	}
	else if (clk_rise)
	{
	    prev_ch2_serial = scc_ch2_serial;
	}

	if (clk_rise)
	{
	    scc_ch2_serial = testbit(scc_ch2_data, (scc_ch2_cycles & 0x7));
	}

	if (!current_pins.pin_res)
	{
	    scc_ch2_current_vol = 0;
	}
	else if (!prev_ch2_mul_rst && scc_ch2_mul_rst)
	{
	    scc_ch2_current_vol = scc_ch2_vol;
	}

	if (prev_ch2_serial == scc_ch2_serial)
	{
	    scc_ch2_weighted_vol = 0;
	    scc_ch2_weighted_vol_carry = false;
	}
	else
	{
	    uint8_t ch1_vol_mask = (scc_ch2_serial) ? 0xF : 0;
	    scc_ch2_weighted_vol = ((scc_ch2_serial << 4) | (scc_ch2_current_vol ^ ch1_vol_mask));
	    scc_ch2_weighted_vol_carry = scc_ch2_serial;
	}

	if (!scc_ch2_mul_rst)
	{
	    scc_ch2_accshift = 0;
	}
	else if (clk_rise)
	{
	    scc_ch2_accshift = scc_ch2_accshift_next;
	}

	scc_ch2_accshift_next = getNextAccshift(scc_ch2_accshift, scc_ch2_weighted_vol, scc_ch2_weighted_vol_carry);

	if (!scc_ch2_mul_rst)
	{
	    scc_ch2_lower = 0;
	}
	else if (clk_rise)
	{
	    scc_ch2_lower = ((testbit(scc_ch2_accshift, 0) << 2) | ((scc_ch2_lower >> 1) & 0x3));
	}

	if (!scc_ch2_mul_rst)
	{
	    scc_ch2_accshift_en_next = false;
	}
	else if (clk_fall)
	{
	    scc_ch2_accshift_en_next = (testbit(scc_ch2_cycles, 3) && testbit(scc_ch2_cycles, 0));
	}

	if (!scc_ch2_mul_rst)
	{
	    scc_ch2_accshift_en = false;
	}
	else if (!prev_ch2_accshift_en_next && scc_ch2_accshift_en_next)
	{
	    scc_ch2_accshift_en = true;
	}

	if (!scc_ch2_mute)
	{
	    scc_ch2_out = 0;
	}
	else if (!prev_ch2_accshift_en && scc_ch2_accshift_en)
	{
	    uint8_t sound_out = ((scc_ch2_accshift << 3) | scc_ch2_lower);
	    scc_ch2_out = sound_out;
	}

	prev_ch2_accshift_en = scc_ch2_accshift_en;
	prev_ch2_accshift_en_next = scc_ch2_accshift_en_next;
	prev_ch2_mul_rst = scc_ch2_mul_rst;

	if (!current_pins.pin_res)
	{
	    scc_ch3_mul_rst = true;
	}
	else if (clk_rise)
	{
	    scc_ch3_mul_rst = scc_ch3_fcounter_ld;
	}

	if (!scc_ch3_mul_rst)
	{
	    prev_ch3_serial = false;
	}
	else if (clk_rise)
	{
	    prev_ch3_serial = scc_ch3_serial;
	}

	if (clk_rise)
	{
	    scc_ch3_serial = testbit(scc_ch3_data, (scc_ch3_cycles & 0x7));
	}

	if (!current_pins.pin_res)
	{
	    scc_ch3_current_vol = 0;
	}
	else if (!prev_ch3_mul_rst && scc_ch3_mul_rst)
	{
	    scc_ch3_current_vol = scc_ch3_vol;
	}

	if (prev_ch3_serial == scc_ch3_serial)
	{
	    scc_ch3_weighted_vol = 0;
	    scc_ch3_weighted_vol_carry = false;
	}
	else
	{
	    uint8_t ch1_vol_mask = (scc_ch3_serial) ? 0xF : 0;
	    scc_ch3_weighted_vol = ((scc_ch3_serial << 4) | (scc_ch3_current_vol ^ ch1_vol_mask));
	    scc_ch3_weighted_vol_carry = scc_ch3_serial;
	}

	if (!scc_ch3_mul_rst)
	{
	    scc_ch3_accshift = 0;
	}
	else if (clk_rise)
	{
	    scc_ch3_accshift = scc_ch3_accshift_next;
	}

	scc_ch3_accshift_next = getNextAccshift(scc_ch3_accshift, scc_ch3_weighted_vol, scc_ch3_weighted_vol_carry);

	if (!scc_ch3_mul_rst)
	{
	    scc_ch3_lower = 0;
	}
	else if (clk_rise)
	{
	    scc_ch3_lower = ((testbit(scc_ch3_accshift, 0) << 2) | ((scc_ch3_lower >> 1) & 0x3));
	}

	if (!scc_ch3_mul_rst)
	{
	    scc_ch3_accshift_en_next = false;
	}
	else if (clk_fall)
	{
	    scc_ch3_accshift_en_next = (testbit(scc_ch3_cycles, 3) && testbit(scc_ch3_cycles, 0));
	}

	if (!scc_ch3_mul_rst)
	{
	    scc_ch3_accshift_en = false;
	}
	else if (!prev_ch3_accshift_en_next && scc_ch3_accshift_en_next)
	{
	    scc_ch3_accshift_en = true;
	}

	if (!scc_ch3_mute)
	{
	    scc_ch3_out = 0;
	}
	else if (!prev_ch3_accshift_en && scc_ch3_accshift_en)
	{
	    uint8_t sound_out = ((scc_ch3_accshift << 3) | scc_ch3_lower);
	    scc_ch3_out = sound_out;
	}

	prev_ch3_accshift_en = scc_ch3_accshift_en;
	prev_ch3_accshift_en_next = scc_ch3_accshift_en_next;
	prev_ch3_mul_rst = scc_ch3_mul_rst;
    }

    uint8_t K051649::getNextAccshift(uint8_t accshift, uint8_t weighted_vol, bool weighted_vol_carry)
    {
	uint8_t accshift_a = ((testbit(accshift, 4) << 4) | ((accshift >> 1) & 0xF));
	uint8_t accshift_b = (weighted_vol + weighted_vol_carry);
	return ((accshift_a + accshift_b) & 0x1F);
    }

    void K051649::tickMixer()
    {
	if (!current_pins.pin_res)
	{
	    current_pins.audio_out = 0;
	}
	else if (clk_rise)
	{
	    uint16_t audio_out = (scc_ch1_out + scc_ch2_out + scc_ch3_out + scc_ch4_out + scc_ch5_out);
	    current_pins.audio_out = (audio_out & 0x7FF);
	}
    }
};