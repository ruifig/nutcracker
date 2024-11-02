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
#include "Workspace.h"

namespace nutcracker
{

BEGIN_EVENT_TABLE(LocalsWnd, BaseWatchesWnd)
	EVT_SHOW(LocalsWnd::OnShow)
END_EVENT_TABLE()

LocalsWnd::LocalsWnd(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
	: BaseWatchesWnd(false, parent,id,pos,size,style)
{
}

LocalsWnd::~LocalsWnd()
{
}

void LocalsWnd::updateState()
{
	m_treeData.clear();
	m_treeData.startRefresh();

	auto info = gWorkspace->debuggerGetBreakInfo();
	if (!info)
	{
		m_treeData.endRefresh();
		return;
	}

	int idcounter = 0;
	auto root = m_treeData.setupRoot(TreeCtrlUtil::TreeItemID(idcounter++));
	const auto& locals = info->callstack[info->currentCallstackFrame].locals;

	for (auto& v : locals)
	{
		addEntry(root, v, idcounter);
	}

	adjustSize(this->GetSize().GetX());

	m_treeData.endRefresh();
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






