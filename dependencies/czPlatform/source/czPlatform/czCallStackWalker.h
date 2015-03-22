/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com

	created:	2012/02/12
	created:	12:2:2012   16:10
	filename: 	czCallStackWalker.h
	
	purpose:
	Call stack inspection
*********************************************************************/


#ifndef czCallStackWalker_h__
#define czCallStackWalker_h__

#include "czPlatformPrerequisites.h"

#include "czUTF8String.h"

// Only supported for windows at the moment
#if CZ_CALLSTACKWALKER_ENABLED
#if  CZ_PLATFORM!=CZ_PLATFORM_WIN32
	#error CallStack only supported for Win32 at the moment. Set CZ_CALLSTACKWALKER_ENABLED to 0 in your config file
#endif
#if defined(__MINGW32__ )
	#error CallStack not support for MINGW32
#endif

#include <imagehlp.h>

namespace cz
{
	/*! \addtogroup Debugging
		@{
	*/

	/*!
	* \brief Single call stack address
	*/
	struct CallStackAddress
	{
	public:
		CallStackAddress()
		{
			mAddress = 0;
		}

		/*! Checks if it has a valid address */
		bool check() const
		{
			return (mAddress!=0) ? true : false;
		}

		/*! Returns the default output format.
		*/	
		inline static const char* getDefaultFormat()
		{
			return "%m!%s File %fs Line %l\n";
		}

		/*! writes the address information to a stream, using the specified format.
		\param os Output stream to write to
		\param format Format string

		You can use the following printf style formatters in the string: <BR><BR>
		format argument
		"%m"  : module <BR>
		"%f"  : file (complete path) <BR>
		"%fs" : file (only the file name) <BR>
		"%l"  : line <BR>
		"%ld" : line's displacement <BR>
		"%s"  : symbol <BR>
		"%sd" : symbol's displacement <BR>
		<BR>
		for example	 <BR>
		"%f(%l) : %m at %s\n" <BR>
		"%m.%s + %sd bytes, in %f:line %l\n" <BR>
		*/
		bool generateText(std::ostream& os, const char* format) const;

	protected:
		friend class CallStack;

		void setAddress(DWORD64 addr)
		{
			mAddress = addr;
		}

		// symbol handler queries
		unsigned getModule(char* buffer, unsigned len) const;
		unsigned getSymbol(char* buffer, unsigned len, DWORD* displacement = 0) const;
		unsigned getFileLine(char* buffer, unsigned len, unsigned* line, DWORD* displacement = 0) const;
		static bool getLineFromAddr(HANDLE hProc, DWORD64 addr, DWORD* displacement, IMAGEHLP_LINE64 * line);

		DWORD64 mAddress;
	};

	/*! A complete call stack
	 #init needs to be called to initialized required internals, before capturing stacks, and #term
	 called at application shutdown.
	 To capture a call stack, use the method #capture .
	 */
	class CallStack
	{
	public:
		enum
		{
			MAXFRAMES = 20
		};

		CallStack()
		{
			mFrameCount = 0;
		}

		/*! Initializes the required internals for callstack capture to work */
		static void init();
		/*! Frees internal resources used. */
		static void term();

		/*! Captures a callstack.
		\param callstack Where to save the callstack information
		*/
		static void capture(CallStack &callstack);

		//! Clears the call stack information
		void reset()
		{
			mFrameCount = 0;
		}

		//! Returns how deep is the call stack
		int getNumFrames() const
		{
			return mFrameCount;
		}

		/*! Returns the callstack address information at the requested index
		\param index What frame to get. Use #getNumFrames to know how many frames are available
		*/
		const CallStackAddress& getAddr(int index) const
		{
			return mAddrs[index];
		}

		/*! Outputs to the callstack information to the the standard output.
		\param framesToSkip How many stack addresses to skip. This is useful to remove from the output
		functions used for the capture itself.
		\param format
			Output format. Look at CallStackAddress::generateText for more information.
			If NULL, it will use the default format.
		*/
		void output(int framesToSkip, const char* format=NULL) const;

		/*! Returns a string with the callstack information
		\param framesToSkip How many stack addresses to skip. This is useful to remove from the output
		functions used for the capture itself.
		\param format
			Output format. Look at CallStackAddress::generateText for more information.
			If NULL, it will use the default format.
		*/
		const char* createOutput(int framesToSkip, const char* format=NULL) const;

	private:

		void addAddr(DWORD64 addr)
		{
			if (mFrameCount<MAXFRAMES)
			{
				mAddrs[mFrameCount].setAddress(addr);
				mFrameCount++;
			}
		}

		static bool loadModuleSymbols(HANDLE hProcess, HMODULE hMod);

		static bool msInitialized;
		int mFrameCount;

		CallStackAddress mAddrs[MAXFRAMES];
	};

	/*
		@}
	*/

} // namespace cz


#endif // CZ_CALLSTACKWALKER_ENABLED

#endif // czCallStackWalker_h__
