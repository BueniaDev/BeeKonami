#include "k052109.h"
using namespace beekonami::video;
using namespace std;

namespace beekonami::video
{
    K052109::K052109()
    {

    }

    K052109::~K052109()
    {

    }

    uint8_t K052109::read(uint16_t addr)
    {
	uint8_t data = 0;
	return data;
    }

    void K052109::write(uint16_t addr, uint8_t data)
    {
	uint16_t starting_addr = getStartingAddr();

	if ((addr >= starting_addr) && (addr < (starting_addr + 0x6000)))
	{
	    uint32_t vram_offs = (addr - starting_addr);

	    int bank = 0;

	    if (vram_offs < 0x2000)
	    {
		bank = 1;
	    }
	    else if ((vram_offs >= 0x2000) && (vram_offs < 0x4000))
	    {
		bank = 2;
	    }

	    writeVRAM(bank, vram_offs, data);

	    if ((bank == 1) && ((vram_offs & 0x1FFF) >= 0x1C00))
	    {
		int reg = (vram_offs & 0x1F80);

		switch (reg)
		{
		    case 0x1C00: reg_1C00 = data; break;
		    case 0x1D80: reg_1D80 = data; break;
		    case 0x1F00: reg_1F00 = data; break;
		    default:
		    {
			cout << "Writing value of " << hex << int(data) << " to K052109 register of " << hex << int(reg) << endl;
		    }
		    break;
		}
	    }
	}
    }

    void K052109::setRMRD(bool line)
    {
	is_rm_rd = line;
    }

    void K052109::render()
    {
	renderFixed();
	renderLayerA();
	renderLayerB();
    }

    void K052109::renderFixed()
    {
	for (int x = 0; x < 512; x++)
	{
	    for (int y = 0; y < 256; y++)
	    {
		int xpos = x;
		int ypos = y;

		int row = (ypos >> 3);
		int col = (xpos >> 3);

		int pixel = (xpos & 0x7);
		int line = (ypos & 0x7);

		uint32_t offs = ((row * 64) + col);

		uint16_t vram_value = readRender(offs);

		uint8_t color = (vram_value >> 8);

		uint8_t bank_val = testbit(color, 3) ? reg_1F00 : reg_1D80;

		if (testbit(color, 2))
		{
		    bank_val >>= 4;
		}
		else
		{
		    bank_val &= 0xF;
		}

		int cab_pins = ((bank_val >> 2) & 0x3);

		uint8_t color_val = color;

		if (!testbit(reg_1C00, 5))
		{
		    color_val = ((color_val & 0xF3) | ((bank_val & 0x3) << 2));
		}

		auto &addr = tile_addr.at(0).at(x, y);

		addr.line = line;
		addr.vram_lsb = (vram_value & 0xFF);
		addr.color = color_val;
		addr.cab_pins = cab_pins;
		addr.pixelx = pixel;
	    }
	}
    }

    void K052109::renderLayerA()
    {
	for (int x = 0; x < 512; x++)
	{
	    for (int y = 0; y < 256; y++)
	    {
		int xpos = x;
		int ypos = y;

		int row = (ypos >> 3);
		int col = (xpos >> 3);

		int pixel = (xpos & 0x7);
		int line = (ypos & 0x7);

		uint32_t offs = ((row * 64) + col);

		uint16_t vram_value = readRender(0x800 + offs);

		uint8_t color = (vram_value >> 8);

		uint8_t bank_val = testbit(color, 3) ? reg_1F00 : reg_1D80;

		if (testbit(color, 2))
		{
		    bank_val >>= 4;
		}
		else
		{
		    bank_val &= 0xF;
		}

		int cab_pins = ((bank_val >> 2) & 0x3);

		uint8_t color_val = color;

		if (!testbit(reg_1C00, 5))
		{
		    color_val = ((color_val & 0xF3) | ((bank_val & 0x3) << 2));
		}

		auto &addr = tile_addr.at(1).at(x, y);

		addr.line = line;
		addr.vram_lsb = (vram_value & 0xFF);
		addr.color = color_val;
		addr.cab_pins = cab_pins;
		addr.pixelx = pixel;
	    }
	}
    }

    void K052109::renderLayerB()
    {
	for (int x = 0; x < 512; x++)
	{
	    for (int y = 0; y < 256; y++)
	    {
		int xpos = x;
		int ypos = y;

		int row = (ypos >> 3);
		int col = (xpos >> 3);

		int pixel = (xpos & 0x7);
		int line = (ypos & 0x7);

		uint32_t offs = ((row * 64) + col);

		uint16_t vram_value = readRender(0x1000 + offs);

		uint8_t color = (vram_value >> 8);

		uint8_t bank_val = testbit(color, 3) ? reg_1F00 : reg_1D80;

		if (testbit(color, 2))
		{
		    bank_val >>= 4;
		}
		else
		{
		    bank_val &= 0xF;
		}

		int cab_pins = ((bank_val >> 2) & 0x3);

		uint8_t color_val = color;

		if (!testbit(reg_1C00, 5))
		{
		    color_val = ((color_val & 0xF3) | ((bank_val & 0x3) << 2));
		}

		auto &addr = tile_addr.at(2).at(x, y);

		addr.line = line;
		addr.vram_lsb = (vram_value & 0xFF);
		addr.color = color_val;
		addr.cab_pins = cab_pins;
		addr.pixelx = pixel;
	    }
	}
    }
};