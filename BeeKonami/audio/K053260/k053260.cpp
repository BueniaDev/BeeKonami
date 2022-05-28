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

// BeeKonami-K053260
// Chip name: K053260 (KDSC)
// Chip description: Four-channel PCM/ADPCM sound chip with 8-bit communication ports
//
// BueniaDev's Notes:
//
// This implementation is derived from MAME's implementation, which can be found here:
// https://github.com/mamedev/mame/blob/master/src/devices/sound/k053260.cpp
// 
// The following features are currently unimplemented:
//
// Register reads
// Communication ports

#include "k053260.h"
using namespace beekonami::audio;

namespace beekonami::audio
{
    K053260::K053260()
    {

    }

    K053260::~K053260()
    {

    }

    void K053260::key_on(kdsc_voice &channel)
    {
	channel.current_addr = (channel.is_kadpcm) ? 1 : 0;
	channel.counter = 0xFC0;
	channel.sample = 0;
	channel.is_playing = true;
    }

    void K053260::key_off(kdsc_voice &channel)
    {
	channel.current_addr = 0;
	channel.sample = 0;
	channel.is_playing = true;
    }

    void K053260::update_pan_volume(kdsc_voice &channel)
    {
	channel.pan_volume[0] = (channel.volume * pan_mul_table[0][channel.pan_reg]);
	channel.pan_volume[1] = (channel.volume * pan_mul_table[1][channel.pan_reg]);
    }

    void K053260::write_voice_reg(int ch_num, int offs, uint8_t data)
    {
	offs &= 0x7;
	auto &channel = voices[ch_num];

	switch (offs)
	{
	    case 0: channel.pitch_reg = ((channel.pitch_reg & 0xF00) | data); break;
	    case 1: channel.pitch_reg = ((channel.pitch_reg & 0xFF) | ((data & 0xF) << 8)); break;
	    case 2: channel.length = ((channel.length & 0xFF00) | data); break;
	    case 3: channel.length = ((channel.length & 0xFF) | (data << 8)); break;
	    case 4: channel.start_addr = ((channel.start_addr & 0x1FFF00) | data); break;
	    case 5: channel.start_addr = ((channel.start_addr & 0x1F00FF) | (data << 8)); break;
	    case 6: channel.start_addr = ((channel.start_addr & 0xFFFF) | ((data & 0x1F) << 16)); break;
	    case 7:
	    {
		channel.volume = (data & 0x7F);
		update_pan_volume(channel);
	    }
	    break;
	}
    }

    void K053260::writeReg(int offs, uint8_t data)
    {
	offs &= 0x3F;

	if ((offs >= 0x08) && (offs <= 0x27))
	{
	    int voice_num = ((offs - 8) / 8);
	    int reg = (offs & 0x7);
	    write_voice_reg(voice_num, reg, data);
	}
	else
	{
	    switch (offs)
	    {
		case 0x02:
		case 0x03:
		{
		    cout << "Writing to minion-to-master port " << dec << int(offs - 2) << endl;
		}
		break;
		case 0x28:
		{
		    for (int i = 0; i < 4; i++)
		    {
			auto &channel = voices[i];
			if (!channel.prev_keyon && testbit(data, i))
			{
			    key_on(channel);
			}
			else if (!testbit(data, i))
			{
			    key_off(channel);
			}

			channel.prev_keyon = testbit(data, i);
		    }
		}
		break;
		case 0x2A:
		{
		    for (int i = 0; i < 4; i++)
		    {
			auto &channel = voices[i];

			channel.is_loop = testbit(data, i);
			channel.is_kadpcm = testbit(data, (4 + i));
		    }
		}
		break;
		case 0x2C:
		{
		    voices[0].pan_reg = (data & 0x7);
		    voices[1].pan_reg = ((data >> 3) & 0x7);
		    update_pan_volume(voices[0]);
		    update_pan_volume(voices[1]);
		}
		break;
		case 0x2D:
		{
		    voices[2].pan_reg = (data & 0x7);
		    voices[3].pan_reg = ((data >> 3) & 0x7);
		    update_pan_volume(voices[2]);
		    update_pan_volume(voices[3]);
		}
		break;
		case 0x2F:
		{
		    cout << "Writing to K053260 mode register" << endl;
		    is_output_enabled = testbit(data, 1);
		}
		break;
		default: break;
	    }
	}
    }

