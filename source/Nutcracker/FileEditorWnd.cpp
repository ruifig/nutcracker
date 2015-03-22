/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com
	
	purpose:
	
*********************************************************************/

#include "NutcrackerPCH.h"
#include "UIDefs.h"
#include "FileEditorWnd.h"
#include "FileEditorGroupWnd.h"
#include "FileEditorStyles.h"
#include "Workspace.h"
#include "BreakpointsWnd.h"

#include "art/xpm/breakpoint_on.xpm"
#include "art/xpm/breakpoint_off.xpm"
#include "art/xpm/breakpoint_invalid.xpm"
#include "art/xpm/debugcursor.xpm"

namespace nutcracker
{

enum
{
	MARGIN_LINE_NUMBERS=0,
	MARGIN_BREAKPOINTS,
	MARGIN_FOLD,
	ID_TextCtrl
};

// Markers
enum
{
	MARK_BREAKPOINT_ON=0,
	MARK_BREAKPOINT_OFF,
	MARK_BREAKPOINT_INVALID,
	MARK_DEBUGCURSOR
};


#define TO_TXTLINE(line) ((line)-1)
#define FROM_TXTLINE(line) ((line)+1)


BEGIN_EVENT_TABLE(FileEditorWnd, FileEditorWnd_Auto)
	EVT_STC_CHARADDED(ID_FileEditorTextCtrl, FileEditorWnd::OnCharacterAdded)
	EVT_STC_UPDATEUI(ID_FileEditorTextCtrl, FileEditorWnd::OnUpdateUI)
	EVT_STC_DOUBLECLICK(ID_FileEditorTextCtrl, FileEditorWnd::OnDoubleClick)
	EVT_STC_MODIFIED(ID_FileEditorTextCtrl, FileEditorWnd::OnTextChanged)
	EVT_STC_SAVEPOINTREACHED(ID_FileEditorTextCtrl, FileEditorWnd::OnTextSavePoint)
	EVT_CHAR_HOOK(FileEditorWnd::OnCharHook)
	EVT_TIMER(wxID_ANY, FileEditorWnd::OnTimer)
END_EVENT_TABLE()


FileEditorWnd::FileEditorWnd(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
	: FileEditorWnd_Auto(parent,id,pos,size,style)
{
	m_textCtrl->SetSizeHints(wxSize(640, 350));

	gWorkspace->registerListener(this, [this](const DataEvent& evt)
	{
		if (evt.id == DataEventID::BreakpointToggle || evt.id==DataEventID::BreakpointValidated || evt.id==DataEventID::BreakpointRemoved)
		{
			auto brk = static_cast<const BreakpointEvent&>(evt).brk;
			if (brk->file == m_file)
			{
				if (evt.id == DataEventID::BreakpointRemoved)
					deleteBreakpointMarkers(brk);
				else
					syncBreakpoint(brk);
			}
		}
		else if (evt.id == DataEventID::DebugStop || evt.id==DataEventID::DebugResume)
		{
			m_textCtrl->MarkerDeleteAll(MARK_DEBUGCURSOR);
		}
		else if (evt.isViewOptionsEvent())
		{
			updateViewOptions();
		}
		else if (evt.id == DataEventID::FindResultsClear)
		{

		}
	});
}

FileEditorWnd::~FileEditorWnd()
{
	gWorkspace->removeFileSaveFunc(m_file->id);
	gWorkspace->setFileDirty(m_file->id, false);
	gWorkspace->dropBreakpointChanges(m_file->id);
	gWorkspace->removeListener(this);
}

std::shared_ptr<const File> FileEditorWnd::getFile()
{
	return m_file;
}

void FileEditorWnd::setCommonStyle()
{
	m_textCtrl->SetMarginWidth(MARGIN_LINE_NUMBERS, 50);
	m_textCtrl->SetMarginType(MARGIN_LINE_NUMBERS, wxSTC_MARGIN_NUMBER);

	m_textCtrl->SetMarginWidth(MARGIN_BREAKPOINTS, 16);
	m_textCtrl->SetMarginType(MARGIN_BREAKPOINTS, wxSTC_MARGIN_SYMBOL);
	m_textCtrl->SetMarginSensitive(MARGIN_BREAKPOINTS, true);

	m_textCtrl->SetWrapMode(wxSTC_WRAP_NONE);

	m_textCtrl->MarkerDefineBitmap(MARK_BREAKPOINT_ON, wxImage(breakpoint_on_xpm));
	m_textCtrl->MarkerDefineBitmap(MARK_BREAKPOINT_OFF, wxImage(breakpoint_off_xpm));
	m_textCtrl->MarkerDefineBitmap(MARK_BREAKPOINT_INVALID, wxImage(breakpoint_invalid_xpm));
	m_textCtrl->MarkerDefineBitmap(MARK_DEBUGCURSOR, wxImage(debugcursor_xpm));

	// ---- Enable code folding
	m_textCtrl->SetMarginWidth(MARGIN_FOLD, 15);
	m_textCtrl->SetMarginType(MARGIN_FOLD, wxSTC_MARGIN_SYMBOL);
	m_textCtrl->SetMarginMask(MARGIN_FOLD, wxSTC_MASK_FOLDERS);
	m_textCtrl->SetMarginSensitive(MARGIN_FOLD, true);
	m_textCtrl->SetFoldMarginColour(true, wxColour(220, 220, 220));
	m_textCtrl->SetFoldMarginHiColour(true, clDefaultBkg);
	//wxColour foldMarginColour(clDefaultBkg);
	//m_textCtrl->StyleSetBackground(MARGIN_FOLD, foldMarginColour);

	// Properties found from http://www.scintilla.org/SciTEDoc.html
	m_textCtrl->SetProperty(wxT("fold"), wxT("1"));
	m_textCtrl->SetProperty(wxT("fold.comment"), wxT("1"));
	m_textCtrl->SetProperty(wxT("fold.compact"), wxT("1"));
	m_textCtrl->SetProperty(wxT("fold.preprocessor"), wxT("1"));
	// Disable automatic detection of inactive code due to #if #else #endif,
	// since lots of defines will be outside the file
	m_textCtrl->SetProperty("lexer.cpp.track.preprocessor", "0");

	wxColour grey(100, 100, 100);
	wxColour fcolour(240, 240, 240);
	m_textCtrl->MarkerDefine(wxSTC_MARKNUM_FOLDER, wxSTC_MARK_BOXPLUS);
	m_textCtrl->MarkerSetForeground(wxSTC_MARKNUM_FOLDER, fcolour);
	m_textCtrl->MarkerSetBackground(wxSTC_MARKNUM_FOLDER, grey);

	m_textCtrl->MarkerDefine(wxSTC_MARKNUM_FOLDEROPEN, wxSTC_MARK_BOXMINUS);
	m_textCtrl->MarkerSetForeground(wxSTC_MARKNUM_FOLDEROPEN, fcolour);
	m_textCtrl->MarkerSetBackground(wxSTC_MARKNUM_FOLDEROPEN, grey);

	m_textCtrl->MarkerDefine(wxSTC_MARKNUM_FOLDERSUB, wxSTC_MARK_VLINE);
	m_textCtrl->MarkerSetForeground(wxSTC_MARKNUM_FOLDERSUB, grey);
	m_textCtrl->MarkerSetBackground(wxSTC_MARKNUM_FOLDERSUB, grey);

	m_textCtrl->MarkerDefine(wxSTC_MARKNUM_FOLDEREND, wxSTC_MARK_BOXPLUSCONNECTED);
	m_textCtrl->MarkerSetForeground(wxSTC_MARKNUM_FOLDEREND, fcolour);
	m_textCtrl->MarkerSetBackground(wxSTC_MARKNUM_FOLDEREND, grey);

	m_textCtrl->MarkerDefine(wxSTC_MARKNUM_FOLDEROPENMID, wxSTC_MARK_BOXMINUSCONNECTED);
	m_textCtrl->MarkerSetForeground(wxSTC_MARKNUM_FOLDEROPENMID, fcolour);
	m_textCtrl->MarkerSetBackground(wxSTC_MARKNUM_FOLDEROPENMID, grey);

	m_textCtrl->MarkerDefine(wxSTC_MARKNUM_FOLDERMIDTAIL, wxSTC_MARK_TCORNER);
	m_textCtrl->MarkerSetForeground(wxSTC_MARKNUM_FOLDERMIDTAIL, grey);
	m_textCtrl->MarkerSetBackground(wxSTC_MARKNUM_FOLDERMIDTAIL, grey);

	m_textCtrl->MarkerDefine(wxSTC_MARKNUM_FOLDERTAIL, wxSTC_MARK_LCORNER);
	m_textCtrl->MarkerSetForeground(wxSTC_MARKNUM_FOLDERTAIL, grey);
	m_textCtrl->MarkerSetBackground(wxSTC_MARKNUM_FOLDERTAIL, grey);
	// ---- End of code folding part

	// Indentation settings
	auto tabWidth = m_textCtrl->GetTabWidth();
	m_textCtrl->SetUseTabs(true);
	m_textCtrl->SetIndent(4);
	m_textCtrl->SetTabWidth(4);
	m_textCtrl->SetTabIndents(true);
	m_textCtrl->SetBackSpaceUnIndents(true);

	m_textCtrl->SetWhitespaceForeground(true, gWhiteSpaceColour);

	// Set indicator styles
	for (int i = 0; i < (int)gIndicatorStyles.size(); i++)
	{
		m_textCtrl->IndicatorSetStyle(i, gIndicatorStyles[i].style);
		m_textCtrl->IndicatorSetForeground(i, gIndicatorStyles[i].colour);
		m_textCtrl->IndicatorSetAlpha(i, gIndicatorStyles[i].alpha);
	}

	m_textCtrl->SetCaretLineBackground(clCurrentLineBkg);
	m_textCtrl->SetCaretLineVisible(true);
}

void FileEditorWnd::setFile(const std::shared_ptr<const File>& file, int line, int col, bool columnIsOffset)
{
	if (m_file != file)
	{
		if (m_file)
			gWorkspace->removeFileSaveFunc(m_file->id);

		m_file = file;
		gWorkspace->setFileDirty(m_file->id, false);
		gWorkspace->setFileSaveFunc(m_file->id, [this](const std::shared_ptr<const File>& file)
		{
			return m_textCtrl->SaveFile(file->fullpath.widen());
		});

		// Find the language we want
		UTF8String ext = file->extension;
		LanguageInfo* lang = nullptr;
		for (auto& l : gLanguages)
		{
			if (cz::find(l.fileextensions, ext) != l.fileextensions.end())
			{
				lang = &l;
				break;
			}
		}

		m_textCtrl->Freeze();

		// Set the default shared style before StyleClearAll, because as specified in scintilla documentation,
		// StyleClearAll will reset all other styles to wxSTC_STYLE_DEFAULT
		setStyle(m_textCtrl, gSharedStyles[0]);
		m_textCtrl->StyleClearAll();
		if (lang)
			m_textCtrl->SetLexer(lang->lexer);
		setCommonStyle();
		setStyles(m_textCtrl, gSharedStyles);
		// Set styles specific to the language
		if (lang)
			setStyles(m_textCtrl, lang->styles);

		updateViewOptions();
	
		m_textCtrl->SetReadOnly(false);
		m_isLoading = true;
		m_textCtrl->LoadFile(file->fullpath.c_str(), wxTEXT_TYPE_ANY);
		m_isLoading = false;
		m_textCtrl->Connect(wxEVT_STC_MARGINCLICK, wxStyledTextEventHandler(FileEditorWnd::OnMarginClick), NULL, this);
		m_textCtrl->Thaw();
		m_textCtrl->SetSavePoint();
		m_textCtrl->EmptyUndoBuffer();

		gWorkspace->iterateBreakpoints(m_file->id, [&](const Breakpoint* brk)
		{
			syncBreakpoint(brk);
		});

		updateErrorMarkers();
		updateMarkers();

		m_textCtrl->Fit();
		this->Layout();
	}

	if (line >= 1 && col >= 0)
	{
		if (columnIsOffset)
		{
			// Column is a real offset into the text (doesn't take into account tab width)
			int pos = m_textCtrl->PositionFromLine(TO_TXTLINE(line));
			m_textCtrl->GotoPos(pos + col);
		} else{
			// Column is a visual offset into the text (takes into account tab width)
			int pos = m_textCtrl->FindColumn(TO_TXTLINE(line), col);
			m_textCtrl->GotoPos(pos);
			// There seems to be a bug with GotoPos, where the Horizontal scrolling gets out of sync, so I'm
			// just resetting it.
			// Maybe this bug: http://sourceforge.net/p/scintilla/bugs/1467/
			//This will of course mean that it will not scroll at all to show the desired column,
			// but (although the caret will still be at the desired column)
			m_textCtrl->SetXOffset(0);
		}
	}


	//m_textCtrl->VerticalCentreCaret();
}

void FileEditorWnd::checkReload()
{
	auto file = getFile();
	FileTime t = FileTime::get(file->fullpath, FileTime::kModified);
	if (t<= file->filetime)
		return;

	m_file.reset();
	setFile(file, -1, 0);
}
static int getBreakpointMarker(const Breakpoint* brk)
{
	if (gWorkspace->debuggerActive())
	{
		if (brk->enabled)
			return brk->valid ? MARK_BREAKPOINT_ON : MARK_BREAKPOINT_INVALID;
		else
			return MARK_BREAKPOINT_OFF;
	}
	else
	{
		return brk->enabled ? MARK_BREAKPOINT_ON : MARK_BREAKPOINT_OFF;
	}
}

void FileEditorWnd::deleteBreakpointMarkers(const Breakpoint* brk)
{
	m_textCtrl->MarkerDelete(TO_TXTLINE(brk->line), MARK_BREAKPOINT_INVALID);
	m_textCtrl->MarkerDelete(TO_TXTLINE(brk->line), MARK_BREAKPOINT_ON);
	m_textCtrl->MarkerDelete(TO_TXTLINE(brk->line), MARK_BREAKPOINT_OFF);
}

void FileEditorWnd::syncBreakpoint(const Breakpoint* brk)
{
	if (brk->markerHandle == -1)
	{
		CZ_ASSERT(brk->line != -1);
		deleteBreakpointMarkers(brk);
		gWorkspace->setBreakpointPos(
			brk, brk->line, m_textCtrl->MarkerAdd(TO_TXTLINE(brk->line), getBreakpointMarker(brk)));
	}
	else
	{
		int txtline = m_textCtrl->MarkerLineFromHandle(brk->markerHandle);
		gWorkspace->setBreakpointPos(brk, FROM_TXTLINE(txtline), brk->markerHandle);
		int markers = m_textCtrl->MarkerGet(txtline);
		if (brk->enabled && (markers& ((1<< MARK_BREAKPOINT_OFF) | (1<<MARK_BREAKPOINT_INVALID))))
		{
			deleteBreakpointMarkers(brk);
			gWorkspace->setBreakpointPos(brk, brk->line, m_textCtrl->MarkerAdd(TO_TXTLINE(brk->line), getBreakpointMarker(brk)));
		}
		else if (!brk->enabled && (markers&(1 << MARK_BREAKPOINT_ON)))
		{
			deleteBreakpointMarkers(brk);
			gWorkspace->setBreakpointPos(brk, brk->line, m_textCtrl->MarkerAdd(TO_TXTLINE(brk->line), MARK_BREAKPOINT_OFF));
		}
	}
}

void FileEditorWnd::syncBreakInfo(const BreakInfo& brk)
{
	if (brk.file != m_file)
		return;

	m_textCtrl->MarkerDeleteAll(MARK_DEBUGCURSOR);
	m_textCtrl->MarkerAdd(TO_TXTLINE(brk.line), MARK_DEBUGCURSOR);
	setFile(m_file, brk.line, 0);
}


void FileEditorWnd::OnMarginClick(wxStyledTextEvent& event)
{
	switch(event.GetMargin())
	{
		case MARGIN_BREAKPOINTS:
		{
			int txtline = m_textCtrl->LineFromPosition(event.GetPosition());
			int markers = m_textCtrl->MarkerGet(txtline);

			if (markers&((1<<MARK_BREAKPOINT_ON)|(1<<MARK_BREAKPOINT_INVALID)))
			{
				gWorkspace->removeBreakpoint(m_file->id, FROM_TXTLINE(txtline));
				m_textCtrl->MarkerDelete(txtline, MARK_BREAKPOINT_ON);
			}
			else if (markers&(1 << MARK_BREAKPOINT_OFF))
			{
				auto brk = gWorkspace->toggleBreakpoint(m_file->id, FROM_TXTLINE(txtline));
				CZ_ASSERT(brk->enabled);
				gWorkspace->setBreakpointPos(brk, brk->line, -1);
				syncBreakpoint(brk);
			}
			else
			{
				// If we are running, the breakpoint is set as invalid, until we get the confirmation from the target that
				// the breakpoint is ok
				auto handle =m_textCtrl->MarkerAdd(txtline, gWorkspace->debuggerActive() ? MARK_BREAKPOINT_INVALID : MARK_BREAKPOINT_ON);
				gWorkspace->addBreakpoint(m_file->id, FROM_TXTLINE(txtline), handle);
			}
			
		}
		break;
		case MARGIN_FOLD:
		{
			int txtline = m_textCtrl->LineFromPosition(event.GetPosition());
			int levelClick = m_textCtrl->GetFoldLevel(txtline);

			if ((levelClick & wxSTC_FOLDLEVELHEADERFLAG) > 0)
			{
				m_textCtrl->ToggleFold(txtline);
			}
		}
		break;
	}
}

void FileEditorWnd::updateMarkers()
{
	/*
	m_textCtrl->MarkerDeleteAll(MARK_BREAKPOINT_ON);
	m_textCtrl->MarkerDeleteAll(MARK_BREAKPOINT_OFF);
	m_textCtrl->MarkerDeleteAll(MARK_BREAKPOINT_INVALID);
	m_textCtrl->MarkerDeleteAll(MARK_DEBUGCURSOR);

	// TODO

	auto& debugpos = gSession->getLastSourcePos(m_file->isCSource());
	if (debugpos.first == m_file)
		m_textCtrl->MarkerAdd(debugpos.second-1, MARK_DEBUGCURSOR);
	*/
}

void FileEditorWnd::updateErrorMarkers()
{
	m_textCtrl->SetIndicatorCurrent(kINDIC_Warning);
	m_textCtrl->IndicatorClearRange(0, m_textCtrl->GetLength());
	m_textCtrl->SetIndicatorCurrent(kINDIC_Error);
	m_textCtrl->IndicatorClearRange(0, m_textCtrl->GetLength());

	auto file = getFile();

	for (auto& e : file->errorLines)
	{
		int line = e.line;
		m_textCtrl->SetIndicatorCurrent( e.isError ? kINDIC_Error : kINDIC_Warning);
		int startpos = m_textCtrl->PositionFromLine(TO_TXTLINE(line));
		int length = m_textCtrl->GetLineLength(TO_TXTLINE(line));
		m_textCtrl->IndicatorFillRange(startpos, length);
	}
}

void FileEditorWnd::OnTextChanged(wxStyledTextEvent& event)
{
	int flags = event.GetModificationType();
	if ((flags&wxSTC_MOD_INSERTTEXT) == wxSTC_MOD_INSERTTEXT ||
		(flags&wxSTC_MOD_DELETETEXT) == wxSTC_MOD_DELETETEXT)
	{
		if (!m_isLoading)
			gWorkspace->setFileDirty(m_file->id, true);
		gFileEditorGroupWnd->setPageTitle(m_file);

		gWorkspace->iterateBreakpoints(m_file->id, [&](const Breakpoint* brk)
		{
			if (brk->markerHandle != -1)
				gWorkspace->setBreakpointPos(brk, FROM_TXTLINE(m_textCtrl->MarkerLineFromHandle(brk->markerHandle)), brk->markerHandle);
		});
	}
}

void FileEditorWnd::OnTextSavePoint(wxStyledTextEvent& event)
{
	gWorkspace->setFileDirty(m_file->id, false);
	gFileEditorGroupWnd->setPageTitle(m_file);
}

void FileEditorWnd::OnUpdateUI(wxStyledTextEvent& event)
{
	//
	// Process brace matching
	// Based on http://permalink.gmane.org/gmane.comp.lib.scintilla.net/146
	//
	{
		auto isbrace = [](char ch)
		{
			return ch == ']' || ch == '[' || ch == '{' || ch == '}' || ch == '(' || ch == ')' || ch == '<' || ch == '>';
		};

		int brace_position = m_textCtrl->GetCurrentPos() - 1;
		char ch = (char)m_textCtrl->GetCharAt(brace_position);
		if (isbrace(ch))
		{
			int has_match = m_textCtrl->BraceMatch(brace_position);
			if (has_match > -1)
				m_textCtrl->BraceHighlight(has_match, brace_position);
			else
				m_textCtrl->BraceBadLight(brace_position);
		}
		else
		{
			brace_position = m_textCtrl->GetCurrentPos();
			char ch = (char)m_textCtrl->GetCharAt(brace_position);
			if (isbrace(ch))
			{
				int has_match = m_textCtrl->BraceMatch(brace_position);
				if (has_match > -1)
					m_textCtrl->BraceHighlight(has_match, brace_position);
				else
					m_textCtrl->BraceBadLight(brace_position);
			}
			else
			{
				m_textCtrl->BraceHighlight(-1, -1);
			}
		}
	}

	//
	// Disable word matching if we don't have a selection
	//
	if (!m_textCtrl->GetSelectedText().Length())
	{
		m_textCtrl->SetIndicatorCurrent(kINDIC_WordMatch);
		m_textCtrl->IndicatorClearRange(0, m_textCtrl->GetLength());
	}
}

void FileEditorWnd::OnDoubleClick(wxStyledTextEvent& event)
{
	int flags = wxSTC_FIND_WHOLEWORD|wxSTC_FIND_MATCHCASE;
	wxString txt = m_textCtrl->GetSelectedText().Trim();

	// The check for empty is required, as it seems if I double click on an empty line so that the selected text is empty, the loop
	// below will never finished.
	// The check for empty after trimming is not required, but there is no point in using it
	if (txt.size()==0)
		return;

	int pos=m_textCtrl->FindText(0, m_textCtrl->GetLength(), txt, flags );
	while(pos!=-1)
	{
		m_textCtrl->SetIndicatorCurrent(kINDIC_WordMatch);
		m_textCtrl->IndicatorFillRange(pos, txt.Length());
		pos=m_textCtrl->FindText(pos+1, m_textCtrl->GetLength(), txt, flags );
	}
}

void FileEditorWnd::onPageChanged()
{
	updateErrorMarkers();
	updateMarkers();
}

int FileEditorWnd::findCurrentWordStart(int pos, std::string& token)
{
	int col; // Column not taking into account the tab width
	m_textCtrl->GetCurLine(&col);
	int txtline = m_textCtrl->LineFromPosition(pos);

	auto isValidSymbolChar = [](char ch) {
		return ch == '_' || (ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z');
	};

	int start = pos;
	while (isValidSymbolChar(m_textCtrl->GetCharAt(start - 1)))
	{
		start--;
	}

	const char* data = m_textCtrl->GetCharacterPointer();
	token.append(data + start, data + pos);
	return (col - (token.size()))+1; // Scintilla lines/rows are 0 based, so add 1
}

void FileEditorWnd::showAutoComplete()
{
}

void FileEditorWnd::showCallTip(bool updatePos)
{
}

void FileEditorWnd::OnCharacterAdded(wxStyledTextEvent& event)
{
	char chr = (char)event.GetKey();
	int currentLine = m_textCtrl->GetCurrentLine();
	if (chr== '\n')
	{
		int lineInd = 0;
		if (currentLine>0)
			lineInd = m_textCtrl->GetLineIndentation(currentLine-1);
		if (lineInd==0)
			return;

		m_textCtrl->SetLineIndentation(currentLine, lineInd);
		m_textCtrl->LineEnd();
		return;
	}

	char previousChr = m_textCtrl->GetCharAt(m_textCtrl->GetCurrentPos() - 2 );
	if (chr == '.' || (chr == '>' && previousChr == '-'))
		showAutoComplete();
	else if (chr == '(')
		showCallTip(true);
	else if (chr == ')')
	{
		if (m_textCtrl->CallTipActive())
			m_textCtrl->CallTipCancel();
	}
	else if (m_textCtrl->CallTipActive())
		showCallTip(false);
}

void FileEditorWnd::save()
{
	auto file = getFile();
	if (!file)
		return;

	try {
		if (!gWorkspace->saveFile(m_file->id))
			throw std::runtime_error(cz::formatString("Error saving file '%s'", m_file->fullpath.c_str()));
	}
	catch(std::exception& e)
	{
		showException(e);
	}
}

void FileEditorWnd::getPositionForParser(int& line, int& column, bool adjustForSymbol)
{
	// The control gives as a zero-based line number. libclang expects 1.. based
	int currentChar = m_textCtrl->GetCharAt(m_textCtrl->GetCurrentPos());
	line = FROM_TXTLINE(m_textCtrl->GetCurrentLine());
	column = 0;
	// This gets us the true offset in the line, without taking into account the tab width, which is what libclang
	// expects
	m_textCtrl->GetCurLine(&column);

	// It seems if we place the cursor at the first character, libclang considers that position to belong to the
	// previous cursor, and not what what word represents, so we advance the column if the character to the right
	// is a valid start of a C identifier
	if (adjustForSymbol)
	{
		if ((currentChar >= 'A' && currentChar <= 'Z') || (currentChar >= 'a' && currentChar <= 'z') || currentChar == '_')
			column++;
	}
}

void FileEditorWnd::OnCharHook(wxKeyEvent& event)
{
	int modifiers = event.GetModifiers();
	auto keycode = event.GetKeyCode();

	if (modifiers == wxMOD_CONTROL && keycode == 'S') // Save
	{
		save();
	}
	else if (modifiers == wxMOD_CONTROL && keycode == 'R')
	{
		m_textCtrl->DiscardEdits();
		m_textCtrl->LoadFile(m_file->fullpath.widen());
	}
	/*
	else if (modifiers == wxMOD_CONTROL && keycode == WXK_F7) // Compile current file
	{
		try
		{
			buildutil::compileFile(BuildVariables(), m_file, wxStringToUtf8(gMainFrame->getActiveConfigurationName()));
			fireCustomAppEvent(CAE_BuildFinished);
		}
		catch (std::exception& e)
		{
			showException(e);
		}

		this->SetFocus();
	}
	else if (modifiers == wxMOD_CONTROL && keycode == '-') // Previous cursor position
	{
		gFileEditorGroupWnd->cursorHistory_Previous();
	}
	else if (modifiers == (wxMOD_CONTROL | wxMOD_SHIFT) && keycode == '-') // Next cursor position
	{
		gFileEditorGroupWnd->cursorHistory_Next();
	}
	else if (modifiers == wxMOD_CONTROL && keycode == ' ') // Explicitly shows auto complete
	{
		showAutoComplete();
	}
	else if (modifiers == (wxMOD_CONTROL | wxMOD_SHIFT) && keycode == ' ') // Explicitly show the calltip for function call
	{
		showCallTip(true);
	}
	else if (modifiers == wxMOD_ALT && keycode == 'O') // switch between C and H files
	{
		auto ext = m_file->getExtension();
		auto basename = Filename(m_file->name).getBaseFilename();
		UTF8String othername;
		if (ext == "c")
			othername = basename + ".h";
		else if (ext == "h")
			othername = basename + ".c";

		if (othername != "")
		{
			auto f = cz::apcpuide::gWorkspace->findFile(othername);
			if (f)
				gFileEditorGroupWnd->gotoFile(f, -1, -1);
		}
	}
	else if (modifiers == wxMOD_ALT && keycode == 'M') // Search symbols in current file
	{
		FindSymbolDlg dlg(this, m_file->getFullPath().c_str(), FINDSYMBOL_FUNCTIONS);
		dlg.setDelays(250, 250);
		dlg.setFilter("");
		dlg.ShowModal();
		UTF8String file; int line; int col;
		if (dlg.getResult(file, line, col))
			gFileEditorGroupWnd->gotoFile(file, line, col);
	}
	else if (modifiers == (wxMOD_CONTROL | wxMOD_SHIFT) && keycode == 'G') // Open header file under cursor (in #include statement)
	{
		int line, column;
		getPositionForParser(line, column);
		if (column == 0)
			column++;
		auto filename =
			gWorkspace->getCParser().findHeaderAtPos(m_file->getFullPath().c_str(), line, column);
		if (filename != "")
		{
			gFileEditorGroupWnd->cursorHistory_AddHistory(gFileEditorGroupWnd->getCurrentLocation());
			gFileEditorGroupWnd->gotoFile(UTF8String(filename.c_str()));
		}
	}
	else if (modifiers == (wxMOD_CONTROL | wxMOD_ALT) && (keycode == WXK_F11 || keycode == WXK_F12)) // Goto declaration
	{
		int line, column;
		getPositionForParser(line, column);
		cparser::Location2 loc =
			gWorkspace->getCParser().findDeclaration(m_file->getFullPath().c_str(), line, column);
		if (loc.isValid())
		{
			gFileEditorGroupWnd->cursorHistory_AddHistory(gFileEditorGroupWnd->getCurrentLocation());
			gFileEditorGroupWnd->gotoFile(UTF8String(loc.filename.c_str()), loc.line, loc.column - 1, true);
		}
	}
	else if (modifiers == 0 && (keycode == WXK_F11 || keycode == WXK_F12)) // Goto definition
	{
		int line, column;
		getPositionForParser(line, column);
		cparser::Location2 loc =
			gWorkspace->getCParser().findDefinition(m_file->getFullPath().c_str(), line, column);
		if (loc.isValid())
		{
			gFileEditorGroupWnd->cursorHistory_AddHistory(gFileEditorGroupWnd->getCurrentLocation());
			gFileEditorGroupWnd->gotoFile(UTF8String(loc.filename.c_str()), loc.line, loc.column - 1, true);
		}
	}
	*/
	else // Skip this event (nothing we have to do)
	{
		event.Skip();
	}
}

std::pair<int, int> FileEditorWnd::getCursorLocation()
{
	return std::make_pair(FROM_TXTLINE(m_textCtrl->GetCurrentLine()), m_textCtrl->GetColumn(m_textCtrl->GetCurrentPos()));
}

void FileEditorWnd::setFocusToEditor()
{
	m_textCtrl->SetFocus();
}

bool FileEditorWnd::editorHasFocus()
{
	return m_textCtrl->HasFocus();
}

void FileEditorWnd::updateViewOptions()
{
	auto options = gWorkspace->getViewOptions();
	m_textCtrl->SetIndentationGuides(options->view_indentation ? wxSTC_IV_LOOKBOTH : wxSTC_IV_NONE);
	m_textCtrl->SetViewWhiteSpace( options->view_whitespaces ? wxSTC_WS_VISIBLEALWAYS : wxSTC_WS_INVISIBLE);
	m_textCtrl->SetViewEOL( options->view_eol ? true : false);
	m_textCtrl->SetEdgeMode(wxSTC_EDGE_LINE);
	m_textCtrl->SetEdgeColumn(80);
	m_textCtrl->SetEdgeColour(wxColour(210,210,210));
}

void FileEditorWnd::recolourise(bool reparse)
{
	/*
	m_textCtrl->PrivateLexerCall((int)cparser::LexerCall::Enable, (void*)(reparse ? 1 : 0));
	m_textCtrl->Colourise(0, m_textCtrl->GetLength());
	m_textCtrl->PrivateLexerCall((int)cparser::LexerCall::Disable, nullptr);
	*/
}

void FileEditorWnd::OnTimer(wxTimerEvent& evt)
{
	if (getFile()->extension=="nut")
	{
		// TODO
		/*
		void *res = m_textCtrl->PrivateLexerCall((int)cparser::LexerCall::GetNeedsUpdate, nullptr);
		if (res != (void*)(0))
		{
			recolourise(true);
		}
		*/
	}
}

wxString FileEditorWnd::getWordUnderCursor()
{
	return m_textCtrl->GetTextRange(m_textCtrl->WordStartPosition(m_textCtrl->GetCurrentPos(), true),
									m_textCtrl->WordEndPosition(m_textCtrl->GetCurrentPos(), true));
}

} // namespace nutcracker


