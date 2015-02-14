// Precompiled header

#pragma once

// Predefinitions for windows.h go here
//#define WIN32_LEAN_AND_MEAN	// Exclude rarely-used stuff from Windows headers
#include "targetver.h"			// Set the supported window features
#include <WinSock2.h>

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#ifdef max
	#undef max
	#undef min
#endif

#include "wx/app.h"
#include "wx/grid.h"
#include "wx/treectrl.h"
#include "wx/spinctrl.h"
#include "wx/artprov.h"
#include "wx/clipbrd.h"
#include "wx/image.h"
#include "wx/colordlg.h"
#include "wx/wxhtml.h"
#include "wx/imaglist.h"
#include "wx/dataobj.h"
#include "wx/dcclient.h"
#include "wx/bmpbuttn.h"
#include "wx/menu.h"
#include "wx/toolbar.h"
#include "wx/statusbr.h"
#include "wx/msgdlg.h"
#include "wx/textdlg.h"
#include <wx/stdpaths.h>
#include "wx/aui/aui.h"
#include "wx/stc/stc.h"
#include "wx/xrc/xmlres.h"
#include "wx/dir.h"
#include "wx/regex.h"
#include "wx/dcbuffer.h"

#include <wx/valnum.h>
#include <wx/stc/private.h>

// For the custom Scintilla lexer
#include <string>
#include <map>
#include <vector>
#include "Scintilla.h"
#include "SciLexer.h"
#include "ILexer.h"
#include "LexerModule.h"
#include "OptionSet.h"
#include "CharacterSet.h"
#include "WordList.h"
#include "LexAccessor.h"
#include "StyleContext.h"
#include "SparseState.h"
#include "Catalogue.h"


// Specify Win32xx specific predefinitions here
//#define NO_USING_NAMESPACE		// Don't use Win32xx namespace

typedef  unsigned char  byte;

#if defined(_MSC_VER) && _MSC_VER == 1200 	// For Visual Studio 6
#pragma warning (disable : 4786)	// identifier was truncated
#pragma warning (disable : 4702)	// unreachable code (bugs in Microsoft's STL)
#endif

// Rarely modified header files should be included here
#include <vector>
#include <set>
#include <map>
#include <unordered_map>
#include <string>
#include <sstream>		// Add support for stringstream
#include <algorithm>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <locale>

#include "czPlatform/czPlatformPrerequisites.h"
#include "czPlatform/czPlatformStableHeaders.h"
#include "czPlatform/czFileSystem.h"
#include "czPlatform/czLogger.h"
#include "czPlatform/czFile.h"
#include "czPlatform/czFileSystem.h"
#include "czPlatform/czAlgorithm.h"
#include "czPlatform/czIniFile.h"
#include "czPlatform/czFNVHash.h"
#include "czPlatform/czUTF8String.h"
#include "czPlatform/czScopeGuard.h"
#include "czPlatform/czParameters.h"

#define FORWARD_DECLARE_VIEWCLASS(CLASS) namespace cz { namespace view { class CLASS; } }

