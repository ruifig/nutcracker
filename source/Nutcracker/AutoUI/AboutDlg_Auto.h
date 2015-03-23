/////////////////////////////////////////////////////////////////////////////
// Name:        About_Auto.h
// Purpose:     
// Author:      Rui Figueira
// Modified by: 
// Created:     22/03/2015 23:31:09
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef _ABOUT_AUTO_H_
#define _ABOUT_AUTO_H_


/*!
 * Includes
 */

////@begin includes
#include "wx/gbsizer.h"
#include "wx/statline.h"
#include "wx/hyperlink.h"
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
#define SYMBOL_ABOUTDLG_AUTO_STYLE wxCAPTION|wxSYSTEM_MENU|wxCLOSE_BOX|wxNO_BORDER|wxTAB_TRAVERSAL
#define SYMBOL_ABOUTDLG_AUTO_TITLE _("About")
#define SYMBOL_ABOUTDLG_AUTO_IDNAME ID_ABOUTDLG_AUTO
#define SYMBOL_ABOUTDLG_AUTO_SIZE wxSize(400, 300)
#define SYMBOL_ABOUTDLG_AUTO_POSITION wxDefaultPosition
////@end control identifiers


/*!
 * AboutDlg_Auto class declaration
 */

class AboutDlg_Auto: public wxDialog
{    
	DECLARE_DYNAMIC_CLASS( AboutDlg_Auto )
	DECLARE_EVENT_TABLE()

public:
	/// Constructors
	AboutDlg_Auto();
	AboutDlg_Auto( wxWindow* parent, wxWindowID id = SYMBOL_ABOUTDLG_AUTO_IDNAME, const wxString& caption = SYMBOL_ABOUTDLG_AUTO_TITLE, const wxPoint& pos = SYMBOL_ABOUTDLG_AUTO_POSITION, const wxSize& size = SYMBOL_ABOUTDLG_AUTO_SIZE, long style = SYMBOL_ABOUTDLG_AUTO_STYLE );

	/// Creation
	bool Create( wxWindow* parent, wxWindowID id = SYMBOL_ABOUTDLG_AUTO_IDNAME, const wxString& caption = SYMBOL_ABOUTDLG_AUTO_TITLE, const wxPoint& pos = SYMBOL_ABOUTDLG_AUTO_POSITION, const wxSize& size = SYMBOL_ABOUTDLG_AUTO_SIZE, long style = SYMBOL_ABOUTDLG_AUTO_STYLE );

	/// Destructor
	~AboutDlg_Auto();

	/// Initialises member variables
	void Init();

	/// Creates the controls and sizers
	void CreateControls();

////@begin AboutDlg_Auto event handler declarations

////@end AboutDlg_Auto event handler declarations

////@begin AboutDlg_Auto member function declarations

	/// Retrieves bitmap resources
	wxBitmap GetBitmapResource( const wxString& name );

	/// Retrieves icon resources
	wxIcon GetIconResource( const wxString& name );
////@end AboutDlg_Auto member function declarations

	/// Should we show tooltips?
	static bool ShowToolTips();

////@begin AboutDlg_Auto member variables
	wxStaticBitmap* m_logo;
	wxStaticText* m_buildVersion;
	/// Control identifiers
	enum {
		ID_ABOUTDLG_AUTO = 10000,
		ID_PANEL = 10001,
		ID_HYPERLINKCTRL = 10002,
		ID_HYPERLINKCTRL1 = 10003,
		ID_HYPERLINKCTRL2 = 10004,
		ID_HYPERLINKCTRL3 = 10005
	};
////@end AboutDlg_Auto member variables
};

#endif
	// _ABOUT_AUTO_H_
