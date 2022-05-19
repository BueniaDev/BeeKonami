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

// BeeKonami-K051649
// Chip name: K051649 (SCC)/K052539 (SCC+)
// Chip description: Wavetable sound chip
//
// BueniaDev's Notes:
//
// This implementation is derived from MAME's implementation, which can be found here:
// https://github.com/mamedev/mame/blob/master/src/devices/sound/k051649.cpp
// 
// The following features are currently unimplemented:
//
// Register reads

#include "k051649.h"
using namespace beekonami::audio;

namespace beekonami::audio
{
    K051649::K051649()
    {

    }

    K051649::~K051649()
    {

    }

    void K051649::waveform_write(int offs, uint8_t data)
    {
	// Wave RAM is read-only?
	if (testbit(test_reg, 6) || (testbit(test_reg, 7) && (offs >= 0x60)))
	{
	    return;
	}

	if (offs >= 0x60)
	{
	    channels[3].waveram[(offs & 0x1F)] = data;
	    channels[4].waveram[(offs & 0x1F)] = data;
	}
	else
	{
	    channels[(offs >> 5)].waveram[(offs & 0x1F)] = data;
	}
    }

    void K051649::waveform_write_k052539(int offs, uint8_t data)
    {
	// Wave RAM is read-only?
	if (testbit(test_reg, 6))
	{
	    return;
	}

	channels[(offs >> 5)].waveram[(offs & 0x1F)] = data;
    }

    void K051649::volume_write(int offs, uint8_t data)
    {
	channels[(offs & 0x7)].volume = (data & 0xF);
    }

    void K051649::frequency_write(int offs, uint8_t data)
    {
	bool is_freq_hi = testbit(offs, 0);
	offs >>= 1;

	auto &channel = channels[offs];

	if (testbit(test_reg, 5))
	{
	    channel.counter = 0;
	    channel.clock = 0;
	}
	// TODO: Is this correct?
	else if (channel.frequency < 9)
	{
	    channel.clock = 0;
	}

	if (is_freq_hi)
	{
	    channel.frequency = ((channel.frequency & 0xFF) | ((data & 0xF) << 8));
	}
	else
	{
	    channel.frequency = ((channel.frequency & 0xF00) | data);
	}
    }

    void K051649::key_on_write(uint8_t data)
    {
	for (int i = 0; i < 5; i++)
	{
	    channels[i].is_key_on = testbit(data, i);
	}
    }

    void K051649::test_reg_write(uint8_t data)
    {
	test_reg = data;
    }

    uint32_t K051649::get_sample_rate(uint32_t clock_rate)
    {
	return clock_rate;
    }

    void K051649::init(SCCType type)
    {
	set_chip_type(type);
	reset();
    }

    void K051649::set_chip_type(SCCType type)
    {
	chip_type = type;
    }

    void K051649::reset()
    {
	for (auto &channel : channels)
	{
	    channel.frequency = 0;
	    channel.volume = 0xF;
	    channel.counter = 0;
	    channel.clock = 0;
	    channel.is_key_on = false;
	}

	test_reg = 0;
    }

    void K051649::writeIO(int port, uint8_t data)
    {
	if ((port & 1) == 0)
	{
	    chip_address = data;
	}
	else
	{
	    int port_number = (port >> 1);

	    switch (port_number)
	    {
		case 0x00:
		case 0x04:
		{
		    if (is_k052539())
		    {
			waveform_write_k052539(chip_address, data);
		    }
		    else
		    {
			waveform_write(chip_address, data);
		    }
		}
		break;
		case 0x01:
		{
		    frequency_write(chip_address, data);
		}
		break;
		case 0x02:
		{
		    volume_write(chip_address, data);
		}
		break;
		case 0x03:
		{
		    key_on_write(data);
		}
		break;
		case 0x05:
		{
		    test_reg_write(data);
		}
		break;
	    }
	}
    }

    void K051649::clockchip()
    {
	for (auto &channel : channels)
	{
	    if (channel.frequency > 8)
	    {
		if ((channel.clock--) <= 0)
		{
		    channel.counter = ((channel.counter + 1) & 0x1F);
		    channel.clock = channel.frequency;
		}

		if (channel.is_key_on)
		{
		    int32_t sample = ((channel.waveram[channel.counter] * channel.volume) >> 4);
		    channel.output = (sample * 32);
		}
	    }
	}
    }

    vector<int32_t> K051649::get_samples()
    {
	int32_t mixed_sample = 0;

	for (auto &channel : channels)
	{
	    int32_t old_sample = mixed_sample;
	    int32_t new_sample = clamp(channel.output, -32768, 32767);
	    mixed_sample = (old_sample + new_sample);
	}

	vector<int32_t> final_samples;
	final_samples.push_back(mixed_sample);
	return final_samples;
    }
};