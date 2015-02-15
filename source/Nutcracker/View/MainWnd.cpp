/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com
	
	purpose:
	
*********************************************************************/

#include "NutcrackerPCH.h"
#include "MainWnd.h"

#include "Document/Project.h"
#include "AppEvents.h"

namespace cz
{
namespace view
{

BEGIN_EVENT_TABLE(MainWnd, MainWnd_Auto)
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

	virtual void outputLog(int cat, Logger::LogLevel level, const char* str) override
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
	: MainWnd_Auto((wxFrame*)nullptr, wxID_ANY, "Nutcracker IDE", wxDefaultPosition, wxSize(1024,768))
{
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

	document::Project prj(Filesystem::getSingleton().getCWD());
	prj.populate();
}

MainWnd::~MainWnd()
{
}

void MainWnd::OnIdle(wxIdleEvent& evt)
{
	m_asyncFuncs.tick(false);
}

void MainWnd::addAsyncFunc(std::function<void()> f)
{
	m_asyncFuncs.send(std::move(f));
}

void MainWnd::OnMenuClick(wxCommandEvent& event)
{
	wxMenu* menu = static_cast<wxMenu*>(event.GetEventObject());
	bool val = menu->IsChecked(event.GetId());

	switch(event.GetId())
	{
		case ID_MENU_VIEW_INDENTATION:
			uiState.view_ShowIndentationGuides = val;
			fireAppEvent(AppEventID::ViewOptionsChanged);
			break;
		case ID_MENU_VIEW_WHITESPACE:
			uiState.view_Whitespace = val;
			fireAppEvent(AppEventID::ViewOptionsChanged);
		break;
		case ID_MENU_VIEW_EOL:
			uiState.view_EOL = val;
			fireAppEvent(AppEventID::ViewOptionsChanged);
		break;
	}

}

} // namespace view
} // namespace cz
