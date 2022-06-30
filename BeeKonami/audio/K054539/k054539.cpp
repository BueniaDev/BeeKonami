/*
    This file is part of the BeeKonami engine.
    Copyright (C) 2022 BueniaDev.

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

// BeeKonami-K054539
// Chip name: K054539
// Chip description: Eight-voice PCM/DPCM sound chip
//
// BueniaDev's Notes:
//
// This implementation is derived from both MAME's implementation and my own reverse-engineering efforts.
//
// MAME's implementation can be found here
// https://github.com/mamedev/mame/blob/master/src/devices/sound/k054539.cpp
// 
// The following features are currently unimplemented:
// Chip reads
// Reverb
// Timer handling
// ROM/RAM-related functionality

#include "k054539.h"
using namespace beekonami::audio;

namespace beekonami::audio
{
    K054539::K054539()
    {

    }

    K054539::~K054539()
    {

    }

    uint8_t K054539::readROM(uint32_t addr)
    {
	return (addr < k054539_rom.size()) ? k054539_rom.at(addr) : 0;
    }

    void K054539::key_on(k054539_channel &channel)
    {
	if (!is_reg_disable)
	{
	    channel.is_keyon = true;
	    // Channel address is set to the starting address upon key-on
	    // (this was handled via a cheap hack in MAME's implementation)
	    channel.current_address = channel.start_address;
	    channel.current_pos = 0;
	    channel.is_high_nibble = false;
	}
    }

    void K054539::key_off(k054539_channel &channel)
    {
	if (!is_reg_disable)
	{
	    channel.is_keyon = false;
	}
    }

    void K054539::write_reg(int ch, int offs, uint8_t data)
    {
	offs &= 0x1F;
	ch &= 7;

	auto &channel = channels[ch];

	switch (offs)
	{
	    // Delta frequency address (bits 0-7)
	    case 0x00:
	    {
		channel.delta_addr = ((channel.delta_addr & 0xFFFF00) | data);
	    }
	    break;
	    // Delta frequency address (bits 8-15)
	    case 0x01:
	    {
		channel.delta_addr = ((channel.delta_addr & 0xFF00FF) | (data << 8));
	    }
	    break;
	    // Delta frequency address (bits 16-23)
	    case 0x02:
	    {
		channel.delta_addr = ((channel.delta_addr & 0x00FFFF) | (data << 16));
	    }
	    break;
	    // Volume register
	    case 0x03:
	    {
		channel.volume = data;
	    }
	    break;
	    // Reverb volume register
	    case 0x04:
	    {
		channel.reverb_volume = data;
	    }
	    break;
	    // Panpot register
	    case 0x05:
	    {
		channel.pan_reg = data;
	    }
	    break;
	    // Reverb delay (bits 0-7)
	    case 0x06:
	    {
		channel.reverb_delay = ((channel.reverb_delay & 0xFF00) | data);
	    }
	    break;
	    // Reverb delay (bits 8-15)
	    case 0x07:
	    {
		channel.reverb_delay = ((channel.reverb_delay & 0xFF) | (data << 8));
	    }
	    break;
	    // Loop start address (bits 0-7)
	    case 0x08:
	    {
		channel.loop_address = ((channel.loop_address & 0xFFFF00) | data);
	    }
	    break;
	    // Loop start address (bits 8-15)
	    case 0x09:
	    {
		channel.loop_address = ((channel.loop_address & 0xFF00FF) | (data << 8));
	    }
	    break;
	    // Loop start address (bits 16-23)
	    case 0x0A:
	    {
		channel.loop_address = ((channel.loop_address & 0x00FFFF) | (data << 16));
	    }
	    break;
	    // Channel start address (bits 0-7)
	    case 0x0C:
	    {
		channel.start_address = ((channel.start_address & 0xFFFF00) | data);
	    }
	    break;
	    // Channel start address (bits 8-15)
	    case 0x0D:
	    {
		channel.start_address = ((channel.start_address & 0xFF00FF) | (data << 8));
	    }
	    break;
	    // Channel start address (bits 16-23)
	    case 0x0E:
	    {
		channel.start_address = ((channel.start_address & 0x00FFFF) | (data << 16));
	    }
	    break;
	}
    }

    uint32_t K054539::get_sample_rate(uint32_t clock_rate)
    {
	return (clock_rate / 384);
    }

    void K054539::init()
    {
	reset();
    }

    void K054539::config(int flags)
    {
	is_reverse_stereo = testbit(flags, 0);
	is_reverb_disabled = testbit(flags, 1);
	is_keyon_update = testbit(flags, 2);
    }

    void K054539::reset()
    {
	config(0);
	is_keyon_update = true;
	voltab.fill(0);

	for (int ch = 0; ch < 8; ch++)
	{
	    channels[ch].number = ch;
	}

	for (auto &channel : channels)
	{
	    channel.is_keyon = false;
	    channel.output.fill(0);
	}

	// Calculate volume table
	for (int i = 0; i < 256; i++)
	{
	    double volume = (pow(10.0, (-36.0 * (double)i / (double)0x40) / 20.0) / 4.0);
	    voltab[i] = int32_t(volume * 58980.f); // Volume gain of 1.80
	}

	// Calculate panpot table
	for (int i = 0; i < 15; i++)
	{
	    pantab[i] = sqrt((double)i) / sqrt((double)0xE);
	}
    }

    void K054539::writeROM(size_t rom_size, size_t data_start, size_t data_len, vector<uint8_t> rom_data)
    {
	k054539_rom.resize(rom_size, 0xFF);

	uint32_t data_length = data_len;
	uint32_t data_end = (data_start + data_len);

	if (data_start > rom_size)
	{
	    return;
	}

	if (data_end > rom_size)
	{
	    data_length = (rom_size - data_start);
	}

	copy(rom_data.begin(), (rom_data.begin() + data_length), (k054539_rom.begin() + data_start));
    }

    void K054539::writeRAM(uint16_t addr, uint8_t data)
    {
	if (addr < 0x100)
	{
	    int ch = (addr >> 5);
	    int offs = (addr & 0x1F);
	    // Write to channel register
	    write_reg(ch, offs, data);
	}
	else if ((addr >= 0x200) && (addr < 0x210))
	{
	    int ch_addr = (addr - 0x200);
	    int ch = (ch_addr >> 1);
	    int offs = (ch_addr & 0x1);
	    auto &channel = channels[ch];

	    switch (offs)
	    {
		case 0:
		{
		    // Bits 2-3: Channel sample type (0=8-bit PCM, 1=16-bit LSB-first PCM, 2=4-bit DPCM)
		    // Bit 5: Channel reverse bit (0=Normal, 1=Reversed)
		    channel.is_reverse = testbit(data, 5);
		    channel.ch_type = ((data >> 2) & 0x3);
		}
		break;
		case 1:
		{
		    // Bit 0: Channel loop enable bit (1=Looping enabled)
		    channel.is_loop = testbit(data, 0);
		}
		break;
	    }
	}
	else
	{
	    switch (addr)
	    {
		// Key-on register (1=Key-on)
		case 0x214:
		{
		    for (int ch = 0; ch < 8; ch++)
		    {
			if (testbit(data, ch))
			{
			    key_on(channels[ch]);
			}
		    }
		}
		break;
		// Key-on register (1=Key-off)
		case 0x215:
		{
		    for (int ch = 0; ch < 8; ch++)
		    {
			if (testbit(data, ch))
			{
			    key_off(channels[ch]);
			}
		    }
		}
		break;
		// Control register
		// Bit 0=Enable PCM output (1=Enabled)
		// Bit 7=Disable register update (1=Disabled)
		case 0x22F:
		{
		    // cout << "Writing to K054539 control register" << endl;
		    enable_pcm = testbit(data, 0);
		    is_reg_disable = testbit(data, 7);
		}
		break;
		default:
		{
		    // cout << "Writing value of " << hex << int(data) << " to K054539 register of " << hex << int(addr) << endl;
		}
		break;
	    }
	}
    }

    void K054539::clockchip()
    {
	if (!enable_pcm)
	{
	    return;
	}

	for (int ch = 0; ch < 8; ch++)
	{
	    auto &channel = channels[ch];
	    if (!channel.is_keyon)
	    {
		continue;
	    }

	    int delta = channel.delta_addr;
	    int volume = channel.volume;
	    int pan = channel.pan_reg;

	    if (channel.is_reverse)
	    {
		delta = -delta;
	    }

	    // Calculate panpot index
	    if ((pan >= 0x81) && (pan <= 0x8F))
	    {
		pan -= 0x81;
	    }
	    else if ((pan >= 0x11) && (pan <= 0x1F))
	    {
		pan -= 0x11;
	    }
	    else
	    {
		pan = 0x7;
	    }

	    // Calculate volume
	    int32_t lvol = (voltab[volume] * pantab[pan]);
	    int32_t rvol = (voltab[volume] * pantab[(0xE - pan)]);

	    lvol = min<int32_t>(lvol, 58980);
	    rvol = min<int32_t>(rvol, 58980);

	    uint32_t position = (channel.current_pos + delta);
	    channel.current_pos = (position & 0xFFFF);

	    if (position < 0x10000)
	    {
		continue;
	    }

	    switch (channel.ch_type)
	    {
		// 8-bit PCM
		case 0:
		{
		    if (channel.is_reverse)
		    {
			channel.current_address -= 1;
		    }
		    else
		    {
			channel.current_address += 1;
		    }

		    channel.prev_value = channel.current_value;

		    channel.current_byte = readROM(channel.current_address);

		    if ((channel.current_byte == 0x80) && channel.is_loop)
		    {
			channel.current_address = channel.loop_address;
			channel.current_byte = readROM(channel.current_address);
		    }

		    if (channel.current_byte == 0x80)
		    {
			key_off(channel);
			channel.current_value = 0;
			break;
		    }

		    channel.current_value = (int8_t(channel.current_byte) << 8);
		}
		break;
		// 16-bit PCM (LSB first)
		case 1:
		{
		    if (channel.is_reverse)
		    {
			channel.current_address -= 2;
		    }
		    else
		    {
			channel.current_address += 2;
		    }

		    channel.prev_value = channel.current_value;

		    uint16_t sample = (readROM(channel.current_address) | (readROM(channel.current_address + 1) << 8));

		    if ((sample == 0x8000) && channel.is_loop)
		    {
			channel.current_address = channel.loop_address;
			sample = (readROM(channel.current_address) | (readROM(channel.current_address + 1) << 8));
		    }

		    if (sample == 0x8000)
		    {
			key_off(channel);
			channel.current_value = 0;
			break;
		    }

		    channel.current_value = int16_t(sample);
		}
		break;
		// 4-bit DPCM
		case 2:
		{
		    if (!channel.is_high_nibble)
		    {
			if (channel.is_reverse)
			{
			    channel.current_address -= 1;
			}
			else
			{
			    channel.current_address += 1;
			}

			channel.current_byte = readROM(channel.current_address);
		    }

		    if ((channel.current_byte == 0x88) && channel.is_loop)
		    {
			channel.current_address = channel.loop_address;
			channel.current_byte = readROM(channel.current_address);
		    }

		    if (channel.current_byte == 0x88)
		    {
			key_off(channel);
			channel.current_value = 0;
			break;
		    }

		    uint8_t data = (uint8_t(channel.current_byte << (4 * channel.is_high_nibble)) >> 4);
		    channel.is_high_nibble = !channel.is_high_nibble;

		    channel.prev_value = channel.current_value;
		    channel.current_value = (channel.prev_value + (dpcm_table[(data & 0xF)] << 8));
		    channel.current_value = clamp<int32_t>(channel.current_value, -32768, 32767);
		}
		break;
		default: break;
	    }

	    // Calculate sample (via linear-interpolation)
	    int32_t result = (((channel.prev_value * int32_t((channel.current_pos ^ 0xFFFF) + 1)) + (channel.current_value * int32_t(channel.current_pos))) >> 16);

	    // Factor in channel volume to calculate the final sample
	    channel.output[0] = ((result * lvol) >> 15);
	    channel.output[1] = ((result * rvol) >> 15);
	}
    }

    vector<int32_t> K054539::get_samples()
    {
	array<int32_t, 2> mixed_samples = {0, 0};

	for (auto &channel : channels)
	{
	    for (int i = 0; i < 2; i++)
	    {
		int32_t old_sample = mixed_samples[i];
		int32_t new_sample = clamp(channel.output[i], -32768, 32767);
		mixed_samples[i] = (old_sample + new_sample);
	    }
	}

	vector<int32_t> final_samples;
	final_samples.push_back(mixed_samples[0]);
	final_samples.push_back(mixed_samples[1]);
	return final_samples;
    }
};