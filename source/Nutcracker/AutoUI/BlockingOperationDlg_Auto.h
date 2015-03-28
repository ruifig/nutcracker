/////////////////////////////////////////////////////////////////////////////
// Name:        BlockingOperationDlg_Auto.h
// Purpose:     
// Author:      Rui Figueira
// Modified by: 
// Created:     28/03/2015 15:54:42
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef _BLOCKINGOPERATIONDLG_AUTO_H_
#define _BLOCKINGOPERATIONDLG_AUTO_H_


/*!
 * Includes
 */

////@begin includes
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
#define SYMBOL_BLOCKINGOPERATIONDLG_AUTO_STYLE wxCAPTION|wxSYSTEM_MENU|wxSTAY_ON_TOP|wxNO_BORDER|wxTAB_TRAVERSAL
#define SYMBOL_BLOCKINGOPERATIONDLG_AUTO_TITLE _("BlockingOperationDlg_Auto")
#define SYMBOL_BLOCKINGOPERATIONDLG_AUTO_IDNAME ID_BLOCKINGOPERATIONDLG_AUTO
#define SYMBOL_BLOCKINGOPERATIONDLG_AUTO_SIZE wxSize(300, 100)
#define SYMBOL_BLOCKINGOPERATIONDLG_AUTO_POSITION wxDefaultPosition
////@end control identifiers


/*!
 * BlockingOperationDlg_Auto class declaration
 */

class BlockingOperationDlg_Auto: public wxDialog
{    
	DECLARE_DYNAMIC_CLASS( BlockingOperationDlg_Auto )
	DECLARE_EVENT_TABLE()

public:
	/// Constructors
	BlockingOperationDlg_Auto();
	BlockingOperationDlg_Auto( wxWindow* parent, wxWindowID id = SYMBOL_BLOCKINGOPERATIONDLG_AUTO_IDNAME, const wxString& caption = SYMBOL_BLOCKINGOPERATIONDLG_AUTO_TITLE, const wxPoint& pos = SYMBOL_BLOCKINGOPERATIONDLG_AUTO_POSITION, const wxSize& size = SYMBOL_BLOCKINGOPERATIONDLG_AUTO_SIZE, long style = SYMBOL_BLOCKINGOPERATIONDLG_AUTO_STYLE );

	/// Creation
	bool Create( wxWindow* parent, wxWindowID id = SYMBOL_BLOCKINGOPERATIONDLG_AUTO_IDNAME, const wxString& caption = SYMBOL_BLOCKINGOPERATIONDLG_AUTO_TITLE, const wxPoint& pos = SYMBOL_BLOCKINGOPERATIONDLG_AUTO_POSITION, const wxSize& size = SYMBOL_BLOCKINGOPERATIONDLG_AUTO_SIZE, long style = SYMBOL_BLOCKINGOPERATIONDLG_AUTO_STYLE );

	/// Destructor
	~BlockingOperationDlg_Auto();

	/// Initialises member variables
	void Init();

	/// Creates the controls and sizers
	void CreateControls();

////@begin BlockingOperationDlg_Auto event handler declarations

	/// wxEVT_CLOSE_WINDOW event handler for ID_BLOCKINGOPERATIONDLG_AUTO
	virtual void OnCloseWindow( wxCloseEvent& event );

	/// wxEVT_IDLE event handler for ID_BLOCKINGOPERATIONDLG_AUTO
	virtual void OnIdle( wxIdleEvent& event );

	/// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BLOCKINGOPERATIONDLG_CANCEL
	virtual void OnCancelClick( wxCommandEvent& event );

////@end BlockingOperationDlg_Auto event handler declarations

////@begin BlockingOperationDlg_Auto member function declarations

	/// Retrieves bitmap resources
	wxBitmap GetBitmapResource( const wxString& name );

	/// Retrieves icon resources
	wxIcon GetIconResource( const wxString& name );
////@end BlockingOperationDlg_Auto member function declarations

	/// Should we show tooltips?
	static bool ShowToolTips();

////@begin BlockingOperationDlg_Auto member variables
	wxStaticText* m_txtCtrl;
	/// Control identifiers
	enum {
		ID_BLOCKINGOPERATIONDLG_AUTO = 10000,
		ID_PANEL = 10001,
		ID_BLOCKINGOPERATIONDLG_CANCEL = 10000
	};
////@end BlockingOperationDlg_Auto member variables
};

#endif
	// _BLOCKINGOPERATIONDLG_AUTO_H_
