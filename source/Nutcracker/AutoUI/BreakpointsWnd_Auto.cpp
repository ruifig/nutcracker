/////////////////////////////////////////////////////////////////////////////
// Name:        BreakpointsWnd_Auto.cpp
// Purpose:     
// Author:      Rui Figueira
// Modified by: 
// Created:     21/02/2015 20:24:17
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#include "NutcrackerPCH.h"

////@begin includes
////@end includes

#include "BreakpointsWnd_Auto.h"

////@begin XPM images
////@end XPM images


/*
 * BreakpointsWnd_Auto type definition
 */

IMPLEMENT_DYNAMIC_CLASS( BreakpointsWnd_Auto, wxPanel )


/*
 * BreakpointsWnd_Auto event table definition
 */

BEGIN_EVENT_TABLE( BreakpointsWnd_Auto, wxPanel )

////@begin BreakpointsWnd_Auto event table entries
	EVT_BUTTON( ID_BUTTON, BreakpointsWnd_Auto::OnToggleBreakpointsClick )
	EVT_GRID_CELL_LEFT_CLICK( BreakpointsWnd_Auto::OnCellClick )
	EVT_GRID_CELL_LEFT_DCLICK( BreakpointsWnd_Auto::OnCellDClick )
	EVT_GRID_CELL_CHANGED( BreakpointsWnd_Auto::OnCellChanged )
////@end BreakpointsWnd_Auto event table entries

END_EVENT_TABLE()


/*
 * BreakpointsWnd_Auto constructors
 */

BreakpointsWnd_Auto::BreakpointsWnd_Auto()
{
	Init();
}

BreakpointsWnd_Auto::BreakpointsWnd_Auto( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
{
	Init();
	Create(parent, id, pos, size, style);
}


/*
 * BreakpointsWnd_Auto creator
 */

bool BreakpointsWnd_Auto::Create( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
{
////@begin BreakpointsWnd_Auto creation
	wxPanel::Create( parent, id, pos, size, style );

	CreateControls();
	Centre();
////@end BreakpointsWnd_Auto creation
	return true;
}


/*
 * BreakpointsWnd_Auto destructor
 */

BreakpointsWnd_Auto::~BreakpointsWnd_Auto()
{
////@begin BreakpointsWnd_Auto destruction
////@end BreakpointsWnd_Auto destruction
}


/*
 * Member initialisation
 */

void BreakpointsWnd_Auto::Init()
{
////@begin BreakpointsWnd_Auto member initialisation
	m_grid = NULL;
////@end BreakpointsWnd_Auto member initialisation
}


/*
 * Control creation for BreakpointsWnd_Auto
 */

void BreakpointsWnd_Auto::CreateControls()
{    
////@begin BreakpointsWnd_Auto content construction
	BreakpointsWnd_Auto* itemPanel1 = this;

	wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
	itemPanel1->SetSizer(itemBoxSizer2);

	wxBoxSizer* itemBoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
	itemBoxSizer2->Add(itemBoxSizer3, 0, wxALIGN_LEFT|wxALL, 0);

	wxButton* itemButton4 = new wxButton( itemPanel1, ID_BUTTON, _("Button"), wxDefaultPosition, wxDefaultSize, 0 );
	itemBoxSizer3->Add(itemButton4, 0, wxALIGN_CENTER_VERTICAL|wxALL, 0);

	m_grid = new wxGrid( itemPanel1, ID_GRID, wxDefaultPosition, wxSize(200, 150), wxHSCROLL|wxVSCROLL );
	m_grid->SetDefaultColSize(50);
	m_grid->SetDefaultRowSize(20);
	m_grid->SetColLabelSize(20);
	m_grid->SetRowLabelSize(0);
	m_grid->CreateGrid(1, 2, wxGrid::wxGridSelectRows);
	itemBoxSizer2->Add(m_grid, 1, wxGROW|wxALL|wxADJUST_MINSIZE, 0);

////@end BreakpointsWnd_Auto content construction
}


/*
 * Should we show tooltips?
 */

bool BreakpointsWnd_Auto::ShowToolTips()
{
	return true;
}

/*
 * Get bitmap resources
 */

wxBitmap BreakpointsWnd_Auto::GetBitmapResource( const wxString& name )
{
	// Bitmap retrieval
////@begin BreakpointsWnd_Auto bitmap retrieval
	wxUnusedVar(name);
	return wxNullBitmap;
////@end BreakpointsWnd_Auto bitmap retrieval
}

/*
 * Get icon resources
 */

wxIcon BreakpointsWnd_Auto::GetIconResource( const wxString& name )
{
	// Icon retrieval
////@begin BreakpointsWnd_Auto icon retrieval
	wxUnusedVar(name);
	return wxNullIcon;
////@end BreakpointsWnd_Auto icon retrieval
}


/*
 * wxEVT_GRID_CELL_LEFT_CLICK event handler for ID_GRID
 */

void BreakpointsWnd_Auto::OnCellClick( wxGridEvent& event )
{
////@begin wxEVT_GRID_CELL_LEFT_CLICK event handler for ID_GRID in BreakpointsWnd_Auto.
	// Before editing this code, remove the block markers.
	event.Skip();
////@end wxEVT_GRID_CELL_LEFT_CLICK event handler for ID_GRID in BreakpointsWnd_Auto. 
}


/*
 * wxEVT_GRID_CELL_LEFT_DCLICK event handler for ID_GRID
 */

void BreakpointsWnd_Auto::OnCellDClick( wxGridEvent& event )
{
////@begin wxEVT_GRID_CELL_LEFT_DCLICK event handler for ID_GRID in BreakpointsWnd_Auto.
	// Before editing this code, remove the block markers.
	event.Skip();
////@end wxEVT_GRID_CELL_LEFT_DCLICK event handler for ID_GRID in BreakpointsWnd_Auto. 
}


/*
 * wxEVT_GRID_CELL_CHANGED event handler for ID_GRID
 */

void BreakpointsWnd_Auto::OnCellChanged( wxGridEvent& event )
{
////@begin wxEVT_GRID_CELL_CHANGED event handler for ID_GRID in BreakpointsWnd_Auto.
	// Before editing this code, remove the block markers.
	event.Skip();
////@end wxEVT_GRID_CELL_CHANGED event handler for ID_GRID in BreakpointsWnd_Auto. 
}


/*
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON
 */

void BreakpointsWnd_Auto::OnToggleBreakpointsClick( wxCommandEvent& event )
{
////@begin wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON in BreakpointsWnd_Auto.
	// Before editing this code, remove the block markers.
	event.Skip();
////@end wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON in BreakpointsWnd_Auto. 
}

