/////////////////////////////////////////////////////////////////////////////
// Name:        CallstackWnd_Auto.cpp
// Purpose:     
// Author:      Rui Figueira
// Modified by: 
// Created:     23/02/2015 00:43:41
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#include "NutcrackerPCH.h"

////@begin includes
////@end includes

#include "CallstackWnd_Auto.h"

////@begin XPM images
////@end XPM images


/*
 * CallstackWnd_Auto type definition
 */

IMPLEMENT_DYNAMIC_CLASS( CallstackWnd_Auto, wxPanel )


/*
 * CallstackWnd_Auto event table definition
 */

BEGIN_EVENT_TABLE( CallstackWnd_Auto, wxPanel )

////@begin CallstackWnd_Auto event table entries
	EVT_SIZE( CallstackWnd_Auto::OnSize )
	EVT_GRID_CELL_LEFT_CLICK( CallstackWnd_Auto::OnCellClick )
	EVT_GRID_CELL_LEFT_DCLICK( CallstackWnd_Auto::OnCellDClick )
	EVT_GRID_CELL_CHANGED( CallstackWnd_Auto::OnCellChanged )
////@end CallstackWnd_Auto event table entries

END_EVENT_TABLE()


/*
 * CallstackWnd_Auto constructors
 */

CallstackWnd_Auto::CallstackWnd_Auto()
{
	Init();
}

CallstackWnd_Auto::CallstackWnd_Auto( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
{
	Init();
	Create(parent, id, pos, size, style);
}


/*
 * CallstackWnd_Auto creator
 */

bool CallstackWnd_Auto::Create( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
{
////@begin CallstackWnd_Auto creation
	wxPanel::Create( parent, id, pos, size, style );

	CreateControls();
	if (GetSizer())
	{
		GetSizer()->SetSizeHints(this);
	}
	Centre();
////@end CallstackWnd_Auto creation
	return true;
}


/*
 * CallstackWnd_Auto destructor
 */

CallstackWnd_Auto::~CallstackWnd_Auto()
{
////@begin CallstackWnd_Auto destruction
////@end CallstackWnd_Auto destruction
}


/*
 * Member initialisation
 */

void CallstackWnd_Auto::Init()
{
////@begin CallstackWnd_Auto member initialisation
	m_grid = NULL;
////@end CallstackWnd_Auto member initialisation
}


/*
 * Control creation for CallstackWnd_Auto
 */

void CallstackWnd_Auto::CreateControls()
{    
////@begin CallstackWnd_Auto content construction
	CallstackWnd_Auto* itemPanel1 = this;

	wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
	itemPanel1->SetSizer(itemBoxSizer2);

	m_grid = new wxGrid( itemPanel1, ID_GRID, wxDefaultPosition, wxSize(200, 150), wxHSCROLL|wxVSCROLL );
	m_grid->SetDefaultColSize(50);
	m_grid->SetDefaultRowSize(20);
	m_grid->SetColLabelSize(0);
	m_grid->SetRowLabelSize(0);
	m_grid->CreateGrid(1, 2, wxGrid::wxGridSelectRows);
	itemBoxSizer2->Add(m_grid, 1, wxGROW|wxALL|wxADJUST_MINSIZE, 0);

////@end CallstackWnd_Auto content construction
}


/*
 * Should we show tooltips?
 */

bool CallstackWnd_Auto::ShowToolTips()
{
	return true;
}

/*
 * Get bitmap resources
 */

wxBitmap CallstackWnd_Auto::GetBitmapResource( const wxString& name )
{
	// Bitmap retrieval
////@begin CallstackWnd_Auto bitmap retrieval
	wxUnusedVar(name);
	return wxNullBitmap;
////@end CallstackWnd_Auto bitmap retrieval
}

/*
 * Get icon resources
 */

wxIcon CallstackWnd_Auto::GetIconResource( const wxString& name )
{
	// Icon retrieval
////@begin CallstackWnd_Auto icon retrieval
	wxUnusedVar(name);
	return wxNullIcon;
////@end CallstackWnd_Auto icon retrieval
}


/*
 * wxEVT_GRID_CELL_LEFT_CLICK event handler for ID_GRID
 */

void CallstackWnd_Auto::OnCellClick( wxGridEvent& event )
{
////@begin wxEVT_GRID_CELL_LEFT_CLICK event handler for ID_GRID in CallstackWnd_Auto.
	// Before editing this code, remove the block markers.
	event.Skip();
////@end wxEVT_GRID_CELL_LEFT_CLICK event handler for ID_GRID in CallstackWnd_Auto. 
}


/*
 * wxEVT_GRID_CELL_LEFT_DCLICK event handler for ID_GRID
 */

void CallstackWnd_Auto::OnCellDClick( wxGridEvent& event )
{
////@begin wxEVT_GRID_CELL_LEFT_DCLICK event handler for ID_GRID in CallstackWnd_Auto.
	// Before editing this code, remove the block markers.
	event.Skip();
////@end wxEVT_GRID_CELL_LEFT_DCLICK event handler for ID_GRID in CallstackWnd_Auto. 
}


/*
 * wxEVT_GRID_CELL_CHANGED event handler for ID_GRID
 */

void CallstackWnd_Auto::OnCellChanged( wxGridEvent& event )
{
////@begin wxEVT_GRID_CELL_CHANGED event handler for ID_GRID in CallstackWnd_Auto.
	// Before editing this code, remove the block markers.
	event.Skip();
////@end wxEVT_GRID_CELL_CHANGED event handler for ID_GRID in CallstackWnd_Auto. 
}


/*
 * wxEVT_SIZE event handler for ID_CALLSTACKWND_AUTO
 */

void CallstackWnd_Auto::OnSize( wxSizeEvent& event )
{
////@begin wxEVT_SIZE event handler for ID_CALLSTACKWND_AUTO in CallstackWnd_Auto.
	// Before editing this code, remove the block markers.
	event.Skip();
////@end wxEVT_SIZE event handler for ID_CALLSTACKWND_AUTO in CallstackWnd_Auto. 
}

