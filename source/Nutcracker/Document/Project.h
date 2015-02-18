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
	explicit ProjectItem(ProjectItemType type_, UTF8String fullpath_);
	ProjectItem(const ProjectItem&) = delete;
	virtual ~ProjectItem();
	UTF8String fullpath;
	UTF8String name;
	ProjectItemType type;
	ProjectItemId id;
	UTF8String getDirectory() const;
};

std::shared_ptr<struct File> createFile(UTF8String fullpath);
std::shared_ptr<struct Folder> createFolder(UTF8String fullpath);

// Represents a file in the project
struct File : public ProjectItem
{
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
protected:
	friend std::shared_ptr<struct File> cz::document::createFile(UTF8String);
	explicit File(UTF8String fullpath_);
};

struct ProjectItemCompare
{
	bool operator() (const std::shared_ptr<ProjectItem>& a, const std::shared_ptr<ProjectItem>& b)
	{
		if (a->type != b->type)
			return a->type < b->type;
		else
			return a->name < b->name;
	}
};

struct Folder : public ProjectItem
{
	virtual ~Folder() {}
	std::set<std::shared_ptr<ProjectItem>, ProjectItemCompare> items;
protected:
	friend std::shared_ptr<struct Folder> createFolder(UTF8String);
	explicit Folder(UTF8String fullpath_);
};


class Project
{
public:
	Project();
	~Project();

	void addFolder(const UTF8String& path);
	std::shared_ptr<File> getFile(ProjectItemId id);
	std::shared_ptr<File> getFile(const UTF8String& path);

	void resyncFolders();
	std::shared_ptr<const Folder> getRoot();
private:

	void add(const std::shared_ptr<Folder>& parent, const std::shared_ptr<ProjectItem>& item);
	void populateFromDir(const std::shared_ptr<Folder>& folder);
	std::shared_ptr<Folder> m_root;
};

} // namespace document
} // namespace cz