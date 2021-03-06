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

#ifndef K053251_H
#define K053251_H

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
	enum K053251Priority : int
	{
	    CI0 = 0,
	    CI1 = 1,
	    CI2 = 2,
	    CI3 = 3,
	    CI4 = 4,
	};

	class K053251
	{
	    public:
		K053251();
		~K053251();

		void init();
		void write(int reg, uint8_t data);

		void set_priority(int layer, uint8_t data);
		void set_input(int layer, uint16_t data);

		int get_palette_index(K053251Priority layer);
		uint16_t get_output(K053251Priority layer);

		K053251Priority get_top_layer();

		void set_shadow(int data);
		int get_shadow();

	    private:
		template<typename T>
		bool testbit(T reg, int bit)
		{
		    return ((reg >> bit) & 1) ? true : false;
		}

		template<typename T>
		bool inRangeEx(T reg, int low, int high)
		{
		    int val = int(reg);
		    return ((val >= low) && (val <= high));
		}

		array<int, 5> palette_index;
		array<int, 5> layer_priorities;
		array<int, 3> priority_inputs;
		array<bool, 3> is_priority_enabled;
		array<int, 5> layer_inputs;

		int shadow_reg = 0;

		array<int, 3> shadow_priorities;

		array<bool, 5> is_transparent;
		bool is_prior_swap = false;

		array<K053251Priority, 5> layer_order;

		int get_shadow_priority();
		int get_priority(K053251Priority index);

		void calc_priority();
	};
    };

};

#endif // K053251_H