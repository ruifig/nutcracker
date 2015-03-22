/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com
	
	purpose:
	
*********************************************************************/

#include "NutcrackerPCH.h"
#include "MainWnd.h"

#include "AppEvents.h"
#include "Interpreter.h"
#include "FileEditorGroupWnd.h"
#include "Workspace.h"
#include "FindFileDlg.h"

namespace nutcracker
{

/*
Control ids
*/
enum
{
	ID_MENU_INTERPRETER_FIRST = wxID_HIGHEST+1,
	ID_MENU_INTERPRETER_LAST = ID_MENU_INTERPRETER_FIRST+100
};

BEGIN_EVENT_TABLE(MainWnd, MainWnd_Auto)
	//EVT_MENU(ID_MENU_INTERPRETER_FIRST, MainWnd::OnMenuClick)
	EVT_DROP_FILES(MainWnd::OnDropFiles)
END_EVENT_TABLE()

//////////////////////////////////////////////////////////////////////////
// MainWndLoogerOutput
//
// Thread safe logger output.
// It will queue log messages to be processed in the main thread.
//
//////////////////////////////////////////////////////////////////////////

class MainWndLoggerOutput : public cz::LoggerOutput
{
public:
	MainWndLoggerOutput(cz::AsyncCommandQueueExplicit* cmdQueue, wxTextCtrl* ctrl) : m_queue(cmdQueue), m_ctrl(ctrl)
	{
	}

	virtual void outputLog(int cat, cz::Logger::LogLevel level, const char* str) override
	{
		std::string s(str);
		m_queue->send([=]()
		{
			m_ctrl->AppendText(s);
		});
	}

