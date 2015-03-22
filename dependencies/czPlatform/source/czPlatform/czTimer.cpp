/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com
	
	purpose:
	
*********************************************************************/

#include "czPlatformStableHeaders.h"

#include "czTimer.h"

namespace cz
{

HighResolutionTimer::HighResolutionTimer()
{
	reset();
}

void HighResolutionTimer::reset()
{
	LARGE_INTEGER perf;
	QueryPerformanceFrequency( &perf );
	m_secondsPerCycle = (double)1.0 / perf.QuadPart;
	QueryPerformanceCounter( &m_base );
}

double HighResolutionTimer::seconds()
{
	LARGE_INTEGER val;
	QueryPerformanceCounter( &val );
	return  double(val.QuadPart - m_base.QuadPart) * m_secondsPerCycle;
}


} // namespace cz
