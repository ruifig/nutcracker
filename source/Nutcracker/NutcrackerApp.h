#pragma once

namespace nutcracker
{

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

	std::unique_ptr<cz::PlatformRoot> m_platformRoot;
};

DECLARE_APP(NutcrackerApp)

} // namespace nutcracker

