/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com
	
	purpose:
	
*********************************************************************/
#pragma once

#include "AutoUI/FindFileDlg_Auto.h"
#include "Workspace.h"
#include "UIDefs.h"

namespace nutcracker
{

class FindFileDlg : public FindFileDlg_Auto
{
public:
	FindFileDlg(wxWindow* parent);
	std::shared_ptr<const File> getResult();
private:
	DECLARE_EVENT_TABLE()
	void updateGrid();
	virtual void OnTextctrlTextUpdated(wxCommandEvent& event) override;
	virtual void OnGridCellDClick(wxGridEvent& event) override;
	void OnTimer(wxTimerEvent& event);
	void OnCharHook(wxKeyEvent& event);
	void doPick(int row);

	std::vector<std::pair<int, std::shared_ptr<const File>>> m_files;
	std::shared_ptr<const File> m_picked;
	wxTimer m_timer;
	bool m_needsUpdate = false;
};

} // namespace nutcracker



