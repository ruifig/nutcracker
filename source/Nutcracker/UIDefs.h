#pragma once


namespace cz
{

namespace document
{
	class Project;
	struct ProjectItem;
	class Interpreter;
}

namespace view
{
	class FileEditorWnd;
	extern class MainWnd* gMainWnd;
	extern class WorkspaceWnd* gWorkspaceWnd;
	extern class FileEditorGroupWnd* gFileEditorGroupWnd;
	extern wxImageList gImageListSmall;
	extern wxImageList gImageList32x32;
	extern std::shared_ptr<document::Project> gProject;

	enum
	{
		SMALLIMG_IDX_FOLDER,
		SMALLIMG_IDX_FILE_OTHER,
		SMALLIMG_IDX_FILE_NUT,
		SMALLIMG_IDX_NUT,
		SMALLIMG_IDX_REFRESH,
		SMALLIMG_IDX_MAX
	};

	enum
	{
		BIGIMG_IDX_NUT,
		BIGIMG_MAX
	};

}

cz::UTF8String wxStringToUtf8(const wxString& str);
void showError(const char* title, const char* fmt, ...);
void showMsg(const char* title, const char* fmt, ...);
void showException(std::exception& e);

struct UIState
{
	bool view_ShowIndentationGuides = false;
	bool view_Whitespace = false;
	bool view_EOL = false;
	std::vector<std::unique_ptr<document::Interpreter>> interpreters;
	document::Interpreter* currentInterpreter=nullptr;
};

extern std::unique_ptr<UIState> uiState;
extern std::unique_ptr<cz::Parameters> gParameters;

}

#define CZ_TODO CZ_UNEXPECTED
