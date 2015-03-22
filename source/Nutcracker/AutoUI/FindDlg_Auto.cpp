/////////////////////////////////////////////////////////////////////////////
// Name:        FindDlg_Auto.cpp
// Purpose:     
// Author:      Rui Figueira
// Modified by: 
// Created:     22/03/2015 15:34:48
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#include "NutcrackerPCH.h"

////@begin includes
////@end includes

#include "FindDlg_Auto.h"

////@begin XPM images
////@end XPM images


/*
 * FindDlg_Auto type definition
 */

IMPLEMENT_DYNAMIC_CLASS( FindDlg_Auto, wxDialog )


/*
 * FindDlg_Auto event table definition
 */

BEGIN_EVENT_TABLE( FindDlg_Auto, wxDialog )

////@begin FindDlg_Auto event table entries
	EVT_TEXT_ENTER( ID_TEXTCTRL, FindDlg_Auto::OnTextCtrlEnter )
	EVT_BUTTON( ID_BUTTON, FindDlg_Auto::OnFindClick )
////@end FindDlg_Auto event table entries

END_EVENT_TABLE()


/*
 * FindDlg_Auto constructors
 */

FindDlg_Auto::FindDlg_Auto()
{
	Init();
}

FindDlg_Auto::FindDlg_Auto( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
	Init();
	Create(parent, id, caption, pos, size, style);
}


/*
 * FindDlg_Auto creator
 */

bool FindDlg_Auto::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin FindDlg_Auto creation
	SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
	wxDialog::Create( parent, id, caption, pos, size, style );

	CreateControls();
	if (GetSizer())
	{
		GetSizer()->SetSizeHints(this);
	}
	Centre();
////@end FindDlg_Auto creation
	return true;
}


/*
 * FindDlg_Auto destructor
 */

FindDlg_Auto::~FindDlg_Auto()
{
////@begin FindDlg_Auto destruction
////@end FindDlg_Auto destruction
}


/*
 * Member initialisation
 */

void FindDlg_Auto::Init()
{
////@begin FindDlg_Auto member initialisation
	m_whatTxtCtrl = NULL;
	m_findBtn = NULL;
	m_whereCbx = NULL;
	m_matchCaseChk = NULL;
	m_matchWholeWordChk = NULL;
////@end FindDlg_Auto member initialisation
}


/*
 * Control creation for FindDlg_Auto
 */

void FindDlg_Auto::CreateControls()
{    
////@begin FindDlg_Auto content construction
	FindDlg_Auto* itemDialog1 = this;

	wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
	itemDialog1->SetSizer(itemBoxSizer2);

	wxGridBagSizer* itemGridBagSizer3 = new wxGridBagSizer(0, 0);
	itemGridBagSizer3->SetEmptyCellSize(wxSize(10, 20));
	itemBoxSizer2->Add(itemGridBagSizer3, 1, wxGROW|wxALL, 5);

	wxStaticText* itemStaticText4 = new wxStaticText( itemDialog1, wxID_STATIC, _("Fi&nd What:"), wxDefaultPosition, wxDefaultSize, 0 );
	itemGridBagSizer3->Add(itemStaticText4, wxGBPosition(0, 0), wxGBSpan(1, 1), wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

	m_whatTxtCtrl = new wxTextCtrl( itemDialog1, ID_TEXTCTRL, wxEmptyString, wxDefaultPosition, wxSize(300, -1), wxTE_PROCESS_ENTER );
	itemGridBagSizer3->Add(m_whatTxtCtrl, wxGBPosition(0, 1), wxGBSpan(1, 1), wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

	wxStaticText* itemStaticText6 = new wxStaticText( itemDialog1, wxID_STATIC, _("&Look in :"), wxDefaultPosition, wxDefaultSize, 0 );
	itemGridBagSizer3->Add(itemStaticText6, wxGBPosition(1, 0), wxGBSpan(1, 1), wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

	wxBoxSizer* itemBoxSizer7 = new wxBoxSizer(wxVERTICAL);
	itemGridBagSizer3->Add(itemBoxSizer7, wxGBPosition(0, 2), wxGBSpan(2, 1), wxALIGN_CENTER_HORIZONTAL|wxALIGN_TOP|wxALL, 0);

	m_findBtn = new wxButton( itemDialog1, ID_BUTTON, _("&Find"), wxDefaultPosition, wxDefaultSize, 0 );
	itemBoxSizer7->Add(m_findBtn, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

	wxButton* itemButton9 = new wxButton( itemDialog1, wxID_CANCEL, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	itemBoxSizer7->Add(itemButton9, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

	wxArrayString m_whereCbxStrings;
	m_whereCbxStrings.Add(_("Entire Workspace"));
	m_whereCbxStrings.Add(_("Current File"));
	m_whereCbx = new wxChoice( itemDialog1, ID_CHOICE, wxDefaultPosition, wxDefaultSize, m_whereCbxStrings, 0 );
	m_whereCbx->SetStringSelection(_("Entire Workspace"));
	itemGridBagSizer3->Add(m_whereCbx, wxGBPosition(1, 1), wxGBSpan(1, 1), wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

	m_matchCaseChk = new wxCheckBox( itemDialog1, ID_CHECKBOX, _("Match &case"), wxDefaultPosition, wxDefaultSize, 0 );
	m_matchCaseChk->SetValue(false);
	itemGridBagSizer3->Add(m_matchCaseChk, wxGBPosition(2, 1), wxGBSpan(1, 1), wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

	m_matchWholeWordChk = new wxCheckBox( itemDialog1, ID_CHECKBOX1, _("Match &whole word"), wxDefaultPosition, wxDefaultSize, 0 );
	m_matchWholeWordChk->SetValue(false);
	itemGridBagSizer3->Add(m_matchWholeWordChk, wxGBPosition(3, 1), wxGBSpan(1, 1), wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

	itemGridBagSizer3->AddGrowableCol(1);

////@end FindDlg_Auto content construction
}


/*
 * Should we show tooltips?
 */

bool FindDlg_Auto::ShowToolTips()
{
	return true;
}

/*
 * Get bitmap resources
 */

wxBitmap FindDlg_Auto::GetBitmapResource( const wxString& name )
{
	// Bitmap retrieval
////@begin FindDlg_Auto bitmap retrieval
	wxUnusedVar(name);
	return wxNullBitmap;
////@end FindDlg_Auto bitmap retrieval
}

/*
 * Get icon resources
 */

wxIcon FindDlg_Auto::GetIconResource( const wxString& name )
{
	// Icon retrieval
////@begin FindDlg_Auto icon retrieval
	wxUnusedVar(name);
	return wxNullIcon;
////@end FindDlg_Auto icon retrieval
}


/*
 * wxEVT_COMMAND_TEXT_ENTER event handler for ID_TEXTCTRL
 */

void FindDlg_Auto::OnTextCtrlEnter( wxCommandEvent& event )
{
////@begin wxEVT_COMMAND_TEXT_ENTER event handler for ID_TEXTCTRL in FindDlg_Auto.
	// Before editing this code, remove the block markers.
	event.Skip();
////@end wxEVT_COMMAND_TEXT_ENTER event handler for ID_TEXTCTRL in FindDlg_Auto. 
}


/*
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON
 */

void FindDlg_Auto::OnFindClick( wxCommandEvent& event )
{
////@begin wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON in FindDlg_Auto.
	// Before editing this code, remove the block markers.
	event.Skip();
////@end wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON in FindDlg_Auto. 
}

