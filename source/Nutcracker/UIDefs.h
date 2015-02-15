#pragma once


namespace cz
{

namespace document
{
	class Project;
	struct ProjectItem;
}

namespace view
{
	class FileEditorWnd;
	extern class WorkspaceWnd* gWorkspaceWnd;
	extern class FileEditorGroupWnd* gFileEditorGroupWnd;
	extern wxImageList gImageListSmall;
	extern std::shared_ptr<document::Project> gProject;

	enum
	{
		SMALLIMG_IDX_FOLDER,
		SMALLIMG_IDX_FILE_OTHER,
		SMALLIMG_IDX_FILE_NUT,
		SMALLIMG_IDX_MAX
	};

}

cz::UTF8String wxStringToUtf8(const wxString& str);
void showError(const char* title, const char* fmt, ...);
void showMsg(const char* title, const char* fmt, ...);
void showException(std::exception& e);

}

#define CZ_TODO CZ_UNEXPECTED
