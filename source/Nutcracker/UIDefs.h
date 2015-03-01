
#pragma once

namespace nutcracker
{
	class Interpreter;
	class Workspace;
	struct File;
	struct Breakpoint;

	class FileEditorWnd;
	extern class MainWnd* gMainWnd;
	extern bool gShuttingDown;
	extern class WorkspaceWnd* gWorkspaceWnd;
	extern class FileEditorGroupWnd* gFileEditorGroupWnd;
	extern class BreakpointsWnd* gBreakpointsWnd;
	extern wxImageList gImageListSmall;
	extern wxImageList gImageList32x32;
	extern std::shared_ptr<Workspace> gWorkspace;

	enum
	{
		SMALLIMG_IDX_FOLDER,
		SMALLIMG_IDX_FILE_OTHER,
		SMALLIMG_IDX_FILE_NUT,
		SMALLIMG_IDX_NUT,
		SMALLIMG_IDX_REFRESH,
		SMALLIMG_IDX_CALLSTACK_CURRENT,
		SMALLIMG_IDX_MAX
	};

	enum
	{
		BIGIMG_IDX_NUT,
		BIGIMG_MAX
	};

	cz::UTF8String wxStringToUtf8(const wxString& str);
	void showError(const char* title, const char* fmt, ...);
	void showMsg(const char* title, const char* fmt, ...);
	void showException(std::exception& e);

	struct UIState
	{
		std::vector<std::unique_ptr<Interpreter>> interpreters;
		Interpreter* currentInterpreter=nullptr;
	};

	extern std::unique_ptr<UIState> gUIState;
	extern std::unique_ptr<cz::Parameters> gParameters;

	#define CZ_TODO CZ_UNEXPECTED

} // namespace nutcracker

