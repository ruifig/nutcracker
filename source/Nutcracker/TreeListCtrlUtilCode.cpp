/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com
	
	purpose:
	Miscelaneous utility code for tree controls
*********************************************************************/

#include "NutcrackerPCH.h"
#include "TreeListCtrlUtilCode.h"

namespace nutcracker
{

namespace TreeCtrlUtil
{

//////////////////////////////////////////////////////////////////////////
//		TreeListCtrlItemData
//////////////////////////////////////////////////////////////////////////


TreeListCtrlItemData::TreeListCtrlItemData(wxTreeListCtrl* parent, const TreeItemID id_) : TreeItemData(id_), m_ctrl(parent)
{
}

TreeListCtrlItemData::~TreeListCtrlItemData()
{
}

void TreeListCtrlItemData::_retrieveLayout()
{
	if (m_ctrl->GetRootItem()==m_wxTreeListItem)
		 return;

	if (m_wxTreeListItem.IsOk())
	{
		m_expanded = m_ctrl->IsExpanded(m_wxTreeListItem);
		m_selected = m_ctrl->IsSelected(m_wxTreeListItem);
		//m_visible = m_ctrl->IsVisible(m_wxTreeListItem);
	}
	else
	{
		m_expanded = false;
		m_selected = false;
		//m_visible = false;
	}
}

//////////////////////////////////////////////////////////////////////////
//		TreeListCtrlData
//////////////////////////////////////////////////////////////////////////

TreeListCtrlData::TreeListCtrlData(wxFrame* parentFrame, wxTreeListCtrl* ctrl) : m_frame(parentFrame), m_ctrl(ctrl)
{
}

TreeListCtrlData::~TreeListCtrlData()
{
	clear();
}

const std::shared_ptr<TreeItemData>& TreeListCtrlData::get(const TreeItemID& id)
{
	auto& data = m_layout[id];
	if (!data)
		data = std::make_shared<TreeListCtrlItemData>(m_ctrl, id);

	//
	// This little bit, allows us to keep a record of what items we touched
	// so at the end of the refresh we keep only those
	if (m_layoutprune)
		m_templayout[id] = data;

	return data;
}

std::shared_ptr<TreeItemData> TreeListCtrlData::findByWxTreeListItem(const wxTreeListItem& id)
{
	auto p = m_ctrl->GetItemData(id);
	CZ_ASSERT(p);
	return static_cast<TreeListItemBase*>(p)->data;
}


wxTreeListCtrl* TreeListCtrlData::getCtrl()
{
	return m_ctrl;
}

void TreeListCtrlData::iterateItems(const std::function<void(wxTreeListItem itemid, TreeListItemBase*)>& func)
{
	iterateTreeItems<TreeListItemBase>(m_ctrl, m_ctrl->GetRootItem(), func);
}

void TreeListCtrlData::clear()
{
	if (m_ctrl)
	{
		m_ctrl->Freeze();
		m_ctrl->DeleteAllItems();
		m_ctrl->Thaw();
	}

	m_layout.clear();
	m_templayout.clear();
}

void TreeListCtrlData::retrieveLayout()
{
	for(auto& i : m_layout)
		i.second->_retrieveLayout();

	m_firstVisibleId = TreeItemID();
	m_firstWxVisibleId = wxTreeListItem();

	m_firstWxVisibleId = m_ctrl->GetSelection();
	/*
	iterateItems([this](const wxTreeItemId& id, TreeListItemBase* item)
	{
		if (!m_firstVisibleId.isValid() && m_ctrl->IsVisible(id))
			m_firstVisibleId = item->data->getItemId();
	});
	*/
}

void TreeListCtrlData::startRefresh()
{
	m_ctrl->Freeze();
	retrieveLayout();
	m_ctrl->DeleteAllItems();
	m_layoutprune = true;
	m_selectedId = wxTreeListItem();
}

void TreeListCtrlData::endRefresh()
{
	for(auto& f : m_later)
		f();
	m_later.clear();

	// Only reselect the previously selected item if the window has focus,
	// otherwise it will steal the focus from from whatever window we are using at the moment
	if (m_frame->HasFocus() && m_ctrl->HasFocus() && m_selectedId.IsOk())
	{
		m_ctrl->Select(m_selectedId);
	}

	// Replace the layout with the one we just rebuilt
	m_layout = m_templayout;
	m_layoutprune = false;

	/*
	if (m_firstWxVisibleId.IsOk())
		m_ctrl->EnsureVisible(m_firstWxVisibleId);
		*/

	m_ctrl->Thaw();
}

std::shared_ptr<TreeListCtrlItemData> TreeListCtrlData::setupRoot(const TreeItemID& id)
{
	auto rootid = m_ctrl->GetRootItem();
	auto data = std::static_pointer_cast<TreeListCtrlItemData>(get(id));
	data->m_wxTreeListItem = rootid;
	m_ctrl->SetItemData(rootid, new TreeListItemBase(data));
	return data;
}

std::shared_ptr<TreeListCtrlItemData> TreeListCtrlData::addItem(const std::shared_ptr<TreeItemData>& parentItem,
														const TreeItemID& id, const cz::UTF8String& text)
{
	auto wxid =
		m_ctrl->AppendItem(std::static_pointer_cast<TreeListCtrlItemData>(parentItem)->m_wxTreeListItem, text.widen());
	auto data = std::static_pointer_cast<TreeListCtrlItemData>(get(id));
	data->m_wxTreeListItem = wxid;
	m_ctrl->SetItemData(wxid, new TreeListItemBase(data));
	if (data->m_expanded)
		m_later.push_back( [=]() { m_ctrl->Expand(wxid); });
	if (data->m_selected)
		m_selectedId = wxid;
	if (data->m_id==m_firstVisibleId)
		m_firstWxVisibleId = wxid;
	return data;
}

} // namespace TreeCtrlUtil

} // namespace nutcracker

