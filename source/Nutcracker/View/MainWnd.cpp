/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com
	
	purpose:
	
*********************************************************************/

#include "NutcrackerPCH.h"
#include "MainWnd.h"


namespace cz
{
namespace view
{

BEGIN_EVENT_TABLE(MainWnd, MainWnd_Auto)
END_EVENT_TABLE()

MainWnd::MainWnd()
	: MainWnd_Auto((wxFrame*)nullptr, wxID_ANY, "Nutcracker IDE", wxDefaultPosition, wxSize(1024,768))
{
}

MainWnd::~MainWnd()
{
}

} // namespace view
} // namespace cz
