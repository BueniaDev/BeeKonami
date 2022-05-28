#include <k007232.h>
#include <fstream>
#define DR_WAV_IMPLEMENTATION
#include "dr_wav.h"
using namespace beekonami::audio;
using namespace std;

float out_step = 0.0f;
float in_step = 0.0f;
float out_time = 0.0f;

vector<int16_t> sample_buffer;

vector<int32_t> get_sample(K007232 &chip)
{
    while (out_step > out_time)
    {
	chip.clockchip();
	out_time += in_step;
    }

    out_time -= out_step;
    return chip.get_samples();
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

void write_reg(K007232 &chip, int reg, uint8_t data)
{
    bool is_volume = chip.writeReg(reg, data);

    if (is_volume)
    {
	chip.setVolume((data >> 4), (data & 0xF));
	// chip.setVolume((data & 0xF), (data >> 4));
    }
}

void read_reg(K007232 &chip, int reg)
{
    chip.readReg(reg);
}

struct K007232Entry
{
    bool is_write = false;
    int reg = 0;
    uint8_t data = 0x00;
    bool is_end = false;
};

#define chip_name(game) writes_##game

#define chip_start(game) vector<K007232Entry> chip_name(game) = {
#define chip_read(reg) {true, ((reg) & 0xF), 0xFF, false},
#define chip_write(reg, data) {true, ((reg) & 0xF), (data), false},
#define chip_end {false, 0x0, 0x00, true} };

chip_start(tmnt)
    chip_write(0xD, 0x00)
    chip_write(0xC, 0x00)
    chip_write(0xC, 0x00)
    chip_write(0xD, 0x00)
    chip_write(0x2, 0xE0)
    chip_write(0x3, 0x9B)
    chip_write(0x4, 0x01)
    chip_write(0xC, 0xF0)
    chip_write(0x0, 0xD3)
    chip_write(0x1, 0x0F)
    chip_read(0x5)
chip_end

chip_start(aliens1)
    chip_write(0xC, 0x00)
    chip_write(0xD, 0x00)

    chip_write(0x8, 0x2D)
    chip_write(0x9, 0x5D)
    chip_write(0xA, 0x00)
    chip_write(0x6, 0xC0)
    chip_write(0x7, 0x0F)
    chip_write(0xC, 0xF0)
    chip_read(0xB)
chip_end

chip_start(aliens2)
    chip_write(0xC, 0x00)
    chip_write(0xD, 0x00)

    chip_write(0x8, 0xA5)
    chip_write(0x9, 0x84)
    chip_write(0xA, 0x00)
    chip_write(0x6, 0xCA)
    chip_write(0x7, 0x0F)
    chip_write(0xC, 0xF0)
    chip_read(0xB)
chip_end

void write_fifo(K007232 &chip, vector<K007232Entry> &entries)
{
    for (auto &entry : entries)
    {
	if (entry.is_end)
	{
	    break;
	}

	if (entry.is_write)
	{
	    write_reg(chip, entry.reg, entry.data);
	}
	else
	{
	    read_reg(chip, entry.reg);
	}
    }
}

int main(int argc, char *argv[])
{
    // string rom_name = "875b04.e05";
    string rom_name = "963a26.c13";
    vector<uint8_t> rom_data;

    if (!loadfile(rom_name, rom_data))
    {
	return 1;
    }

    K007232 chip;

    out_step = chip.get_sample_rate(3579545);
    in_step = 48000;
    out_time = 0.f;

    chip.init();
    chip.writeROM(rom_data);
    // write_fifo(chip, chip_name(aliens1));
    // write_fifo(chip, chip_name(aliens2));
    write_fifo(chip, chip_name(tmnt));

    for (int i = 0; i < (48000 * 5); i++)
    {
	auto samples = get_sample(chip);

	int32_t result = (samples.at(0) + samples.at(1));
	int16_t sample = clamp<int16_t>(result, -32768, 32767);
	sample_buffer.push_back(sample);
	sample_buffer.push_back(sample);
    }

    drwav wav;
    drwav_data_format format;
    format.container = drwav_container_riff;
    format.format = DR_WAVE_FORMAT_PCM;
    format.channels = 2;
    format.sampleRate = 48000;
    format.bitsPerSample = 16;
    drwav_init_file_write(&wav, "sample.wav", &format, NULL);

    drwav_write_raw(&wav, (sample_buffer.size() * 2), sample_buffer.data());
    drwav_uninit(&wav);

    return 0;
}