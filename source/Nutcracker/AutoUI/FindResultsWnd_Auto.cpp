/////////////////////////////////////////////////////////////////////////////
// Name:        FindResultsWnd_Auto.cpp
// Purpose:     
// Author:      Rui Figueira
// Modified by: 
// Created:     22/03/2015 16:35:35
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#include "NutcrackerPCH.h"

////@begin includes
////@end includes

#include "FindResultsWnd_Auto.h"

////@begin XPM images
////@end XPM images


/*
 * FindResultsWnd_Auto type definition
 */

IMPLEMENT_DYNAMIC_CLASS( FindResultsWnd_Auto, wxPanel )


/*
 * FindResultsWnd_Auto event table definition
 */

BEGIN_EVENT_TABLE( FindResultsWnd_Auto, wxPanel )

////@begin FindResultsWnd_Auto event table entries
	EVT_GRID_CELL_LEFT_DCLICK( FindResultsWnd_Auto::OnGridCellDClick )
////@end FindResultsWnd_Auto event table entries

END_EVENT_TABLE()


/*
 * FindResultsWnd_Auto constructors
 */

FindResultsWnd_Auto::FindResultsWnd_Auto()
{
	Init();
}

FindResultsWnd_Auto::FindResultsWnd_Auto( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
{
	Init();
	Create(parent, id, pos, size, style);
}


/*
 * FindResultsWnd_Auto creator
 */

bool FindResultsWnd_Auto::Create( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
{
////@begin FindResultsWnd_Auto creation
	wxPanel::Create( parent, id, pos, size, style );

	CreateControls();
	if (GetSizer())
	{
		GetSizer()->SetSizeHints(this);
	}
	Centre();
////@end FindResultsWnd_Auto creation
	return true;
}


/*
 * FindResultsWnd_Auto destructor
 */

FindResultsWnd_Auto::~FindResultsWnd_Auto()
{
////@begin FindResultsWnd_Auto destruction
////@end FindResultsWnd_Auto destruction
}


/*
 * Member initialisation
 */

void FindResultsWnd_Auto::Init()
{
////@begin FindResultsWnd_Auto member initialisation
	m_grid = NULL;
////@end FindResultsWnd_Auto member initialisation
}


/*
 * Control creation for FindResultsWnd_Auto
 */

void FindResultsWnd_Auto::CreateControls()
{    
////@begin FindResultsWnd_Auto content construction
	FindResultsWnd_Auto* itemPanel1 = this;

	wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
	itemPanel1->SetSizer(itemBoxSizer2);

	m_grid = new wxGrid( itemPanel1, ID_GRID, wxDefaultPosition, wxSize(200, 150), wxSUNKEN_BORDER|wxHSCROLL|wxVSCROLL );
	m_grid->SetDefaultColSize(50);
	m_grid->SetDefaultRowSize(20);
	m_grid->SetColLabelSize(20);
	m_grid->SetRowLabelSize(0);
	m_grid->CreateGrid(1, 5, wxGrid::wxGridSelectRows);
	itemBoxSizer2->Add(m_grid, 1, wxGROW|wxALL, 5);

////@end FindResultsWnd_Auto content construction
}


/*
 * Should we show tooltips?
 */

bool FindResultsWnd_Auto::ShowToolTips()
{
	return true;
}

/*
 * Get bitmap resources
 */

wxBitmap FindResultsWnd_Auto::GetBitmapResource( const wxString& name )
{
	// Bitmap retrieval
////@begin FindResultsWnd_Auto bitmap retrieval
	wxUnusedVar(name);
	return wxNullBitmap;
////@end FindResultsWnd_Auto bitmap retrieval
}

/*
 * Get icon resources
 */

wxIcon FindResultsWnd_Auto::GetIconResource( const wxString& name )
{
	// Icon retrieval
////@begin FindResultsWnd_Auto icon retrieval
	wxUnusedVar(name);
	return wxNullIcon;
////@end FindResultsWnd_Auto icon retrieval
}


/*
 * wxEVT_GRID_CELL_LEFT_DCLICK event handler for ID_GRID
 */

void FindResultsWnd_Auto::OnGridCellDClick( wxGridEvent& event )
{
////@begin wxEVT_GRID_CELL_LEFT_DCLICK event handler for ID_GRID in FindResultsWnd_Auto.
	// Before editing this code, remove the block markers.
	event.Skip();
////@end wxEVT_GRID_CELL_LEFT_DCLICK event handler for ID_GRID in FindResultsWnd_Auto. 
}