	cz::AsyncCommandQueueExplicit* m_queue;
	wxTextCtrl* m_ctrl;
};

//////////////////////////////////////////////////////////////////////////
// MainWnd
//////////////////////////////////////////////////////////////////////////

static wxIconBundle gAppIcons;


MainWnd::MainWnd()
	: MainWnd_Auto((wxFrame*)nullptr, wxID_ANY, "Nutcracker IDE", wxDefaultPosition)
{
	gMainWnd = this;

	{
		wxIcon iconSmall;
		iconSmall.CopyFromBitmap(gImageListSmall.GetBitmap(SMALLIMG_IDX_NUT));
		gAppIcons.AddIcon(iconSmall);
		//wxIcon iconBig;
		//iconBig.CopyFromBitmap(gImageList32x32.GetBitmap(BIGIMG_IDX_NUT));
		//gAppIcons.AddIcon(iconBig);
		SetIcons(gAppIcons);
	}

	m_logger = std::make_unique<MainWndLoggerOutput>(&m_asyncFuncs, m_logTextCtrl);
	cz::Logger::getSingleton().addOutput(m_logger.get());


	//
	//
	//
	gWorkspace->loadConfig();
	m_menuView->FindItem(ID_MENU_VIEW_INDENTATION)->Check(gWorkspace->getViewOptions()->view_indentation);
	m_menuView->FindItem(ID_MENU_VIEW_WHITESPACE)->Check(gWorkspace->getViewOptions()->view_whitespaces);
	m_menuView->FindItem(ID_MENU_VIEW_EOL)->Check(gWorkspace->getViewOptions()->view_eol);

	//
	// Initialize interpreters
	//
	{
		gWorkspace->loadInterpreters();
		int interpreterMenuId = ID_MENU_INTERPRETER_FIRST;

		//bind(wxevt_command_menu_selected, &mainwnd::oninterpreterclick, this, interpretermenuid);
		//m_menuinterpreters->append(interpretermenuid++, "test", wxemptystring, wxitem_check);

		wxMenuItem* current = nullptr;
		for(int index=0; index<gWorkspace->getNumInterpreters(); index++)
		{
			auto i = gWorkspace->getInterpreter(index);
			auto item = m_menuInterpreters->AppendRadioItem(interpreterMenuId, i->getName().c_str());
			if (i == gWorkspace->getCurrentInterpreter())
				current = item;
			Bind(wxEVT_COMMAND_MENU_SELECTED, &MainWnd::OnInterpreterClick, this, interpreterMenuId);
			interpreterMenuId++;
		}
		current->Check();
	}

	Connect(wxEVT_CHAR_HOOK, wxKeyEventHandler(MainWnd::OnCharHook));

	DragAcceptFiles(true);

	gWorkspace->registerListener(this, [this](const DataEvent& evt)
	{
		if (evt.isDebugEvent() || evt.id==DataEventID::WorkspaceDirty || evt.id==DataEventID::WorkspaceChanges)
		{
			wxString t = (gWorkspace->getName() + " ").widen();
			if (gWorkspace->isDirty())
				t = wxString("*") + t;
			if (gWorkspace->debuggerActive())
				t += gWorkspace->debuggerGetBreakInfo() ? "(Debugging)" : "(Running)";
			t += " - Nutcracker IDE";
			SetTitle(t);
			if (evt.isDebugEvent())
			{
				updateDebugMenu();
			}
		}
	});

	updateDebugMenu();

}

MainWnd::~MainWnd()
{
	gFileEditorGroupWnd->forceCloseAll();
	gWorkspace->saveConfig();
	gWorkspace.reset();
	gShuttingDown = true;
	cz::Logger::getSingleton().removeOutput(m_logger.get());
}

void MainWnd::OnIdle(wxIdleEvent& evt)
{
	m_asyncFuncs.tick(false);
}


void MainWnd::OnMenuClick(wxCommandEvent& event)
{
	wxMenu* menu = static_cast<wxMenu*>(event.GetEventObject());
	bool val = menu->IsChecked(event.GetId());

	switch(event.GetId())
	{
		case ID_MENU_VIEW_INDENTATION:
			gWorkspace->setViewIdentation(val);
		break;
		case ID_MENU_VIEW_WHITESPACE:
			gWorkspace->setViewWhitespaces(val);
		break;
		case ID_MENU_VIEW_EOL:
			gWorkspace->setViewEOL(val);
		break;
	}

}

void MainWnd::OnInterpreterClick(wxCommandEvent& event)
{
	gWorkspace->setCurrentInterpreter(event.GetId() - ID_MENU_INTERPRETER_FIRST);
}

void MainWnd::OnCharHook(wxKeyEvent& event)
{

	if (!this->IsActive())
	{
		event.Skip();
		return;
	}

	const int modifiers = event.GetModifiers();
	const auto keycode = event.GetKeyCode();

	//
	// Edit Menu
	if (modifiers == (wxMOD_SHIFT | wxMOD_ALT) && keycode == 'O' )
	{
		OnMenuEditFindFileInWorkspace(wxCommandEvent());
	}
	//
	// Debug Menu
	if (modifiers == wxMOD_NONE && keycode==WXK_F5)
	{
		OnMenuDebugStartOrContinue(wxCommandEvent());
	}
	else if (modifiers == wxMOD_CONTROL && keycode == WXK_F5)
	{
		OnMenuDebugStartWithoutDebugging(wxCommandEvent());
	}
	else if (modifiers == wxMOD_SHIFT && keycode == WXK_F5)
	{
		OnMenuDebugStopDebugging(wxCommandEvent());
	}
	else if (modifiers == (wxMOD_CONTROL|wxMOD_ALT) && keycode==WXK_CANCEL /*CANCEL is BREAK */)
	{
		OnMenuDebugBreak(wxCommandEvent());
	}
	else if (modifiers == wxMOD_NONE && keycode == WXK_F10)
	{
		OnMenuDebugStepOver(wxCommandEvent());
	}
	else if (modifiers == wxMOD_NONE && keycode == WXK_F11)
	{
		OnMenuDebugStepInto(wxCommandEvent());
	}
	else if (modifiers == wxMOD_SHIFT && keycode == WXK_F11)
	{
		OnMenuDebugStepOut(wxCommandEvent());
	}
	//
	// Unknown
	else
	{
		event.Skip();
	}
}

wxString SaveAuiNotebookPerspective(wxAuiNotebook* notebook) {
	auto& m_mgr = const_cast<wxAuiManager&>(notebook->GetAuiManager());
   // Build list of panes/tabs
   wxString tabs;
   wxAuiPaneInfoArray& all_panes = m_mgr.GetAllPanes();
    const size_t pane_count = all_panes.GetCount();

    for (size_t i = 0; i < pane_count; ++i)
    {
      wxAuiPaneInfo& pane = all_panes.Item(i);
      if (pane.name == wxT("dummy"))
            continue;
        //wxTabFrame* tabframe = (wxTabFrame*)pane.window;
      
      if (!tabs.empty()) tabs += wxT("|");
      tabs += pane.name;
      tabs += wxT("=");
      
	  /*
      // add tab id's
      size_t page_count = tabframe->m_tabs->GetPageCount();
      for (size_t p = 0; p < page_count; ++p)
      {
         wxAuiNotebookPage& page = tabframe->m_tabs->GetPage(p);
         const size_t page_idx = m_tabs.GetIdxFromWindow(page.window);
         
         if (p) tabs += wxT(",");

         if ((int)page_idx == m_curpage) tabs += wxT("*");
         else if ((int)p == tabframe->m_tabs->GetActivePage()) tabs += wxT("+");
         tabs += wxString::Format(wxT("%u"), page_idx);
      }
	  */
   }
   tabs += wxT("@");

   // Add frame perspective
   tabs += m_mgr.SavePerspective();

   return tabs;
}

/*
bool wxAuiNotebook::LoadPerspective(const wxString& layout) {
   // Remove all tab ctrls (but still keep them in main index)
   const size_t tab_count = m_tabs.GetPageCount();
   for (size_t i = 0; i < tab_count; ++i) {
      wxWindow* wnd = m_tabs.GetWindowFromIdx(i);

      // find out which onscreen tab ctrl owns this tab
      wxAuiTabCtrl* ctrl;
      int ctrl_idx;
      if (!FindTab(wnd, &ctrl, &ctrl_idx))
         return false;

      // remove the tab from ctrl
      if (!ctrl->RemovePage(wnd))
         return false;
   }
   RemoveEmptyTabFrames();

   size_t sel_page = 0;
   
   wxString tabs = layout.BeforeFirst(wxT('@'));
   while (1)
    {
      const wxString tab_part = tabs.BeforeFirst(wxT('|'));
      
      // if the string is empty, we're done parsing
        if (tab_part.empty())
            break;

      // Get pane name
      const wxString pane_name = tab_part.BeforeFirst(wxT('='));

      // create a new tab frame
      wxTabFrame* new_tabs = new wxTabFrame;
      new_tabs->m_tabs = new wxAuiTabCtrl(this,
                                 m_tab_id_counter++,
                                 wxDefaultPosition,
                                 wxDefaultSize,
                                 wxNO_BORDER|wxWANTS_CHARS);
      new_tabs->m_tabs->SetArtProvider(m_tabs.GetArtProvider()->Clone());
      new_tabs->SetTabCtrlHeight(m_tab_ctrl_height);
      new_tabs->m_tabs->SetFlags(m_flags);
      wxAuiTabCtrl *dest_tabs = new_tabs->m_tabs;

      // create a pane info structure with the information
      // about where the pane should be added
      wxAuiPaneInfo pane_info = wxAuiPaneInfo().Name(pane_name).Bottom().CaptionVisible(false);
      m_mgr.AddPane(new_tabs, pane_info);

      // Get list of tab id's and move them to pane
      wxString tab_list = tab_part.AfterFirst(wxT('='));
      while(1) {
         wxString tab = tab_list.BeforeFirst(wxT(','));
         if (tab.empty()) break;
         tab_list = tab_list.AfterFirst(wxT(','));

         // Check if this page has an 'active' marker
         const wxChar c = tab[0];
         if (c == wxT('+') || c == wxT('*')) {
            tab = tab.Mid(1); 
         }

         const size_t tab_idx = wxAtoi(tab.c_str());
         if (tab_idx >= GetPageCount()) continue;

         // Move tab to pane
         wxAuiNotebookPage& page = m_tabs.GetPage(tab_idx);
         const size_t newpage_idx = dest_tabs->GetPageCount();
         dest_tabs->InsertPage(page.window, page, newpage_idx);

         if (c == wxT('+')) dest_tabs->SetActivePage(newpage_idx);
         else if ( c == wxT('*')) sel_page = tab_idx;
      }
      dest_tabs->DoShowHide();

      tabs = tabs.AfterFirst(wxT('|'));
   }
   
   // Load the frame perspective
   const wxString frames = layout.AfterFirst(wxT('@'));
   m_mgr.LoadPerspective(frames);

   // Force refresh of selection
   m_curpage = -1;
   SetSelection(sel_page);

   return true;
}
*/

void MainWnd::OnExitClick(wxCommandEvent& event)
{
	SaveAuiNotebookPerspective(m_notebook);
	// true is to force the frame to close
	Close(false);
}

void MainWnd::OnCloseWindow(wxCloseEvent& event)
{
	if (event.CanVeto() && (gFileEditorGroupWnd->hasDirtyFiles() || gWorkspace->isDirty()))
	{
		try
		{
			int ret = wxMessageBox("Workspace has changed.\nSave changes?", wxMessageBoxCaptionStr, wxYES_NO|wxCANCEL|wxICON_QUESTION);
			
			if (ret==wxYES)
			{
				saveWorkspace();
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
		catch (std::exception& e)
		{
			showException(e);
			event.Veto();
			return;
		}
	}

	event.Skip();
}

void MainWnd::OnMenuFileSaveWorkspace(wxCommandEvent& event)
{
	try
	{
		saveWorkspace();
	}
	catch (std::exception& e)
	{
		showException(e);
	}
}

void MainWnd::saveWorkspace()
{
	wxString path = gWorkspace->getFullPath().widen();
	if(path=="")
	{
		wxFileDialog fileDlg(this, "Save workspace", wxGetCwd(), "", "Nutcracker Workspace (*.nws)|*.nws", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
		if (fileDlg.ShowModal() == wxID_CANCEL)
			return;

		path = fileDlg.GetPath();
	}

	gFileEditorGroupWnd->saveAll();
	gWorkspace->save(path.c_str().AsWChar());
}

void MainWnd::OnMenuFileLoadWorkspace(wxCommandEvent& event)
{
	if (gWorkspace->debuggerActive())
	{
		showError("Workspace Load", "Can't load a workspace while debugging.");
		return;
	}

	while(gWorkspace->isDirty())
	{
		auto res = wxMessageBox("Current workspace has been changed.\nSave changes?", wxMessageBoxCaptionStr, wxYES_NO | wxCANCEL | wxICON_WARNING);
		if (res == wxCANCEL)
			return;
		else if (res == wxYES)
			saveWorkspace();
		else
			break;
	}

	try
	{
		wxFileDialog openFileDialog(this, "Open workspace", wxGetCwd(), "", "Nutcracker Workspace (*.nws)|*.nws",
			wxFD_OPEN | wxFD_FILE_MUST_EXIST);

		if (openFileDialog.ShowModal() == wxID_CANCEL)
			return;

		cz::UTF8String filename = wxStringToUtf8(openFileDialog.GetPath());
		gFileEditorGroupWnd->forceCloseAll();
		gWorkspace->load(filename);
	}
	catch (std::exception& e)
	{
		showException(e);
	}

}

void MainWnd::OnMenuFileCloseWorkspace(wxCommandEvent& event)
{
	try
	{
		if (gWorkspace->isDirty())
		{
			auto res = wxMessageBox("Current workspace has been changed.\nSave changes?", wxMessageBoxCaptionStr, wxYES_NO | wxCANCEL | wxICON_WARNING);
			if (res == wxCANCEL)
				return;
			else if (res == wxYES)
				saveWorkspace();
		}

		gFileEditorGroupWnd->forceCloseAll();
		gWorkspace->close();
	}
	catch (std::exception& e)
	{
		showException(e);
	}

}

void MainWnd::OnMenuEditFindFileInWorkspace(wxCommandEvent& event)
{
	FindFileDlg dlg(this);
	dlg.ShowModal();
	if (auto file = dlg.getResult())
		gFileEditorGroupWnd->gotoFile(file);
}

void MainWnd::updateDebugMenu()
{
	wxMenuItem* item;

		item = m_menuDebug->FindItem(ID_MENU_DEBUG_START_OR_CONTINUE);
		item->SetItemLabel(wxString(gWorkspace->debuggerActive() ? wxT("Continue\t") : wxT("Start Debugging")) + "\tF5");
		item->Enable(!(gWorkspace->debuggerActive() && gWorkspace->debuggerGetBreakInfo() == nullptr));

		item = m_menuDebug->FindItem(ID_MENU_DEBUG_START_NODEBUG);
		item->Enable(!gWorkspace->debuggerActive());

		item = m_menuDebug->FindItem(ID_MENU_DEBUG_STOP_DEBUGGING);
		item->Enable(gWorkspace->debuggerActive());

		item = m_menuDebug->FindItem(ID_MENU_DEBUG_BREAK);
		item->Enable(gWorkspace->debuggerActive() && gWorkspace->debuggerGetBreakInfo()==nullptr);

		item = m_menuDebug->FindItem(ID_MENU_DEBUG_STEPOVER);
		item->Enable(gWorkspace->debuggerActive() && gWorkspace->debuggerGetBreakInfo());

		item = m_menuDebug->FindItem(ID_MENU_DEBUG_STEPINTO);
		item->Enable(gWorkspace->debuggerActive() && gWorkspace->debuggerGetBreakInfo());

		item = m_menuDebug->FindItem(ID_MENU_DEBUG_STEPOUT);
		item->Enable(gWorkspace->debuggerActive() && gWorkspace->debuggerGetBreakInfo());
}

void MainWnd::OnMenuDebugStartOrContinue(wxCommandEvent& event)
{
	if (gWorkspace->debuggerActive())
		gWorkspace->debuggerResume();
	else
	{
		auto file = gFileEditorGroupWnd->getCurrentFile();
		if (!file)
			showError("No file selected", "No file selected to start debugger!\nOpen a file first");
		else
			gWorkspace->debuggerStart(file->id);
	}
}

void MainWnd::OnMenuDebugStartWithoutDebugging(wxCommandEvent& event)
{
	if (gWorkspace->debuggerActive())
		return;
	auto file = gFileEditorGroupWnd->getCurrentFile();
	if (!file)
		showError("No file selected", "No file selected to start execution!\nOpen a file first");
	else
		gWorkspace->run(file->id);
}

void MainWnd::OnMenuDebugStopDebugging(wxCommandEvent& event)
{
	if (!gWorkspace->debuggerActive())
		return;
	gWorkspace->debuggerTerminate();
}

void MainWnd::OnMenuDebugBreak(wxCommandEvent& event)
{
	if (!gWorkspace->debuggerActive())
		return;
	gWorkspace->debuggerSuspend();
}

void MainWnd::OnMenuDebugStepOver(wxCommandEvent& event)
{
	gWorkspace->debuggerStepOver();
}

void MainWnd::OnMenuDebugStepInto(wxCommandEvent& event)
{
	gWorkspace->debuggerStepInto();
}

void MainWnd::OnMenuDebugStepOut(wxCommandEvent& event)
{
	gWorkspace->debuggerStepReturn();
}


void MainWnd::OnSetFocus(wxFocusEvent& event)
{
}

void MainWnd::OnKillFocus(wxFocusEvent& event)
{

}

void MainWnd::OnDropFiles(wxDropFilesEvent& event)
{
	wxString* dropped = event.GetFiles();
	for (int i = 0; i < event.GetNumberOfFiles(); i++)
	{
		auto fname = wxStringToUtf8(*dropped);
		if (Filesystem::getSingleton().isExistingFile(fname))
		{
			auto file = gWorkspace->createFile(fname);
			if (file)
				gFileEditorGroupWnd->gotoFile(file);
		}
		else if (Filesystem::getSingleton().isExistingDirectory(fname))
		{
			gWorkspace->addFolder(fname);
		}

		dropped++;
	}

}


} // namespace nutcracker
