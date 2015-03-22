/////////////////////////////////////////////////////////////////////////////
// Name:        FindResultsWnd_Auto.h
// Purpose:     
// Author:      Rui Figueira
// Modified by: 
// Created:     22/03/2015 16:35:35
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef _FINDRESULTSWND_AUTO_H_
#define _FINDRESULTSWND_AUTO_H_


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
#define SYMBOL_FINDRESULTSWND_AUTO_STYLE wxTAB_TRAVERSAL
#define SYMBOL_FINDRESULTSWND_AUTO_TITLE _("FindResultsWnd_Auto")
#define SYMBOL_FINDRESULTSWND_AUTO_IDNAME ID_FINDRESULTSWND_AUTO
#define SYMBOL_FINDRESULTSWND_AUTO_SIZE wxSize(400, 300)
#define SYMBOL_FINDRESULTSWND_AUTO_POSITION wxDefaultPosition
////@end control identifiers


/*!
 * FindResultsWnd_Auto class declaration
 */

class FindResultsWnd_Auto: public wxPanel
{    
	DECLARE_DYNAMIC_CLASS( FindResultsWnd_Auto )
	DECLARE_EVENT_TABLE()

public:
	/// Constructors
	FindResultsWnd_Auto();
	FindResultsWnd_Auto( wxWindow* parent, wxWindowID id = SYMBOL_FINDRESULTSWND_AUTO_IDNAME, const wxPoint& pos = SYMBOL_FINDRESULTSWND_AUTO_POSITION, const wxSize& size = SYMBOL_FINDRESULTSWND_AUTO_SIZE, long style = SYMBOL_FINDRESULTSWND_AUTO_STYLE );

	/// Creation
	bool Create( wxWindow* parent, wxWindowID id = SYMBOL_FINDRESULTSWND_AUTO_IDNAME, const wxPoint& pos = SYMBOL_FINDRESULTSWND_AUTO_POSITION, const wxSize& size = SYMBOL_FINDRESULTSWND_AUTO_SIZE, long style = SYMBOL_FINDRESULTSWND_AUTO_STYLE );

	/// Destructor
	~FindResultsWnd_Auto();

	/// Initialises member variables
	void Init();

	/// Creates the controls and sizers
	void CreateControls();

////@begin FindResultsWnd_Auto event handler declarations

	/// wxEVT_GRID_CELL_LEFT_DCLICK event handler for ID_GRID
	virtual void OnGridCellDClick( wxGridEvent& event );

////@end FindResultsWnd_Auto event handler declarations

////@begin FindResultsWnd_Auto member function declarations

	/// Retrieves bitmap resources
	wxBitmap GetBitmapResource( const wxString& name );

	/// Retrieves icon resources
	wxIcon GetIconResource( const wxString& name );
////@end FindResultsWnd_Auto member function declarations

	/// Should we show tooltips?
	static bool ShowToolTips();

////@begin FindResultsWnd_Auto member variables
	wxGrid* m_grid;
	/// Control identifiers
	enum {
		ID_FINDRESULTSWND_AUTO = 10000,
		ID_GRID = 10001
	};
////@end FindResultsWnd_Auto member variables
};

#endif
	// _FINDRESULTSWND_AUTO_H_
