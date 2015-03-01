/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com
	
	purpose:
	
*********************************************************************/
#pragma once

#include "AutoUI/CallstackWnd_Auto.h"
#include "AppEvents.h"

namespace nutcracker
{

struct BreakInfo;

class CallstackWnd : public CallstackWnd_Auto
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

	void updateState();
	void adjustSize(int width);
	void updateFrameMarker(int previous, int current);

	int m_infoColMinSize = 0;
	bool m_pendingUpdate = false;
	int m_currentIndex = 0;
};

} // namespace nutcracker


