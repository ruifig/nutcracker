/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com
	
	purpose:
	Miscelaneous utility code for tree controls
*********************************************************************/

#pragma once

#include "BaseTreeUtilCode.h"

namespace nutcracker
{

namespace TreeCtrlUtil
{

template <class TreeItemType>
void iterateTreeItems(wxTreeListCtrl* ctrl, wxTreeListItem start,
					  const std::function<void(wxTreeListItem item, TreeItemType*)>& func)
{
	if (!start.IsOk())
		return;
	auto item = ctrl->GetFirstChild(start);
	while(item.IsOk())
	{
		auto data = ctrl->GetItemData(item);
		func(item, data ? static_cast<TreeItemType*>(data) : nullptr);

		iterateTreeItems<TreeItemType>(ctrl, item, func);

		item = ctrl->GetNextSibling(item);
	}
}

struct TreeListItemBase : public wxClientData
{
	TreeListItemBase() { }
	explicit TreeListItemBase(std::shared_ptr<TreeItemData> data_) : data(data_) {}
	std::shared_ptr<TreeItemData> data;
};

class TreeListCtrlItemData  : public TreeItemData
{
public:
	explicit TreeListCtrlItemData(wxTreeListCtrl* parent, const TreeItemID id_ = TreeItemID());
	virtual ~TreeListCtrlItemData();

	const wxTreeListItem& getWxItem()
	{
		return m_wxTreeListItem;
	}

protected:
	friend class TreeListCtrlData;
	virtual void _retrieveLayout() override;
	wxTreeListItem m_wxTreeListItem;
	wxTreeListCtrl* m_ctrl;
};

/**
 * Puts everything together
 */
class TreeListCtrlData
{
public:
	TreeListCtrlData() {}
	explicit TreeListCtrlData(wxFrame* parentFrame, wxTreeListCtrl* ctrl);
	TreeListCtrlData(const TreeListCtrlData& other) = default;
	virtual ~TreeListCtrlData();
	const std::shared_ptr<TreeItemData>& get(const TreeItemID& id);

	template<class T>
	std::shared_ptr<T> getItemCustomData(const TreeItemID& id)
	{
		auto item = get(id);
		if (!item || !item->getCustomData())
			return nullptr;
		return std::static_pointer_cast<T>(item->getCustomData());
	}

	std::shared_ptr<TreeItemData> findByWxTreeListItem(const wxTreeListItem& id);
	wxTreeListCtrl* getCtrl();

	void iterateItems(const std::function<void(wxTreeListItem itemid, TreeListItemBase*)>& func);
	void clear();

	void startRefresh();
	std::shared_ptr<TreeListCtrlItemData> setupRoot(const TreeItemID& id);
	std::shared_ptr<TreeListCtrlItemData> addItem(const std::shared_ptr<TreeItemData>& parentItem, const TreeItemID& id, const cz::UTF8String& text);
	void endRefresh();
protected:
	void retrieveLayout();

	std::map<TreeItemID, std::shared_ptr<TreeItemData>> m_layout;
	std::map<TreeItemID, std::shared_ptr<TreeItemData>> m_templayout;
	bool m_layoutprune = false;
	wxFrame* m_frame = nullptr;
	wxTreeListCtrl* m_ctrl = nullptr;
	// updated when refreshing the tree
	wxTreeListItem m_selectedId;
	// using a TreeItemID instead of wxTreeItemID, because once we delate all the tree items,
	// wxTreeItemId values are invalidated
	TreeItemID m_firstVisibleId; 
	wxTreeListItem m_firstWxVisibleId;
	// used to collect code to run we finish refreshing the tree
	std::vector<std::function<void()>> m_later;
};


} // namespace TreeCtrlUtil

} // namespace nutcracker


