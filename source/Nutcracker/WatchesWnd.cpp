/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com
	
	purpose:
	
*********************************************************************/

#include "NutcrackerPCH.h"
#include "WatchesWnd.h"

namespace nutcracker
{

BEGIN_EVENT_TABLE(WatchesWnd, BaseWatchesWnd)
	EVT_SHOW(WatchesWnd::OnShow)
END_EVENT_TABLE()

WatchesWnd::WatchesWnd(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
	: BaseWatchesWnd(parent,id,pos,size,style)
{
}

WatchesWnd::~WatchesWnd()
{
}

void WatchesWnd::onAppEvent(const AppEvent& evt)
{
}

void WatchesWnd::updateState()
{
	m_tree->Freeze();

	adjustSize(this->GetSize().GetX());

	m_tree->Thaw();
}

void WatchesWnd::adjustSize(int width)
{
	/*
	int col1Size = m_grid->GetColSize(0);
	int remaining = width - col1Size;
	m_grid->SetColSize(1,std::max(m_infoColMinSize, remaining));
	m_grid->SetMargins(0 - wxSYS_VSCROLL_X, 0);
	*/
}


void WatchesWnd::OnShow(wxShowEvent& evt)
{
	updateState();
}

void WatchesWnd::OnSize(wxSizeEvent& evt)
{
	auto size = evt.GetSize();
	adjustSize(size.GetX());
	evt.Skip();
}

} // namespace nutcracker






