/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com
	
	purpose:
	
*********************************************************************/
#pragma once

#include "BaseWatchesWnd.h"

namespace nutcracker
{

class LocalsWnd : public BaseWatchesWnd
{
public:
	LocalsWnd(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style);
	virtual ~LocalsWnd();
	
private:
	DECLARE_EVENT_TABLE()

	// BaseWatchesWnd
	virtual void OnSize(wxSizeEvent& evt) override;
	void OnShow(wxShowEvent& evt);

	virtual void updateState() override;
};

} // namespace nutcracker


