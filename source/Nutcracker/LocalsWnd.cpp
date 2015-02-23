/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com
	
	purpose:
	
*********************************************************************/

#include "NutcrackerPCH.h"
#include "LocalsWnd.h"
#include "UIDefs.h"

namespace nutcracker
{

BEGIN_EVENT_TABLE(LocalsWnd, BaseWatchesWnd)
	EVT_SHOW(LocalsWnd::OnShow)
END_EVENT_TABLE()

LocalsWnd::LocalsWnd(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
	: BaseWatchesWnd(parent,id,pos,size,style)
{
}

LocalsWnd::~LocalsWnd()
{
}

void LocalsWnd::onAppEvent(const AppEvent& evt)
{
	switch (evt.id)
	{
		case AppEventID::DebugStarted:
			gUIState->debugSession->breakListeners.add(this,
				[&](const std::shared_ptr<const BreakInfo>& info)
			{
				m_info = info;
			});
	}
}

void LocalsWnd::updateState()
{
	m_tree->Freeze();

	if (!m_info)
	{
		m_tree->DeleteAllItems();
		m_tree->Thaw();
		return;
	}

	adjustSize(this->GetSize().GetX());

	m_tree->Thaw();
}

void LocalsWnd::adjustSize(int width)
{
	/*
	int col1Size = m_grid->GetColSize(0);
	int remaining = width - col1Size;
	m_grid->SetColSize(1,std::max(m_infoColMinSize, remaining));
	m_grid->SetMargins(0 - wxSYS_VSCROLL_X, 0);
	*/
}


void LocalsWnd::OnShow(wxShowEvent& evt)
{
	updateState();
}

void LocalsWnd::OnSize(wxSizeEvent& evt)
{
	auto size = evt.GetSize();
	adjustSize(size.GetX());
	evt.Skip();
}

} // namespace nutcracker





