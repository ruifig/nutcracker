/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com
	
	purpose:
	
*********************************************************************/

#include "NutcrackerPCH.h"
#include "WorkspaceWnd.h"

namespace cz
{
namespace view
{


BEGIN_EVENT_TABLE(WorkspaceWnd, wxPanel)
END_EVENT_TABLE()

WorkspaceWnd::WorkspaceWnd(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
{
	wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
	sizer->SetMinSize(wxSize(160, 250));
	SetSizer(sizer);
}

WorkspaceWnd::~WorkspaceWnd()
{
}

} // namespace view
} // namespace cz
