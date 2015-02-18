#pragma once

#include "UIDefs.h"

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
	AppEventFileSaved(cz::view::FileEditorWnd* wnd) : AppEvent(AppEventID::FileSaved), wnd(wnd)
	{
	}
	cz::view::FileEditorWnd* wnd;
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