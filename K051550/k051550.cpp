#include "k051550.h"
using namespace beekonami;
using namespace std;

namespace beekonami
{
    K051550::K051550()
    {

    }

    K051550::~K051550()
    {

    }

    void K051550::init()
    {
	current_pins = {};
	current_state = PowerOn;
	current_time_ps = 0;
    }

    void K051550::tick(uint64_t time_ps)
    {
	current_time_ps += time_ps;
	switch (current_state)
	{
	    case PowerOn:
	    {
		current_pins.pin_reset = false;

		if (current_time_ps >= fromMS(100))
		{
		    current_pins.pin_reset = true;
		    current_time_ps = 0;
		    current_state = WatchdogIdle;
		}
	    }
	    break;
	    case WatchdogIdle:
	    {
		current_pins.pin_reset = true;

		if (prev_ck && !current_pins.pin_ck)
		{
		    cout << "Kicking watchdog..." << endl;
		    current_time_ps = 0;
		}
		else if (current_time_ps >= fromMS(10))
		{
		    cout << "Watchdog resetting..." << endl;
		    current_pins.pin_reset = false;
		    current_time_ps = 0;
		    current_state = WatchdogReset;
		}

		prev_ck = current_pins.pin_ck;
	    }
	    break;
	    case WatchdogReset:
	    {
		current_pins.pin_reset = false;

		if (current_time_ps >= fromMS(2))
		{
		    cout << "Watchdog reset complete" << endl;
		    current_pins.pin_reset = true;
		    current_time_ps = 0;
		    current_state = WatchdogIdle;
		}
	    }
	    break;
	}

	current_pins.pin_count1 = current_pins.pin_out1;
	current_pins.pin_count2 = current_pins.pin_out2;
    }
};