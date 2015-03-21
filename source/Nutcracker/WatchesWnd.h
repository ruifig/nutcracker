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

class WatchesWnd : public BaseWatchesWnd
{
public:
	WatchesWnd(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style);
	virtual ~WatchesWnd();
	
private:
	DECLARE_EVENT_TABLE()

	// BaseWatchesWnd
	virtual void OnSize(wxSizeEvent& evt) override;
	void OnShow(wxShowEvent& evt);
	virtual void OnItemActivated(wxTreeListEvent& event) override;
	virtual void OnCharHook(wxKeyEvent& event) override;
	void OnTextChar(wxKeyEvent& event);
	void OnTextKillFocus(wxFocusEvent& event);

	void updateState();
	void adjustSize(int width);

	int getSelectedWatchID(wxRect* rectDst=nullptr, int* colDst=nullptr);

	// Edit box used to add/edit watches
	wxTextCtrl* m_txtCtrl;
	int m_selectedWatchID=0;
};

} // namespace nutcracker


