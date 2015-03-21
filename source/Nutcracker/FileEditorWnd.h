/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com
	
	purpose:
	
*********************************************************************/

#pragma once

#include "AutoUI/FileEditorWnd_Auto.h"
#include "AppEvents.h"
#include "Interpreter.h"

namespace nutcracker
{

class FileEditorWnd : public FileEditorWnd_Auto
{
public:
	FileEditorWnd(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxDefaultSize, long style = 0);
	virtual ~FileEditorWnd();

	std::shared_ptr<const File> getFile();
	void setFile(const std::shared_ptr<const File>& file, int line=0, int col=0, bool columnIsOffset=false);
	void setFocusToEditor();
	wxString getWordUnderCursor();
	bool editorHasFocus();
	void checkReload();
	void save();
	void onPageChanged();
	std::pair<int, int> getCursorLocation();
	void syncBreakInfo(const BreakInfo& brk);
	void deleteBreakpointMarkers(const Breakpoint* brk);

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

	void syncBreakpoint(const Breakpoint* brk);
	void setCommonStyle();
	void updateViewOptions();
	void showAutoComplete();
	void showCallTip(bool updatePos);
	void updateErrorMarkers();
	void updateMarkers();
	int findCurrentWordStart(int pos, std::string& token);
	void getPositionForParser(int& line, int& column, bool adjustForSymbol);
	void recolourise(bool reparse);

	std::shared_ptr<const File> m_file;
	bool m_isLoading = false; // used to detect if we are loading the file, and thus ignore some events
};

} // namespace nutcracker


