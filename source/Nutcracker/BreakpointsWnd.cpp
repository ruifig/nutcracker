/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com
	
	purpose:
	
*********************************************************************/

#include "NutcrackerPCH.h"
#include "NutcrackerApp.h"
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
	Connect(wxEVT_CHAR_HOOK, wxKeyEventHandler(BreakpointsWnd::OnCharHook));

	gWorkspace->registerListener(this, [this](const DataEvent& evt)
	{
		if (evt.isBreakpointEvent())
		{
			tryUpdateState();
		}
		else if (evt.id == DataEventID::DebugStop)
		{
			tryUpdateState(true);
		}
	});
}

BreakpointsWnd::~BreakpointsWnd()
{
}

void BreakpointsWnd::tryUpdateState(bool force)
{
	if (m_pendingUpdate && !force)
		return;

	m_pendingUpdate = true;
	postAppLambdaEvent([this, force]()
	{
		if (IsShownOnScreen() || force)
			updateState();
		m_pendingUpdate = false;
	});
}

void BreakpointsWnd::updateState()
{
	m_grid->Freeze();

	if (m_grid->GetNumberRows())
		m_grid->DeleteRows(0, m_grid->GetNumberRows());
	m_grid->AppendRows(gWorkspace->getBreakpointCount());

	int r = 0;
	gWorkspace->iterateBreakpoints([&](const Breakpoint* b)
	{
		m_grid->SetCellRenderer(r, 0, new wxGridCellBoolRenderer);
		m_grid->SetCellEditor(r, 0, new wxGridCellBoolEditor);
		m_grid->SetReadOnly(r,0,true);
		m_grid->SetCellValue(r, 0, b->enabled ? wxT("1") : wxT("0"));

		m_grid->SetCellValue(
			r, 1, wxString::Format(wxT("%s, line %d"), b->file->name.c_str(), b->line));
		m_grid->SetReadOnly(r,1,true);
		r++;
	});

	m_grid->AutoSizeColumns(true);
	m_infoColMinSize = m_grid->GetColSize(1);
	adjustSize(this->GetSize().GetX());

	m_selectedRow = std::max(0, std::min(m_selectedRow, gWorkspace->getBreakpointCount()));
	m_grid->SetGridCursor(m_selectedRow, 0);
	m_grid->SelectRow(m_selectedRow);
	m_grid->Thaw();
}

// Tips on how to adjust grid size taken from
// http://stackoverflow.com/questions/7946778/wxgrid-shows-large-empty-border-on-right
//
void BreakpointsWnd::adjustSize(int width)
{
	int col1Size = m_grid->GetColSize(0);
	int remaining = width - col1Size;
	m_grid->SetColSize(1,std::max(m_infoColMinSize, remaining));
	m_grid->SetMargins(0 - wxSYS_VSCROLL_X, 0);
}

void BreakpointsWnd::OnCharHook(wxKeyEvent& evt)
{
	if (evt.GetKeyCode() == WXK_DELETE)
	{
		int r = m_grid->GetGridCursorRow();
		m_selectedRow = r;
		gWorkspace->removeBreakpoint(r);
	}
	else if (evt.GetKeyCode() == WXK_SPACE)
	{
		int r = m_grid->GetGridCursorRow();
		m_selectedRow = r;
		gWorkspace->toggleBreakpoint(r);
	}
	else
	{
		evt.Skip();
	}
}

void BreakpointsWnd::OnCellChanged(wxGridEvent& evt)
{
}

void BreakpointsWnd::OnCellClick(wxGridEvent& evt)
{
	int r = evt.GetRow();
	int c = evt.GetCol();

	if (c==0 && r<=gWorkspace->getBreakpointCount())
	{
		gWorkspace->toggleBreakpoint(r);
		updateState();
	}

	evt.Skip();
}

void BreakpointsWnd::OnCellDClick(wxGridEvent& evt)
{
	int r = evt.GetRow();
	int c = evt.GetCol();
	if (c != 0 && r <= gWorkspace->getBreakpointCount())
	{
		auto b = gWorkspace->getBreakpoint(r);
		gFileEditorGroupWnd->gotoFile(b->file, b->line);
	}
	evt.Skip();
}

void BreakpointsWnd::OnShow(wxShowEvent& evt)
{
	if (gWorkspace)
		updateState();
}

void BreakpointsWnd::OnSize(wxSizeEvent& evt)
{
	auto size = evt.GetSize();
	adjustSize(size.GetX());
	evt.Skip();
}


} // namespace nutcracker


