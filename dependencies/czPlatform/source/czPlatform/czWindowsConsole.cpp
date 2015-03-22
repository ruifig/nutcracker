/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com
	
	purpose:
	
*********************************************************************/

#include "czPlatformStableHeaders.h"

#if CZ_PLATFORM == CZ_PLATFORM_WIN32

#include "czWindowsConsole.h"
#include <fcntl.h>
#include <io.h>
#include <iostream>
#include <fstream>

namespace cz
{

#ifndef _USE_OLD_IOSTREAMS
using namespace std;
#endif

WindowsConsole::WindowsConsole()
{
	mConsoleHandle = INVALID_HANDLE_VALUE;
}
 WindowsConsole::WindowsConsole(int width, int height, int bufferWidth, int bufferHeight)
 {
	 mConsoleHandle = INVALID_HANDLE_VALUE;;
	 init(width, height, bufferWidth, bufferHeight);
 }

WindowsConsole::~WindowsConsole()
{
	if (mOwnsConsole && mConsoleHandle!=INVALID_HANDLE_VALUE)
		FreeConsole();
}

void WindowsConsole::init(int width, int height, int bufferWidth, int bufferHeight)
{
	CZ_ASSERT(mConsoleHandle==INVALID_HANDLE_VALUE);
	if (mConsoleHandle!=INVALID_HANDLE_VALUE)
		return;

	CONSOLE_SCREEN_BUFFER_INFO coninfo;

	// allocate a console for this app
	// If the application already has a console, it fails
	mOwnsConsole = AllocConsole()==TRUE;

	mConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	// set the screen buffer to be big enough to let us scroll text
	GetConsoleScreenBufferInfo(mConsoleHandle, &coninfo);
	// Set the screen buffer size
	coninfo.dwSize.Y = bufferHeight;
	coninfo.dwSize.X = bufferWidth;
	SetConsoleScreenBufferSize(mConsoleHandle, coninfo.dwSize);
	// Set the real window size (need to be smaller than the buffer
	coninfo.srWindow.Bottom = height-1;
	coninfo.srWindow.Right = width-1;
	SetConsoleWindowInfo (mConsoleHandle, TRUE, &coninfo.srWindow );
	center();
	enableUTF8Support();
	setTextColour(CONSOLE_COLOUR_WHITE);
}

void WindowsConsole::redirectStd(bool redirectStdOut, bool redirectStdIn, bool redirectStdErr, bool redirectCPP)
{
	int descriptor;
	long lStdHandle;
	FILE *fp;

	if (redirectStdOut)
	{
		// redirect unbuffered STDOUT to the console
		lStdHandle = (long)GetStdHandle(STD_OUTPUT_HANDLE);
		descriptor = _open_osfhandle(lStdHandle, _O_TEXT);
		fp = _fdopen( descriptor, "w" );
		*stdout = *fp;
		setvbuf( stdout, NULL, _IONBF, 0 );	
	}

	if (redirectStdIn)
	{
		// redirect unbuffered STDIN to the console
		lStdHandle = (long)GetStdHandle(STD_INPUT_HANDLE);
		descriptor = _open_osfhandle(lStdHandle, _O_TEXT);
		fp = _fdopen( descriptor, "r" );
		*stdin = *fp;
		setvbuf( stdin, NULL, _IONBF, 0 );	
	}

	if (redirectStdErr)
	{
		// redirect unbuffered STDERR to the console
		lStdHandle = (long)GetStdHandle(STD_ERROR_HANDLE);
		descriptor = _open_osfhandle(lStdHandle, _O_TEXT);
		fp = _fdopen( descriptor, "w" );
		*stderr = *fp;
		setvbuf( stderr, NULL, _IONBF, 0 );	
	}

	if (redirectCPP)
	{
		// make cout, wcout, cin, wcin, wcerr, cerr, wclog and clog
		// point to console as well
		ios::sync_with_stdio();	
	}
}

void WindowsConsole::minimize()
{
	HWND hConsoleWnd = GetConsoleWindow();
	ShowWindow(hConsoleWnd, SW_MINIMIZE);
}

void WindowsConsole::center()
{
	// Reposition windows
	RECT screen;
	GetWindowRect(GetDesktopWindow(), &screen);
	int screenWidth = screen.right - screen.left + 1;
	int screenHeight = screen.bottom - screen.top +1;

	HWND hConsoleWnd = GetConsoleWindow();

	RECT console;
	GetWindowRect(hConsoleWnd, &console);
	int consoleWidth = console.right - console.left + 1;
	int consoleHeight = console.bottom - console.top +1;

	SetWindowPos(hConsoleWnd,0, (screenWidth-consoleWidth)/2,(screenHeight-consoleHeight)/2, 0,0 , SWP_NOSIZE|SWP_NOZORDER);

	SwitchToThisWindow(hConsoleWnd, TRUE);
}

void WindowsConsole::setTextColour(WindowsConsole::Colour colour)
{
	SetConsoleTextAttribute(mConsoleHandle,colour);
	mCurrColour = colour;
}
void WindowsConsole::print(const char* str)
{
	DWORD written;
	// Using WriteConsoleA, to force using Ansi/UTF8
	CZ_CHECK(WriteConsoleA(mConsoleHandle, str, static_cast<DWORD>(strlen(str)), &written, NULL )==TRUE);
}

void WindowsConsole::printf(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	char buf[512];
	if (cz_vsnprintf(buf, sizeof(buf), format, args) == sizeof(buf))
		buf[sizeof(buf)-1] = 0;
	va_end(args);
	print(buf);
}

void WindowsConsole::outputLog(int cat, Logger::LogLevel level, const char* str)
{
	Colour color = mCurrColour;
	if (level==Logger::LOG_INFO)
		setTextColour(CONSOLE_COLOUR_WHITE);
	else if (level==Logger::LOG_WARNING)
		setTextColour(CONSOLE_COLOUR_YELLOW);
	else
		setTextColour(CONSOLE_COLOUR_RED);
	print(str);
	setTextColour(color);
}

void WindowsConsole::enableUTF8Support()
{
	BOOL ret = SetConsoleOutputCP(65001); // utf codepage, as in http://msdn.microsoft.com/en-us/library/dd317756(v=vs.85).aspx
	ret = SetConsoleCP(65001);
}

}

#endif // CZ_PLATFORM_WIN32
