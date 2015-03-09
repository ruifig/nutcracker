/////////////////////////////////////////////////////////////////////////////
// Name:        FindFileDlg_Auto.cpp
// Purpose:     
// Author:      Rui Figueira
// Modified by: 
// Created:     09/03/2015 13:32:59
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#include "NutcrackerPCH.h"

////@begin includes
////@end includes

#include "FindFileDlg_Auto.h"

////@begin XPM images
////@end XPM images


/*
 * FindFileDlg_Auto type definition
 */

IMPLEMENT_DYNAMIC_CLASS( FindFileDlg_Auto, wxDialog )


/*
 * FindFileDlg_Auto event table definition
 */

BEGIN_EVENT_TABLE( FindFileDlg_Auto, wxDialog )

////@begin FindFileDlg_Auto event table entries
	EVT_GRID_CELL_LEFT_DCLICK( FindFileDlg_Auto::OnGridCellDClick )
	EVT_TEXT( ID_TEXTCTRL, FindFileDlg_Auto::OnTextctrlTextUpdated )
////@end FindFileDlg_Auto event table entries

END_EVENT_TABLE()


/*
 * FindFileDlg_Auto constructors
 */

FindFileDlg_Auto::FindFileDlg_Auto()
{
	Init();
}

FindFileDlg_Auto::FindFileDlg_Auto( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
	Init();
	Create(parent, id, caption, pos, size, style);
}


/*
 * FindFileDlg_Auto creator
 */

bool FindFileDlg_Auto::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin FindFileDlg_Auto creation
	SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
	wxDialog::Create( parent, id, caption, pos, size, style );

	CreateControls();
	if (GetSizer())
	{
		GetSizer()->SetSizeHints(this);
	}
	Centre();
////@end FindFileDlg_Auto creation
	return true;
}


/*
 * FindFileDlg_Auto destructor
 */

FindFileDlg_Auto::~FindFileDlg_Auto()
{
////@begin FindFileDlg_Auto destruction
////@end FindFileDlg_Auto destruction
}


/*
 * Member initialisation
 */

void FindFileDlg_Auto::Init()
{
////@begin FindFileDlg_Auto member initialisation
	m_grid = NULL;
	m_txt = NULL;
////@end FindFileDlg_Auto member initialisation
}


/*
 * Control creation for FindFileDlg_Auto
 */

void FindFileDlg_Auto::CreateControls()
{    
////@begin FindFileDlg_Auto content construction
	FindFileDlg_Auto* itemDialog1 = this;

	wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
	itemDialog1->SetSizer(itemBoxSizer2);

	m_grid = new wxGrid( itemDialog1, ID_GRID, wxDefaultPosition, wxSize(700, 200), wxSUNKEN_BORDER|wxHSCROLL|wxVSCROLL );
	m_grid->SetDefaultColSize(100);
	m_grid->SetDefaultRowSize(20);
	m_grid->SetColLabelSize(25);
	m_grid->SetRowLabelSize(0);
	m_grid->CreateGrid(1, 2, wxGrid::wxGridSelectRows);
	itemBoxSizer2->Add(m_grid, 1, wxGROW|wxALL, 5);

	wxFlexGridSizer* itemFlexGridSizer4 = new wxFlexGridSizer(2, 2, 0, 0);
	itemBoxSizer2->Add(itemFlexGridSizer4, 0, wxGROW|wxALL, 0);

	m_txt = new wxTextCtrl( itemDialog1, ID_TEXTCTRL, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	itemFlexGridSizer4->Add(m_txt, 1, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

	itemFlexGridSizer4->AddGrowableCol(0);

////@end FindFileDlg_Auto content construction
}


/*
 * Should we show tooltips?
 */

bool FindFileDlg_Auto::ShowToolTips()
{
	return true;
}

/*
 * Get bitmap resources
 */

wxBitmap FindFileDlg_Auto::GetBitmapResource( const wxString& name )
{
	// Bitmap retrieval
////@begin FindFileDlg_Auto bitmap retrieval
	wxUnusedVar(name);
	return wxNullBitmap;
////@end FindFileDlg_Auto bitmap retrieval
}

/*
 * Get icon resources
 */

wxIcon FindFileDlg_Auto::GetIconResource( const wxString& name )
{
	// Icon retrieval
////@begin FindFileDlg_Auto icon retrieval
	wxUnusedVar(name);
	return wxNullIcon;
////@end FindFileDlg_Auto icon retrieval
}


/*
 * wxEVT_COMMAND_TEXT_UPDATED event handler for ID_TEXTCTRL
 */

void FindFileDlg_Auto::OnTextctrlTextUpdated( wxCommandEvent& event )
{
////@begin wxEVT_COMMAND_TEXT_UPDATED event handler for ID_TEXTCTRL in FindFileDlg_Auto.
	// Before editing this code, remove the block markers.
	event.Skip();
////@end wxEVT_COMMAND_TEXT_UPDATED event handler for ID_TEXTCTRL in FindFileDlg_Auto. 
}


/*
 * wxEVT_GRID_CELL_LEFT_DCLICK event handler for ID_GRID
 */

void FindFileDlg_Auto::OnGridCellDClick( wxGridEvent& event )
{
////@begin wxEVT_GRID_CELL_LEFT_DCLICK event handler for ID_GRID in FindFileDlg_Auto.
	// Before editing this code, remove the block markers.
	event.Skip();
////@end wxEVT_GRID_CELL_LEFT_DCLICK event handler for ID_GRID in FindFileDlg_Auto. 
}

