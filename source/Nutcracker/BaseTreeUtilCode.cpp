/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com
	
	purpose:
	Code shared for the utility classes that deal with wxTreeCtrl and
	wxTreeListCtrl
*********************************************************************/

#include "NutcrackerPCH.h"
#include "BaseTreeUtilCode.h"

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

TreeItemData::TreeItemData(const TreeItemID id_) : m_id(id_)
{
}

TreeItemData::~TreeItemData()
{
}

const TreeItemID& TreeItemData::getItemId() const
{
	return m_id;
}

const std::shared_ptr<TreeItemCustomData>& TreeItemData::getCustomData()
{
	return m_customdata;
}

void TreeItemData::setCustomData(const std::shared_ptr<TreeItemCustomData>& customdata)
{
	m_customdata = customdata;
}


} // namespace TreeCtrlUtil

} // namespace nutcracker

