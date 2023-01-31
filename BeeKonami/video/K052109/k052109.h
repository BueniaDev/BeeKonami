#ifndef K052109_H
#define K052109_H

#include "utils.h"
using namespace beekonami;

namespace beekonami
{
    namespace video
    {
	class K052109Interface
	{
	    public:
		K052109Interface()
		{

		}

		~K052109Interface()
		{

		}

		virtual uint16_t readRender(uint16_t)
		{
		    return 0;
		}

		virtual void writeVRAM(int, uint16_t, uint16_t)
		{
		    return;
		}
	};

	class K052109
	{
	    public:
		K052109();
		~K052109();

		void reset();

		uint8_t read(uint16_t addr);
		void write(uint16_t addr, uint8_t data);
		void setRMRD(bool line);

		void setInterface(K052109Interface *cb)
		{
		    inter = cb;
		}

		void render();

		k052109gfx &getTileAddr()
		{
		    return tile_addr;
		}

	    private:
		template<typename T>
		bool testbit(T reg, int bit)
		{
		    return ((reg >> bit) & 1) ? true : false;
		}

		bool is_rm_rd = false;

		K052109Interface *inter = NULL;

		k052109gfx tile_addr;

		void renderFixed();
		void renderLayerA();
		void renderLayerB();


		uint8_t reg_1C00 = 0;
		uint8_t reg_1C80 = 0;
		uint8_t reg_1D00 = 0;
		uint8_t reg_1D80 = 0;
		uint8_t reg_1E00 = 0;
		uint8_t reg_1F00 = 0;

		uint16_t getStartingAddr()
		{
		    uint16_t addr = 0;
		    int bank = (reg_1C00 & 0x3);

		    switch (bank)
		    {
			case 0: addr = 0x6000; break;
			case 1: addr = 0x4000; break;
			case 2: addr = 0x2000; break;
			case 3: addr = 0x0000; break;
		    }

		    return addr;
		}

		uint16_t readRender(uint16_t addr)
		{
		    addr &= 0x1FFF;

		    if (inter == NULL)
		    {
			return 0;
		    }

		    return inter->readRender(addr);
		}

		void writeVRAM(int bank, uint16_t addr, uint8_t data)
		{
		    uint16_t vram_data = ((data << 8) | data);
		    addr &= 0x1FFF;
		    if (inter != NULL)
		    {
			inter->writeVRAM(bank, addr, vram_data);
		    }
		}
	};
    };
};




#endif // K052109_H