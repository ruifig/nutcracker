#include "NutcrackerPCH.h"
#include "AppEvents.h"

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
