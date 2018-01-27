#include "NutcrackerPCH.h"
#include "BaseWatchesWnd.h"
#include "Workspace.h"

namespace nutcracker
{

/*
Control ids
*/
/*
enum
{
	ID_ITEM_ACTIVATED = wxID_HIGHEST+1
};
*/

IMPLEMENT_ABSTRACT_CLASS( BaseWatchesWnd, wxPanel )

BEGIN_EVENT_TABLE( BaseWatchesWnd, wxPanel )
	EVT_SIZE( BaseWatchesWnd::OnSize )
	EVT_TREELIST_ITEM_ACTIVATED(wxID_ANY, BaseWatchesWnd::OnItemActivated)
	EVT_CHAR(BaseWatchesWnd::OnCharHook)
END_EVENT_TABLE()


BaseWatchesWnd::BaseWatchesWnd()
{
	Init();
}

BaseWatchesWnd::BaseWatchesWnd(bool isWatches, wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
{
	Init();
	Create(parent, id, pos, size, style);

	wxWindow* wnd = parent;
	while (!wnd->IsKindOf(wxCLASSINFO(wxFrame)))
		wnd = wnd->GetParent();
	m_treeData = TreeCtrlUtil::TreeListCtrlData(static_cast<wxFrame*>(wnd), m_tree);
	m_tree->GetDataView()->SetIndent(16);
	m_tree->GetDataView()->SetWindowStyleFlag(wxDV_ROW_LINES | wxDV_HORIZ_RULES | wxDV_VERT_RULES);
	m_tree->AppendColumn(isWatches ? "Expression" :  "Name");
	m_tree->AppendColumn("Value");
	m_tree->AppendColumn("Type");

	gWorkspace->registerListener(this, [this, isWatches](const DataEvent& evt)
	 {
		if (evt.isBreakpointEvent() || evt.id == DataEventID::DebugStart || evt.id == DataEventID::DebugStop ||
			evt.id == DataEventID::DebugBreak || evt.id == DataEventID::DebugChangedCallstackFrame || evt.id==DataEventID::DebugResume ||
			(isWatches && evt.id==DataEventID::WatchesChanged))
		{
			tryUpdateState();
		}
	});

	Connect(wxEVT_CHAR_HOOK, wxKeyEventHandler(BaseWatchesWnd::OnCharHook));
}


bool BaseWatchesWnd::Create( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
{
	wxPanel::Create( parent, id, pos, size, style );

	CreateControls();
	if (GetSizer())
	{
		GetSizer()->SetSizeHints(this);
	}
	Centre();
	return true;
}


BaseWatchesWnd::~BaseWatchesWnd()
{
}


void BaseWatchesWnd::Init()
{
	m_tree = NULL;
}


void BaseWatchesWnd::CreateControls()
{    
	BaseWatchesWnd* itemPanel1 = this;

	wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
	itemPanel1->SetSizer(itemBoxSizer2);

	m_tree = new wxTreeListCtrl(itemPanel1, ID_TREE, wxDefaultPosition, wxSize(200, 150), wxTR_EDIT_LABELS);
	itemBoxSizer2->Add(m_tree, 1, wxGROW|wxALL, 0);
}


/*
 * Should we show tooltips?
 */
bool BaseWatchesWnd::ShowToolTips()
{
	return true;
}

/*
 * Get bitmap resources
 */
wxBitmap BaseWatchesWnd::GetBitmapResource( const wxString& name )
{
	wxUnusedVar(name);
	return wxNullBitmap;
}

/*
 * Get icon resources
 */
wxIcon BaseWatchesWnd::GetIconResource( const wxString& name )
{
	wxUnusedVar(name);
	return wxNullIcon;
}

void BaseWatchesWnd::OnSize( wxSizeEvent& event )
{
	event.Skip();
}

void BaseWatchesWnd::OnItemActivated(wxTreeListEvent& event)
{
}

void BaseWatchesWnd::OnCharHook(wxKeyEvent& event)
{
}

void BaseWatchesWnd::adjustSize(int width)
{
	/*
	int col1Size = m_grid->GetColSize(0);
	int remaining = width - col1Size;
	m_grid->SetColSize(1,std::max(m_infoColMinSize, remaining));
	m_grid->SetMargins(0 - wxSYS_VSCROLL_X, 0);
	*/
}

void BaseWatchesWnd::tryUpdateState(bool force)
{
	if (m_pendingUpdate && !force)
		return;

	m_pendingUpdate = true;
	postAppLambdaEvent([this, force]()
	{
		if (IsShownOnScreen() || force)
			updateState();
		m_pendingUpdate = false;
	});
}

std::shared_ptr<TreeCtrlUtil::TreeListCtrlItemData> BaseWatchesWnd::addEntry(
	const std::shared_ptr<TreeCtrlUtil::TreeListCtrlItemData>& parentItem, const TableEntry& entry, int& idcounter,
	int extra)
{
	auto itemRoot = m_treeData.addItem(parentItem, TreeCtrlUtil::TreeItemID(idcounter++, extra), entry.getName().c_str());

	if (entry.val)
	{
		m_tree->SetItemText(itemRoot->getWxItem(), 1, entry.getValue().c_str());
		m_tree->SetItemText(itemRoot->getWxItem(), 2, entry.getType().c_str());
		if (entry.val->isTable())
		{
			auto tbl = static_cast<ValueBaseTable*>(entry.val.get());
			for (auto& v : tbl->entries)
			{
				addEntry(itemRoot, v, idcounter);
			}
		}
	}
	else
	{
		m_tree->SetItemText(itemRoot->getWxItem(), 1, "INVALID");
		m_tree->SetItemText(itemRoot->getWxItem(), 2, "INVALID");
	}

	return itemRoot;
}

} // namespace nutcracker


