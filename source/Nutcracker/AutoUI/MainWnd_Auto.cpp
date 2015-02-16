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
#include "../View/WorkspaceWnd.h"
#include "../View/FileEditorGroupWnd.h"
#include "wx/imaglist.h"
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
	m_menuInterpreters = NULL;
	m_workspaceWnd = NULL;
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
	itemMenu3->Append(ID_MENUITEM, _("New workspace"), wxEmptyString, wxITEM_NORMAL);
	itemMenu3->Append(ID_MENUITEM1, _("Open workspace"), wxEmptyString, wxITEM_NORMAL);
	menuBar->Append(itemMenu3, _("File"));
	wxMenu* itemMenu6 = new wxMenu;
	itemMenu6->Append(ID_MENU_VIEW_INDENTATION, _("Indentation guides"), wxEmptyString, wxITEM_CHECK);
	itemMenu6->Append(ID_MENU_VIEW_WHITESPACE, _("Whitespaces"), wxEmptyString, wxITEM_CHECK);
	itemMenu6->Append(ID_MENU_VIEW_EOL, _("End of line"), wxEmptyString, wxITEM_CHECK);
	menuBar->Append(itemMenu6, _("View"));
	m_menuInterpreters = new wxMenu;
	menuBar->Append(m_menuInterpreters, _("Interpreter"));
	itemFrame1->SetMenuBar(menuBar);

	wxAuiToolBar* itemAuiToolBar11 = new wxAuiToolBar( itemFrame1, ID_TOOLBAR, wxDefaultPosition, wxDefaultSize, wxAUI_TB_GRIPPER );
	itemAuiToolBar11->Realize();
	itemFrame1->GetAuiManager().AddPane(itemAuiToolBar11, wxAuiPaneInfo()
		.ToolbarPane().Name(wxT("Pane1")).Top().Layer(10).CaptionVisible(false).CloseButton(false).DestroyOnClose(false).Resizable(false).Floatable(false).Gripper(true));

	m_workspaceWnd = new cz::view::WorkspaceWnd( itemFrame1, ID_FOREIGN, wxDefaultPosition, wxSize(100, 100), wxSIMPLE_BORDER );
	itemFrame1->GetAuiManager().AddPane(m_workspaceWnd, wxAuiPaneInfo()
		.Name(wxT("Pane2")).CaptionVisible(false).CloseButton(false).DestroyOnClose(false).Resizable(true).Floatable(false));

	cz::view::FileEditorGroupWnd* itemWindow13 = new cz::view::FileEditorGroupWnd( itemFrame1, ID_WINDOW, wxDefaultPosition, wxSize(100, 100), wxSIMPLE_BORDER );
	itemFrame1->GetAuiManager().AddPane(itemWindow13, wxAuiPaneInfo()
		.Name(wxT("Pane4")).Centre().CaptionVisible(false).CloseButton(false).DestroyOnClose(false).Resizable(true).Floatable(false));

	wxAuiNotebook* itemAuiNotebook14 = new wxAuiNotebook( itemFrame1, ID_AUINOTEBOOK, wxDefaultPosition, wxSize(-1, 200), wxAUI_NB_DEFAULT_STYLE|wxAUI_NB_TOP );

	wxPanel* itemPanel15 = new wxPanel( itemAuiNotebook14, ID_PANEL, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
	wxBoxSizer* itemBoxSizer16 = new wxBoxSizer(wxVERTICAL);
	itemPanel15->SetSizer(itemBoxSizer16);

	m_logTextCtrl = new wxTextCtrl( itemPanel15, ID_TEXTCTRL, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY|wxTE_RICH|wxTE_AUTO_URL|wxNO_BORDER );
	itemBoxSizer16->Add(m_logTextCtrl, 1, wxGROW|wxALL, 5);

	itemAuiNotebook14->AddPage(itemPanel15, _("Log"), false);

	itemFrame1->GetAuiManager().AddPane(itemAuiNotebook14, wxAuiPaneInfo()
		.Name(wxT("Pane3")).Bottom().CaptionVisible(false).CloseButton(false).DestroyOnClose(false).Resizable(true).Floatable(false));

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
 * wxEVT_COMMAND_MENU_SELECTED event handler for ID_MENU_VIEW_IDENTATION
 */

void MainWnd_Auto::OnMenuClick( wxCommandEvent& event )
{
////@begin wxEVT_COMMAND_MENU_SELECTED event handler for ID_MENU_VIEW_IDENTATION in MainWnd_Auto.
	// Before editing this code, remove the block markers.
	event.Skip();
////@end wxEVT_COMMAND_MENU_SELECTED event handler for ID_MENU_VIEW_IDENTATION in MainWnd_Auto. 
}

