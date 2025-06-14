#include <cstdio>
using namespace std;

int main()
{
    for (size_t i = 0; i < 256; i++)
    {
	printf("%.9f,input.IN,%d\n", double(i) / 1.0e6 + 1.0e-3, int(i / 8));
    }

    return 0;
}