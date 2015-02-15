/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com
	
	purpose:
	
*********************************************************************/

#include "AutoUI/FileEditorGroupWnd_Auto.h"
#include "Document/Project.h"
#include "AppEvents.h"

namespace cz
{
namespace view
{

class FileEditorGroupWnd : public FileEditorGroupWnd_Auto , public AppEventListener
{
public:
	FileEditorGroupWnd(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style);
	virtual ~FileEditorGroupWnd();

	void gotoFile(document::File* file, int line = -1, int col = 0, bool columnIsOffset = false);
	void setPageTitle(document::File* file);
	wxString getWordUnderCursor();
	FileEditorWnd* getEditorWithFocus();
	void forceCloseAll();
private:
	DECLARE_EVENT_TABLE()

	virtual void OnPageClose(wxAuiNotebookEvent& event) override;
	virtual void OnPageClosed(wxAuiNotebookEvent& event) override;
	virtual void OnPageChanged(wxAuiNotebookEvent& event) override;
	virtual void OnTabRightUp(wxAuiNotebookEvent& event) override;

	void OnLocateInWorkspace( wxCommandEvent& event );
	void OnOpenContainingFolder( wxCommandEvent& event );
	void OnCloseAllButThis( wxCommandEvent& event );
	void OnTimer(wxTimerEvent& evt);

	virtual void onAppEvent(const AppEvent& evt);
	FileEditorWnd* getCurrentPage();
	document::File* getCurrentFile();

	void iterateFiles(std::function<void(FileEditorWnd*)> func);
	class FileEditorWnd* findFileWnd(document::ProjectItemId id, int* index);

	// Used to remember what file tab we right clicked (for the popup menu)
	FileEditorWnd* m_selectedFileTab=nullptr;
	wxTimer m_timer;
};

} // namespace view
} // namespace cz

