/////////////////////////////////////////////////////////////////////////////
// Name:        FileEditorWnd_Auto.cpp
// Purpose:     
// Author:      Rui Figueira
// Modified by: 
// Created:     15/02/2015 00:04:33
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#include "NutcrackerPCH.h"

////@begin includes
////@end includes

#include "FileEditorWnd_Auto.h"

////@begin XPM images
////@end XPM images


/*
 * FileEditorWnd_Auto type definition
 */

IMPLEMENT_DYNAMIC_CLASS( FileEditorWnd_Auto, wxPanel )


/*
 * FileEditorWnd_Auto event table definition
 */

BEGIN_EVENT_TABLE( FileEditorWnd_Auto, wxPanel )

////@begin FileEditorWnd_Auto event table entries
////@end FileEditorWnd_Auto event table entries

END_EVENT_TABLE()


/*
 * FileEditorWnd_Auto constructors
 */

FileEditorWnd_Auto::FileEditorWnd_Auto()
{
	Init();
}

FileEditorWnd_Auto::FileEditorWnd_Auto( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
{
	Init();
	Create(parent, id, pos, size, style);
}


/*
 * FileEditorWnd_Auto creator
 */

bool FileEditorWnd_Auto::Create( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
{
////@begin FileEditorWnd_Auto creation
	wxPanel::Create( parent, id, pos, size, style );

	CreateControls();
	if (GetSizer())
	{
		GetSizer()->SetSizeHints(this);
	}
	Centre();
////@end FileEditorWnd_Auto creation
	return true;
}


/*
 * FileEditorWnd_Auto destructor
 */

FileEditorWnd_Auto::~FileEditorWnd_Auto()
{
////@begin FileEditorWnd_Auto destruction
////@end FileEditorWnd_Auto destruction
}


/*
 * Member initialisation
 */

void FileEditorWnd_Auto::Init()
{
////@begin FileEditorWnd_Auto member initialisation
	m_textCtrl = NULL;
////@end FileEditorWnd_Auto member initialisation
}


/*
 * Control creation for FileEditorWnd_Auto
 */

void FileEditorWnd_Auto::CreateControls()
{    
////@begin FileEditorWnd_Auto content construction
	FileEditorWnd_Auto* itemPanel1 = this;

	wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
	itemPanel1->SetSizer(itemBoxSizer2);

	m_textCtrl = new wxStyledTextCtrl( itemPanel1, ID_FileEditorTextCtrl, wxDefaultPosition, wxSize(100, 100), wxSIMPLE_BORDER );
	itemBoxSizer2->Add(m_textCtrl, 1, wxGROW|wxALL, 0);

////@end FileEditorWnd_Auto content construction
}


/*
 * Should we show tooltips?
 */

bool FileEditorWnd_Auto::ShowToolTips()
{
	return true;
}

/*
 * Get bitmap resources
 */

wxBitmap FileEditorWnd_Auto::GetBitmapResource( const wxString& name )
{
	// Bitmap retrieval
////@begin FileEditorWnd_Auto bitmap retrieval
	wxUnusedVar(name);
	return wxNullBitmap;
////@end FileEditorWnd_Auto bitmap retrieval
}

/*
 * Get icon resources
 */

wxIcon FileEditorWnd_Auto::GetIconResource( const wxString& name )
{
	// Icon retrieval
////@begin FileEditorWnd_Auto icon retrieval
	wxUnusedVar(name);
	return wxNullIcon;
////@end FileEditorWnd_Auto icon retrieval
}
