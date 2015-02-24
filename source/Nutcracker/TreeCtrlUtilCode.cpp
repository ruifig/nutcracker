/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com
	
	purpose:
	Miscelaneous utility code for tree controls
*********************************************************************/

#include "NutcrackerPCH.h"
#include "TreeCtrlUtilCode.h"

namespace nutcracker
{

namespace TreeCtrlUtil
{

//////////////////////////////////////////////////////////////////////////
//		TreeCtrlItemData
//////////////////////////////////////////////////////////////////////////

class TreeCtrlItemData  : public TreeItemData
{
public:
	explicit TreeCtrlItemData(wxTreeCtrl* parent, const TreeItemID id_ = TreeItemID());
	virtual ~TreeCtrlItemData();

protected:
	friend class TreeCtrlData;
	virtual void _retrieveLayout() override;
	wxTreeItemId m_wxTreeItemId;
	wxTreeCtrl* m_ctrl;
};


TreeCtrlItemData::TreeCtrlItemData(wxTreeCtrl* parent, const TreeItemID id_) : TreeItemData(id_), m_ctrl(parent)
{
}

TreeCtrlItemData::~TreeCtrlItemData()
{
}

void TreeCtrlItemData::_retrieveLayout()
{
	if ( m_ctrl->GetWindowStyleFlag()&wxTR_HIDE_ROOT &&
		 m_ctrl->GetRootItem()==m_wxTreeItemId)
		 return;

	if (m_wxTreeItemId.IsOk())
	{
		m_expanded = m_ctrl->IsExpanded(m_wxTreeItemId);
		m_selected = m_ctrl->IsSelected(m_wxTreeItemId);
		m_visible = m_ctrl->IsVisible(m_wxTreeItemId);
	}
	else
	{
		m_expanded = false;
		m_selected = false;
		m_visible = false;
	}
}

//////////////////////////////////////////////////////////////////////////
//		TreeCtrlData
//////////////////////////////////////////////////////////////////////////

TreeCtrlData::TreeCtrlData(wxFrame* parentFrame, wxTreeCtrl* ctrl) : m_frame(parentFrame), m_ctrl(ctrl)
{
}

TreeCtrlData::~TreeCtrlData()
{
	clear();
}

const std::shared_ptr<TreeItemData>& TreeCtrlData::get(const TreeItemID& id)
{
	auto& data = m_layout[id];
	if (!data)
		data = std::make_shared<TreeCtrlItemData>(m_ctrl, id);

	//
	// This little bit, allows us to keep a record of what items we touched
	// so at the end of the refresh we keep only those
	if (m_layoutprune)
		m_templayout[id] = data;

	return data;
}

std::shared_ptr<TreeItemData> TreeCtrlData::findByWxTreeItemId(const wxTreeItemId& id)
{
	auto p = m_ctrl->GetItemData(id);
	CZ_ASSERT(p);
	return static_cast<TreeItemBase*>(p)->data;
}


wxTreeCtrl* TreeCtrlData::getCtrl()
{
	return m_ctrl;
}

void TreeCtrlData::iterateItems(const std::function<void(wxTreeItemId itemid, TreeItemBase*)>& func)
{
	iterateTreeItems<TreeItemBase>(m_ctrl, m_ctrl->GetRootItem(), func);
}

void TreeCtrlData::clear()
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

void TreeCtrlData::retrieveLayout()
{
	for(auto& i : m_layout)
		i.second->_retrieveLayout();

	m_firstVisibleId = TreeItemID();
	m_firstWxVisibleId = wxTreeItemId();
	iterateItems([this](const wxTreeItemId& id, TreeItemBase* item)
	{
		if (!m_firstVisibleId.isValid() && m_ctrl->IsVisible(id))
			m_firstVisibleId = item->data->getItemId();
	});
}

void TreeCtrlData::startRefresh()
{
	m_ctrl->Freeze();
	retrieveLayout();
	m_ctrl->DeleteAllItems();
	m_ctrl->AddRoot("Root");
	m_layoutprune = true;
	m_selectedId = wxTreeItemId();
}

void TreeCtrlData::endRefresh()
{
	for(auto& f : m_later)
		f();
	m_later.clear();

	// Only reselect the previously selected item if the window has focus,
	// otherwise it will steal the focus from from whatever window we are using at the moment
	if (m_frame->HasFocus() && m_ctrl->HasFocus() && m_selectedId.IsOk())
	{
		m_ctrl->SelectItem(m_selectedId,true);
	}

	// Replace the layout with the one we just rebuilt
	m_layout = m_templayout;
	m_layoutprune = false;

	if (m_firstWxVisibleId.IsOk())
		m_ctrl->EnsureVisible(m_firstWxVisibleId);

	m_ctrl->Thaw();
}

std::shared_ptr<TreeItemData> TreeCtrlData::setupRoot(const TreeItemID& id, int image)
{
	auto rootid = m_ctrl->GetRootItem();
	auto data = std::static_pointer_cast<TreeCtrlItemData>(get(id));
	data->m_wxTreeItemId = rootid;
	m_ctrl->SetItemData(rootid, new TreeItemBase(data));
	m_ctrl->SetItemImage(rootid, image);
	return data;
}

std::shared_ptr<TreeItemData> TreeCtrlData::addItem(const std::shared_ptr<TreeItemData>& parentItem, const TreeItemID& id, const cz::UTF8String& text, int image)
{
	auto wxid = m_ctrl->AppendItem(std::static_pointer_cast<TreeCtrlItemData>(parentItem)->m_wxTreeItemId, text.widen(), image);
	auto data = std::static_pointer_cast<TreeCtrlItemData>(get(id));
	data->m_wxTreeItemId = wxid;
	m_ctrl->SetItemData(wxid, new TreeItemBase(data));
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

