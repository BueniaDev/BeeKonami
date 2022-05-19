#include <k052109.h>
#include <k051962.h>
#include <sstream>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
using namespace beekonami::video;
using namespace std;

#include "palette.inl"

template<typename T>
bool testbit(T reg, int bit)
{
    return ((reg >> bit) & 1) ? true : false;
}

bool loadfile(string filename, vector<uint8_t> &data)
{
    ifstream file(filename, ios::in | ios::binary | ios::ate);

    if (!file.is_open())
    {
	return false;
    }

    if (file.is_open())
    {
	streampos size = file.tellg();
	data.resize(size, 0);
	file.seekg(0, ios::beg);
	file.read((char*)data.data(), data.size());
 	file.close();
    }

    return true;
}

uint8_t read_byte(vector<uint8_t> data, uint32_t &pc)
{
    if (pc >= data.size())
    {
	return 0x00;
    }

    return data.at(pc++);
}

uint16_t read_word(vector<uint8_t> data, uint32_t &pc)
{
    uint8_t high = read_byte(data, pc);
    uint8_t low = read_byte(data, pc);

    return ((high << 8) | low);
}

uint32_t read_long(vector<uint8_t> data, uint32_t &pc)
{
    uint16_t high = read_word(data, pc);
    uint16_t low = read_word(data, pc);

    return ((high << 16) | low);
}

vector<uint8_t> read_vector(vector<uint8_t> data, uint32_t &pc)
{
    vector<uint8_t> data_vec;
    size_t vec_size = read_long(data, pc);

    data_vec.resize(vec_size, 0);

    auto begin = (data.begin() + pc);
    auto end = (begin + vec_size);

    copy(begin, end, data_vec.begin());
    pc += vec_size;
    return data_vec;
}

string read_string(vector<uint8_t> data, uint32_t &pc, int length)
{
    string data_str;

    for (int i = 0; i < length; i++)
    {
	data_str.push_back(read_byte(data, pc));
    }

    return data_str;
}

vector<uint8_t> tile_rom;
array<uint8_t, (512 * 256 * 3)> framebuffer;

array<uint8_t, 3> fetchLevel(int level)
{
    uint8_t color = ((level * 255) / 15);
    return {color, color, color};
}

array<uint8_t, 3> getColor(int pal_num)
{
    array<uint8_t, 3> color = {0, 0, 0};
    pal_num &= 0xFF;

    color[0] = (palette.at(pal_num) << 4);
    color[1] = (palette.at(0x100 + pal_num) << 4);
    color[2] = (palette.at(0x200 + pal_num) << 4);

    return color;
}

void set_pixel(int xpos, int ypos, array<uint8_t, 3> color)
{
    uint32_t pixel_offs = ((xpos + (ypos * 512)) * 3);

    for (int i = 0; i < 3; i++)
    {
	framebuffer.at(pixel_offs + i) = color.at(i);
    }
}

auto shuriboy_callback = [](uint8_t tile_num, uint8_t color_attrib, int bank) -> uint32_t
{
    uint32_t tile_addr = tile_num;
    tile_addr |= (((color_attrib & 0xC) << 6) | (bank << 10));

    if ((color_attrib & 0x2) != 0)
    {
	tile_addr |= 0x1000;
    }

    return tile_addr;
};

void save_png(int frame_num)
{
    stringstream filename;
    filename << "frame" << frame_num << ".png";
    stbi_write_png(filename.str().c_str(), 512, 256, 3, framebuffer.data(), (512 * 3));
}

