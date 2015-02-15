/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com
	
	purpose:
	
*********************************************************************/

#include "AutoUI/FileEditorWnd_Auto.h"
#include "Document/Project.h"
#include "AppEvents.h"

namespace cz
{
namespace view
{

class FileEditorWnd : public FileEditorWnd_Auto, public AppEventListener
{
public:
	FileEditorWnd(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxDefaultSize, long style = 0);
	virtual ~FileEditorWnd();

	document::ProjectItemId getFileId();
	document::File* getFile();
	void setFile(document::File* file, int line=0, int col=0, bool columnIsOffset=false);
	void setFocusToEditor();
	wxString getWordUnderCursor();
	bool editorHasFocus();
	void checkReload();
	void save();
	void onPageChanged();
	std::pair<int, int> getCursorLocation();
private:
	DECLARE_EVENT_TABLE()
	void OnCharacterAdded(wxStyledTextEvent& evt);
	void OnUpdateUI(wxStyledTextEvent& evt);
	void OnDoubleClick(wxStyledTextEvent& evt);
	void OnTextChanged(wxStyledTextEvent& evt);
	void OnTextSavePoint(wxStyledTextEvent& evt);
	void OnCharHook(wxKeyEvent& event);
	void OnMarginClick(wxStyledTextEvent& event);
	void OnTimer(wxTimerEvent& evt);

	void setCommonStyle();
	void updateViewOptions();
	virtual void onAppEvent(const AppEvent& evt);
	void showAutoComplete();
	void showCallTip(bool updatePos);
	void updateErrorMarkers();
	void updateMarkers();
	int findCurrentWordStart(int pos, std::string& token);
	void getPositionForParser(int& line, int& column, bool adjustForSymbol);
	void recolourise(bool reparse);

	document::ProjectItemId m_fileId = 0;

};

} // namespace view
} // namespace cz


