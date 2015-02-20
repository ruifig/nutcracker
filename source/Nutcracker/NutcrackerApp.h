#pragma once


class NutcrackerApp : public wxApp
{
	DECLARE_CLASS(NutcrackerApp)
	DECLARE_EVENT_TABLE()

public:
	/// Constructor
	NutcrackerApp();

	void Init();

	/// Initialises the application
	virtual bool OnInit();

	/// Called on exit
	virtual int OnExit();


private:

	void OnLambdaEvent(wxCommandEvent& evt);

	cz::PlatformRoot m_platformRoot;
};

DECLARE_APP(NutcrackerApp)
