/////////////////////////////////////////////////////////////////////////////
// Name:        BlockingOperationDlg_Auto.cpp
// Purpose:     
// Author:      Rui Figueira
// Modified by: 
// Created:     28/03/2015 15:54:42
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#include "NutcrackerPCH.h"

////@begin includes
////@end includes

#include "BlockingOperationDlg_Auto.h"

////@begin XPM images
////@end XPM images


/*
 * BlockingOperationDlg_Auto type definition
 */

IMPLEMENT_DYNAMIC_CLASS( BlockingOperationDlg_Auto, wxDialog )


/*
 * BlockingOperationDlg_Auto event table definition
 */

BEGIN_EVENT_TABLE( BlockingOperationDlg_Auto, wxDialog )

////@begin BlockingOperationDlg_Auto event table entries
	EVT_CLOSE( BlockingOperationDlg_Auto::OnCloseWindow )
	EVT_IDLE( BlockingOperationDlg_Auto::OnIdle )
	EVT_BUTTON( ID_BLOCKINGOPERATIONDLG_CANCEL, BlockingOperationDlg_Auto::OnCancelClick )
////@end BlockingOperationDlg_Auto event table entries

END_EVENT_TABLE()


/*
 * BlockingOperationDlg_Auto constructors
 */

BlockingOperationDlg_Auto::BlockingOperationDlg_Auto()
{
	Init();
}

BlockingOperationDlg_Auto::BlockingOperationDlg_Auto( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
	Init();
	Create(parent, id, caption, pos, size, style);
}


/*
 * BlockingOperationDlg_Auto creator
 */

bool BlockingOperationDlg_Auto::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin BlockingOperationDlg_Auto creation
	SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
	wxDialog::Create( parent, id, caption, pos, size, style );

	CreateControls();
	if (GetSizer())
	{
		GetSizer()->SetSizeHints(this);
	}
	Centre();
////@end BlockingOperationDlg_Auto creation
	return true;
}


/*
 * BlockingOperationDlg_Auto destructor
 */

BlockingOperationDlg_Auto::~BlockingOperationDlg_Auto()
{
////@begin BlockingOperationDlg_Auto destruction
////@end BlockingOperationDlg_Auto destruction
}


/*
 * Member initialisation
 */

void BlockingOperationDlg_Auto::Init()
{
////@begin BlockingOperationDlg_Auto member initialisation
	m_txtCtrl = NULL;
////@end BlockingOperationDlg_Auto member initialisation
}


/*
 * Control creation for BlockingOperationDlg_Auto
 */

void BlockingOperationDlg_Auto::CreateControls()
{    
////@begin BlockingOperationDlg_Auto content construction
	BlockingOperationDlg_Auto* itemDialog1 = this;

	wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
	itemDialog1->SetSizer(itemBoxSizer2);

	wxPanel* itemPanel3 = new wxPanel( itemDialog1, ID_PANEL, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
	itemBoxSizer2->Add(itemPanel3, 1, wxGROW|wxALL, 0);

	wxBoxSizer* itemBoxSizer4 = new wxBoxSizer(wxVERTICAL);
	itemPanel3->SetSizer(itemBoxSizer4);

	m_txtCtrl = new wxStaticText( itemPanel3, wxID_STATIC, _("TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST  FINISHED"), wxDefaultPosition, wxDefaultSize, 0 );
	m_txtCtrl->Wrap(280);
	itemBoxSizer4->Add(m_txtCtrl, 1, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

	wxButton* itemButton6 = new wxButton( itemPanel3, ID_BLOCKINGOPERATIONDLG_CANCEL, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	itemBoxSizer4->Add(itemButton6, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

////@end BlockingOperationDlg_Auto content construction
}


/*
 * Should we show tooltips?
 */

bool BlockingOperationDlg_Auto::ShowToolTips()
{
	return true;
}

/*
 * Get bitmap resources
 */

wxBitmap BlockingOperationDlg_Auto::GetBitmapResource( const wxString& name )
{
	// Bitmap retrieval
////@begin BlockingOperationDlg_Auto bitmap retrieval
	wxUnusedVar(name);
	return wxNullBitmap;
////@end BlockingOperationDlg_Auto bitmap retrieval
}

/*
 * Get icon resources
 */

wxIcon BlockingOperationDlg_Auto::GetIconResource( const wxString& name )
{
	// Icon retrieval
////@begin BlockingOperationDlg_Auto icon retrieval
	wxUnusedVar(name);
	return wxNullIcon;
////@end BlockingOperationDlg_Auto icon retrieval
}


/*
 * wxEVT_IDLE event handler for ID_BLOCKINGOPERATIONDLG_AUTO
 */

void BlockingOperationDlg_Auto::OnIdle( wxIdleEvent& event )
{
////@begin wxEVT_IDLE event handler for ID_BLOCKINGOPERATIONDLG_AUTO in BlockingOperationDlg_Auto.
	// Before editing this code, remove the block markers.
	event.Skip();
////@end wxEVT_IDLE event handler for ID_BLOCKINGOPERATIONDLG_AUTO in BlockingOperationDlg_Auto. 
}


/*
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_CANCEL
 */

void BlockingOperationDlg_Auto::OnCancelClick( wxCommandEvent& event )
{
////@begin wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_CANCEL in BlockingOperationDlg_Auto.
	// Before editing this code, remove the block markers.
	event.Skip();
////@end wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_CANCEL in BlockingOperationDlg_Auto. 
}


/*
 * wxEVT_CLOSE_WINDOW event handler for ID_BLOCKINGOPERATIONDLG_AUTO
 */

void BlockingOperationDlg_Auto::OnCloseWindow( wxCloseEvent& event )
{
////@begin wxEVT_CLOSE_WINDOW event handler for ID_BLOCKINGOPERATIONDLG_AUTO in BlockingOperationDlg_Auto.
	// Before editing this code, remove the block markers.
	event.Skip();
////@end wxEVT_CLOSE_WINDOW event handler for ID_BLOCKINGOPERATIONDLG_AUTO in BlockingOperationDlg_Auto. 
}

