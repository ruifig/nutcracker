/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com
	
	purpose:
	
*********************************************************************/

#include "NutcrackerPCH.h"
#include "UIDefs.h"
#include "FileEditorGroupWnd.h"
#include "FileEditorWnd.h"
#include "WorkspaceWnd.h"

namespace cz
{
namespace view
{
/*
Control ids
*/
enum
{
	ID_MENU_LocateInWorkspace = wxID_HIGHEST+1,
	ID_MENU_OpenContaininFolder,
	ID_MENU_CloseAllButThis
};

BEGIN_EVENT_TABLE(FileEditorGroupWnd, FileEditorGroupWnd_Auto)
	EVT_MENU(ID_MENU_LocateInWorkspace, FileEditorGroupWnd::OnLocateInWorkspace)
	EVT_MENU(ID_MENU_OpenContaininFolder, FileEditorGroupWnd::OnOpenContainingFolder)
	EVT_MENU(ID_MENU_CloseAllButThis, FileEditorGroupWnd::OnCloseAllButThis)
    EVT_TIMER(wxID_ANY,FileEditorGroupWnd::OnTimer)
END_EVENT_TABLE()

FileEditorGroupWnd::FileEditorGroupWnd(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
	: FileEditorGroupWnd_Auto(parent,id,pos,size,style), m_timer(this)
{
	m_timer.Start(2000);
	gFileEditorGroupWnd = this;
}

FileEditorGroupWnd::~FileEditorGroupWnd()
{
}

void FileEditorGroupWnd::OnPageClose(wxAuiNotebookEvent& event)
{
	try
	{
		wxWindow* page = m_notebook->GetPage(event.GetSelection());
		assert(page->IsKindOf(CLASSINFO(FileEditorWnd)));
		FileEditorWnd* v = static_cast<FileEditorWnd*>(page);

		if (v->getFile()->dirty)
		{
			int ret = wxMessageBox("File has changed.\nSave changes?", wxMessageBoxCaptionStr, wxYES_NO|wxCANCEL|wxICON_QUESTION);
			
			if (ret==wxYES)
			{
				v->save();
			}
			else if (ret==wxNO)
			{
				// do nothing
			}
			else // CANCEL
			{
				event.Veto();
				return;
			}
		}

	}
	catch (std::exception& e)
	{
		showException(e);
		event.Veto();
		return;
	}

	event.Skip();
}

void FileEditorGroupWnd::OnPageClosed(wxAuiNotebookEvent& event)
{

}

FileEditorWnd* FileEditorGroupWnd::findFileWnd(document::ProjectItemId id, int* index)
{
	// Check if this file is already opened
	for (size_t i = 0; i < m_notebook->GetPageCount(); i++)
	{
		wxWindow* page = m_notebook->GetPage(i);
		assert(page->IsKindOf(CLASSINFO(FileEditorWnd)));
		FileEditorWnd* v = static_cast<FileEditorWnd*>(page);
		if (v->getFileId() == id)
		{
			if (index)
				*index = (int)i;
			return v;
		}
	}

	return NULL;
}

void FileEditorGroupWnd::iterateFiles(std::function<void(FileEditorWnd*)> func)
{
	for (size_t i = 0; i < m_notebook->GetPageCount(); i++)
	{
		wxWindow* page = m_notebook->GetPage(i);
		assert(page->IsKindOf(CLASSINFO(FileEditorWnd)));
		FileEditorWnd* v = static_cast<FileEditorWnd*>(page);
		func(v);
	}
}

void FileEditorGroupWnd::gotoFile(document::File* file, int line, int col, bool columnIsOffset)
{
	try
	{
		// Check if the file is already open
		int index;
		FileEditorWnd* w = findFileWnd(file->id, &index);
		if (w)
		{
			w->setFile(file, line, col);
			m_notebook->ChangeSelection(index);
			w->setFocusToEditor();
			return;
		}

		//
		// Not found, so create a new page
		w = new FileEditorWnd(m_notebook);
		auto guard = cz::scopeGuard([w]{delete w; });

		w->setFile(file, line, col);
		m_notebook->AddPage(w, file->name.c_str(), true, -1);
		auto pageIndex = m_notebook->GetPageIndex(w);
		m_notebook->SetPageToolTip(pageIndex, file->fullname.widen());
		setPageTitle(file);
		w->setFocusToEditor();
		guard.dismiss();
	}
	catch (std::exception& e)
	{
		showException(e);
	}
}

void FileEditorGroupWnd::setPageTitle(document::File* file)
{
	try
	{
		int index;
		auto fwnd = findFileWnd(file->id, &index);
		if (fwnd)
			m_notebook->SetPageText(index, wxString::Format(L"%s%s", file->name.widen(), file->dirty ? L"*" : L""));
	}
	catch(std::exception& e)
	{
		showException(e);
	}
}

void FileEditorGroupWnd::onAppEvent(const AppEvent& evt)
{
	switch (evt.id)
	{
		case AppEventID::FileSaved:
		{
			FileEditorWnd* wnd = static_cast<const AppEventFileSaved&>(evt).wnd;
			setPageTitle(wnd->getFile());
		}
		break;

		case AppEventID::OpenWorkspace:
		case AppEventID::NewWorkspace:
			//cursorHistory_Clear();
			break;
	}
}

FileEditorWnd* FileEditorGroupWnd::getCurrentPage()
{
	wxWindow* page = m_notebook->GetCurrentPage();
	if (!page)
		return nullptr;
	assert(page->IsKindOf(CLASSINFO(FileEditorWnd)));
	return static_cast<FileEditorWnd*>(page);
}

document::File* FileEditorGroupWnd::getCurrentFile()
{
	auto page = getCurrentPage();
	if (!page)
		return nullptr;
	return page->getFile();
}

void FileEditorGroupWnd::OnTabRightUp(wxAuiNotebookEvent& event)
{
	wxWindow* page = m_notebook->GetPage(event.GetSelection());
	assert(page->IsKindOf(CLASSINFO(FileEditorWnd)));
	m_selectedFileTab = static_cast<FileEditorWnd*>(page);

	// Determine the position of the mouse (this is in screen coords)
	wxPoint mouseposition = wxGetMousePosition();

	// Get the location of the top left of the window
	wxPoint topleft = GetScreenRect().GetTopLeft();

	// Subtract the location of the top left corner of the window from the
	// mouse position to get the mouse position relative to the window
	// instead of the screen.
	mouseposition -= topleft;

	wxMenu menu;
	menu.Append(ID_MENU_LocateInWorkspace, wxT("Locate in workspace"));
	menu.Append(ID_MENU_OpenContaininFolder, wxT("Open containing folder"));
	menu.Append(ID_MENU_CloseAllButThis, wxT("Close All BUT this"));

	PopupMenu(&menu, mouseposition);

}

void FileEditorGroupWnd::OnLocateInWorkspace(wxCommandEvent& event)
{
	gWorkspaceWnd->locateFile(m_selectedFileTab->getFileId());
}

void FileEditorGroupWnd::OnOpenContainingFolder(wxCommandEvent& event)
{
	cz::UTF8String res;
	cz::Filesystem::getSingleton().fullPath(res, m_selectedFileTab->getFile()->getDirectory());
	wxExecute(wxString("explorer \"") + res.widen() + "\"");
}

void FileEditorGroupWnd::OnCloseAllButThis(wxCommandEvent& event)
{
	try
	{
		std::vector<FileEditorWnd*> others;
		for(size_t i=0; i<m_notebook->GetPageCount();i++)
			if (m_notebook->GetPage(i)!=m_selectedFileTab)
				others.push_back(static_cast<FileEditorWnd*>(m_notebook->GetPage(i)));

		while(others.size())
		{
			auto f = others.back();
			others.pop_back();
			if (f->getFile()->dirty)
			{
				int ret = wxMessageBox(
					wxString(cz::formatString("Save file ""%s"" ?", f->getFile()->fullname.c_str())) ,
					wxMessageBoxCaptionStr, wxYES_NO | wxCANCEL | wxICON_QUESTION );

				if (ret == wxYES)
				{
					f->save();
				}
				else if (ret == wxNO)
				{
					// do nothing
				}
				else // CANCEL
				{
					break;
				}
			}

			m_notebook->DeletePage(m_notebook->GetPageIndex(f));
		}
	}
	catch (std::exception& e)
	{
		showException(e);
		return;
	}

	event.Skip();
}

void FileEditorGroupWnd::OnPageChanged(wxAuiNotebookEvent& event)
{
	FileEditorWnd *page = getCurrentPage();
	page->onPageChanged();
}

void FileEditorGroupWnd::forceCloseAll()
{
	m_notebook->DeleteAllPages();
}

void FileEditorGroupWnd::OnTimer(wxTimerEvent& evt)
{
	for (size_t i = 0; i < m_notebook->GetPageCount(); i++)
	{
		static_cast<FileEditorWnd*>(m_notebook->GetPage(i))->checkReload();
	}
	m_timer.Start(2000);
}

wxString FileEditorGroupWnd::getWordUnderCursor()
{
	if (FileEditorWnd* fwnd = getEditorWithFocus())
		return fwnd->getWordUnderCursor();
	else
		return "";
}

FileEditorWnd* FileEditorGroupWnd::getEditorWithFocus()
{
	FileEditorWnd* wnd = nullptr;
	iterateFiles([&](FileEditorWnd* fwnd)
	{
		if (fwnd->editorHasFocus())
		{
			wnd = fwnd;
		}
	});

	return wnd;
}

} // namespace view
} // namespace cz
