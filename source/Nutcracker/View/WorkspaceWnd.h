/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com
	
	purpose:
	
*********************************************************************/

#pragma once


namespace cz
{
namespace view
{

class WorkspaceWnd : public wxPanel
{
public:
	WorkspaceWnd(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style);
	virtual ~WorkspaceWnd();

private:
	DECLARE_EVENT_TABLE()

};

} // namespace view
} // namespace cz
