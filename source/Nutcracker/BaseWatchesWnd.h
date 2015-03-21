#pragma once

#include "wx/treelist.h"
#include "TreeListCtrlUtilCode.h"
#include "AppEvents.h"
#include "Interpreter.h"


class wxTreeListCtrl;


#define SYMBOL_BASEWATCHESWND_STYLE wxTAB_TRAVERSAL
#define SYMBOL_BASEWATCHESWND_TITLE _("BaseWatchesWnd")
#define SYMBOL_BASEWATCHESWND_IDNAME ID_BASEWATCHESWND
#define SYMBOL_BASEWATCHESWND_SIZE wxSize(400, 300)
#define SYMBOL_BASEWATCHESWND_POSITION wxDefaultPosition


namespace nutcracker
{

class BaseWatchesWnd: public wxPanel
{    
	DECLARE_DYNAMIC_CLASS( BaseWatchesWnd )
	DECLARE_EVENT_TABLE()

public:
	/// Constructors
	BaseWatchesWnd();
	BaseWatchesWnd(bool isWatches, wxWindow* parent, wxWindowID id = SYMBOL_BASEWATCHESWND_IDNAME,
				   const wxPoint& pos = SYMBOL_BASEWATCHESWND_POSITION, const wxSize& size = SYMBOL_BASEWATCHESWND_SIZE,
				   long style = SYMBOL_BASEWATCHESWND_STYLE);

	/// Creation
	bool Create(wxWindow* parent, wxWindowID id = SYMBOL_BASEWATCHESWND_IDNAME,
				const wxPoint& pos = SYMBOL_BASEWATCHESWND_POSITION, const wxSize& size = SYMBOL_BASEWATCHESWND_SIZE,
				long style = SYMBOL_BASEWATCHESWND_STYLE);

	/// Destructor
	~BaseWatchesWnd();

	/// Initialises member variables
	void Init();

	/// Creates the controls and sizers
	void CreateControls();

	virtual void OnSize( wxSizeEvent& event );
	virtual void OnItemActivated(wxTreeListEvent& event);
	virtual void OnCharHook(wxKeyEvent& event);

	/// Retrieves bitmap resources
	wxBitmap GetBitmapResource( const wxString& name );

	/// Retrieves icon resources
	wxIcon GetIconResource( const wxString& name );

	/// Should we show tooltips?
	static bool ShowToolTips();

	wxTreeListCtrl* m_tree;
	TreeCtrlUtil::TreeListCtrlData m_treeData;
	bool m_pendingUpdate = false;
	wxPoint m_dclickPos;

	virtual void updateState() = 0;

	void adjustSize(int width);
	void tryUpdateState(bool force = false);
	std::shared_ptr<TreeCtrlUtil::TreeListCtrlItemData> addEntry(
		const std::shared_ptr<TreeCtrlUtil::TreeListCtrlItemData>& parentItem, const TableEntry& entry, int& idcounter,
		int extra = 0);

	/// Control identifiers
	enum {
		ID_BASEWATCHESWND = 10000,
		ID_TREE = 10002,
		ID_TXTCTRL = 10003
	};
};


} // namespace nutcracker


