#include <k054000.h>
using namespace beekonami::machine;
using namespace std;

struct K054000Entry
{
    bool is_write = false;
    int reg = 0;
    uint8_t data = 0x00;
    bool expected_val = false;
    bool is_end = false;
};

#define test_name(name) writes_##name

#define test_start(name) vector<K054000Entry> test_name(name) = {
#define test_write(reg, data) {true, (((reg) >> 1) & 0x1F), (data), false, false},
#define test_status(is_true) {false, 0, 0xFF, is_true, false},
#define test_end {false, 0, 0x00, false, true} };

test_start(thndrx2)
    test_write(0x3, 0x00)
    test_write(0x5, 0x00)
    test_write(0x7, 0x00)
    test_write(0x9, 0x00)
    test_write(0xD, 0x00)
    test_write(0xF, 0x00)
    test_write(0x13, 0x00)
    test_write(0x15, 0x00)
    test_write(0x17, 0x00)
    test_write(0x19, 0x00)
    test_write(0x1D, 0x00)
    test_write(0x1F, 0x00)
    test_write(0x23, 0x00)
    test_write(0x25, 0x00)
    test_write(0x27, 0x00)
    test_write(0x2B, 0x00)
    test_write(0x2D, 0x00)
    test_write(0x2F, 0x00)
    test_status(false)
    test_write(0x3, 0xFF)
    test_status(true)
    test_write(0x2B, 0xFF)
    test_status(false)
    test_write(0x5, 0xFF)
    test_status(true)
    test_write(0x2D, 0xFF)
    test_status(false)
    test_write(0x7, 0xFF)
    test_status(true)
    test_write(0x2F, 0xFF)
    test_status(false)
    test_write(0x13, 0xFF)
    test_status(true)
    test_write(0x23, 0xFF)
    test_status(false)
    test_write(0x15, 0xFF)
    test_status(true)
    test_write(0x25, 0xFF)
    test_status(false)
    test_write(0x17, 0xFF)
    test_status(true)
    test_write(0x27, 0xFF)
    test_status(false)
    test_write(0x9, 0xFF)
    test_status(true)
    test_write(0xD, 0xFF)
    test_status(false)
    test_write(0x19, 0xFF)
    test_status(true)
    test_write(0xF, 0xFF)
    test_status(false)
    test_write(0xD, 0x00)
    test_status(true)
    test_write(0x1D, 0xFF)
    test_status(false)
    test_write(0xF, 0x00)
    test_status(true)
    test_write(0x1F, 0xFF)
    test_status(false)
test_end



void write_reg(K054000 &chip, int reg, uint8_t data)
{
    reg &= 0x1F;
    chip.writeReg(reg, data);
}

bool check_status(K054000 &chip, bool expected_val)
{
    uint8_t status_reg = chip.readReg(0x18);
    bool actual_val = ((status_reg & 0x1) != 0);

    if (expected_val != actual_val)
    {
	cout << "Assertion failed: expected " << int(expected_val) << ", but instead got " << int(actual_val) << endl;
	return false;
    }

    return true;
}

bool write_fifo(K054000 &chip, vector<K054000Entry> &entries)
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
	    if (!check_status(chip, entry.expected_val))
	    {
		return false;
	    }
	}
    }

    return true;
}

#define run_test(name) write_fifo(chip, test_name(name))

int main(int argc, char *argv[])
{
    K054000 chip;
    chip.init();

    if (!run_test(thndrx2))
    {
	cout << "Failed Thunder Cross 2 tests." << endl;
	return 1;
    }

    cout << "All tests passed!" << endl;
    return 0;
}