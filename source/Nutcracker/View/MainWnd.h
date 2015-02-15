/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com
	
	purpose:
	
*********************************************************************/

#include "AutoUI/MainWnd_Auto.h"

namespace cz
{
namespace view
{

class MainWndLoggerOutput;

class MainWnd : public MainWnd_Auto
{
public:
	MainWnd();
	virtual ~MainWnd();

	// Adds a function to be called on the next tick
	void addAsyncFunc(std::function<void()> f);
private:
	DECLARE_EVENT_TABLE()

	virtual void OnIdle(wxIdleEvent& evt) override;
	virtual void OnMenuClick(wxCommandEvent& event) override;

	std::unique_ptr<MainWndLoggerOutput> m_logger;
	cz::AsyncCommandQueueExplicit m_asyncFuncs;
};

} // namespace view
} // namespace cz
