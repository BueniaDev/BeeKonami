#include <iostream>
#include <iomanip>
#include <cstdint>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
using namespace std;

uint8_t convertDAC(float val)
{
    float val_conv = (val / 4.256507f);
    val_conv = clamp<float>(val_conv, 0.0f, 1.0f);
    return uint8_t(val_conv * 255.f);
}

string toHexStr(uint8_t val)
{
    stringstream ss;
    ss << "0x" << hex << uppercase << setw(2) << setfill('0') << int(val);
    return ss.str();
}

int main(int argc, char *argv[])
{
    ifstream file;
    file.open("values.tab");

    if (!file.is_open())
    {
	cout << "Could not open values.tab" << endl;
	return 1;
    }

    string line;
    vector<float> values;

    while (getline(file, line))
    {
	float val = stof(line);
	values.push_back(val);
    }

    cout << "Value size: " << dec << values.size() << endl;

    if (values.size() != 32)
    {
	cout << "Invalid value vector size of " << dec << values.size() << endl;
	values.clear();
	return 1;
    }

    ofstream out_file("values_out.bin", ios::out | ios::binary | ios::trunc);

    for (int i = 0; i < 32; i++)
    {
	uint8_t val = convertDAC(values.at(i));
	cout << "RGB value for color " << dec << int(i) << ": " << toHexStr(val) << endl;

	out_file.write((char*)&val, 1);
    }

    out_file.close();

    return 0;
}