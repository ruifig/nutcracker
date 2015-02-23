#pragma once

#include "wx/treelist.h"


class wxTreeListCtrl;


#define SYMBOL_BASEWATCHESWND_STYLE wxTAB_TRAVERSAL
#define SYMBOL_BASEWATCHESWND_TITLE _("BaseWatchesWnd")
#define SYMBOL_BASEWATCHESWND_IDNAME ID_BASEWATCHESWND
#define SYMBOL_BASEWATCHESWND_SIZE wxSize(400, 300)
#define SYMBOL_BASEWATCHESWND_POSITION wxDefaultPosition


class BaseWatchesWnd: public wxPanel
{    
	DECLARE_DYNAMIC_CLASS( BaseWatchesWnd )
	DECLARE_EVENT_TABLE()

public:
	/// Constructors
	BaseWatchesWnd();
	BaseWatchesWnd( wxWindow* parent, wxWindowID id = SYMBOL_BASEWATCHESWND_IDNAME, const wxPoint& pos = SYMBOL_BASEWATCHESWND_POSITION, const wxSize& size = SYMBOL_BASEWATCHESWND_SIZE, long style = SYMBOL_BASEWATCHESWND_STYLE );

	/// Creation
	bool Create( wxWindow* parent, wxWindowID id = SYMBOL_BASEWATCHESWND_IDNAME, const wxPoint& pos = SYMBOL_BASEWATCHESWND_POSITION, const wxSize& size = SYMBOL_BASEWATCHESWND_SIZE, long style = SYMBOL_BASEWATCHESWND_STYLE );

	/// Destructor
	~BaseWatchesWnd();

	/// Initialises member variables
	void Init();

	/// Creates the controls and sizers
	void CreateControls();

	virtual void OnSize( wxSizeEvent& event );

	/// Retrieves bitmap resources
	wxBitmap GetBitmapResource( const wxString& name );

	/// Retrieves icon resources
	wxIcon GetIconResource( const wxString& name );

	/// Should we show tooltips?
	static bool ShowToolTips();

	wxTreeListCtrl* m_tree;

	/// Control identifiers
	enum {
		ID_BASEWATCHESWND = 10000,
		ID_TREE = 10002
	};
};



