/////////////////////////////////////////////////////////////////////////////
// Name:        WorkspaceWnd_Auto.cpp
// Purpose:     
// Author:      Rui Figueira
// Modified by: 
// Created:     16/02/2015 19:25:39
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#include "NutcrackerPCH.h"

////@begin includes
#include "wx/imaglist.h"
////@end includes

#include "WorkspaceWnd_Auto.h"

////@begin XPM images
////@end XPM images


/*
 * WorkspaceWnd_Auto type definition
 */

IMPLEMENT_DYNAMIC_CLASS( WorkspaceWnd_Auto, wxPanel )


/*
 * WorkspaceWnd_Auto event table definition
 */

BEGIN_EVENT_TABLE( WorkspaceWnd_Auto, wxPanel )

////@begin WorkspaceWnd_Auto event table entries
	EVT_BUTTON( ID_BITMAPBUTTON, WorkspaceWnd_Auto::OnRefreshClick )
	EVT_TREE_ITEM_ACTIVATED( ID_TREECTRL, WorkspaceWnd_Auto::OnTreeItemActivated )
	EVT_TREE_ITEM_MENU( ID_TREECTRL, WorkspaceWnd_Auto::OnTreeItemMenu )
////@end WorkspaceWnd_Auto event table entries

END_EVENT_TABLE()


/*
 * WorkspaceWnd_Auto constructors
 */

WorkspaceWnd_Auto::WorkspaceWnd_Auto()
{
	Init();
}

WorkspaceWnd_Auto::WorkspaceWnd_Auto( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
{
	Init();
	Create(parent, id, pos, size, style);
}


/*
 * WorkspaceWnd_Auto creator
 */

bool WorkspaceWnd_Auto::Create( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
{
////@begin WorkspaceWnd_Auto creation
	wxPanel::Create( parent, id, pos, size, style );

	CreateControls();
	if (GetSizer())
	{
		GetSizer()->SetSizeHints(this);
	}
	Centre();
////@end WorkspaceWnd_Auto creation
	return true;
}


/*
 * WorkspaceWnd_Auto destructor
 */

WorkspaceWnd_Auto::~WorkspaceWnd_Auto()
{
////@begin WorkspaceWnd_Auto destruction
////@end WorkspaceWnd_Auto destruction
}


/*
 * Member initialisation
 */

void WorkspaceWnd_Auto::Init()
{
////@begin WorkspaceWnd_Auto member initialisation
	m_refreshBtn = NULL;
	m_treeCtrl = NULL;
////@end WorkspaceWnd_Auto member initialisation
}


/*
 * Control creation for WorkspaceWnd_Auto
 */

void WorkspaceWnd_Auto::CreateControls()
{    
////@begin WorkspaceWnd_Auto content construction
	WorkspaceWnd_Auto* itemPanel1 = this;

	wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
	itemPanel1->SetSizer(itemBoxSizer2);

	wxBoxSizer* itemBoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
	itemBoxSizer2->Add(itemBoxSizer3, 0, wxGROW|wxALL, 1);

	m_refreshBtn = new wxBitmapButton( itemPanel1, ID_BITMAPBUTTON, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	itemBoxSizer3->Add(m_refreshBtn, 1, wxGROW|wxALL, 0);

	m_treeCtrl = new wxTreeCtrl( itemPanel1, ID_TREECTRL, wxDefaultPosition, wxSize(80, 200), wxTR_HAS_BUTTONS |wxTR_LINES_AT_ROOT|wxTR_HIDE_ROOT|wxTR_SINGLE|wxNO_BORDER );
	itemBoxSizer2->Add(m_treeCtrl, 1, wxGROW|wxALL, 0);

////@end WorkspaceWnd_Auto content construction
}


/*
 * Should we show tooltips?
 */

bool WorkspaceWnd_Auto::ShowToolTips()
{
	return true;
}

/*
 * Get bitmap resources
 */

wxBitmap WorkspaceWnd_Auto::GetBitmapResource( const wxString& name )
{
	// Bitmap retrieval
////@begin WorkspaceWnd_Auto bitmap retrieval
	wxUnusedVar(name);
	return wxNullBitmap;
////@end WorkspaceWnd_Auto bitmap retrieval
}

/*
 * Get icon resources
 */

wxIcon WorkspaceWnd_Auto::GetIconResource( const wxString& name )
{
	// Icon retrieval
////@begin WorkspaceWnd_Auto icon retrieval
	wxUnusedVar(name);
	return wxNullIcon;
////@end WorkspaceWnd_Auto icon retrieval
}


/*
 * wxEVT_COMMAND_TREE_ITEM_ACTIVATED event handler for ID_TREECTRL
 */

void WorkspaceWnd_Auto::OnTreeItemActivated( wxTreeEvent& event )
{
////@begin wxEVT_COMMAND_TREE_ITEM_ACTIVATED event handler for ID_TREECTRL in WorkspaceWnd_Auto.
	// Before editing this code, remove the block markers.
	event.Skip();
////@end wxEVT_COMMAND_TREE_ITEM_ACTIVATED event handler for ID_TREECTRL in WorkspaceWnd_Auto. 
}


/*
 * wxEVT_COMMAND_TREE_ITEM_MENU event handler for ID_TREECTRL
 */

void WorkspaceWnd_Auto::OnTreeItemMenu( wxTreeEvent& event )
{
////@begin wxEVT_COMMAND_TREE_ITEM_MENU event handler for ID_TREECTRL in WorkspaceWnd_Auto.
	// Before editing this code, remove the block markers.
	event.Skip();
////@end wxEVT_COMMAND_TREE_ITEM_MENU event handler for ID_TREECTRL in WorkspaceWnd_Auto. 
}


/*
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BITMAPBUTTON
 */

void WorkspaceWnd_Auto::OnRefreshClick( wxCommandEvent& event )
{
////@begin wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BITMAPBUTTON in WorkspaceWnd_Auto.
	// Before editing this code, remove the block markers.
	event.Skip();
////@end wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BITMAPBUTTON in WorkspaceWnd_Auto. 
}

