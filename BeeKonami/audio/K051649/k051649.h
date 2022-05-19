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

#ifndef K051649_H
#define K051649_H

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
	enum SCCType
	{
	    K051649_Chip,
	    K052539_Chip,
	};

	class K051649
	{
	    public:
		K051649();
		~K051649();

		uint32_t get_sample_rate(uint32_t clock_rate);
		void init(SCCType type = K051649_Chip);
		void writeIO(int port, uint8_t data);
		void clockchip();
		vector<int32_t> get_samples();

	    private:
		template<typename T>
		bool testbit(T reg, int bit)
		{
		    return ((reg >> bit) & 1) ? true : false;
		}

		void set_chip_type(SCCType type);

		SCCType chip_type;

		bool is_k052539()
		{
		    return (chip_type == SCCType::K052539_Chip);
		}

		void reset();

		int chip_address = 0;

		struct scc_channel
		{
		    array<int8_t, 32> waveram;
		    uint16_t frequency = 0;
		    int volume = 0xF;
		    bool is_key_on = false;
		    uint8_t counter = 0;
		    uint32_t clock = 0;
		    int32_t output = 0;
		};

		array<scc_channel, 5> channels;

		void waveform_write(int offs, uint8_t data);
		void waveform_write_k052539(int offs, uint8_t data);
		void volume_write(int offs, uint8_t data);
		void frequency_write(int offs, uint8_t data);
		void key_on_write(uint8_t data);
		void test_reg_write(uint8_t data);

		uint8_t test_reg = 0;
	};
    };
};


#endif // KO51649_H