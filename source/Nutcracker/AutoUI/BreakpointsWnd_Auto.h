/////////////////////////////////////////////////////////////////////////////
// Name:        BreakpointsWnd_Auto.h
// Purpose:     
// Author:      Rui Figueira
// Modified by: 
// Created:     21/02/2015 20:24:17
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef _BREAKPOINTSWND_AUTO_H_
#define _BREAKPOINTSWND_AUTO_H_


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
#define SYMBOL_BREAKPOINTSWND_AUTO_STYLE wxTAB_TRAVERSAL
#define SYMBOL_BREAKPOINTSWND_AUTO_TITLE _("BreakpointsWnd_Auto")
#define SYMBOL_BREAKPOINTSWND_AUTO_IDNAME ID_BREAKPOINTSWND_AUTO
#define SYMBOL_BREAKPOINTSWND_AUTO_SIZE wxSize(400, 300)
#define SYMBOL_BREAKPOINTSWND_AUTO_POSITION wxDefaultPosition
////@end control identifiers


/*!
 * BreakpointsWnd_Auto class declaration
 */

class BreakpointsWnd_Auto: public wxPanel
{    
	DECLARE_DYNAMIC_CLASS( BreakpointsWnd_Auto )
	DECLARE_EVENT_TABLE()

public:
	/// Constructors
	BreakpointsWnd_Auto();
	BreakpointsWnd_Auto( wxWindow* parent, wxWindowID id = SYMBOL_BREAKPOINTSWND_AUTO_IDNAME, const wxPoint& pos = SYMBOL_BREAKPOINTSWND_AUTO_POSITION, const wxSize& size = SYMBOL_BREAKPOINTSWND_AUTO_SIZE, long style = SYMBOL_BREAKPOINTSWND_AUTO_STYLE );

	/// Creation
	bool Create( wxWindow* parent, wxWindowID id = SYMBOL_BREAKPOINTSWND_AUTO_IDNAME, const wxPoint& pos = SYMBOL_BREAKPOINTSWND_AUTO_POSITION, const wxSize& size = SYMBOL_BREAKPOINTSWND_AUTO_SIZE, long style = SYMBOL_BREAKPOINTSWND_AUTO_STYLE );

	/// Destructor
	~BreakpointsWnd_Auto();

	/// Initialises member variables
	void Init();

	/// Creates the controls and sizers
	void CreateControls();

////@begin BreakpointsWnd_Auto event handler declarations

	/// wxEVT_SIZE event handler for ID_BREAKPOINTSWND_AUTO
	virtual void OnSize( wxSizeEvent& event );

	/// wxEVT_GRID_CELL_LEFT_CLICK event handler for ID_GRID
	virtual void OnCellClick( wxGridEvent& event );

	/// wxEVT_GRID_CELL_LEFT_DCLICK event handler for ID_GRID
	virtual void OnCellDClick( wxGridEvent& event );

	/// wxEVT_GRID_CELL_CHANGED event handler for ID_GRID
	virtual void OnCellChanged( wxGridEvent& event );

////@end BreakpointsWnd_Auto event handler declarations

////@begin BreakpointsWnd_Auto member function declarations

	/// Retrieves bitmap resources
	wxBitmap GetBitmapResource( const wxString& name );

	/// Retrieves icon resources
	wxIcon GetIconResource( const wxString& name );
////@end BreakpointsWnd_Auto member function declarations

	/// Should we show tooltips?
	static bool ShowToolTips();

////@begin BreakpointsWnd_Auto member variables
	wxGrid* m_grid;
	/// Control identifiers
	enum {
		ID_BREAKPOINTSWND_AUTO = 10000,
		ID_GRID = 10002
	};
////@end BreakpointsWnd_Auto member variables
};

#endif
	// _BREAKPOINTSWND_AUTO_H_
