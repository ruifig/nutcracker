/////////////////////////////////////////////////////////////////////////////
// Name:        FileEditorGroupWnd_Auto.cpp
// Purpose:     
// Author:      Rui Figueira
// Modified by: 
// Created:     14/02/2015 10:28:35
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#include "NutcrackerPCH.h"

////@begin includes
#include "wx/imaglist.h"
////@end includes

#include "FileEditorGroupWnd_Auto.h"

////@begin XPM images
////@end XPM images


/*
 * FileEditorGroupWnd_Auto type definition
 */

IMPLEMENT_DYNAMIC_CLASS( FileEditorGroupWnd_Auto, wxPanel )


/*
 * FileEditorGroupWnd_Auto event table definition
 */

BEGIN_EVENT_TABLE( FileEditorGroupWnd_Auto, wxPanel )

////@begin FileEditorGroupWnd_Auto event table entries
////@end FileEditorGroupWnd_Auto event table entries

END_EVENT_TABLE()


/*
 * FileEditorGroupWnd_Auto constructors
 */

FileEditorGroupWnd_Auto::FileEditorGroupWnd_Auto()
{
	Init();
}

FileEditorGroupWnd_Auto::FileEditorGroupWnd_Auto( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
{
	Init();
	Create(parent, id, pos, size, style);
}


/*
 * FileEditorGroupWnd_Auto creator
 */

bool FileEditorGroupWnd_Auto::Create( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
{
////@begin FileEditorGroupWnd_Auto creation
	wxPanel::Create( parent, id, pos, size, style );

	CreateControls();
	if (GetSizer())
	{
		GetSizer()->SetSizeHints(this);
	}
	Centre();
////@end FileEditorGroupWnd_Auto creation
	return true;
}


/*
 * FileEditorGroupWnd_Auto destructor
 */

FileEditorGroupWnd_Auto::~FileEditorGroupWnd_Auto()
{
////@begin FileEditorGroupWnd_Auto destruction
////@end FileEditorGroupWnd_Auto destruction
}


/*
 * Member initialisation
 */

void FileEditorGroupWnd_Auto::Init()
{
////@begin FileEditorGroupWnd_Auto member initialisation
	m_notebook = NULL;
////@end FileEditorGroupWnd_Auto member initialisation
}


/*
 * Control creation for FileEditorGroupWnd_Auto
 */

void FileEditorGroupWnd_Auto::CreateControls()
{    
////@begin FileEditorGroupWnd_Auto content construction
	FileEditorGroupWnd_Auto* itemPanel1 = this;

	wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
	itemPanel1->SetSizer(itemBoxSizer2);

	m_notebook = new wxAuiNotebook( itemPanel1, ID_AUINOTEBOOK, wxDefaultPosition, wxSize(430, 200), wxAUI_NB_DEFAULT_STYLE|wxAUI_NB_TOP );

	itemBoxSizer2->Add(m_notebook, 1, wxGROW|wxALL, 0);

////@end FileEditorGroupWnd_Auto content construction
}


/*
 * Should we show tooltips?
 */

bool FileEditorGroupWnd_Auto::ShowToolTips()
{
	return true;
}

/*
 * Get bitmap resources
 */

wxBitmap FileEditorGroupWnd_Auto::GetBitmapResource( const wxString& name )
{
	// Bitmap retrieval
////@begin FileEditorGroupWnd_Auto bitmap retrieval
	wxUnusedVar(name);
	return wxNullBitmap;
////@end FileEditorGroupWnd_Auto bitmap retrieval
}

/*
 * Get icon resources
 */

wxIcon FileEditorGroupWnd_Auto::GetIconResource( const wxString& name )
{
	// Icon retrieval
////@begin FileEditorGroupWnd_Auto icon retrieval
	wxUnusedVar(name);
	return wxNullIcon;
////@end FileEditorGroupWnd_Auto icon retrieval
}