    uint8_t K053260::read_rom(uint32_t addr)
    {
	return (addr < kadpcm_rom.size()) ? kadpcm_rom.at(addr) : 0;
    }

    void K053260::clock_voice(kdsc_voice &channel)
    {
	if (!is_output_enabled || !channel.is_playing)
	{
	    channel.output.fill(0);
	    return;
	}

	channel.counter += 64;

	while (channel.counter >= 0x1000)
	{
	    channel.counter = ((channel.counter - 0x1000) + channel.pitch_reg);

	    uint32_t byte_pos = ++channel.current_addr >> (channel.is_kadpcm ? 1 : 0);
	    /*
		YES, pre-increment. Seriously, playback must start 1 byte after the
		start address written to the register, or else ADPCM sounds will either
		have DC offsets (e.g. TMNT2 theme song) or overflow and end up all
		distorted (e.g. various Vendetta sound effects).

		The "headers" in the Simpsons and Vendetta sound ROMs provide
		further evidence of this quirk (i.e. the start addresses listed in the
		ROM header are all 1 greater than the start addresses the CPU ultimately writes
		to the K053260).
	    */

	    if (byte_pos > channel.length)
	    {
		if (channel.is_loop)
		{
		    channel.current_addr = 0;
		    channel.sample = 0;
		    byte_pos = 0;
		}
		else
		{
		    channel.is_playing = false;
		    return;
		}
	    }

	    uint8_t rom_data = read_rom(channel.start_addr + byte_pos);

	    if (channel.is_kadpcm)
	    {
		int kadpcm_position = 0;
		if (testbit(channel.current_addr, 0))
		{
		    kadpcm_position = (rom_data >> 4);
		}
		else
		{
		    kadpcm_position = (rom_data & 0xF);
		}

		channel.sample += kadpcm_table[kadpcm_position];
	    }
	    else
	    {
		channel.sample = rom_data;
	    }
	}

	channel.output[0] = ((channel.sample * channel.pan_volume[0]) >> 15);
	channel.output[1] = ((channel.sample * channel.pan_volume[1]) >> 15);
    }

    uint32_t K053260::get_sample_rate(uint32_t clock_rate)
    {
	return (clock_rate / 64);
    }

    void K053260::init()
    {
	reset();
    }

    void K053260::reset()
    {
	for (auto &voice : voices)
	{
	    voice.current_addr = 0;
	    voice.counter = 0;
	    voice.sample = 0;
	    voice.start_addr = 0;
	    voice.length = 0;
	    voice.pitch_reg = 0;
	    voice.volume = 0;
	    voice.pan_reg = 0;
	    voice.is_loop = false;
	    voice.is_kadpcm = false;
	    voice.is_playing = false;
	    voice.output.fill(0);
	    update_pan_volume(voice);
	}

	keyon_reg = 0;
    }

    void K053260::writeROM(size_t rom_size, size_t data_start, size_t data_len, vector<uint8_t> rom_data)
    {
	kadpcm_rom.resize(rom_size, 0xFF);

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

	copy(rom_data.begin(), (rom_data.begin() + data_length), (kadpcm_rom.begin() + data_start));
    }

    void K053260::clockchip()
    {
	for (auto &voice : voices)
	{
	    clock_voice(voice);
	}
    }

    vector<int32_t> K053260::get_samples()
    {
	array<int32_t, 2> mixed_samples = {0, 0};

	for (auto &voice : voices)
	{
	    for (int i = 0; i < 2; i++)
	    {
		int32_t old_sample = mixed_samples[i];
		int32_t new_sample = clamp(voice.output[i], -32768, 32767);
		mixed_samples[i] = (old_sample + new_sample);
	    }
	}

	vector<int32_t> final_samples;
	final_samples.push_back(mixed_samples[0]);
	final_samples.push_back(mixed_samples[1]);
	return final_samples;
    }
}