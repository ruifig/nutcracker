/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com
	
	purpose:
	
*********************************************************************/
#pragma once

#include "BaseWatchesWnd.h"
#include "AppEvents.h"

namespace nutcracker
{

class WatchesWnd : public BaseWatchesWnd, public AppEventListener
{
public:
	WatchesWnd(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style);
	virtual ~WatchesWnd();
	
private:
	DECLARE_EVENT_TABLE()

	// BaseWatchesWnd
	virtual void OnSize(wxSizeEvent& evt) override;
	void OnShow(wxShowEvent& evt);

	// AppEventListener
	virtual void onAppEvent(const AppEvent& evt) override;

	void updateState();
	void adjustSize(int width);
};

} // namespace nutcracker


