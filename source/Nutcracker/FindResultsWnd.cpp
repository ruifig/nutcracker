/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com
	
	purpose:
	
*********************************************************************/

#include "NutcrackerPCH.h"
#include "FindResultsWnd.h"
#include "FileEditorGroupWnd.h"
#include "FileEditorWnd.h"
#include "Workspace.h"

namespace nutcracker {

enum FindGridCol
{
	kFC_Text,
	kFC_File,
	kFC_Line,
	kFC_Col,
	kFC_FileFullPath,
};

FindResultsWnd::FindResultsWnd(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
	: FindResultsWnd_Auto(parent, id, pos, size, style)
{
	gFindResultsWnd = this;

	m_grid->SetColLabelValue(kFC_Text, "Text");
	m_grid->SetColLabelValue(kFC_File, "File");
	m_grid->SetColLabelValue(kFC_Line, "Line");
	m_grid->SetColLabelValue(kFC_Col, "Col");
	m_grid->SetColLabelValue(kFC_FileFullPath, "Full path");

	gWorkspace->registerListener(this, [this](const DataEvent& evt)
	{
		if (evt.isFindResultsEvent())
		{
			tryUpdateState();
		}
	});
}

void FindResultsWnd::tryUpdateState(bool force)
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

void FindResultsWnd::updateState()
{
	//
	// Setup the grid with the results
	m_grid->Freeze();
	if (m_grid->GetNumberRows())
		m_grid->DeleteRows(0, m_grid->GetNumberRows());

	gWorkspace->iterateFindResult([&](const FindResult* res)
	{
		m_grid->AppendRows();
		int r = m_grid->GetNumberRows() - 1;
		m_grid->SetCellValue(r, kFC_Text, res->text);
		m_grid->SetCellValue(r, kFC_File, res->file->name.widen());
		m_grid->SetCellValue(r, kFC_Line, wxString::Format("%d", res->pos.line));
		m_grid->SetCellValue(r, kFC_Col, wxString::Format("%d", res->pos.col));
		m_grid->SetCellValue(r, kFC_FileFullPath, res->file->fullpath.widen());

		for (int c = 0; c < m_grid->GetNumberCols(); c++)
			m_grid->SetReadOnly(r, c, true);
	});

	m_grid->AutoSize();
	m_grid->Thaw();
	this->Layout();
}

void FindResultsWnd::find(const wxString& what, LookWhere lookWhere, bool matchCase, bool matchWholeWord)
{
	if (!gWorkspace || wxString(what).Trim()=="")
		return;

	gWorkspace->clearFindResults();

	//
	// Determine in what files we want to look
	//
	std::vector<std::shared_ptr<const File>> files;
	switch(lookWhere)
	{
		case kLook_Workspace:
			gWorkspace->iterateFiles([&](const std::shared_ptr<const File>& file)
			{
				files.push_back(file);
				return true;
			});
		break;
		case kLook_CurrentFile:
			{
				auto& currentfile = gFileEditorGroupWnd->getCurrentFile();
				if (!currentfile)
					throw std::runtime_error("No file currently open.");
				files.push_back(currentfile);
			}
			break;
		default:
			return;
	}

	int searchFlags = 0;
	if (matchCase)
		searchFlags |= wxSTC_FIND_MATCHCASE;
	if (matchWholeWord)
		searchFlags |= wxSTC_FIND_WHOLEWORD;


	//
	// I'm creating a temporary File editor window, to search in files not open at the moment
	//
    wxStyledTextCtrl* tmptxtctrl = new wxStyledTextCtrl( this );
	tmptxtctrl->Hide();

	try
	{
		for (auto& f : files)
		{
			FileEditorWnd* feditorwnd = gFileEditorGroupWnd->findFileWnd(f, nullptr);
			if (feditorwnd)
			{
				tmptxtctrl->SetText(feditorwnd->m_textCtrl->GetText());
			}
			else
			{
				tmptxtctrl->LoadFile(f->fullpath.widen());
			}

			tmptxtctrl->SetSearchFlags(searchFlags);

			int pos=0;
			tmptxtctrl->SetTargetStart(0);
			tmptxtctrl->SetTargetEnd(tmptxtctrl->GetLength()-1);
			while((pos=tmptxtctrl->SearchInTarget(what))!=-1)
			{
				tmptxtctrl->GotoPos(pos);
				// GetCurrentLine returns the line text with the newline characters, which causes the grid rows to grow,
				// so we need to remove the newline characters at the end
				gWorkspace->addFindResult(wxString(tmptxtctrl->GetLine(tmptxtctrl->GetCurrentLine())).Trim(), f, tmptxtctrl->GetCurrentLine()+1, tmptxtctrl->GetColumn(pos));
				tmptxtctrl->SetTargetStart(pos+1);
				tmptxtctrl->SetTargetEnd(tmptxtctrl->GetLength()-1);
			}
		}

	}
	catch(std::exception& e)
	{
		showException(e);
	}

	tmptxtctrl->Destroy();


}

void FindResultsWnd::OnGridCellDClick(wxGridEvent& event)
{
	int r = event.GetRow();
	int c = event.GetCol();
	cz::UTF8String filefullpath = wxStringToUtf8(m_grid->GetCellValue(r, kFC_FileFullPath));
	long line = 0;
	m_grid->GetCellValue(r, kFC_Line).ToLong(&line);
	long col = 0;
	m_grid->GetCellValue(r, kFC_Col).ToLong(&col);
	auto file = gWorkspace->createFile(filefullpath);
	if (file)
		gFileEditorGroupWnd->gotoFile(file, line, col);
}


} // namespace nutcracker

