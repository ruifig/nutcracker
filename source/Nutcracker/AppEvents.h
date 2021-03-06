#pragma once

#include "UIDefs.h"
#include "NutcrackerApp.h"

namespace nutcracker
{

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
