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

#ifndef K053260_H
#define K053260_H

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
	class K053260
	{
	    public:
		K053260();
		~K053260();

		uint32_t get_sample_rate(uint32_t clock_rate);
		void init();
		void setDebugMode(bool is_enabled);
		void writeROM(size_t rom_size, size_t data_start, size_t data_len, vector<uint8_t> rom_data);
		uint8_t readMaster(int addr);
		void writeMaster(int addr, uint8_t data);
		uint8_t readReg(int offs);
		void writeReg(int offs, uint8_t data);
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

		bool is_debug_mode = false;

		array<uint8_t, 2> master_ports;
		array<uint8_t, 2> minion_ports;

		void write_voice_reg(int ch_num, int offs, uint8_t data);

		struct kdsc_voice
		{
		    uint16_t pitch_reg = 0;
		    uint16_t length = 0;
		    uint32_t start_addr = 0;
		    int volume = 0;
		    int pan_reg = 0;
		    bool is_loop = false;
		    bool is_kadpcm = false;
		    bool is_playing = false;
		    bool prev_keyon = false;
		    uint16_t counter = 0;
		    uint32_t current_addr = 0;
		    int8_t sample = 0;
		    array<int32_t, 2> output = {0, 0};
		    array<int, 2> pan_volume = {0, 0};
		};

		void key_on(kdsc_voice &channel);
		void key_off(kdsc_voice &channel);
		void update_pan_volume(kdsc_voice &channel);

		void clock_voice(kdsc_voice &channel);

		uint8_t read_rom(uint32_t addr);

		array<kdsc_voice, 4> voices;

		int chip_address = 0;

		uint8_t keyon_reg = 0;
		bool is_read_rom = false;
		bool is_output_enabled = false;

		array<array<int, 8>, 2> pan_mul_table = 
		{
		    0, 65536, 59870, 53684, 46341, 37950, 26656,     0,
		    0,     0, 26656, 37950, 46341, 53684, 59870, 65536
		};

		array<int8_t, 16> kadpcm_table = 
		{
		       0,   1,   2,   4,  8, 16, 32, 64,
		    -128, -64, -32, -16, -8, -4, -2, -1
		};

		vector<uint8_t> kadpcm_rom;
	};
    };
};

#endif // K053260_H