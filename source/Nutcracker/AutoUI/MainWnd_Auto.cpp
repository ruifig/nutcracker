/////////////////////////////////////////////////////////////////////////////
// Name:        MainWnd_Auto.cpp
// Purpose:     
// Author:      Rui Figueira
// Modified by: 
// Created:     13/02/2015 21:16:57
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#include "NutcrackerPCH.h"

////@begin includes
#include "../WorkspaceWnd.h"
#include "../FileEditorGroupWnd.h"
#include "wx/imaglist.h"
#include "../BreakpointsWnd.h"
#include "../CallstackWnd.h"
#include "../LocalsWnd.h"
#include "../WatchesWnd.h"
////@end includes

#include "MainWnd_Auto.h"

////@begin XPM images
////@end XPM images


/*
 * MainWnd_Auto type definition
 */

IMPLEMENT_CLASS( MainWnd_Auto, wxFrame )


/*
 * MainWnd_Auto event table definition
 */

BEGIN_EVENT_TABLE( MainWnd_Auto, wxFrame )

////@begin MainWnd_Auto event table entries
	EVT_CLOSE( MainWnd_Auto::OnCloseWindow )
	EVT_IDLE( MainWnd_Auto::OnIdle )
	EVT_SET_FOCUS( MainWnd_Auto::OnSetFocus )
	EVT_KILL_FOCUS( MainWnd_Auto::OnKillFocus )
	EVT_MENU( ID_MENU_FILE_SAVEWORKSPACE, MainWnd_Auto::OnMenuFileSaveWorkspace )
	EVT_MENU( ID_MENU_FILE_LOADWORKSPACE, MainWnd_Auto::OnMenuFileLoadWorkspace )
	EVT_MENU( ID_MENU_FILE_CLOSEWORKSPACE, MainWnd_Auto::OnMenuFileCloseWorkspace )
	EVT_MENU( wxID_EXIT, MainWnd_Auto::OnExitClick )
	EVT_MENU( ID_MENU_VIEW_INDENTATION, MainWnd_Auto::OnMenuClick )
	EVT_MENU( ID_MENU_VIEW_WHITESPACE, MainWnd_Auto::OnMenuClick )
	EVT_MENU( ID_MENU_VIEW_EOL, MainWnd_Auto::OnMenuClick )
////@end MainWnd_Auto event table entries

END_EVENT_TABLE()


/*
 * MainWnd_Auto constructors
 */

MainWnd_Auto::MainWnd_Auto()
{
	Init();
}

MainWnd_Auto::MainWnd_Auto( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
	Init();
	Create( parent, id, caption, pos, size, style );
}


/*
 * MainWnd_Auto creator
 */

bool MainWnd_Auto::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin MainWnd_Auto creation
	wxFrame::Create( parent, id, caption, pos, size, style );

	CreateControls();
	Centre();
////@end MainWnd_Auto creation
	return true;
}


/*
 * MainWnd_Auto destructor
 */

MainWnd_Auto::~MainWnd_Auto()
{
////@begin MainWnd_Auto destruction
	GetAuiManager().UnInit();
////@end MainWnd_Auto destruction
}


/*
 * Member initialisation
 */

void MainWnd_Auto::Init()
{
////@begin MainWnd_Auto member initialisation
	m_menuView = NULL;
	m_menuInterpreters = NULL;
	m_workspaceWnd = NULL;
	m_notebook = NULL;
	m_logTextCtrl = NULL;
////@end MainWnd_Auto member initialisation
}


/*
 * Control creation for MainWnd_Auto
 */

