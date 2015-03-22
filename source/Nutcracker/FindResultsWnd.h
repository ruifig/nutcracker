/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com
	
	purpose:
	
*********************************************************************/

#pragma once

#include "AutoUI/FindResultsWnd_Auto.h"
#include "UIDefs.h"
#include "FindDlg.h"

namespace nutcracker
{

class FindResultsWnd : public FindResultsWnd_Auto
{
public:
	FindResultsWnd(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style);
	void find(const wxString& what, LookWhere lookWhere, bool matchCase, bool matchWholeWorld);

private:
	virtual void OnGridCellDClick( wxGridEvent& event ) override;
	void tryUpdateState(bool force = false);
	void updateState();
	bool m_pendingUpdate = false;
};

} // namespace nutcracker

