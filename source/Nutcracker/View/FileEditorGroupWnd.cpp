/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com
	
	purpose:
	
*********************************************************************/

#include "NutcrackerPCH.h"
#include "FileEditorGroupWnd.h"


namespace cz
{
namespace view
{

BEGIN_EVENT_TABLE(FileEditorGroupWnd, FileEditorGroupWnd_Auto)
END_EVENT_TABLE()

FileEditorGroupWnd::FileEditorGroupWnd(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
	: FileEditorGroupWnd_Auto(parent,id,pos,size,style)
{
}

FileEditorGroupWnd::~FileEditorGroupWnd()
{
}

} // namespace view
} // namespace cz
