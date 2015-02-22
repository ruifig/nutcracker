/////////////////////////////////////////////////////////////////////////////
// Name:        FileEditorWnd_Auto.h
// Purpose:     
// Author:      Rui Figueira
// Modified by: 
// Created:     15/02/2015 00:04:33
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef _FILEEDITORWND_AUTO_H_
#define _FILEEDITORWND_AUTO_H_


/*!
 * Includes
 */

////@begin includes
////@end includes

/*!
 * Forward declarations
 */

////@begin forward declarations
class wxStyledTextCtrl;
////@end forward declarations

/*!
 * Control identifiers
 */

////@begin control identifiers
#define SYMBOL_FILEEDITORWND_AUTO_STYLE wxTAB_TRAVERSAL
#define SYMBOL_FILEEDITORWND_AUTO_TITLE _("File Editor")
#define SYMBOL_FILEEDITORWND_AUTO_IDNAME ID_FILEEDITORWND_AUTO
#define SYMBOL_FILEEDITORWND_AUTO_SIZE wxSize(400, 300)
#define SYMBOL_FILEEDITORWND_AUTO_POSITION wxDefaultPosition
////@end control identifiers


/*!
 * FileEditorWnd_Auto class declaration
 */

class FileEditorWnd_Auto: public wxPanel
{    
	DECLARE_DYNAMIC_CLASS( FileEditorWnd_Auto )
	DECLARE_EVENT_TABLE()

public:
	/// Constructors
	FileEditorWnd_Auto();
	FileEditorWnd_Auto( wxWindow* parent, wxWindowID id = SYMBOL_FILEEDITORWND_AUTO_IDNAME, const wxPoint& pos = SYMBOL_FILEEDITORWND_AUTO_POSITION, const wxSize& size = SYMBOL_FILEEDITORWND_AUTO_SIZE, long style = SYMBOL_FILEEDITORWND_AUTO_STYLE );

	/// Creation
	bool Create( wxWindow* parent, wxWindowID id = SYMBOL_FILEEDITORWND_AUTO_IDNAME, const wxPoint& pos = SYMBOL_FILEEDITORWND_AUTO_POSITION, const wxSize& size = SYMBOL_FILEEDITORWND_AUTO_SIZE, long style = SYMBOL_FILEEDITORWND_AUTO_STYLE );

	/// Destructor
	~FileEditorWnd_Auto();

	/// Initialises member variables
	void Init();

	/// Creates the controls and sizers
	void CreateControls();

////@begin FileEditorWnd_Auto event handler declarations

////@end FileEditorWnd_Auto event handler declarations

////@begin FileEditorWnd_Auto member function declarations

	/// Retrieves bitmap resources
	wxBitmap GetBitmapResource( const wxString& name );

	/// Retrieves icon resources
	wxIcon GetIconResource( const wxString& name );
////@end FileEditorWnd_Auto member function declarations

	/// Should we show tooltips?
	static bool ShowToolTips();

////@begin FileEditorWnd_Auto member variables
	wxStyledTextCtrl* m_textCtrl;
	/// Control identifiers
	enum {
		ID_FILEEDITORWND_AUTO = 10000,
		ID_FileEditorTextCtrl = 10001
	};
////@end FileEditorWnd_Auto member variables
};

#endif
	// _FILEEDITORWND_AUTO_H_
