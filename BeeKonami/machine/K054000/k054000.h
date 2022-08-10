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

#ifndef K054000_H
#define K054000_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cstdint>
#include <cmath>
#include <array>
#include <vector>
#include <functional>
using namespace std;

namespace beekonami
{
    namespace machine
    {
	class K054000
	{
	    public:
		K054000();
		~K054000();

		void init();

		uint8_t readReg(int reg);
		void writeReg(int reg, uint8_t data);

		int readACX()
		{
		    return acx_reg;
		}

		int readACY()
		{
		    return acy_reg;
		}

		int readBCX()
		{
		    return bcx_reg;
		}

		int readBCY()
		{
		    return bcy_reg;
		}

	    private:
		template<typename T>
		bool testbit(T reg, int bit)
		{
		    return ((reg >> bit) & 1) ? true : false;
		}

		array<uint8_t, 4> acx_raw;
		array<uint8_t, 4> acy_raw;
		array<uint8_t, 4> bcy_raw;
		array<uint8_t, 4> bcx_raw;

		int acx_reg = 0;
		int acy_reg = 0;
		int bcx_reg = 0;
		int bcy_reg = 0;

		int aax_reg = 0;
		int aay_reg = 0;
		int bax_reg = 0;
		int bay_reg = 0;

		int32_t convert_result(array<uint8_t, 4> buffer);
		int32_t convert_result_delta(array<uint8_t, 4> buffer);

		bool axis_check(uint32_t ac, uint32_t bc, uint32_t aa, uint32_t ba);
	};
    };
};

#endif // K054000_H