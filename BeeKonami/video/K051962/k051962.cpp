#include "k051962.h"
using namespace beekonami::video;

namespace beekonami::video
{
    K051962::K051962()
    {

    }

    K051962::~K051962()
    {

    }

    void K051962::reset()
    {
	return;
    }

    void K051962::setTileAddr(k052109gfx &addr)
    {
	tile_addr = addr;
    }

    void K051962::render()
    {
	for (int layer = 0; layer < 3; layer++)
	{
	    for (int xpos = 0; xpos < 512; xpos++)
	    {
		for (int ypos = 0; ypos < 256; ypos++)
		{
		    auto &addr = tile_addr.at(layer).at(xpos, ypos);

		    int py = addr.line;
		    int px = addr.pixelx;

		    uint32_t tile_addr = makeAddr(addr);
		    uint8_t color_attrib = makeColor(addr.color);

		    int tile_offs = ((tile_addr * 32) + (py * 4));

		    int pixel_color = 0;

		    for (int pixel = 3; pixel >= 0; pixel--)
		    {
			uint8_t tile_value = readTileROM(tile_offs + pixel);
			pixel_color |= (((tile_value >> (7 - px)) & 0x1) << pixel);
		    }

		    int pixel_addr = (pixel_color | (color_attrib & 0xF0));

		    if (layer != 0)
		    {
			pixel_addr |= ((color_attrib & 0xF) << 8);
		    }

		    gfx_addr.at(layer).at(xpos, ypos) = pixel_addr;
		}
	    }
	}
    }
};