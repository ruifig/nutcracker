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

template<class TreeItemType>
void iterateTreeItems(wxTreeCtrl* ctrl, wxTreeItemId start, const std::function<void(wxTreeItemId itemid, TreeItemType*)>& func)
{
	if (!start.IsOk())
		return;
	wxTreeItemIdValue cookie;
	auto itemid = ctrl->GetFirstChild(start, cookie);
	while(itemid.IsOk())
	{
		auto data = ctrl->GetItemData(itemid);
		func(itemid, data ? static_cast<TreeItemType*>(data) : nullptr);

		iterateTreeItems<TreeItemType>(ctrl, itemid, func);

		itemid = ctrl->GetNextChild(start, cookie);
	}
}

struct TreeItemBase : public wxTreeItemData
{
	TreeItemBase() { }
	explicit TreeItemBase(std::shared_ptr<TreeItemData> data_) : data(data_) {}
	std::shared_ptr<TreeItemData> data;
};


/**
 * Puts everything together
 */
class TreeCtrlData
{
public:
	TreeCtrlData() {}
	explicit TreeCtrlData(wxFrame* parentFrame, wxTreeCtrl* ctrl);
	TreeCtrlData(const TreeCtrlData& other) = default;
	virtual ~TreeCtrlData();
	const std::shared_ptr<TreeItemData>& get(const TreeItemID& id);

	template<class T>
	std::shared_ptr<T> getItemCustomData(const TreeItemID& id)
	{
		auto item = get(id);
		if (!item || !item->getCustomData())
			return nullptr;
		return std::static_pointer_cast<T>(item->getCustomData());
	}

	std::shared_ptr<TreeItemData> findByWxTreeItemId(const wxTreeItemId& id);
	wxTreeCtrl* getCtrl();

	void iterateItems(const std::function<void(wxTreeItemId itemid, TreeItemBase*)>& func);
	void clear();

	void startRefresh();
	std::shared_ptr<TreeItemData> setupRoot(const TreeItemID& id, int image);
	std::shared_ptr<TreeItemData> addItem(const std::shared_ptr<TreeItemData>& parentItem, const TreeItemID& id, const cz::UTF8String& text, int image);
	void endRefresh();
protected:
	void retrieveLayout();

	std::map<TreeItemID, std::shared_ptr<TreeItemData>> m_layout;
	std::map<TreeItemID, std::shared_ptr<TreeItemData>> m_templayout;
	bool m_layoutprune = false;
	wxFrame* m_frame = nullptr;
	wxTreeCtrl* m_ctrl = nullptr;
	// updated when refreshing the tree
	wxTreeItemId m_selectedId;
	// using a TreeItemID instead of wxTreeItemID, because once we delate all the tree items,
	// wxTreeItemId values are invalidated
	TreeItemID m_firstVisibleId; 
	wxTreeItemId m_firstWxVisibleId;
	// used to collect code to run we finish refreshing the tree
	std::vector<std::function<void()>> m_later;
};


} // namespace TreeCtrlUtil

} // namespace nutcracker


