#include <k053251.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
using namespace beekonami::video;
using namespace std;

void punkshot_test(K053251 &prior_chip)
{
    prior_chip.set_priority(0, 0x3F);
    prior_chip.set_priority(1, 0x20);
    prior_chip.set_priority(2, 0x3F);

    prior_chip.write(9, 0x04);
    prior_chip.write(10, 0x25);
    prior_chip.write(12, 0x04);
    prior_chip.write(1, 0x28);
    prior_chip.write(2, 0x24);
    prior_chip.write(4, 0x2C);
    prior_chip.write(3, 0x34);
    prior_chip.write(6, 0x26);
    prior_chip.write(11, 0x00);

    prior_chip.set_input(0, 0);
    prior_chip.set_input(1, 0);
    prior_chip.set_input(2, 0);
    prior_chip.set_input(3, 0);
    prior_chip.set_input(4, 0);

    prior_chip.set_input(1, 0xF);
    prior_chip.set_input(2, 0xE);
    
    prior_chip.set_input(3, 0xD);
    prior_chip.set_input(4, 0xC);

    uint16_t input = prior_chip.get_output();

    cout << "Color priority: " << hex << int(input) << endl;
}

int main(int argc, char* argv[])
{
    K053251 prior_chip;
    prior_chip.init();
    punkshot_test(prior_chip);
    return 0;
}