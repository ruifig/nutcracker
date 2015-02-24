/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com
	
	purpose:
	
*********************************************************************/

#include "NutcrackerPCH.h"
#include "LocalsWnd.h"
#include "UIDefs.h"
#include "MainWnd.h"

namespace nutcracker
{

BEGIN_EVENT_TABLE(LocalsWnd, BaseWatchesWnd)
	EVT_SHOW(LocalsWnd::OnShow)
END_EVENT_TABLE()

LocalsWnd::LocalsWnd(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
	: BaseWatchesWnd(parent,id,pos,size,style)
{

	wxWindow* wnd = parent;
	while (!wnd->IsKindOf(wxCLASSINFO(wxFrame)))
		wnd = wnd->GetParent();
	m_treeData = TreeCtrlUtil::TreeListCtrlData(static_cast<wxFrame*>(wnd), m_tree);
	m_tree->GetDataView()->SetIndent(16);
	m_tree->GetDataView()->SetWindowStyleFlag(wxDV_ROW_LINES | wxDV_HORIZ_RULES | wxDV_VERT_RULES);
	m_tree->AppendColumn("Name");
	m_tree->AppendColumn("Value");
	m_tree->AppendColumn("Type");
}

LocalsWnd::~LocalsWnd()
{
}

void LocalsWnd::onAppEvent(const AppEvent& evt)
{
	switch (evt.id)
	{
		case AppEventID::DebugStarted:
			gUIState->debugSession->breakListeners.add(this,
				[&](const std::shared_ptr<const BreakInfo>& info)
			{
				m_info = info;
				if (IsShownOnScreen())
					updateState();
			});
			break;
		case AppEventID::CallstackFrameChanged:
			if (IsShownOnScreen())
				updateState();
			break;
	}
}

void LocalsWnd::addVar(const std::shared_ptr<TreeCtrlUtil::TreeListCtrlItemData>& parentItem, const TableEntry& entry, int& idcounter)
{
	auto itemRoot = m_treeData.addItem(parentItem, TreeCtrlUtil::TreeItemID(idcounter++), entry.getName().c_str());
	m_tree->SetItemText(itemRoot->getWxItem(), 1, entry.getValue().c_str());
	m_tree->SetItemText(itemRoot->getWxItem(), 2, entry.getType().c_str());
	if (entry.val->isTable())
	{
		auto tbl = static_cast<ValueBaseTable*>(entry.val.get());
		for (auto& v : tbl->entries)
		{
			addVar(itemRoot, v, idcounter);
		}
	}
}

void LocalsWnd::updateState()
{
	m_treeData.clear();
	m_treeData.startRefresh();

	if (!m_info)
	{
		m_treeData.endRefresh();
		return;
	}

	int idcounter = 0;
	auto root = m_treeData.setupRoot(TreeCtrlUtil::TreeItemID(idcounter++));
	const auto& vars = m_info->callstack[m_info->currentCallstackFrame].vars;

	for (auto& v : vars)
		addVar(root, v, idcounter);

	adjustSize(this->GetSize().GetX());

	m_treeData.endRefresh();
}

void LocalsWnd::adjustSize(int width)
{
	/*
	int col1Size = m_grid->GetColSize(0);
	int remaining = width - col1Size;
	m_grid->SetColSize(1,std::max(m_infoColMinSize, remaining));
	m_grid->SetMargins(0 - wxSYS_VSCROLL_X, 0);
	*/
}


void LocalsWnd::OnShow(wxShowEvent& evt)
{
	if (evt.IsShown())
		updateState();

	evt.Skip();
}

void LocalsWnd::OnSize(wxSizeEvent& evt)
{
	auto size = evt.GetSize();
	adjustSize(size.GetX());
	evt.Skip();
}

} // namespace nutcracker






