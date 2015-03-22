/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com
	
	purpose:
	
*********************************************************************/

#ifndef _CZLOGGER_H_
#define _CZLOGGER_H_

#include "czPlatformPrerequisites.h"
#include "czSingleton.h"
#include "czString.h"
#include "czStringID.h"

#define czDebug(category, fmt, ...) cz::Logger::getSingleton().log(cz::category, cz::Logger::LOG_INFO, fmt, ##__VA_ARGS__)
#define czWarning(category, fmt, ...) cz::Logger::getSingleton().log(cz::category, cz::Logger::LOG_WARNING, fmt, ##__VA_ARGS__)
#define czError(category, fmt, ...) cz::Logger::getSingleton().log(cz::category, cz::Logger::LOG_ERROR, fmt, ##__VA_ARGS__)

namespace cz
{

	// forward declaration
	class LoggerOutput;

	class Logger : public TSingleton<Logger>
	{
	public:
		enum LogLevel
		{
			LOG_INFO,
			LOG_WARNING,
			LOG_ERROR
		};

		void suppress(int cat);
		void unsuppress(int cat);
		void log(int cat, LogLevel level, const char* fmt, ...);
		void logVar(int cat, LogLevel level, const char* fmt, va_list args);
		void setIDEOutput(bool enabled);
		void addOutput(LoggerOutput* output);
		void removeOutput(LoggerOutput* output);
		bool setAutoLineBreak(bool val)
		{
			bool ret = mAutoLineBreak;
			mAutoLineBreak = val;
			return ret;
		}

		Logger();
	private:

		typedef std::vector<LoggerOutput*, STLAllocator<LoggerOutput*, GeneralCategoryAllocator> > OutputsContainer;
		OutputsContainer mOutputs;
		bool mIDEOutput;
		bool mAutoLineBreak;
	};

	//! Classes that want to be used as logger output, need to implement this interface
	class LoggerOutput
	{
	public:
		virtual ~LoggerOutput() {}
		virtual void outputLog(int cat, Logger::LogLevel level, const char* str) = 0;
	};

}

#endif
