//
// CrazyGaze (http://www.crazygaze.com)
// Author : Rui Figueira
// Email  : rui@crazygaze.com
//
// --------------------------------------------------------------
//

#include "czPlatformStableHeaders.h"
#include "czLogger.h"

namespace cz
{

CZ_DEFINE_SINGLETON(Logger)

Logger::Logger()
{
	mIDEOutput = true;
	mAutoLineBreak = true;
}

void Logger::suppress(int cat)
{
	StringID((etStringID)cat).setFlags(IDFLAG_Suppressed);
}

void Logger::unsuppress(int cat)
{
	StringID((etStringID)cat).clearFlags(IDFLAG_Suppressed);
}

void Logger::logVar(int cat, LogLevel level, const char* fmt, va_list args)
{
	StringID catid((etStringID)cat);

	if (catid.hasAllFlags(IDFLAG_Suppressed))
		return;

	char buf[1024*64];
	auto count = cz_vsnprintf(buf, sizeof(buf), fmt, args);
	if (!(count>=0 && count<sizeof(buf)))
		buf[sizeof(buf) - 1] = 0;
	const char *szL = "";

	if (level==LOG_INFO)
	{
		szL = "LOG";
	}
	else if (level==LOG_WARNING)
	{
		szL = "WARNING ";
	}
	else if (level==LOG_ERROR)
	{
		szL = "ERROR ";
	}

	char buf2[1024*64];
	count = cz_snprintf(buf2, sizeof(buf2), "%s:%s: %s %s", catid.getString().c_str(), szL, buf, (mAutoLineBreak) ? "\n" : "");
	if (!(count>=0 && count<sizeof(buf2)))
		buf2[sizeof(buf2) - 1] = 0;
	
#if CZ_PLATFORM==CZ_PLATFORM_WIN32
	if (mIDEOutput)
		OutputDebugStringA(buf2);
#endif

	// Send the output to the registered destinations
	for (size_t i=0; i<mOutputs.size(); i++)
	{
		mOutputs[i]->outputLog(cat, level, buf2);
	}
}

void Logger::log(int cat, LogLevel level, const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	logVar(cat, level, fmt, args);
	va_end(args);
}

void Logger::addOutput(LoggerOutput* output)
{
	CZ_ASSERT(std::find(mOutputs.begin(), mOutputs.end(), output) == mOutputs.end());
	mOutputs.push_back(output);
}

void Logger::removeOutput(LoggerOutput* output)
{
	OutputsContainer::iterator it = std::find(mOutputs.begin(), mOutputs.end(), output);
	if (it!=mOutputs.end())
		mOutputs.erase(it);
}

} // namespace cz

