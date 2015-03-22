/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com
	
	purpose:
	
*********************************************************************/

#pragma once

#include "AutoUI/FindDlg_Auto.h"
#include "Workspace.h"
#include "UIDefs.h"

namespace nutcracker
{

enum LookWhere
{
	kLook_Workspace,
	kLook_CurrentFile
};

class FindDlg : public FindDlg_Auto
{
public:
	FindDlg(wxWindow* parent, LookWhere lookwhere, wxString initialTxt="");
private:
	virtual void OnFindClick( wxCommandEvent& event ) override;
	virtual void OnTextCtrlEnter(wxCommandEvent& event) override;
};

} // namespace nutcracker
