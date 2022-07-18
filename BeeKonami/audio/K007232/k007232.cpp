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

// BeeKonami-K007232
// Chip name: K007232
// Chip description: 2-channel 7-bit PCM player with variable pitch
//
// BueniaDev's Notes:
// This implementation is inspired by MAME's implementation and derived from Furrtek's reverse-engineered schematics for this chip,
// both of which can be found at the links below:
//
// MAME's implementation:
// https://github.com/mamedev/mame/blob/master/src/devices/sound/k007232.cpp
//
// Furrtek's schematics:
// https://github.com/furrtek/VGChips/tree/master/Konami/007232
// 
// This implementation is just about complete, AFAIK.
// In addition, the frequency prescaler, which is unimplemented in MAME's core,
// is fully implemented here.

#include "k007232.h"
using namespace beekonami::audio;

namespace beekonami::audio
{
    K007232::K007232()
    {

    }

    K007232::~K007232()
    {

    }

    uint8_t K007232::fetchROM(k007232_channel &channel, uint32_t addr)
    {
	uint32_t rom_addr = (channel.bank + (addr & 0x1FFFF));
	return (rom_addr < pcm_rom.size()) ? pcm_rom.at(rom_addr) : 0xC0;
    }

    void K007232::setVolume(int volume_a, int volume_b)
    {
	volume_a &= 0xF;
	volume_b &= 0xF;

	// Scale ratio from [0-15] to [0-512]
	double vol_ratio = (512 / 15);

	channels[0].volume = int(double(volume_a) * vol_ratio);
	channels[1].volume = int(double(volume_b) * vol_ratio);
    }

    void K007232::setBank(int bank_a, int bank_b)
    {
	channels[0].bank = (bank_a << 17);
	channels[1].bank = (bank_b << 17);
    }

    uint32_t K007232::get_sample_rate(uint32_t clock_rate)
    {
	// This specific divisor should match up with the overall logic
	return (clock_rate / 4);
    }

    void K007232::init()
    {
	reset();
    }

    void K007232::reset()
    {
	for (auto &channel : channels)
	{
	    channel.channel_counter = 0;
	    channel.channel_freq = 0;
	    channel.prescale_val = 0;
	    channel.start_address = 0;
	    channel.current_addr = 0;
	    channel.is_loop = false;
	    channel.output = 0;
	    channel.volume = 512;
	    channel.bank = 0;
	}
    }

    void K007232::writeROM(size_t rom_size, size_t data_start, size_t data_len, vector<uint8_t> rom_data)
    {
	// Using 0xFF as the default ROM value here is more true to the hardware
	// (unused ROMS read back 0xFF),
	// but 0x40 (ORed with the stop-bit mask of 0x80) would be considered
	// to be the effective 'null' byte on this chip
	pcm_rom.resize(rom_size, 0xFF);

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

	copy(rom_data.begin(), (rom_data.begin() + data_length), (pcm_rom.begin() + data_start));
    }

    // On official schematics (AFAIK),
    // the NRD and NRCS pins are always pulled high
    // TODO: Confirm the validity of the below logic
    uint8_t K007232::readReg(int reg)
    {
	writeReg(reg, 0xFF);
	return 0xFF;
    }

    // Return value: true if we're writing to the chip's external port
    // (usually for volume control), false otherwise
    bool K007232::writeReg(int reg, uint8_t data)
    {
	reg &= 0xF;

	uint32_t pcm_limit = get_pcmlimit();

	switch (reg)
	{
	    case 0x0:
	    {
		channels[0].channel_freq = ((channels[0].channel_freq & 0xF00) | data);
	    }
	    break;
	    case 0x1:
	    {
		channels[0].channel_freq = ((channels[0].channel_freq & 0xFF) | ((data & 0xF) << 8));
		channels[0].prescale_val = ((data >> 4) & 0x3);
	    }
	    break;
	    case 0x2:
	    {
		channels[0].start_address = ((channels[0].start_address & 0x1FF00) | data);
	    }
	    break;
	    case 0x3:
	    {
		channels[0].start_address = ((channels[0].start_address & 0x100FF) | (data << 8));
	    }
	    break;
	    case 0x4:
	    {
		channels[0].start_address = ((channels[0].start_address & 0xFFFF) | (testbit(data, 0) << 16));
	    }
	    break;
	    case 0x5:
	    {
		if (channels[0].start_address < pcm_limit)
		{
		    channels[0].is_playing = true;
		    channels[0].current_addr = channels[0].start_address;
		}
	    }
	    break;
	    case 0x6:
	    {
		channels[1].channel_freq = ((channels[1].channel_freq & 0xF00) | data);
	    }
	    break;
	    case 0x7:
	    {
		channels[1].channel_freq = ((channels[1].channel_freq & 0xFF) | ((data & 0xF) << 8));
		channels[1].prescale_val = ((data >> 4) & 0x3);
	    }
	    break;
	    case 0x8:
	    {
		channels[1].start_address = ((channels[1].start_address & 0x1FF00) | data);
	    }
	    break;
	    case 0x9:
	    {
		channels[1].start_address = ((channels[1].start_address & 0x100FF) | (data << 8));
	    }
	    break;
	    case 0xA:
	    {
		channels[1].start_address = ((channels[1].start_address & 0xFFFF) | (testbit(data, 0) << 16));
	    }
	    break;
	    case 0xB:
	    {
		if (channels[1].start_address < pcm_limit)
		{
		    channels[1].is_playing = true;
		    channels[1].current_addr = channels[1].start_address;
		}
	    }
	    break;
	    case 0xD:
	    {
		channels[1].is_loop = testbit(data, 1);
		channels[0].is_loop = testbit(data, 0);
	    }
	    break;
	    default: break;
	}


	return (reg == 0xC);
    }

    void K007232::clockchip()
    {
	for (auto &channel : channels)
	{
	    int prescale_bit = prescaler_bits.at(channel.prescale_val);
	    int prescale_mask = ((1 << prescale_bit) - 1);

	    channel.channel_counter += 1;

	    if (testbit(channel.channel_counter, prescale_bit))
	    {
		channel.channel_counter = channel.channel_freq;

		if (!channel.is_playing)
		{
		    continue;
		}

		channel.current_addr &= 0x1FFFF;
		uint8_t sample = fetchROM(channel, channel.current_addr++);

		if (testbit(sample, 7) || (channel.current_addr >= get_pcmlimit()))
		{
		    if (channel.is_loop)
		    {
			channel.current_addr = channel.start_address;
		    }
		    else
		    {
			channel.is_playing = false;
		    }
		}

		int32_t output_sample = ((sample & 0x7F) - 0x40);
		channel.output = (output_sample * channel.volume);
	    }
	    
	}
    }

    vector<int32_t> K007232::get_samples()
    {
	vector<int32_t> samples;
	samples.push_back(channels[0].output);
	samples.push_back(channels[1].output);
	return samples;
    }
}