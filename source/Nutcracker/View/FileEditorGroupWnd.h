/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com
	
	purpose:
	
*********************************************************************/

#include "AutoUI/FileEditorGroupWnd_Auto.h"

namespace cz
{
namespace view
{

class FileEditorGroupWnd : public FileEditorGroupWnd_Auto
{
public:
	FileEditorGroupWnd(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style);
	virtual ~FileEditorGroupWnd();
	
private:
	DECLARE_EVENT_TABLE()

};

} // namespace view
} // namespace cz

