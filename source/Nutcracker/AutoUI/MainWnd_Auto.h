/////////////////////////////////////////////////////////////////////////////
// Name:        MainWnd_Auto.h
// Purpose:     
// Author:      Rui Figueira
// Modified by: 
// Created:     13/02/2015 21:16:57
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef _MAINWND_AUTO_H_
#define _MAINWND_AUTO_H_


/*!
 * Includes
 */

////@begin includes
#include "wx/aui/framemanager.h"
#include "wx/frame.h"
#include "wx/aui/auibook.h"
////@end includes

/*!
 * Forward declarations
 */

#if 0
////@begin forward declarations
class wxMenu;
namespace nutcracker { class WorkspaceWnd; }
class wxAuiNotebook;
////@end forward declarations
#else
	FORWARD_DECLARE_VIEWCLASS(WorkspaceWnd);
#endif

/*!
 * Control identifiers
 */

////@begin control identifiers
#define SYMBOL_MAINWND_AUTO_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxMINIMIZE_BOX|wxMAXIMIZE_BOX|wxCLOSE_BOX
#define SYMBOL_MAINWND_AUTO_TITLE _("Nutcracker")
#define SYMBOL_MAINWND_AUTO_IDNAME ID_MAINWND_AUTO
#define SYMBOL_MAINWND_AUTO_SIZE wxSize(1024, 768)
#define SYMBOL_MAINWND_AUTO_POSITION wxDefaultPosition
////@end control identifiers


/*!
 * MainWnd_Auto class declaration
 */

class MainWnd_Auto: public wxFrame
{    
	DECLARE_CLASS( MainWnd_Auto )
	DECLARE_EVENT_TABLE()

public:
	/// Constructors
	MainWnd_Auto();
	MainWnd_Auto( wxWindow* parent, wxWindowID id = SYMBOL_MAINWND_AUTO_IDNAME, const wxString& caption = SYMBOL_MAINWND_AUTO_TITLE, const wxPoint& pos = SYMBOL_MAINWND_AUTO_POSITION, const wxSize& size = SYMBOL_MAINWND_AUTO_SIZE, long style = SYMBOL_MAINWND_AUTO_STYLE );

	bool Create( wxWindow* parent, wxWindowID id = SYMBOL_MAINWND_AUTO_IDNAME, const wxString& caption = SYMBOL_MAINWND_AUTO_TITLE, const wxPoint& pos = SYMBOL_MAINWND_AUTO_POSITION, const wxSize& size = SYMBOL_MAINWND_AUTO_SIZE, long style = SYMBOL_MAINWND_AUTO_STYLE );

	/// Destructor
	~MainWnd_Auto();

	/// Initialises member variables
	void Init();

	/// Creates the controls and sizers
	void CreateControls();

////@begin MainWnd_Auto event handler declarations

	/// wxEVT_CLOSE_WINDOW event handler for ID_MAINWND_AUTO
	virtual void OnCloseWindow( wxCloseEvent& event );

	/// wxEVT_IDLE event handler for ID_MAINWND_AUTO
	virtual void OnIdle( wxIdleEvent& event );

	/// wxEVT_SET_FOCUS event handler for ID_MAINWND_AUTO
	virtual void OnSetFocus( wxFocusEvent& event );

	/// wxEVT_KILL_FOCUS event handler for ID_MAINWND_AUTO
	virtual void OnKillFocus( wxFocusEvent& event );

	/// wxEVT_COMMAND_MENU_SELECTED event handler for ID_MENU_FILE_SAVEWORKSPACE
	virtual void OnMenuFileSaveWorkspace( wxCommandEvent& event );

	/// wxEVT_COMMAND_MENU_SELECTED event handler for ID_MENU_FILE_LOADWORKSPACE
	virtual void OnMenuFileLoadWorkspace( wxCommandEvent& event );

	/// wxEVT_COMMAND_MENU_SELECTED event handler for ID_MENU_FILE_CLOSEWORKSPACE
	virtual void OnMenuFileCloseWorkspace( wxCommandEvent& event );

	/// wxEVT_COMMAND_MENU_SELECTED event handler for wxID_EXIT
	virtual void OnExitClick( wxCommandEvent& event );

	/// wxEVT_COMMAND_MENU_SELECTED event handler for ID_MENU_EDIT_FIND_IN_CURRENTFILE
	virtual void OnMenuEditFind( wxCommandEvent& event );

