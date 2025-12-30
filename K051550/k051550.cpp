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
	// Note: Timing values were based on calculations from the
	// official datasheet of the MB3773
	// (which the K051550 has been found to use),
	// as well as an actual measurement
	// of the CT pin on a real K051550 of 1.224 volts,
	// which was taken from the link below:
	// https://www.arcade-projects.com/threads/konami-simpsons-watchdogging.27546/

	current_time_ps += time_ps;
	switch (current_state)
	{
	    case PowerOn:
	    {
		current_pins.pin_reset = false;

		// Power-on reset time is (CT (in uF) * 1000)
		if (current_time_ps >= fromMS(1224.0))
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
		    current_time_ps = 0;
		}
		// Watchdog idle time is (CT (in uF) * 100) + (CT (in uF) * 20) (worst-case scenario)
		else if (current_time_ps >= fromMS(146.88))
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

		// Watchdog reset time is CT (in uF) * 20
		if (current_time_ps >= fromMS(24.48))
		{
		    // cout << "Watchdog reset complete" << endl;
		    current_pins.pin_reset = true;
		    current_time_ps = 0;
		    current_state = WatchdogIdle;
		}
	    }
	    break;
	}

	// Handle coin counters
	current_pins.pin_count1 = current_pins.pin_out1;
	current_pins.pin_count2 = current_pins.pin_out2;
    }
};