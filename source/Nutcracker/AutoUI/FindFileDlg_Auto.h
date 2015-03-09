/////////////////////////////////////////////////////////////////////////////
// Name:        FindFileDlg_Auto.h
// Purpose:     
// Author:      Rui Figueira
// Modified by: 
// Created:     09/03/2015 13:32:59
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef _FINDFILEDLG_AUTO_H_
#define _FINDFILEDLG_AUTO_H_


/*!
 * Includes
 */

////@begin includes
#include "wx/grid.h"
////@end includes

/*!
 * Forward declarations
 */

////@begin forward declarations
class wxGrid;
////@end forward declarations

/*!
 * Control identifiers
 */

////@begin control identifiers
#define SYMBOL_FINDFILEDLG_AUTO_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX|wxTAB_TRAVERSAL
#define SYMBOL_FINDFILEDLG_AUTO_TITLE _("FindFileDlg_Auto")
#define SYMBOL_FINDFILEDLG_AUTO_IDNAME ID_FINDFILEDLG_AUTO
#define SYMBOL_FINDFILEDLG_AUTO_SIZE wxSize(400, 300)
#define SYMBOL_FINDFILEDLG_AUTO_POSITION wxDefaultPosition
////@end control identifiers


/*!
 * FindFileDlg_Auto class declaration
 */

class FindFileDlg_Auto: public wxDialog
{    
	DECLARE_DYNAMIC_CLASS( FindFileDlg_Auto )
	DECLARE_EVENT_TABLE()

public:
	/// Constructors
	FindFileDlg_Auto();
	FindFileDlg_Auto( wxWindow* parent, wxWindowID id = SYMBOL_FINDFILEDLG_AUTO_IDNAME, const wxString& caption = SYMBOL_FINDFILEDLG_AUTO_TITLE, const wxPoint& pos = SYMBOL_FINDFILEDLG_AUTO_POSITION, const wxSize& size = SYMBOL_FINDFILEDLG_AUTO_SIZE, long style = SYMBOL_FINDFILEDLG_AUTO_STYLE );

	/// Creation
	bool Create( wxWindow* parent, wxWindowID id = SYMBOL_FINDFILEDLG_AUTO_IDNAME, const wxString& caption = SYMBOL_FINDFILEDLG_AUTO_TITLE, const wxPoint& pos = SYMBOL_FINDFILEDLG_AUTO_POSITION, const wxSize& size = SYMBOL_FINDFILEDLG_AUTO_SIZE, long style = SYMBOL_FINDFILEDLG_AUTO_STYLE );

	/// Destructor
	~FindFileDlg_Auto();

	/// Initialises member variables
	void Init();

	/// Creates the controls and sizers
	void CreateControls();

////@begin FindFileDlg_Auto event handler declarations

	/// wxEVT_GRID_CELL_LEFT_DCLICK event handler for ID_GRID
	virtual void OnGridCellDClick( wxGridEvent& event );

	/// wxEVT_COMMAND_TEXT_UPDATED event handler for ID_TEXTCTRL
	virtual void OnTextctrlTextUpdated( wxCommandEvent& event );

////@end FindFileDlg_Auto event handler declarations

////@begin FindFileDlg_Auto member function declarations

	/// Retrieves bitmap resources
	wxBitmap GetBitmapResource( const wxString& name );

	/// Retrieves icon resources
	wxIcon GetIconResource( const wxString& name );
////@end FindFileDlg_Auto member function declarations

	/// Should we show tooltips?
	static bool ShowToolTips();

////@begin FindFileDlg_Auto member variables
	wxGrid* m_grid;
	wxTextCtrl* m_txt;
	/// Control identifiers
	enum {
		ID_FINDFILEDLG_AUTO = 10000,
		ID_GRID = 10001,
		ID_TEXTCTRL = 10002
	};
////@end FindFileDlg_Auto member variables
};

#endif
	// _FINDFILEDLG_AUTO_H_
