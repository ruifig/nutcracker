#pragma once

namespace cz
{
namespace document
{


// Forward declarations
struct File;
class Project;

typedef std::shared_ptr<File> FilePtr;
typedef std::shared_ptr<const File> ConstFilePtr;

enum class ProjectItemType
{
	Folder,
	File
};

typedef FNVHash32 ProjectItemId;

struct ProjectItem
{
	explicit ProjectItem(ProjectItemType type_, UTF8String fullname_, UTF8String name_);
	ProjectItem(const ProjectItem&) = delete;
	virtual ~ProjectItem() {}
	UTF8String fullname;
	UTF8String absolutepath;
	UTF8String name;
	ProjectItemType type;
	ProjectItemId id = 0;
};

// Represents a file in the project
struct File : public ProjectItem
{
	File(UTF8String fullname, UTF8String name);
	virtual ~File() {}
	UTF8String extension;
	FileTime filetime;
	bool dirty = false;

	struct ErrorInfo
	{
		int line;
		bool isError;
		cz::UTF8String desc;
	};
	std::vector<ErrorInfo> errorLines;
};

struct Folder : public ProjectItem
{
	Folder(UTF8String fullname, UTF8String name) : ProjectItem(ProjectItemType::Folder, fullname, name) {}
	virtual ~Folder() {}
	std::vector<std::shared_ptr<ProjectItem>> items;
};

class Project
{
public:
	explicit Project(UTF8String folder);
	~Project();

	//! Populates the project with the files found in the project's folder
	void populate();

	//! Gets the file object given the id
	File* getFile(ProjectItemId id);

	std::shared_ptr<const Folder> getRoot();

private:

	void populateFromDir(std::shared_ptr<Folder> root, const UTF8String& dir);

	cz::UTF8String m_folder;
	std::shared_ptr<Folder> m_root;
	std::unordered_map < u32, std::weak_ptr<ProjectItem>> m_rootMap;
};

} // namespace document
} // namespace cz