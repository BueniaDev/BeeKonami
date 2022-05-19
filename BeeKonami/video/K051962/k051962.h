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

#ifndef K051962_H
#define K051962_H

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
    namespace video
    {
	using gfxaddr = array<uint32_t, 0x4000>;

	using tilebuffer = array<int, 0x20000>;

	using tilefunc = function<uint32_t(uint8_t, uint8_t, int)>;

	class K051962
	{
	    public:
		K051962();
		~K051962();

		void init();
		void set_gfx_rom(vector<uint8_t> tile_rom);
		void set_tile_callback(tilefunc cb);

		void write(uint8_t data);

		tilebuffer render(int layer, gfxaddr tile_addr);

	    private:
		template<typename T>
		bool testbit(T reg, int bit)
		{
		    return ((reg >> bit) & 1) ? true : false;
		}

		vector<uint8_t> gfx_rom;

		int decode_tile(int tile_number, int tile_pixel);
		uint8_t fetch_tile_rom(size_t addr);

		tilefunc tile_callback;

		bool is_flip_screen = false;
		bool is_flipx_enable = false;
	};
    };
};

#endif // K051962_H