void MainWnd_Auto::CreateControls()
{    
////@begin MainWnd_Auto content construction
	MainWnd_Auto* itemFrame1 = this;

	GetAuiManager().SetManagedWindow(this);

	wxMenuBar* menuBar = new wxMenuBar;
	wxMenu* itemMenu3 = new wxMenu;
	itemMenu3->Append(ID_MENU_FILE_SAVEWORKSPACE, _("Save Workspace"), wxEmptyString, wxITEM_NORMAL);
	itemMenu3->Append(ID_MENU_FILE_LOADWORKSPACE, _("Load Workspace"), wxEmptyString, wxITEM_NORMAL);
	itemMenu3->Append(ID_MENU_FILE_CLOSEWORKSPACE, _("Close Workspace"), wxEmptyString, wxITEM_NORMAL);
	itemMenu3->AppendSeparator();
	itemMenu3->Append(wxID_EXIT, _("E&xit"), wxEmptyString, wxITEM_NORMAL);
	menuBar->Append(itemMenu3, _("&File"));
	m_menuView = new wxMenu;
	m_menuView->Append(ID_MENU_VIEW_INDENTATION, _("&Indentation guides"), wxEmptyString, wxITEM_CHECK);
	m_menuView->Append(ID_MENU_VIEW_WHITESPACE, _("&Whitespaces"), wxEmptyString, wxITEM_CHECK);
	m_menuView->Append(ID_MENU_VIEW_EOL, _("&End of line"), wxEmptyString, wxITEM_CHECK);
	menuBar->Append(m_menuView, _("&View"));
	m_menuInterpreters = new wxMenu;
	menuBar->Append(m_menuInterpreters, _("&Interpreter"));
	itemFrame1->SetMenuBar(menuBar);

	wxAuiToolBar* itemAuiToolBar14 = new wxAuiToolBar( itemFrame1, ID_TOOLBAR, wxDefaultPosition, wxDefaultSize, wxAUI_TB_GRIPPER );
	itemAuiToolBar14->Realize();
	itemFrame1->GetAuiManager().AddPane(itemAuiToolBar14, wxAuiPaneInfo()
		.ToolbarPane().Name(wxT("Pane1")).Top().Layer(10).CaptionVisible(false).CloseButton(false).DestroyOnClose(false).Resizable(false).Floatable(false).Gripper(true));

	m_workspaceWnd = new nutcracker::WorkspaceWnd( itemFrame1, ID_FOREIGN, wxDefaultPosition, wxSize(100, 100), wxSIMPLE_BORDER );
	itemFrame1->GetAuiManager().AddPane(m_workspaceWnd, wxAuiPaneInfo()
		.Name(wxT("Pane2")).Caption(_("Workspace")).MinSize(wxSize(80, 80)).BestSize(wxSize(180, -1)).CloseButton(false).DestroyOnClose(false).Resizable(true));

	nutcracker::FileEditorGroupWnd* itemWindow16 = new nutcracker::FileEditorGroupWnd( itemFrame1, ID_WINDOW, wxDefaultPosition, wxSize(200, 100), wxSIMPLE_BORDER );
	itemFrame1->GetAuiManager().AddPane(itemWindow16, wxAuiPaneInfo()
		.Name(wxT("Pane4")).Centre().MinSize(wxSize(100, -1)).CaptionVisible(false).CloseButton(false).DestroyOnClose(false).Resizable(true));

	m_notebook = new wxAuiNotebook( itemFrame1, ID_AUINOTEBOOK, wxDefaultPosition, wxSize(-1, 200), wxAUI_NB_BOTTOM|wxAUI_NB_TAB_SPLIT|wxAUI_NB_TAB_MOVE|wxAUI_NB_TAB_EXTERNAL_MOVE|wxNO_BORDER );

	wxPanel* itemPanel18 = new wxPanel( m_notebook, ID_PANEL, wxDefaultPosition, wxDefaultSize, wxNO_BORDER|wxTAB_TRAVERSAL );
	wxBoxSizer* itemBoxSizer19 = new wxBoxSizer(wxVERTICAL);
	itemPanel18->SetSizer(itemBoxSizer19);

	m_logTextCtrl = new wxTextCtrl( itemPanel18, ID_TEXTCTRL, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY|wxTE_RICH|wxTE_AUTO_URL|wxNO_BORDER );
	itemBoxSizer19->Add(m_logTextCtrl, 1, wxGROW|wxALL, 5);

	m_notebook->AddPage(itemPanel18, _("Log"), false);

	nutcracker::BreakpointsWnd* itemPanel21 = new nutcracker::BreakpointsWnd( m_notebook, ID_PANEL1, wxDefaultPosition, wxDefaultSize, wxNO_BORDER|wxTAB_TRAVERSAL );

	m_notebook->AddPage(itemPanel21, _("Breakpoints"), false);

	nutcracker::CallstackWnd* itemPanel22 = new nutcracker::CallstackWnd( m_notebook, ID_PANEL2, wxDefaultPosition, wxDefaultSize, wxNO_BORDER|wxTAB_TRAVERSAL );

	m_notebook->AddPage(itemPanel22, _("Callstack"), false);

	nutcracker::LocalsWnd* itemPanel23 = new nutcracker::LocalsWnd( m_notebook, ID_PANEL3, wxDefaultPosition, wxDefaultSize, wxNO_BORDER|wxTAB_TRAVERSAL );

	m_notebook->AddPage(itemPanel23, _("Locals"), false);

	nutcracker::WatchesWnd* itemPanel24 = new nutcracker::WatchesWnd( m_notebook, ID_PANEL4, wxDefaultPosition, wxDefaultSize, wxNO_BORDER|wxTAB_TRAVERSAL );

	m_notebook->AddPage(itemPanel24, _("Watches"), false);

	itemFrame1->GetAuiManager().AddPane(m_notebook, wxAuiPaneInfo()
		.Name(wxT("Pane3")).Bottom().MinSize(wxSize(80, 80)).CaptionVisible(false).CloseButton(false).DestroyOnClose(false).Resizable(true).PaneBorder(false));

	GetAuiManager().Update();

////@end MainWnd_Auto content construction
}


/*
 * Should we show tooltips?
 */

bool MainWnd_Auto::ShowToolTips()
{
	return true;
}

/*
 * Get bitmap resources
 */

wxBitmap MainWnd_Auto::GetBitmapResource( const wxString& name )
{
	// Bitmap retrieval
////@begin MainWnd_Auto bitmap retrieval
	wxUnusedVar(name);
	return wxNullBitmap;
////@end MainWnd_Auto bitmap retrieval
}

/*
 * Get icon resources
 */