void render_layer(K052109 &tile_chip, K051962 &tile_gfx_chip, int layer_num, int prior)
{
    bool is_prior = (prior == 0) ? true : false;
    auto gfx_tiles = tile_chip.render();

    auto fixed_tilemap = gfx_tiles[layer_num];
    auto tile_buffer = tile_gfx_chip.render(layer_num, fixed_tilemap);

    for (int xpos = 0; xpos < 512; xpos++)
    {
	for (int ypos = 0; ypos < 256; ypos++)
	{
	    uint32_t pixel_offs = (xpos + (ypos * 512));
	    int gfx_addr = tile_buffer.at(pixel_offs);
	    int tile_num = (gfx_addr & 0xF);
	    int color_base = (layer_num == 0) ? 0 : 8;
	    int color_num = (color_base + ((gfx_addr >> 5) & 0x7));
	    int color_offs = ((color_num * 16) + tile_num);

	    bool priority = ((gfx_addr & 0x10) != 0);

	    if (priority == is_prior)
	    {
		continue;
	    }

	    if (tile_num == 0)
	    {
		continue;
	    }

	    set_pixel(xpos, ypos, getColor(color_offs));
	}
    }
}

void render_frame(K052109 &tile_chip, K051962 &tile_gfx_chip, int frame_num)
{
    framebuffer.fill(0);
    array<int, 3> layer_order = {2, 1, 0};

    for (int prior = 0; prior < 2; prior++)
    {
	for (int layer = 0; layer < 3; layer++)
	{
	    render_layer(tile_chip, tile_gfx_chip, layer_order.at(layer), prior);
	}
    }

    save_png(frame_num);
}

bool interpret_log(vector<uint8_t> data, K052109 &tile_chip, K051962 &tile_gfx_chip)
{
    uint32_t pc = 0;

    string opening_string = read_string(data, pc, 12);

    if (opening_string != "BEEKONAMIDMP")
    {
	cout << "Invalid log file" << endl;
	return false;
    }

    uint8_t major_version = read_byte(data, pc);
    uint8_t minor_version = read_byte(data, pc);

    if ((major_version != 1) || (minor_version != 0))
    {
	cout << "Invalid log file version" << endl;
	return false;
    }

    uint8_t device_byte = read_byte(data, pc);

    if ((device_byte & 0x1) == 0)
    {
	cout << "Invalid device byte" << endl;
	return false;
    }

    auto tile_callback = shuriboy_callback;

    tile_gfx_chip.set_tile_callback(tile_callback);
    tile_chip.init();
    tile_gfx_chip.init();

    bool quit = false;
    int frame_num = 1;

    while (!quit)
    {
	uint8_t command = read_byte(data, pc);

	switch (command)
	{
	    case 0x00:
	    {
		uint16_t address = read_word(data, pc);
		uint8_t value = read_byte(data, pc);
		bool is_ben = tile_chip.write(address, value);

		if (is_ben)
		{
		    tile_gfx_chip.write(value);
		}
	    }
	    break;
	    case 0x01:
	    {
 		tile_chip.set_rmrd_line(true);
	    }
	    break;
	    case 0x02:
	    {
 		tile_chip.set_rmrd_line(false);
	    }
	    break;
	    case 0x03:
	    {
		// cout << "Rendering frame..." << endl;
		render_frame(tile_chip, tile_gfx_chip, frame_num++);
	    }
	    break;
	    case 0x04:
	    {
		cout << "Quitting..." << endl;
		quit = true;
	    }
	    break;
	    case 0x05:
	    {
		tile_rom = read_vector(data, pc);
		tile_gfx_chip.set_gfx_rom(tile_rom);
	    }
	    break;
	    default:
	    {
		cout << "Unrecognized command of " << hex << int(command) << endl;
		exit(1);
	    }
	    break;
	}
    }

    return true;
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
	cout << "Usage: k052109-test [log file]" << endl;
	return 1;
    }

    vector<uint8_t> log_data;

    if (!loadfile(argv[1], log_data))
    {
	cout << "Could not open log file." << endl;
	return 1;
    }

    K052109 tile_chip;
    K051962 tile_gfx_chip;
    if (!interpret_log(log_data, tile_chip, tile_gfx_chip))
    {
	cout << "Could not process log file." << endl;
	return 1;
    }

    tile_rom.clear();
    return 0;
}