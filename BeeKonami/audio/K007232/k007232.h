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

#ifndef K007232_H
#define K007232_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cstdint>
#include <cmath>
#include <array>
#include <vector>
using namespace std;

namespace beekonami
{
    namespace audio
    {
	class K007232
	{
	    public:
		K007232();
		~K007232();

		uint32_t get_sample_rate(uint32_t clock_rate);
		void init();
		void writeROM(size_t rom_size, size_t data_start, size_t data_len, vector<uint8_t> rom_data);
		uint8_t readReg(int reg);
		bool writeReg(int reg, uint8_t data);
		void clockchip();
		vector<int32_t> get_samples();

		void setVolume(int volume_a, int volume_b);

		void writeROM(vector<uint8_t> rom_data)
		{
		    writeROM(rom_data.size(), 0, rom_data.size(), rom_data);
		}

	    private:
		template<typename T>
		bool testbit(T reg, int bit)
		{
		    return ((reg >> bit) & 1) ? true : false;
		}

		void reset();

		vector<uint8_t> pcm_rom;

		uint32_t get_pcmlimit()
		{
		    if (pcm_rom.empty() || (pcm_rom.size() > 0x20000))
		    {
			return (1 << 17);
		    }
		    else
		    {
			return pcm_rom.size();
		    }
		}

		// Prescaler sizes:
		// 0=12-bit
		// 1=8-bit
		// 2=4-bit
		// 3=8-bit
		array<int, 4> prescaler_bits = {12, 8, 4, 8};

		array<int, 2> ch_volume = {0, 0};

		uint8_t fetchROM(uint32_t addr);

		struct k007232_channel
		{
		    uint32_t channel_counter = 0;
		    uint16_t channel_freq = 0;
		    int prescale_val = 0;
		    uint32_t start_address = 0;
		    uint32_t current_addr = 0;
		    bool is_loop = false;
		    bool is_playing = false;
		    int32_t output = 0;
		};

		array<k007232_channel, 2> channels;
	};
    };
};

#endif // K007232_H