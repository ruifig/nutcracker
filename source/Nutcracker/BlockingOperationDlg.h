/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com
	
	purpose:
	
*********************************************************************/

#pragma once

#include "AutoUI/BlockingOperationDlg_Auto.h"
#include "UIDefs.h"

namespace nutcracker
{

class BlockingOperationDlg : public BlockingOperationDlg_Auto
{
public:
	BlockingOperationDlg(wxWindow* parent, wxString title, wxString msg);
	bool show(std::function<bool(bool* cancelOperationFlag)> func);
private:
	virtual void OnIdle(wxIdleEvent& event) override;
	virtual void OnCancelClick(wxCommandEvent& event) override;
	std::function<bool(bool*)> m_func;
	bool m_executing = false;
	bool m_cancel = false;
};

} // namespace nutcracker
