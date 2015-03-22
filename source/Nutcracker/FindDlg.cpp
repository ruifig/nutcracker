/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com
	
	purpose:
	
*********************************************************************/

#include "NutcrackerPCH.h"
#include "FindDlg.h"
#include "FindResultsWnd.h"
#include "MainWnd.h"


namespace nutcracker
{

FindDlg::FindDlg(wxWindow* parent, LookWhere lookwhere, wxString initialTxt)
	: FindDlg_Auto(parent)
{
	m_whereCbx->SetSelection(lookwhere);
	m_whatTxtCtrl->SetValue(initialTxt);
}

void FindDlg::OnFindClick(wxCommandEvent& event)
{
	try
	{
		gFindResultsWnd->find(m_whatTxtCtrl->GetValue(), LookWhere(m_whereCbx->GetSelection()), m_matchCaseChk->GetValue(),
							  m_matchWholeWordChk->GetValue());
		Close();
		gFindResultsWnd->Show();
		gMainWnd->showFindResultsWnd();
	}
	catch (std::exception& e)
	{
		showException(e);
	}
}

void FindDlg::OnTextCtrlEnter(wxCommandEvent& event)
{
	OnFindClick(event);
}

} // namespace nutcracker
