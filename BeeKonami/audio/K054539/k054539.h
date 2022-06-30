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

#ifndef K054539_H
#define K054539_H

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
	class K054539
	{
	    public:
		K054539();
		~K054539();

		uint32_t get_sample_rate(uint32_t clock_rate);
		void init();
		void config(int flags);
		void writeROM(size_t rom_size, size_t data_start, size_t data_len, vector<uint8_t> rom_data);
		void writeRAM(uint16_t addr, uint8_t data);
		void clockchip();
		vector<int32_t> get_samples();

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

		void write_reg(int ch, int offs, uint8_t data);

		bool is_reverse_stereo = false;
		bool is_reverb_disabled = false;
		bool is_keyon_update = false;

		struct k054539_channel
		{
		    int number = 0;
		    bool is_keyon = false;
		    bool is_reverse = false;
		    bool is_loop = false;
		    int ch_type = 0;
		    uint32_t delta_addr = 0;
		    int volume = 0;
		    int reverb_volume = 0;
		    int pan_reg = 0;
		    int reverb_delay = 0;
		    uint32_t loop_address = 0;
		    uint32_t start_address = 0;
		    uint32_t current_address = 0;
		    int current_pos = 0;
		    int32_t prev_value = 0;
		    int32_t current_value = 0;
		    bool is_high_nibble = false;
		    uint8_t current_byte = 0;
		    array<int32_t, 2> output = {0, 0};
		};

		array<k054539_channel, 8> channels;

		bool enable_pcm = false;
		bool is_reg_disable = false;

		void key_on(k054539_channel &channel);
		void key_off(k054539_channel &channel);

		uint8_t readROM(uint32_t addr);

		vector<uint8_t> k054539_rom;

		array<int32_t, 256> voltab;
		array<double, 15> pantab;

		array<int16_t, 16> dpcm_table = 
		{
		    0,   1,   2,   4,   8, 16, 32, 64,
		    0, -64, -32, -16,  -8, -4, -2, -1
		};
	};
    };
};

#endif // K054539_H