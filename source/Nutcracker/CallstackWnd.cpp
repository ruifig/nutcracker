/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com
	
	purpose:
	
*********************************************************************/

#include "NutcrackerPCH.h"
#include "CallstackWnd.h"
#include "Interpreter.h"
#include "Workspace.h"
#include "FileEditorGroupWnd.h"

namespace nutcracker
{

BEGIN_EVENT_TABLE(CallstackWnd, CallstackWnd_Auto)
	EVT_SHOW(CallstackWnd::OnShow)
END_EVENT_TABLE()

CallstackWnd::CallstackWnd(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
	: CallstackWnd_Auto(parent,id,pos,size,style)
{
	m_grid->EnableDragRowSize(false);
	m_grid->EnableDragColMove(false);
	m_grid->EnableDragGridSize(false);
}

CallstackWnd::~CallstackWnd()
{
}

void CallstackWnd::onAppEvent(const AppEvent& evt)
{
	switch (evt.id)
	{
		case AppEventID::DebugStarted:
			gUIState->debugSession->breakListeners.add(
				this, [&](const std::shared_ptr<BreakInfo>& info)
			{
				m_info = info;
				if (IsShownOnScreen())
					updateState();
			});
	};
}

void CallstackWnd::updateState()
{
	m_grid->Freeze();

	if (m_grid->GetNumberRows())
		m_grid->DeleteRows(0, m_grid->GetNumberRows());

	if (!m_info)
	{
		m_grid->Thaw();
		return;
	}

	m_grid->AppendRows(m_info->callstack.size());

	int r = 0;
	for (auto& c : m_info->callstack)
	{
		m_grid->SetReadOnly(r, 0, true);
		m_grid->SetReadOnly(r, 1, true);

		m_grid->SetCellValue(r, 1,
			wxString::Format(wxT("%s!%s() line %d"), c.file->name.c_str(), c.func.c_str(), c.line));

		r++;
	}

	m_grid->AutoSizeColumns(true);
	m_infoColMinSize = m_grid->GetColSize(1);
	adjustSize(this->GetSize().GetX());

	m_grid->Thaw();
}

// Tips on how to adjust grid size taken from
// http://stackoverflow.com/questions/7946778/wxgrid-shows-large-empty-border-on-right
//
void CallstackWnd::adjustSize(int width)
{
	int col1Size = m_grid->GetColSize(0);
	int remaining = width - col1Size;
	m_grid->SetColSize(1,std::max(m_infoColMinSize, remaining));
	m_grid->SetMargins(0 - wxSYS_VSCROLL_X, 0);
}

void CallstackWnd::OnCellChanged(wxGridEvent& evt)
{
}

void CallstackWnd::OnCellClick(wxGridEvent& evt)
{
	evt.Skip();
}

void CallstackWnd::OnCellDClick(wxGridEvent& evt)
{
	int r = evt.GetRow();
	int c = evt.GetCol();

	m_info->currentCallstackFrame = r;
	auto& entry = m_info->callstack[m_info->currentCallstackFrame];
	gFileEditorGroupWnd->gotoFile(entry.file, entry.line-1);
	fireAppEvent(AppEventID::CallstackFrameChanged);
	evt.Skip();
}

void CallstackWnd::OnShow(wxShowEvent& evt)
{
	if (evt.IsShown())
		updateState();
	evt.Skip();
}

void CallstackWnd::OnSize(wxSizeEvent& evt)
{
	auto size = evt.GetSize();
	adjustSize(size.GetX());
	evt.Skip();
}

} // namespace nutcracker



