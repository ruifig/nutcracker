/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com

	created:	2012/02/12
	created:	12:2:2012   16:15
	filename: 	czCallStackWalker.cpp
	
	purpose:
	
*********************************************************************/

#include "czPlatformStableHeaders.h"

#include "czCallStackWalker.h"

#if CZ_CALLSTACKWALKER_ENABLED

#include <ostream>
#pragma comment(lib, "Dbghelp.lib")
#pragma comment(lib, "Psapi.lib")

#include <Psapi.h>
#include <sstream>
#include <iostream>

#pragma warning( push )
#pragma warning( disable : 4996 ) 

namespace cz
{

	/************************************************************************************
	*								CallStackAddress
	*************************************************************************************/

	bool CallStackAddress::generateText(std::ostream& os, const char* format) const
	{

		char buf [512] = {0};
		char fbuf[512] = {0};
		char sbuf[512] = {0};

		os << std::dec;

		unsigned ln = 0;
		DWORD ld = 0;
		DWORD sd = 0;
		const char*	 pf	= 0;
		const char*    ps = 0;

		for (const char* p = format; *p; ++p)
		{				
			if (*p == '%')
			{
				++p; // skips '%'
				char c = *p;
				switch (c)
				{
				case 'm':							
					os << (getModule(buf, sizeof(buf)/sizeof(buf[0])) ? buf : "?.?");
					break;
				case 'f':
					if (!pf) 							
						pf = (getFileLine(fbuf, sizeof(fbuf)/sizeof(buf[0]), &ln, &ld)) ? fbuf : " ";
					if (*(p + 1) == 's')
					{
						const char *pFilename=strrchr(pf,'\\');
						if (pFilename)
						{
							os << ++pFilename;
						}
						++p;
					}
					else
					{
						os << pf;
					}						
					break;
				case 'l':
					if (!pf) 							
						pf = (getFileLine(fbuf, sizeof(fbuf)/sizeof(buf[0]), &ln, &ld)) ? fbuf : " ";
					if (*(p + 1) == 'd') { os << ld; ++p; }
					else os << ln;							
					break;
				case 's':
					if (!ps)
						ps = getSymbol(sbuf, sizeof(sbuf)/sizeof(buf[0]), &sd) ? sbuf : "?()";
					if (*(p + 1) == 'd') { os << sd; ++p; }
					else os << ps;
					break;
				case '%':
					os << '%';
					break;
				default:
					os << '%' << c;	// prints unknown format's argument
					break;
				}
			}
			else
			{
				os << *p;
			}
		}

		return true;

	}

	bool CallStackAddress::getLineFromAddr(HANDLE hProc, DWORD64 addr, DWORD * displacement, IMAGEHLP_LINE64 * line)
	{	
		return 0 != SymGetLineFromAddr64 (hProc, addr, displacement, line);
	}

	unsigned CallStackAddress::getModule(char* buffer, unsigned len) const
	{
		if (!check())
			return 0;

		HANDLE hProc = GetCurrentProcess();
		HMODULE hMod = (HMODULE)SymGetModuleBase64(hProc, mAddress);
		if (!hMod) 
		{
			return 0;
		}

		return GetModuleBaseNameA(hProc, hMod, buffer, len);
	}

	unsigned CallStackAddress::getSymbol(char* buffer, unsigned len, DWORD* displacement) const
	{
		if (!check())
		{
			return 0;
		}

		ULONG64 symbolInfoBuffer[(sizeof(SYMBOL_INFO) +
			MAX_SYM_NAME*sizeof(TCHAR) +
			sizeof(ULONG64) - 1) /
			sizeof(ULONG64)];
		PSYMBOL_INFO pSymbol = (PSYMBOL_INFO)symbolInfoBuffer;

		memset(symbolInfoBuffer, 0, sizeof(symbolInfoBuffer)) ;
		pSymbol->SizeOfStruct = sizeof(SYMBOL_INFO);
		pSymbol->MaxNameLen = MAX_SYM_NAME;


		HANDLE hProc = GetCurrentProcess();
		DWORD64 disp = 0;

		if (SymFromAddr(hProc, mAddress, &disp, pSymbol)==FALSE)
		{
			return 0;
		}

		if (displacement) 
		{
			*displacement = static_cast<DWORD>(disp);
		}

		int formatedLen = _snprintf(buffer, len, "%s()", pSymbol->Name);
		if (formatedLen == -1)
			formatedLen = len-1;
		buffer[formatedLen] = 0;	
		return formatedLen;
	}

