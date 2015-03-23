/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com
	
	purpose:
	
*********************************************************************/

#include "NutcrackerPCH.h"
#include "AboutDlg.h"

namespace nutcracker
{

AboutDlg::AboutDlg(wxWindow* parent) : AboutDlg_Auto(parent)
{
	wxBitmap bitmap(wxT("APP_IMG_64x64_NUT"), wxBITMAP_TYPE_PNG_RESOURCE);
	m_logo->SetBitmap(bitmap);
	m_buildVersion->SetLabel(wxString("Built ") + wxString(__DATE__) + wxT(" , ") + wxString(__TIME__));
}


} // namespace nutcracker
