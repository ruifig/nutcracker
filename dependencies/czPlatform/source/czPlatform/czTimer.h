/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com
	
	purpose:
	Timer services	
*********************************************************************/

#ifndef _czTimer_h_
#define _czTimer_h_


namespace cz
{

	/*! High resolution timer
	To simply time a section of code, do this:
	\code
	HighResolutionTimer timer;
	... some code ...
	double secondsElapsed = timer.seconds();
	.... more code ...
	// Get the total of seconds since the timer creation
	double totalSeconds = timer.seconds();

	// Reset the base time for counting
	timer.reset()
	... some code ....
	// Get the number of seconds since the last reset()
	secondsElapsed = timer.seconds();
	*/
	class HighResolutionTimer {
	public:

		HighResolutionTimer();

		void reset();
		double seconds();
		double milliseconds()
		{
			return seconds() * 1000.0;
		}

	private:
		double m_secondsPerCycle;
		LARGE_INTEGER  m_base;
	};

} // namespace cz

#endif // _czTimer_h_