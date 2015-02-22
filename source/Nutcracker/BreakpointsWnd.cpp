/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com
	
	purpose:
	
*********************************************************************/

#include "NutcrackerPCH.h"
#include "AppEvents.h"
#include "BreakpointsWnd.h"
#include "Workspace.h"
#include "UIDefs.h"
#include "FileEditorGroupWnd.h"
#include "FileEditorWnd.h"

namespace nutcracker
{

BEGIN_EVENT_TABLE(BreakpointsWnd, BreakpointsWnd_Auto)
		EVT_SHOW(BreakpointsWnd::OnShow)
END_EVENT_TABLE()

BreakpointsWnd::BreakpointsWnd(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
	: BreakpointsWnd_Auto(parent,id,pos,size,style)
{
	m_grid->HideRowLabels();
	m_grid->SetDefaultCellAlignment(wxALIGN_LEFT, wxALIGN_CENTER);
	m_grid->EnableDragRowSize(false);
	m_grid->EnableDragColMove(true);
	m_grid->EnableDragGridSize(false);

	m_grid->SetColLabelValue(0, "-");
	m_grid->SetColLabelValue(1, "location");

	gBreakpointsWnd = this;
}

BreakpointsWnd::~BreakpointsWnd()
{
	gBreakpointsWnd = nullptr;
}

void BreakpointsWnd::OnToggleBreakpointsClick(wxCommandEvent& event)
{
	CZ_ASSERT(0); // TODO
}

void BreakpointsWnd::updateState()
{
	m_grid->Freeze();

	if (m_grid->GetNumberRows())
		m_grid->DeleteRows(0, m_grid->GetNumberRows());
	m_grid->AppendRows(gWorkspace->breakpoints.getCount());


	int r = 0;
	gWorkspace->breakpoints.iterate([&](Breakpoint& b)
	{
		m_grid->SetCellRenderer(r, 0, new wxGridCellBoolRenderer);
		m_grid->SetCellEditor(r, 0, new wxGridCellBoolEditor);
		m_grid->SetReadOnly(r,0,true);
		m_grid->SetCellValue(r, 0, b.enabled ? wxT("1") : wxT("0"));

		m_grid->SetCellValue(
			r, 1, wxString::Format(wxT("%s, line %d"), b.file->name.c_str(), b.line+1));
		m_grid->SetReadOnly(r,1,true);
		r++;
	});

	m_grid->AutoSizeColumns(true);
	m_grid->Thaw();

}

void BreakpointsWnd::OnCellChanged(wxGridEvent& evt)
{
}

void BreakpointsWnd::OnCellClick(wxGridEvent& evt)
{
	int r = evt.GetRow();
	int c = evt.GetCol();

	if (c==0 && r<=gWorkspace->breakpoints.getCount())
	{
		auto& b = gWorkspace->breakpoints.getAt(r);
		b.enabled = !b.enabled;
		auto wnd = gFileEditorGroupWnd->findFileWnd(b.file, nullptr);
		if (wnd)
			wnd->syncBreakpoint(b);
		updateState();
	}

	evt.Skip();
}

void BreakpointsWnd::OnCellDClick(wxGridEvent& evt)
{
	int r = evt.GetRow();
	int c = evt.GetCol();
	if (c!=0 && r<=gWorkspace->breakpoints.getCount())
	{
		const auto& b = gWorkspace->breakpoints.getAt(r);
		gFileEditorGroupWnd->gotoFile(b.file, b.line);
	}
	evt.Skip();
}

void BreakpointsWnd::OnShow(wxShowEvent& evt)
{
	updateState();
}

void BreakpointsWnd::onAppEvent(const AppEvent& evt)
{
	switch (evt.id)
	{
		case AppEventID::OpenWorkspace:
		updateState();
		break;
	}
}
} // namespace nutcracker


