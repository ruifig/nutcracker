#pragma once

#include "UIDefs.h"

namespace nutcracker
{

	enum class AppEventID
	{
		OpenWorkspace,
		AddedFolder,
		NewWorkspace,
		FileSaved,
		ViewOptionsChanged
	};

	struct AppEvent
	{
		AppEventID id;
		AppEvent(AppEventID id) : id(id) {}
	};

	struct AppEventFileSaved : public AppEvent
	{
		AppEventFileSaved(FileEditorWnd* wnd) : AppEvent(AppEventID::FileSaved), wnd(wnd)
		{
		}
		FileEditorWnd* wnd;
	};

	class AppEventListener
	{
	public:
		AppEventListener();
		virtual ~AppEventListener();
		virtual void onAppEvent(const AppEvent& evt) = 0;
	private:
	};

	void fireAppEvent(const AppEvent& evt);

	class NutcrackerLambdaEvent;
	wxDECLARE_EVENT(NUTCRACKER_LAMBDA_EVENT, NutcrackerLambdaEvent);

	template<typename T>
	void postAppLambdaEvent(T&& func)
	{
		wxPostEvent(&wxGetApp(), NutcrackerLambdaEvent(std::forward<T>(func)));
	}

	class NutcrackerLambdaEvent : public wxCommandEvent
	{
	public:

		template<typename T>
		NutcrackerLambdaEvent(T&& func) : wxCommandEvent(NUTCRACKER_LAMBDA_EVENT)
		{
			//m_func = std::make_shared<std::function<void()>>([]{ czDebug(ID_Log, "HellO");  });
			m_func = std::make_shared<std::function<void()>>(std::forward<T>(func));
		}

		virtual ~NutcrackerLambdaEvent();
		NutcrackerLambdaEvent(const NutcrackerLambdaEvent& event);
		wxEvent* Clone() const;
		void run();

	private:
		std::shared_ptr<std::function<void()>> m_func;
		bool m_done = false;
	};

	typedef void (wxEvtHandler::*NutrackerLambdaEventFunction)(NutcrackerLambdaEvent &);

} // namespace nutcracker
