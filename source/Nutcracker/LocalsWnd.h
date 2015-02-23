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

namespace nutcracker
{

class LocalsWnd : public BaseWatchesWnd, public AppEventListener
{
public:
	LocalsWnd(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style);
	virtual ~LocalsWnd();
	
private:
	DECLARE_EVENT_TABLE()

	// BaseWatchesWnd
	virtual void OnSize(wxSizeEvent& evt) override;
	void OnShow(wxShowEvent& evt);

	// AppEventListener
	virtual void onAppEvent(const AppEvent& evt) override;

	void updateState();
	void adjustSize(int width);

	std::shared_ptr<const BreakInfo> m_info;
	
};

} // namespace nutcracker


