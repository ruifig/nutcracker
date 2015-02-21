#include "NutcrackerPCH.h"
#include "AppEvents.h"

namespace nutcracker
{

	static std::vector<AppEventListener*> appEventListeners;

	AppEventListener::AppEventListener()
	{
		appEventListeners.push_back(this);
	}

	AppEventListener::~AppEventListener()
	{
		cz::erase(appEventListeners, this);
	}

	void fireAppEvent(const AppEvent& evt)
	{
		for (auto& i : appEventListeners)
			i->onAppEvent(evt);
	}

	wxDEFINE_EVENT(NUTCRACKER_LAMBDA_EVENT, NutcrackerLambdaEvent);

	NutcrackerLambdaEvent::NutcrackerLambdaEvent(const NutcrackerLambdaEvent& event)
		: wxCommandEvent(event)
		, m_func(event.m_func)
	{
	}

	NutcrackerLambdaEvent::~NutcrackerLambdaEvent()
	{
	}

	wxEvent* NutcrackerLambdaEvent::Clone() const
	{
		return new NutcrackerLambdaEvent(*this);
	}

	void NutcrackerLambdaEvent::run()
	{
		assert(m_func);
		m_done = true;
		(*m_func)();
		m_func.reset();
	}

} // namespace nutcracker

