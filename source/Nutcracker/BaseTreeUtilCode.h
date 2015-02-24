/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com
	
	purpose:
	Code shared for the utility classes that deal with wxTreeCtrl and
	wxTreeListCtrl
*********************************************************************/

#pragma once

namespace nutcracker
{

namespace TreeCtrlUtil
{

/*
Uniquely identifies a tree item.
It's up to the user to set the type and id values any way necessary according to the code using this
*/
struct TreeItemID
{
	TreeItemID(cz::u32 type_=0, cz::u32 id0 = 0, cz::u32 id1 = 0, cz::u32 id2 = 0, cz::u32 id3=0);
	cz::u32 itemtype;
	cz::u32 id[4];

	bool isValid() const;
	/**
	 * Comparison operator so it's possible to use it as a map key
	 */
	bool operator<(const TreeItemID& other) const;
	bool operator==(const TreeItemID& other) const;
};

/**
 * User code should inherit from this class to add custom data to the tree
 */
class TreeItemCustomData
{
public:
	TreeItemCustomData() { }
	virtual ~TreeItemCustomData() { }
};

/**
* Tree item data
* This is saved outside the TreeItem itself, so we can save the layout, for when we refresh the tree control
*/
class TreeItemData
{
public:
	explicit TreeItemData(const TreeItemID id_ = TreeItemID());
	virtual ~TreeItemData();

	const TreeItemID& getItemId() const;
	const std::shared_ptr<TreeItemCustomData>& getCustomData();
	void setCustomData(const std::shared_ptr<TreeItemCustomData>& customdata);

	// For internal use
	virtual void _retrieveLayout() = 0;
protected:
	friend class TreeCtrlData;

	TreeItemID m_id;
	bool m_expanded = false;
	bool m_selected = false;
	bool m_visible = false;
	std::shared_ptr<TreeItemCustomData> m_customdata;
};


} // namespace TreeCtrlUtil

} // namespace nutcracker


