/////////////////////////////////////////////////////////////////////////////
// Name:        FindDlg_Auto.h
// Purpose:     
// Author:      Rui Figueira
// Modified by: 
// Created:     22/03/2015 15:34:48
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef _FINDDLG_AUTO_H_
#define _FINDDLG_AUTO_H_


/*!
 * Includes
 */

////@begin includes
#include "wx/gbsizer.h"
////@end includes

/*!
 * Forward declarations
 */

////@begin forward declarations
////@end forward declarations

/*!
 * Control identifiers
 */

////@begin control identifiers
#define SYMBOL_FINDDLG_AUTO_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX|wxTAB_TRAVERSAL
#define SYMBOL_FINDDLG_AUTO_TITLE _("FindDlg_Auto")
#define SYMBOL_FINDDLG_AUTO_IDNAME ID_FINDDLG_AUTO
#define SYMBOL_FINDDLG_AUTO_SIZE wxSize(600, 500)
#define SYMBOL_FINDDLG_AUTO_POSITION wxDefaultPosition
////@end control identifiers


/*!
 * FindDlg_Auto class declaration
 */

class FindDlg_Auto: public wxDialog
{    
	DECLARE_DYNAMIC_CLASS( FindDlg_Auto )
	DECLARE_EVENT_TABLE()

public:
	/// Constructors
	FindDlg_Auto();
	FindDlg_Auto( wxWindow* parent, wxWindowID id = SYMBOL_FINDDLG_AUTO_IDNAME, const wxString& caption = SYMBOL_FINDDLG_AUTO_TITLE, const wxPoint& pos = SYMBOL_FINDDLG_AUTO_POSITION, const wxSize& size = SYMBOL_FINDDLG_AUTO_SIZE, long style = SYMBOL_FINDDLG_AUTO_STYLE );

	/// Creation
	bool Create( wxWindow* parent, wxWindowID id = SYMBOL_FINDDLG_AUTO_IDNAME, const wxString& caption = SYMBOL_FINDDLG_AUTO_TITLE, const wxPoint& pos = SYMBOL_FINDDLG_AUTO_POSITION, const wxSize& size = SYMBOL_FINDDLG_AUTO_SIZE, long style = SYMBOL_FINDDLG_AUTO_STYLE );

	/// Destructor
	~FindDlg_Auto();

	/// Initialises member variables
	void Init();

	/// Creates the controls and sizers
	void CreateControls();

////@begin FindDlg_Auto event handler declarations

	/// wxEVT_COMMAND_TEXT_ENTER event handler for ID_TEXTCTRL
	virtual void OnTextCtrlEnter( wxCommandEvent& event );

	/// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON
	virtual void OnFindClick( wxCommandEvent& event );

////@end FindDlg_Auto event handler declarations

////@begin FindDlg_Auto member function declarations

	/// Retrieves bitmap resources
	wxBitmap GetBitmapResource( const wxString& name );

	/// Retrieves icon resources
	wxIcon GetIconResource( const wxString& name );
////@end FindDlg_Auto member function declarations

	/// Should we show tooltips?
	static bool ShowToolTips();

////@begin FindDlg_Auto member variables
	wxTextCtrl* m_whatTxtCtrl;
	wxButton* m_findBtn;
	wxChoice* m_whereCbx;
	wxCheckBox* m_matchCaseChk;
	wxCheckBox* m_matchWholeWordChk;
	/// Control identifiers
	enum {
		ID_FINDDLG_AUTO = 10000,
		ID_TEXTCTRL = 10001,
		ID_BUTTON = 10002,
		ID_CHOICE = 10004,
		ID_CHECKBOX = 10005,
		ID_CHECKBOX1 = 10006
	};
////@end FindDlg_Auto member variables
};

#endif
	// _FINDDLG_AUTO_H_
