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

namespace cz
{
namespace view
{

BEGIN_EVENT_TABLE(FileEditorWnd, FileEditorWnd_Auto)
	EVT_STC_CHARADDED(ID_FileEditorTextCtrl, FileEditorWnd::OnCharacterAdded)
	EVT_STC_UPDATEUI(ID_FileEditorTextCtrl, FileEditorWnd::OnUpdateUI)
	EVT_STC_DOUBLECLICK(ID_FileEditorTextCtrl, FileEditorWnd::OnDoubleClick)
	EVT_STC_MODIFIED(ID_FileEditorTextCtrl, FileEditorWnd::OnTextChanged)
	EVT_STC_SAVEPOINTREACHED(ID_FileEditorTextCtrl, FileEditorWnd::OnTextSavePoint)
	EVT_CHAR_HOOK(FileEditorWnd::OnCharHook)
	EVT_TIMER(wxID_ANY, FileEditorWnd::OnTimer)
END_EVENT_TABLE()


using namespace cz::document;

FileEditorWnd::FileEditorWnd(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
	: FileEditorWnd_Auto(parent,id,pos,size,style)
{
}

FileEditorWnd::~FileEditorWnd()
{
	if (auto file = getFile())
	{
		file->dirty = false;
	}
}

ProjectItemId FileEditorWnd::getFileId()
{
	return m_fileId;
}

document::File* FileEditorWnd::getFile()
{
	return gProject->getFile(m_fileId);
}

void FileEditorWnd::setFile(document::File* file, int line, int col, bool columnIsOffset)
{
	if (m_fileId.val == 0)
	{
		m_fileId = file->id;


		m_textCtrl->Connect(wxEVT_STC_MARGINCLICK, wxStyledTextEventHandler(FileEditorWnd::OnMarginClick), NULL, this);
		file->dirty = false;
		m_textCtrl->Freeze();
	
		m_textCtrl->SetReadOnly(false);
		m_textCtrl->LoadFile(file->fullname.c_str(), wxTEXT_TYPE_ANY);
		file->filetime = cz::FileTime::get(file->fullname, FileTime::kModified);

		m_textCtrl->Thaw();
		m_textCtrl->SetSavePoint();
		m_textCtrl->EmptyUndoBuffer();

		m_textCtrl->Fit();
		this->Layout();
	}

	if (line >= 0 && col >= 0)
	{
		if (columnIsOffset)
		{
			// Column is a real offset into the text (doesn't take into account tab width)
			int pos = m_textCtrl->PositionFromLine(line - 1);
			m_textCtrl->GotoPos(pos + col);
		} else{
			// Column is a visual offset into the text (takes into account tab width)
			m_textCtrl->GotoPos(m_textCtrl->FindColumn(line - 1, col));
		}
	}

	m_textCtrl->VerticalCentreCaret();
}

void FileEditorWnd::checkReload()
{
	auto file = getFile();
	cz::FileTime t = cz::FileTime::get(file->fullname, FileTime::kModified);
	if (t<= file->filetime)
		return;

	m_fileId.val = 0;
	setFile(file, -1, 0);
}

void FileEditorWnd::OnMarginClick(wxStyledTextEvent& event)
{
	CZ_TODO();
}

void FileEditorWnd::updateErrorMarkers()
{
}

void FileEditorWnd::updateMarkers()
{
}

void FileEditorWnd::OnTextChanged(wxStyledTextEvent& event)
{
	int flags = event.GetModificationType();
	if ((flags&wxSTC_MOD_INSERTTEXT) == wxSTC_MOD_INSERTTEXT ||
		(flags&wxSTC_MOD_DELETETEXT) == wxSTC_MOD_DELETETEXT)
	{
		auto file = getFile();
		file->dirty = true;
		gFileEditorGroupWnd->setPageTitle(file);
	}
}

void FileEditorWnd::OnTextSavePoint(wxStyledTextEvent& event)
{
	auto file = getFile();
	file->dirty = false;
	gFileEditorGroupWnd->setPageTitle(file);
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
		// TODO
		//m_textCtrl->SetIndicatorCurrent(kINDIC_WordMatch);
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
		CZ_TODO();
		//m_textCtrl->SetIndicatorCurrent(kINDIC_WordMatch);
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
	int line = m_textCtrl->LineFromPosition(pos);

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
		m_textCtrl->SaveFile(file->fullname.widen());
		file->dirty = false;
		file->filetime = FileTime::get(file->fullname, FileTime::kModified);
		fireAppEvent(AppEventFileSaved(this));
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
	line = m_textCtrl->GetCurrentLine() + 1;
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
	/*
	else if (modifiers == wxMOD_CONTROL && keycode == WXK_F7) // Compile current file
	{
		try
		{
			buildutil::compileFile(document::BuildVariables(), m_file, wxStringToUtf8(gMainFrame->getActiveConfigurationName()));
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
		cz::UTF8String othername;
		if (ext == "c")
			othername = basename + ".h";
		else if (ext == "h")
			othername = basename + ".c";

		if (othername != "")
		{
			auto f = cz::apcpuide::document::gWorkspace->findFile(othername);
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
			document::gWorkspace->getCParser().findHeaderAtPos(m_file->getFullPath().c_str(), line, column);
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
			document::gWorkspace->getCParser().findDeclaration(m_file->getFullPath().c_str(), line, column);
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
			document::gWorkspace->getCParser().findDefinition(m_file->getFullPath().c_str(), line, column);
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
	return std::make_pair(m_textCtrl->GetCurrentLine()+1, m_textCtrl->GetColumn(m_textCtrl->GetCurrentPos()));
}

void FileEditorWnd::setFocusToEditor()
{
	m_textCtrl->SetFocus();
}

bool FileEditorWnd::editorHasFocus()
{
	return m_textCtrl->HasFocus();
}

void FileEditorWnd::onAppEvent(const AppEvent& evt)
{
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

} // namespace view
} // namespace cz


