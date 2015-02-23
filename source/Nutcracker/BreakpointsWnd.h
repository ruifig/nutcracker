/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com
	
	purpose:
	
*********************************************************************/
#pragma once

#include "AutoUI/BreakpointsWnd_Auto.h"

namespace nutcracker
{

class BreakpointsWnd : public BreakpointsWnd_Auto, public AppEventListener
{
public:
	BreakpointsWnd(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style);
	virtual ~BreakpointsWnd();

	void updateState();
private:

	DECLARE_EVENT_TABLE()
	virtual void OnCellChanged( wxGridEvent& evt ) override;
	virtual void OnToggleBreakpointsClick( wxCommandEvent& event) override;
	virtual void OnCellClick( wxGridEvent& evt ) override;
	virtual void OnCellDClick( wxGridEvent& evt ) override;
	void OnShow(wxShowEvent& evt);

	virtual void onAppEvent(const AppEvent& evt) override;
};

} // namespace nutcracker

