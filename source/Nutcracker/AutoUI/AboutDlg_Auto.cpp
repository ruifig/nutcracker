/////////////////////////////////////////////////////////////////////////////
// Name:        AboutDlg_Auto.cpp
// Purpose:     
// Author:      Rui Figueira
// Modified by: 
// Created:     22/03/2015 23:31:09
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#include "NutcrackerPCH.h"

////@begin includes
////@end includes

#include "AboutDlg_Auto.h"

////@begin XPM images
////@end XPM images


/*
 * AboutDlg_Auto type definition
 */

IMPLEMENT_DYNAMIC_CLASS( AboutDlg_Auto, wxDialog )


/*
 * AboutDlg_Auto event table definition
 */

BEGIN_EVENT_TABLE( AboutDlg_Auto, wxDialog )

////@begin AboutDlg_Auto event table entries
////@end AboutDlg_Auto event table entries

END_EVENT_TABLE()


/*
 * AboutDlg_Auto constructors
 */

AboutDlg_Auto::AboutDlg_Auto()
{
	Init();
}

AboutDlg_Auto::AboutDlg_Auto( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
	Init();
	Create(parent, id, caption, pos, size, style);
}


/*
 * About_Auto creator
 */

bool AboutDlg_Auto::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin AboutDlg_Auto creation
	SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
	wxDialog::Create( parent, id, caption, pos, size, style );

	CreateControls();
	if (GetSizer())
	{
		GetSizer()->SetSizeHints(this);
	}
	Centre();
////@end AboutDlg_Auto creation
	return true;
}


/*
 * AboutDlg_Auto destructor
 */

AboutDlg_Auto::~AboutDlg_Auto()
{
////@begin AboutDlg_Auto destruction
////@end AboutDlg_Auto destruction
}


/*
 * Member initialisation
 */

void AboutDlg_Auto::Init()
{
////@begin AboutDlg_Auto member initialisation
	m_logo = NULL;
	m_buildVersion = NULL;
////@end AboutDlg_Auto member initialisation
}


/*
 * Control creation for About_Auto
 */

void AboutDlg_Auto::CreateControls()
{    
////@begin AboutDlg_Auto content construction
	AboutDlg_Auto* itemDialog1 = this;

	wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
	itemDialog1->SetSizer(itemBoxSizer2);

	wxPanel* itemPanel3 = new wxPanel( itemDialog1, ID_PANEL, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
	itemPanel3->SetBackgroundColour(wxColour(255, 255, 255));
	itemBoxSizer2->Add(itemPanel3, 1, wxGROW|wxALL, 0);

	wxGridBagSizer* itemGridBagSizer4 = new wxGridBagSizer(0, 0);
	itemGridBagSizer4->SetEmptyCellSize(wxSize(10, 20));
	itemPanel3->SetSizer(itemGridBagSizer4);

	m_logo = new wxStaticBitmap( itemPanel3, wxID_STATIC, wxNullBitmap, wxDefaultPosition, wxSize(64, 64), 0 );
	itemGridBagSizer4->Add(m_logo, wxGBPosition(0, 0), wxGBSpan(1, 1), wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

	wxStaticText* itemStaticText6 = new wxStaticText( itemPanel3, wxID_STATIC, _("Nutcracker IDE"), wxDefaultPosition, wxDefaultSize, 0 );
	itemStaticText6->SetForegroundColour(wxColour(128, 128, 128));
	itemStaticText6->SetFont(wxFont(28, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Tahoma")));
	itemGridBagSizer4->Add(itemStaticText6, wxGBPosition(0, 1), wxGBSpan(1, 1), wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

	wxStaticLine* itemStaticLine7 = new wxStaticLine( itemPanel3, wxID_STATIC, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	itemGridBagSizer4->Add(itemStaticLine7, wxGBPosition(1, 1), wxGBSpan(1, 1), wxALIGN_CENTER_HORIZONTAL|wxGROW|wxALL, 5);

	m_buildVersion = new wxStaticText( itemPanel3, wxID_STATIC, _("Build XXXXXXXXXXX"), wxDefaultPosition, wxDefaultSize, 0 );
	itemGridBagSizer4->Add(m_buildVersion, wxGBPosition(2, 1), wxGBSpan(1, 1), wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

	wxGridSizer* itemGridSizer9 = new wxGridSizer(0, 2, 0, 0);
	itemGridBagSizer4->Add(itemGridSizer9, wxGBPosition(3, 1), wxGBSpan(1, 1), wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

	wxStaticText* itemStaticText10 = new wxStaticText( itemPanel3, wxID_STATIC, _("Source code repository:"), wxDefaultPosition, wxDefaultSize, 0 );
	itemGridSizer9->Add(itemStaticText10, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

	wxHyperlinkCtrl* itemHyperlinkCtrl11 = new wxHyperlinkCtrl( itemPanel3, ID_HYPERLINKCTRL, _("http://bitbucket.org/ruifig/nutcracker"), wxT("http://bitbucket.org/ruifig/nutcracker"), wxDefaultPosition, wxDefaultSize, wxHL_DEFAULT_STYLE );
	itemGridSizer9->Add(itemHyperlinkCtrl11, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

	wxStaticText* itemStaticText12 = new wxStaticText( itemPanel3, wxID_STATIC, _("Author:"), wxDefaultPosition, wxDefaultSize, 0 );
	itemGridSizer9->Add(itemStaticText12, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

	wxHyperlinkCtrl* itemHyperlinkCtrl13 = new wxHyperlinkCtrl( itemPanel3, ID_HYPERLINKCTRL1, _("rui@crazygaze.com"), wxT("rui@crazygaze.com"), wxDefaultPosition, wxDefaultSize, wxHL_DEFAULT_STYLE );
	itemGridSizer9->Add(itemHyperlinkCtrl13, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

	itemGridSizer9->Add(5, 5, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

	wxHyperlinkCtrl* itemHyperlinkCtrl15 = new wxHyperlinkCtrl( itemPanel3, ID_HYPERLINKCTRL2, _("http://www.crazygaze.com"), wxT("http://www.crazygaze.com"), wxDefaultPosition, wxDefaultSize, wxHL_DEFAULT_STYLE );
	itemGridSizer9->Add(itemHyperlinkCtrl15, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

	itemGridBagSizer4->AddGrowableRow(1);
	itemGridBagSizer4->AddGrowableCol(1);

////@end AboutDlg_Auto content construction
}


/*
 * Should we show tooltips?
 */

bool AboutDlg_Auto::ShowToolTips()
{
	return true;
}

/*
 * Get bitmap resources
 */

wxBitmap AboutDlg_Auto::GetBitmapResource( const wxString& name )
{
	// Bitmap retrieval
////@begin AboutDlg_Auto bitmap retrieval
	wxUnusedVar(name);
	return wxNullBitmap;
////@end AboutDlg_Auto bitmap retrieval
}

/*
 * Get icon resources
 */

wxIcon AboutDlg_Auto::GetIconResource( const wxString& name )
{
	// Icon retrieval
////@begin AboutDlg_Auto icon retrieval
	wxUnusedVar(name);
	return wxNullIcon;
////@end AboutDlg_Auto icon retrieval
}
