/////////////////////////////////////////////////////////////////////////////
// Name:        CallstackWnd_Auto.h
// Purpose:     
// Author:      Rui Figueira
// Modified by: 
// Created:     23/02/2015 00:43:41
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef _CALLSTACKWND_AUTO_H_
#define _CALLSTACKWND_AUTO_H_


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
#define SYMBOL_CALLSTACKWND_AUTO_STYLE wxTAB_TRAVERSAL
#define SYMBOL_CALLSTACKWND_AUTO_TITLE _("CallstackWnd_Auto")
#define SYMBOL_CALLSTACKWND_AUTO_IDNAME ID_CALLSTACKWND_AUTO
#define SYMBOL_CALLSTACKWND_AUTO_SIZE wxSize(400, 300)
#define SYMBOL_CALLSTACKWND_AUTO_POSITION wxDefaultPosition
////@end control identifiers


/*!
 * CallstackWnd_Auto class declaration
 */

class CallstackWnd_Auto: public wxPanel
{    
	DECLARE_DYNAMIC_CLASS( CallstackWnd_Auto )
	DECLARE_EVENT_TABLE()

public:
	/// Constructors
	CallstackWnd_Auto();
	CallstackWnd_Auto( wxWindow* parent, wxWindowID id = SYMBOL_CALLSTACKWND_AUTO_IDNAME, const wxPoint& pos = SYMBOL_CALLSTACKWND_AUTO_POSITION, const wxSize& size = SYMBOL_CALLSTACKWND_AUTO_SIZE, long style = SYMBOL_CALLSTACKWND_AUTO_STYLE );

	/// Creation
	bool Create( wxWindow* parent, wxWindowID id = SYMBOL_CALLSTACKWND_AUTO_IDNAME, const wxPoint& pos = SYMBOL_CALLSTACKWND_AUTO_POSITION, const wxSize& size = SYMBOL_CALLSTACKWND_AUTO_SIZE, long style = SYMBOL_CALLSTACKWND_AUTO_STYLE );

	/// Destructor
	~CallstackWnd_Auto();

	/// Initialises member variables
	void Init();

	/// Creates the controls and sizers
	void CreateControls();

////@begin CallstackWnd_Auto event handler declarations

	/// wxEVT_SIZE event handler for ID_CALLSTACKWND_AUTO
	virtual void OnSize( wxSizeEvent& event );

	/// wxEVT_GRID_CELL_LEFT_CLICK event handler for ID_GRID
	virtual void OnCellClick( wxGridEvent& event );

	/// wxEVT_GRID_CELL_LEFT_DCLICK event handler for ID_GRID
	virtual void OnCellDClick( wxGridEvent& event );

	/// wxEVT_GRID_CELL_CHANGED event handler for ID_GRID
	virtual void OnCellChanged( wxGridEvent& event );

////@end CallstackWnd_Auto event handler declarations

////@begin CallstackWnd_Auto member function declarations

	/// Retrieves bitmap resources
	wxBitmap GetBitmapResource( const wxString& name );

	/// Retrieves icon resources
	wxIcon GetIconResource( const wxString& name );
////@end CallstackWnd_Auto member function declarations

	/// Should we show tooltips?
	static bool ShowToolTips();

////@begin CallstackWnd_Auto member variables
	wxGrid* m_grid;
	/// Control identifiers
	enum {
		ID_CALLSTACKWND_AUTO = 10000,
		ID_GRID = 10002
	};
////@end CallstackWnd_Auto member variables
};

#endif
	// _CALLSTACKWND_AUTO_H_
