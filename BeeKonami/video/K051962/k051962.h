#ifndef K051962_H
#define K051962_H

#include "utils.h"
using namespace beekonami;

namespace beekonami
{
    namespace video
    {
	class K051962Interface
	{
	    public:
		K051962Interface()
		{

		}

		~K051962Interface()
		{

		}

		virtual uint32_t makeAddr(k052109addr&)
		{
		    return 0;
		}

		virtual uint8_t makeColor(uint8_t)
		{
		    return 0;
		}

		virtual uint8_t readTileROM(uint32_t)
		{
		    return 0;
		}
	};

	class K051962
	{
	    public:
		K051962();
		~K051962();

		void reset();

		void write(uint8_t data);

		void setTileAddr(k052109gfx &addr);
		void render();

		void setInterface(K051962Interface *cb)
		{
		    inter = cb;
		}

		k051962gfx &getGFX()
		{
		    return gfx_addr;
		}

	    private:
		k051962gfx gfx_addr;
		k052109gfx tile_addr;

		K051962Interface *inter = NULL;

		uint32_t makeAddr(k052109addr &addr)
		{
		    if (inter == NULL)
		    {
			return 0;
		    }

		    return inter->makeAddr(addr);
		}

		uint8_t makeColor(uint8_t color)
		{
		    if (inter == NULL)
		    {
			return 0;
		    }

		    return inter->makeColor(color);
		}

		uint8_t readTileROM(uint32_t addr)
		{
		    if (inter == NULL)
		    {
			return 0;
		    }

		    return inter->readTileROM(addr);
		}
	};
    };
};

#endif // K051962_H