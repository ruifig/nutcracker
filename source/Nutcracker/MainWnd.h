/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com
	
	purpose:
	
*********************************************************************/

#pragma once

#include "AutoUI/MainWnd_Auto.h"

namespace nutcracker
{

class MainWndLoggerOutput;

class MainWnd : public MainWnd_Auto
{
public:
	MainWnd();
	virtual ~MainWnd();

	// Adds a function to be called on the next tick
	int getInterpreterIndex();
private:
	DECLARE_EVENT_TABLE()

	virtual void OnIdle(wxIdleEvent& evt) override;
	virtual void OnMenuClick(wxCommandEvent& event) override;
	virtual void OnExitClick(wxCommandEvent& event) override;
	virtual void OnCloseWindow(wxCloseEvent& event) override;
	virtual void OnMenuOpenFile(wxCommandEvent& event) override;
	void OnDropFiles(wxDropFilesEvent& event);

	void OnCharHook(wxKeyEvent& event);
	void OnQuit(wxCommandEvent& event);
	void OnInterpreterClick(wxCommandEvent& event);

	std::unique_ptr<MainWndLoggerOutput> m_logger;
	cz::AsyncCommandQueueExplicit m_asyncFuncs;
};

} // namespace nutcracker