wxIcon MainWnd_Auto::GetIconResource( const wxString& name )
{
	// Icon retrieval
////@begin MainWnd_Auto icon retrieval
	wxUnusedVar(name);
	return wxNullIcon;
////@end MainWnd_Auto icon retrieval
}


/*
 * wxEVT_CLOSE_WINDOW event handler for ID_MAINWND_AUTO
 */

void MainWnd_Auto::OnCloseWindow( wxCloseEvent& event )
{
////@begin wxEVT_CLOSE_WINDOW event handler for ID_MAINWND_AUTO in MainWnd_Auto.
	// Before editing this code, remove the block markers.
	event.Skip();
////@end wxEVT_CLOSE_WINDOW event handler for ID_MAINWND_AUTO in MainWnd_Auto. 
}


/*
 * wxEVT_IDLE event handler for ID_MAINWND_AUTO
 */

void MainWnd_Auto::OnIdle( wxIdleEvent& event )
{
////@begin wxEVT_IDLE event handler for ID_MAINWND_AUTO in MainWnd_Auto.
	// Before editing this code, remove the block markers.
	event.Skip();
////@end wxEVT_IDLE event handler for ID_MAINWND_AUTO in MainWnd_Auto. 
}


/*
 * wxEVT_COMMAND_MENU_SELECTED event handler for wxID_EXIT
 */

void MainWnd_Auto::OnExitClick( wxCommandEvent& event )
{
////@begin wxEVT_COMMAND_MENU_SELECTED event handler for wxID_EXIT in MainWnd_Auto.
	// Before editing this code, remove the block markers.
////@end wxEVT_COMMAND_MENU_SELECTED event handler for wxID_EXIT in MainWnd_Auto. 
}


/*
 * wxEVT_COMMAND_MENU_SELECTED event handler for ID_MENU_VIEW_INDENTATION
 */

void MainWnd_Auto::OnMenuClick( wxCommandEvent& event )
{
////@begin wxEVT_COMMAND_MENU_SELECTED event handler for ID_MENU_VIEW_INDENTATION in MainWnd_Auto.
	// Before editing this code, remove the block markers.
	event.Skip();
////@end wxEVT_COMMAND_MENU_SELECTED event handler for ID_MENU_VIEW_INDENTATION in MainWnd_Auto. 
}


/*
 * wxEVT_SET_FOCUS event handler for ID_MAINWND_AUTO
 */

void MainWnd_Auto::OnSetFocus( wxFocusEvent& event )
{
////@begin wxEVT_SET_FOCUS event handler for ID_MAINWND_AUTO in MainWnd_Auto.
	// Before editing this code, remove the block markers.
	event.Skip();
////@end wxEVT_SET_FOCUS event handler for ID_MAINWND_AUTO in MainWnd_Auto. 
}


/*
 * wxEVT_KILL_FOCUS event handler for ID_MAINWND_AUTO
 */

void MainWnd_Auto::OnKillFocus( wxFocusEvent& event )
{
////@begin wxEVT_KILL_FOCUS event handler for ID_MAINWND_AUTO in MainWnd_Auto.
	// Before editing this code, remove the block markers.
	event.Skip();
////@end wxEVT_KILL_FOCUS event handler for ID_MAINWND_AUTO in MainWnd_Auto. 
}


/*
 * wxEVT_COMMAND_MENU_SELECTED event handler for ID_MENU_FILE_SAVEWORKSPACE
 */

void MainWnd_Auto::OnMenuFileSaveWorkspace( wxCommandEvent& event )
{
////@begin wxEVT_COMMAND_MENU_SELECTED event handler for ID_MENU_FILE_SAVEWORKSPACE in MainWnd_Auto.
	// Before editing this code, remove the block markers.
	event.Skip();
////@end wxEVT_COMMAND_MENU_SELECTED event handler for ID_MENU_FILE_SAVEWORKSPACE in MainWnd_Auto. 
}


/*
 * wxEVT_COMMAND_MENU_SELECTED event handler for ID_MENU_FILE_LOADWORKSPACE
 */

void MainWnd_Auto::OnMenuFileLoadWorkspace( wxCommandEvent& event )
{
////@begin wxEVT_COMMAND_MENU_SELECTED event handler for ID_MENU_FILE_LOADWORKSPACE in MainWnd_Auto.
	// Before editing this code, remove the block markers.
	event.Skip();
////@end wxEVT_COMMAND_MENU_SELECTED event handler for ID_MENU_FILE_LOADWORKSPACE in MainWnd_Auto. 
}


/*
 * wxEVT_COMMAND_MENU_SELECTED event handler for ID_MENU_FILE_CLOSEWORKSPACE
 */

void MainWnd_Auto::OnMenuFileCloseWorkspace( wxCommandEvent& event )
{
////@begin wxEVT_COMMAND_MENU_SELECTED event handler for ID_MENU_FILE_CLOSEWORKSPACE in MainWnd_Auto.
	// Before editing this code, remove the block markers.
	event.Skip();
////@end wxEVT_COMMAND_MENU_SELECTED event handler for ID_MENU_FILE_CLOSEWORKSPACE in MainWnd_Auto. 
}

