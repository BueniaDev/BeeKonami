#ifndef BEEKONAMI_K051550_H
#define BEEKONAMI_K051550_H

#include <iostream>
#include <cstdint>
using namespace std;

namespace beekonami
{
    struct K051550Pins
    {
	bool pin_ck = false;
	bool pin_reset = true;
	bool pin_out1 = false;
	bool pin_out2 = false;
	bool pin_count1 = false;
	bool pin_count2 = false;
    };

    class K051550
    {
	enum K051550State : int
	{
	    PowerOn = 0,
	    WatchdogIdle = 1,
	    WatchdogReset = 2
	};

	public:
	    K051550();
	    ~K051550();

	    void init();
	    void tick(uint64_t time_ps);

	    K051550Pins &getPins()
	    {
		return current_pins;
	    }

	    bool isResetActive()
	    {
		return (current_state == PowerOn);
	    }

	private:
	    K051550Pins current_pins;

	    K051550State current_state;

	    uint64_t current_time_ps = 0;

	    bool prev_ck = false;

	    uint64_t fromMS(float ms)
	    {
		return uint64_t(ms * 1e9);
	    }
    };
};

#endif // BEEKONAMI_K051550_H