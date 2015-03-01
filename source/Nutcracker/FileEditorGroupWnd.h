/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com
	
	purpose:
	
*********************************************************************/

#include "AutoUI/FileEditorGroupWnd_Auto.h"
#include "AppEvents.h"

namespace nutcracker
{

class FileEditorGroupWnd : public FileEditorGroupWnd_Auto
{
public:
	FileEditorGroupWnd(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style);
	virtual ~FileEditorGroupWnd();

	void gotoFile(const std::shared_ptr<const File>& file, int line = -1, int col = 0, bool columnIsOffset = false);
	void setPageTitle(const std::shared_ptr<const File>& file);
	wxString getWordUnderCursor();
	FileEditorWnd* getEditorWithFocus();
	void forceCloseAll();
	std::shared_ptr<const File> getCurrentFile();
	bool hasDirtyFiles();
	void saveAll();
	class FileEditorWnd* findFileWnd(const std::shared_ptr<const File>& file, int* index);
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

	FileEditorWnd* getCurrentPage();

	void iterateFiles(std::function<void(FileEditorWnd*)> func);

	// Used to remember what file tab we right clicked (for the popup menu)
	FileEditorWnd* m_selectedFileTab=nullptr;
	wxTimer m_timer;
};

} // namespace nutcracker