	/// wxEVT_COMMAND_MENU_SELECTED event handler for ID_MENU_EDIT_FINDFILE
	virtual void OnMenuEditFindFileInWorkspace( wxCommandEvent& event );

	/// wxEVT_COMMAND_MENU_SELECTED event handler for ID_MENU_VIEW_INDENTATION
	virtual void OnMenuClick( wxCommandEvent& event );

	/// wxEVT_COMMAND_MENU_SELECTED event handler for ID_MENU_DEBUG_START_OR_CONTINUE
	virtual void OnMenuDebugStartOrContinue( wxCommandEvent& event );

	/// wxEVT_COMMAND_MENU_SELECTED event handler for ID_MENU_DEBUG_START_NODEBUG
	virtual void OnMenuDebugStartWithoutDebugging( wxCommandEvent& event );

	/// wxEVT_COMMAND_MENU_SELECTED event handler for ID_MENU_DEBUG_STOP_DEBUGGING
	virtual void OnMenuDebugStopDebugging( wxCommandEvent& event );

	/// wxEVT_COMMAND_MENU_SELECTED event handler for ID_MENU_DEBUG_BREAK
	virtual void OnMenuDebugBreak( wxCommandEvent& event );

	/// wxEVT_COMMAND_MENU_SELECTED event handler for ID_MENU_DEBUG_STEPOVER
	virtual void OnMenuDebugStepOver( wxCommandEvent& event );

	/// wxEVT_COMMAND_MENU_SELECTED event handler for ID_MENU_DEBUG_STEPINTO
	virtual void OnMenuDebugStepInto( wxCommandEvent& event );

	/// wxEVT_COMMAND_MENU_SELECTED event handler for ID_MENU_DEBUG_STEPOUT
	virtual void OnMenuDebugStepOut( wxCommandEvent& event );

////@end MainWnd_Auto event handler declarations

////@begin MainWnd_Auto member function declarations

	/// Returns the AUI manager object
	wxAuiManager& GetAuiManager() { return m_auiManager; }

	/// Retrieves bitmap resources
	wxBitmap GetBitmapResource( const wxString& name );

	/// Retrieves icon resources
	wxIcon GetIconResource( const wxString& name );
////@end MainWnd_Auto member function declarations

	/// Should we show tooltips?
	static bool ShowToolTips();

////@begin MainWnd_Auto member variables
	wxAuiManager m_auiManager;
	wxMenu* m_menuEdit;
	wxMenu* m_menuView;
	wxMenu* m_menuDebug;
	wxMenu* m_menuInterpreters;
	nutcracker::WorkspaceWnd* m_workspaceWnd;
	wxAuiNotebook* m_notebook;
	wxTextCtrl* m_logTextCtrl;
	/// Control identifiers
	enum {
		ID_MAINWND_AUTO = 10000,
		ID_MENU_FILE_SAVEWORKSPACE = 10016,
		ID_MENU_FILE_LOADWORKSPACE = 10017,
		ID_MENU_FILE_CLOSEWORKSPACE = 10018,
		ID_MENU_EDIT_FIND_IN_CURRENTFILE = 10027,
		ID_MENU_FIND_INWORKSPACE = 10028,
		ID_MENU_EDIT_FINDFILE = 10026,
		ID_MENU_VIEW_INDENTATION = 10003,
		ID_MENU_VIEW_WHITESPACE = 10004,
		ID_MENU_VIEW_EOL = 10005,
		ID_MENU_DEBUG_START_OR_CONTINUE = 10019,
		ID_MENU_DEBUG_START_NODEBUG = 10020,
		ID_MENU_DEBUG_STOP_DEBUGGING = 10021,
		ID_MENU_DEBUG_BREAK = 10022,
		ID_MENU_DEBUG_STEPOVER = 10023,
		ID_MENU_DEBUG_STEPINTO = 10024,
		ID_MENU_DEBUG_STEPOUT = 10025,
		ID_FOREIGN = 10007,
		ID_WINDOW = 10008,
		ID_AUINOTEBOOK = 10009,
		ID_PANEL = 10010,
		ID_TEXTCTRL = 10011,
		ID_PANEL1 = 10012,
		ID_PANEL2 = 10013,
		ID_PANEL3 = 10014,
		ID_PANEL4 = 10015,
		ID_FindResultsWnd = 10001
	};
////@end MainWnd_Auto member variables
};

#endif
	// _MAINWND_AUTO_H_
