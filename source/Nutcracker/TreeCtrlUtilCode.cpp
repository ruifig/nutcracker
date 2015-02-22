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
//		TreeItemID
//////////////////////////////////////////////////////////////////////////


TreeItemID::TreeItemID(cz::u32 type_, cz::u32 id0, cz::u32 id1, cz::u32 id2, cz::u32 id3)
		: itemtype(type_)
{
	id[0] = id0;
	id[1] = id1;
	id[2] = id2;
	id[3] = id3;
}

bool TreeItemID::operator<(const TreeItemID& other) const
{
	if (itemtype<other.itemtype)
		return true;
	if (other.itemtype<itemtype)
		return false;

	for(int i=0; i<3; i++)
	{
		if (id[i]<other.id[i])
			return true;
		if (other.id[i]<id[i])
			return false;
	}

	if (id[3]<other.id[3])
		return true;
	return false;
}

bool TreeItemID::isValid() const
{
	if (itemtype!=0)
		return true;

	for(auto &i : id)
		if (i!=0)
			return true;

	return false;
}

bool TreeItemID::operator==(const TreeItemID& other) const
{
	return memcmp(this, &other, sizeof(*this))==0;
}

//////////////////////////////////////////////////////////////////////////
//		TreeItemData
//////////////////////////////////////////////////////////////////////////

TreeItemData::TreeItemData(wxTreeCtrl* parent, const TreeItemID id_) : m_ctrl(parent), m_id(id_)
{
}

TreeItemData::~TreeItemData()
{
}

const wxTreeItemId& TreeItemData::getWxTreeItemId()
{
	return m_wxTreeItemId;
}

const TreeItemID& TreeItemData::getItemId() const
{
	return m_id;
}

void TreeItemData::retrieveLayout()
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

const std::shared_ptr<TreeItemCustomData>& TreeItemData::getCustomData()
{
	return m_customdata;
}

void TreeItemData::setCustomData(const std::shared_ptr<TreeItemCustomData>& customdata)
{
	m_customdata = customdata;
}

void TreeItemData::setImage(int image, wxTreeItemIcon which)
{
	m_ctrl->SetItemImage(m_wxTreeItemId, image, which);
}

void TreeItemData::expand()
{
	m_ctrl->Expand(m_wxTreeItemId);
}

void TreeItemData::setName(const wxString& name)
{
	m_ctrl->SetItemText(m_wxTreeItemId, name);
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
		data = std::make_shared<TreeItemData>(m_ctrl, id);

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
		i.second->retrieveLayout();

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



	/*
	m_ctrl->GetScrollHelper()->GetScrollPixelsPerUnit(&m_scrollpos.xpixunit, &m_scrollpos.ypixunit);
	m_ctrl->GetScrollHelper()->GetViewStart(&m_scrollpos.xview, &m_scrollpos.yview);
	m_ctrl->GetSize(&m_scrollpos.xsize, &m_scrollpos.ysize);
	m_ctrl->GetVirtualSize(&m_scrollpos.vxsize, &m_scrollpos.vysize);

	m_scrollpos.thumbsize = (int)(m_scrollpos.ysize / m_scrollpos.ypixunit);
	m_scrollpos.totalsize = (int)(m_scrollpos.vysize / m_scrollpos.ypixunit);
	m_scrollpos.maxy = m_scrollpos.totalsize - m_scrollpos.thumbsize;

	m_scrollpos.newy = m_scrollpos.yview;

	if (m_scrollpos.newy<0) m_scrollpos.newy = 0;
	else if (m_scrollpos.newy>m_scrollpos.maxy) m_scrollpos.newy = m_scrollpos.maxy;
	*/

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

	//m_ctrl->GetScrollHelper()->SetScrollbars(m_scrollpos.xpixunit, m_scrollpos.ypixunit, (m_scrollpos.vxsize / m_scrollpos.xpixunit), (m_scrollpos.vysize / m_scrollpos.ypixunit), m_scrollpos.xview, m_scrollpos.newy);
	if (m_firstWxVisibleId.IsOk())
		m_ctrl->EnsureVisible(m_firstWxVisibleId);

	m_ctrl->Thaw();
}

std::shared_ptr<TreeItemData> TreeCtrlData::setupRoot(const TreeItemID& id, int image)
{
	auto rootid = m_ctrl->GetRootItem();
	auto data = get(id);
	data->m_wxTreeItemId = rootid;
	m_ctrl->SetItemData(rootid, new TreeItemBase(data));
	m_ctrl->SetItemImage(rootid, image);
	return data;
}

std::shared_ptr<TreeItemData> TreeCtrlData::addItem(const std::shared_ptr<TreeItemData>& parentItem, const TreeItemID& id, const cz::UTF8String& text, int image)
{
	auto wxid = m_ctrl->AppendItem(parentItem->m_wxTreeItemId, text.widen(), image);
	auto data = get(id);
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

