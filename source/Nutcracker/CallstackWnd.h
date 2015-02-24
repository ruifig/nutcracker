/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com
	
	purpose:
	
*********************************************************************/
#pragma once

#include "AutoUI/CallstackWnd_Auto.h"
#include "Interpreter.h"
#include "AppEvents.h"

namespace nutcracker
{

class CallstackWnd : public CallstackWnd_Auto, public AppEventListener
{
public:
	CallstackWnd(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style);
	virtual ~CallstackWnd();
	
private:
	DECLARE_EVENT_TABLE()

	// CallstackWnd_Auto
	virtual void OnCellChanged( wxGridEvent& evt ) override;
	virtual void OnCellClick( wxGridEvent& evt ) override;
	virtual void OnCellDClick( wxGridEvent& evt ) override;
	virtual void OnSize(wxSizeEvent& evt) override;
	void OnShow(wxShowEvent& evt);

	// AppEventListener
	virtual void onAppEvent(const AppEvent& evt) override;

	void updateState();
	void adjustSize(int width);

	std::shared_ptr<BreakInfo> m_info;
	int m_infoColMinSize = 0;
};

} // namespace nutcracker


