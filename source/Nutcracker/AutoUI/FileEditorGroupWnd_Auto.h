/////////////////////////////////////////////////////////////////////////////
// Name:        FileEditorGroupWnd_Auto.h
// Purpose:     
// Author:      Rui Figueira
// Modified by: 
// Created:     14/02/2015 10:28:35
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef _FILEEDITORGROUPWND_AUTO_H_
#define _FILEEDITORGROUPWND_AUTO_H_


/*!
 * Includes
 */

////@begin includes
#include "wx/aui/auibook.h"
////@end includes

/*!
 * Forward declarations
 */

////@begin forward declarations
class wxAuiNotebook;
////@end forward declarations

/*!
 * Control identifiers
 */

////@begin control identifiers
#define SYMBOL_FILEEDITORGROUPWND_AUTO_STYLE wxTAB_TRAVERSAL
#define SYMBOL_FILEEDITORGROUPWND_AUTO_TITLE _("File Editor Group")
#define SYMBOL_FILEEDITORGROUPWND_AUTO_IDNAME ID_FILEEDITORGROUPWND_AUTO
#define SYMBOL_FILEEDITORGROUPWND_AUTO_SIZE wxSize(400, 300)
#define SYMBOL_FILEEDITORGROUPWND_AUTO_POSITION wxDefaultPosition
////@end control identifiers


/*!
 * FileEditorGroupWnd_Auto class declaration
 */

class FileEditorGroupWnd_Auto: public wxPanel
{    
	DECLARE_DYNAMIC_CLASS( FileEditorGroupWnd_Auto )
	DECLARE_EVENT_TABLE()

public:
	/// Constructors
	FileEditorGroupWnd_Auto();
	FileEditorGroupWnd_Auto( wxWindow* parent, wxWindowID id = SYMBOL_FILEEDITORGROUPWND_AUTO_IDNAME, const wxPoint& pos = SYMBOL_FILEEDITORGROUPWND_AUTO_POSITION, const wxSize& size = SYMBOL_FILEEDITORGROUPWND_AUTO_SIZE, long style = SYMBOL_FILEEDITORGROUPWND_AUTO_STYLE );

	/// Creation
	bool Create( wxWindow* parent, wxWindowID id = SYMBOL_FILEEDITORGROUPWND_AUTO_IDNAME, const wxPoint& pos = SYMBOL_FILEEDITORGROUPWND_AUTO_POSITION, const wxSize& size = SYMBOL_FILEEDITORGROUPWND_AUTO_SIZE, long style = SYMBOL_FILEEDITORGROUPWND_AUTO_STYLE );

	/// Destructor
	~FileEditorGroupWnd_Auto();

	/// Initialises member variables
	void Init();

	/// Creates the controls and sizers
	void CreateControls();

////@begin FileEditorGroupWnd_Auto event handler declarations

	/// wxEVT_COMMAND_AUINOTEBOOK_PAGE_CLOSED event handler for ID_AUINOTEBOOK
	virtual void OnPageClosed( wxAuiNotebookEvent& event );

	/// wxEVT_COMMAND_AUINOTEBOOK_PAGE_CLOSE event handler for ID_AUINOTEBOOK
	virtual void OnPageClose( wxAuiNotebookEvent& event );

	/// wxEVT_COMMAND_AUINOTEBOOK_PAGE_CHANGED event handler for ID_AUINOTEBOOK
	virtual void OnPageChanged( wxAuiNotebookEvent& event );

	/// wxEVT_COMMAND_AUINOTEBOOK_TAB_RIGHT_UP event handler for ID_AUINOTEBOOK
	virtual void OnTabRightUp( wxAuiNotebookEvent& event );

////@end FileEditorGroupWnd_Auto event handler declarations

////@begin FileEditorGroupWnd_Auto member function declarations

	/// Retrieves bitmap resources
	wxBitmap GetBitmapResource( const wxString& name );

	/// Retrieves icon resources
	wxIcon GetIconResource( const wxString& name );
////@end FileEditorGroupWnd_Auto member function declarations

	/// Should we show tooltips?
	static bool ShowToolTips();

////@begin FileEditorGroupWnd_Auto member variables
	wxAuiNotebook* m_notebook;
	/// Control identifiers
	enum {
		ID_FILEEDITORGROUPWND_AUTO = 10000,
		ID_AUINOTEBOOK = 10001
	};
////@end FileEditorGroupWnd_Auto member variables
};

#endif
	// _FILEEDITORGROUPWND_AUTO_H_