	unsigned CallStackAddress::getFileLine(char* buffer, unsigned len, unsigned* line, DWORD* displacement) const
	{
		if (!check())
		{
			return 0;
		}

		IMAGEHLP_LINE64 img_line;
		memset(&img_line, 0, sizeof(IMAGEHLP_LINE64));
		img_line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);

		HANDLE hProc = GetCurrentProcess();

		DWORD disp = 0;

		if (!getLineFromAddr(hProc, mAddress, &disp, &img_line))
		{
			return 0;
		}

		if (displacement) 
		{
			*displacement = disp;
		}

		if (line) 
		{
			*line = img_line.LineNumber;
		}

		strncpy_s(buffer, len, img_line.FileName, strlen(img_line.FileName));

		return lstrlenA(buffer);
	}


	/************************************************************************************
	*								CallStack
	*************************************************************************************/

	bool CallStack::msInitialized = false;

	bool CallStack::loadModuleSymbols(HANDLE hProcess, HMODULE hMod)
	{
		char filename[MAX_PATH+1];
		if (!GetModuleFileNameA(hMod, filename, MAX_PATH))
		{
			return false;
		}

		HANDLE hFile = CreateFileA(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, 0);

		if (hFile == INVALID_HANDLE_VALUE) 
		{
			return false;
		}

		SetLastError(ERROR_SUCCESS);

		if (!SymLoadModule64(hProcess, hFile, filename, 0, (DWORD64)hMod, 0) && 
			ERROR_SUCCESS != GetLastError())
		{
			return false;
		}

		return true;

	}

	void CallStack::init()
	{
		// initialises
		SymSetOptions(SymGetOptions()|SYMOPT_DEFERRED_LOADS|SYMOPT_LOAD_LINES);

		HANDLE hProc = GetCurrentProcess();

		if (::SymInitialize(hProc, 0, TRUE))
		{
			msInitialized = true;

			// enumerate modules
			typedef BOOL (WINAPI *ENUMPROCESSMODULES)(HANDLE, HMODULE*, DWORD, LPDWORD);

			HINSTANCE hInst = LoadLibraryA("psapi.dll");

			if (hInst)
			{				
				ENUMPROCESSMODULES fnEnumProcessModules = (ENUMPROCESSMODULES)GetProcAddress(hInst, "EnumProcessModules");

				DWORD cbNeeded = 0;

				if (fnEnumProcessModules &&
					fnEnumProcessModules(GetCurrentProcess(), 0, 0, &cbNeeded) &&
					cbNeeded)
				{	
					HMODULE * pmod = (HMODULE *)alloca(cbNeeded);
					DWORD cb = cbNeeded;
					if (fnEnumProcessModules(GetCurrentProcess(), pmod, cb, &cbNeeded))
					{
						for (unsigned i = 0; i < cb / sizeof (HMODULE); ++i)
						{								
							if (!loadModuleSymbols(hProc, pmod[i]))
							{
								_ASSERTE(0);
							}									
						}							
					}
				}
				else
				{
					_ASSERTE(0);
				}

				FreeLibrary(hInst);
			}
			else
			{
				_ASSERTE(0);
			}


		}
	}

	void CallStack::term()
	{
		if (msInitialized)
		{
			SymCleanup(GetCurrentProcess());
		}
	}

	#ifdef _M_IX86
		//
		// Disable global optimization and ignore /GS waning caused by
		// inline assembly.
		//
		#pragma optimize( "g", off )
		#pragma warning( push )
		#pragma warning( disable : 4748 )
	#endif

	void CallStack::capture(CallStack &callstack)
	{
		CZ_ASSERT(msInitialized && "CallStack::init not called");

		DWORD MachineType;
		CONTEXT Context;
		STACKFRAME64 StackFrame;

		//
		// Use current context.
		//
		// N.B. GetThreadContext cannot be used on the current thread.
		// Capture own context - on i386, there is no API for that.
		//
	#ifdef _M_IX86
		memset(&Context, 0, sizeof( CONTEXT ) );
		Context.ContextFlags = CONTEXT_CONTROL;

		//
		// Those three registers are enough.
		//
		__asm
		{
	Label:
			mov [Context.Ebp], ebp;
			mov [Context.Esp], esp;
			mov eax, [Label];
			mov [Context.Eip], eax;
		}
	#else
		RtlCaptureContext( &Context );
	#endif

		//
		// Set up stack frame.
		//
		ZeroMemory( &StackFrame, sizeof( STACKFRAME64 ) );
	#ifdef _M_IX86
		MachineType                 = IMAGE_FILE_MACHINE_I386;
		StackFrame.AddrPC.Offset    = Context.Eip;
		StackFrame.AddrPC.Mode      = AddrModeFlat;
		StackFrame.AddrFrame.Offset = Context.Ebp;
		StackFrame.AddrFrame.Mode   = AddrModeFlat;
		StackFrame.AddrStack.Offset = Context.Esp;
		StackFrame.AddrStack.Mode   = AddrModeFlat;
	#elif _M_X64
		MachineType                 = IMAGE_FILE_MACHINE_AMD64;
		StackFrame.AddrPC.Offset    = Context.Rip;
		StackFrame.AddrPC.Mode      = AddrModeFlat;
		StackFrame.AddrFrame.Offset = Context.Rsp;
		StackFrame.AddrFrame.Mode   = AddrModeFlat;
		StackFrame.AddrStack.Offset = Context.Rsp;
		StackFrame.AddrStack.Mode   = AddrModeFlat;
	#elif _M_IA64
		MachineType                 = IMAGE_FILE_MACHINE_IA64;
		StackFrame.AddrPC.Offset    = Context.StIIP;
		StackFrame.AddrPC.Mode      = AddrModeFlat;
		StackFrame.AddrFrame.Offset = Context.IntSp;
		StackFrame.AddrFrame.Mode   = AddrModeFlat;
		StackFrame.AddrBStore.Offset= Context.RsBSP;
		StackFrame.AddrBStore.Mode  = AddrModeFlat;
		StackFrame.AddrStack.Offset = Context.IntSp;
		StackFrame.AddrStack.Mode   = AddrModeFlat;
	#else
	#error "Unsupported platform"
	#endif

		callstack.reset();

		//
		// Dbghelp is is singlethreaded, so acquire a lock.
		//
		// Note that the code assumes that
		// SymInitialize( GetCurrentProcess(), NULL, TRUE ) has
		// already been called.
		//

		//EnterCriticalSection( &DbgHelpLock ); TODO

		while ( callstack.getNumFrames() < CallStack::MAXFRAMES )
		{
			if ( ! StackWalk64(
				MachineType,
				GetCurrentProcess(),
				GetCurrentThread(),
				&StackFrame,
				MachineType == IMAGE_FILE_MACHINE_I386
				? NULL
				: &Context,
				NULL,
				SymFunctionTableAccess64,
				SymGetModuleBase64,
				NULL ) )
			{
				//
				// Maybe it failed, maybe we have finished walking the stack.
				//
				break;
			}

			if ( StackFrame.AddrPC.Offset != 0 )
			{
				//
				// Valid frame.
				//
				callstack.addAddr(StackFrame.AddrPC.Offset);
			}
			else
			{
				//
				// Base reached.
				//
				break;
			}
		}

		//LeaveCriticalSection( &DbgHelpLock ); // TODO
	}

	// Restore
	#ifdef _M_IX86
		#pragma warning( pop )
		#pragma optimize( "g", on )
	#endif


	const char* CallStack::createOutput(int framesToSkip, const char* format) const
	{
		const size_t bufSize = 1024*64;
		static char tempbuf[bufSize];

		std::stringstream ss(std::ios::out);
		for (int i=framesToSkip; i<mFrameCount; i++)
		{
			mAddrs[i].generateText(ss, (format) ? format : CallStackAddress::getDefaultFormat());
		}

		size_t len = ss.str().size();
		if (len>bufSize-1)
			len = bufSize-1;

		memcpy(tempbuf, ss.str().c_str(), len);
		tempbuf[len] = 0;

		return tempbuf;
	}

	void CallStack::output(int framesToSkip, const char* format) const
	{
		std::cout << createOutput(framesToSkip, format);
	}

} // namespace cz

#pragma warning( pop )

#endif // CZ_CALLSTACKWALKER_ENABLED

