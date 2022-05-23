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

#ifndef K052109_H
#define K052109_H

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
    namespace video
    {
	using gfxaddr = array<uint32_t, 0x20000>;
	using tilereadfunc = function<uint32_t(uint8_t, uint8_t, int)>;

	class K052109
	{
	    public:
		K052109();
		~K052109();

		void init();
		void set_gfx_rom(vector<uint8_t> rom);
		void set_tile_read_cb(tilereadfunc cb);

		uint8_t read(uint16_t addr);
		bool write(uint16_t addr, uint8_t data);
		void set_rmrd_line(bool line);

		gfxaddr render(int layer_num);

	    private:
		template<typename T>
		bool testbit(T reg, int bit)
		{
		    return ((reg >> bit) & 1) ? true : false;
		}

		void reset();

		gfxaddr render_fixed();
		gfxaddr render_layer_a();
		gfxaddr render_layer_b();

		uint8_t reg_1C00 = 0;
		uint8_t reg_1D80 = 0;
		uint8_t reg_1F00 = 0;

		array<bool, 2> is_scx_enable = {false, false};
		array<bool, 2> is_scx_interval = {false, false};
		array<bool, 2> is_scy_enable = {false, false};

		uint8_t gfx_rom_bank = 0;

		bool is_flip_screen = false;
		bool is_flip_y_enable = false;
		bool is_rm_rd = false;

		array<uint8_t, 0x4000> vram;

		vector<uint8_t> gfx_rom;

		tilereadfunc tile_read;

		uint8_t fetch_rom(uint32_t addr);
	};
    };
};


#endif // K051960_H