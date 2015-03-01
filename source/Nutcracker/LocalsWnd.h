/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com
	
	purpose:
	
*********************************************************************/
#pragma once

#include "BaseWatchesWnd.h"
#include "AppEvents.h"
#include "Interpreter.h"
#include "TreeListCtrlUtilCode.h"

namespace nutcracker
{

class LocalsWnd : public BaseWatchesWnd
{
public:
	LocalsWnd(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style);
	virtual ~LocalsWnd();
	
private:
	DECLARE_EVENT_TABLE()

	// BaseWatchesWnd
	virtual void OnSize(wxSizeEvent& evt) override;
	void OnShow(wxShowEvent& evt);

	void tryUpdateState(bool force = false);
	void updateState();
	void adjustSize(int width);
	void LocalsWnd::addVar(const std::shared_ptr<TreeCtrlUtil::TreeListCtrlItemData>& parentItem, const TableEntry& entry, int& idcounter);

	TreeCtrlUtil::TreeListCtrlData m_treeData;
	bool m_pendingUpdate = false;
};

} // namespace nutcracker


