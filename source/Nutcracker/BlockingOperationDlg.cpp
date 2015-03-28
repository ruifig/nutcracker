/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com
	
	purpose:
	
*********************************************************************/

#include "NutcrackerPCH.h"
#include "BlockingOperationDlg.h"

namespace nutcracker
{

BlockingOperationDlg::BlockingOperationDlg(wxWindow* parent, wxString title, wxString msg)
	: BlockingOperationDlg_Auto(parent)
{
	SetTitle(title);
	m_txtCtrl->SetLabel(msg);
}


bool BlockingOperationDlg::show(std::function<bool(bool* cancelOperationFlag)> func)
{
	m_func = std::move(func);
	auto res = ShowModal();
	return res == wxID_OK ? true : false;
}

void BlockingOperationDlg::OnIdle(wxIdleEvent& event)
{
	if (m_executing)
		return;

	m_executing = true;
	bool res = m_func(&m_cancel);
	this->EndModal(res ? wxID_OK : wxID_CANCEL);
}

void BlockingOperationDlg::OnCancelClick(wxCommandEvent& event)
{
	m_cancel = true;
}

} // namespace nutcracker
