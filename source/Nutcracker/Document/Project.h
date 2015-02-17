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
	virtual ~ProjectItem() {}
	UTF8String fullpath;
	UTF8String name;
	ProjectItemType type;
	ProjectItemId id;
	UTF8String getDirectory() const;
};

// Represents a file in the project
struct File : public ProjectItem
{
	explicit File(UTF8String fullpath_);
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
	bool looseFile = false;
	std::vector<ErrorInfo> errorLines;
};

struct ProjectItemCompare
{
	bool operator() (const std::shared_ptr<ProjectItem>& a, const std::shared_ptr<ProjectItem>& b)
	{
		if (a->type < b->type)
			return true;
		else
			return a->name < b->name;
	}
};

struct Folder : public ProjectItem
{
	explicit Folder(UTF8String fullpath_);
	virtual ~Folder() {}
	std::set<std::shared_ptr<ProjectItem>, ProjectItemCompare> items;
};


class Project
{
public:
	Project();
	~Project();

	void addFolder(const UTF8String& path);
	File* addLooseFile(const UTF8String& path);
	void resyncFolders();

	//! Gets the file object given the id
	File* getFile(ProjectItemId id);

	std::shared_ptr<const Folder> getRoot();

	void removeLooseFile(ProjectItemId id);
private:

	void populateFromDir(const std::shared_ptr<Folder>& folder);
	void add(const std::shared_ptr<Folder>& parent, const std::shared_ptr<ProjectItem>& item);

	std::shared_ptr<Folder> m_root;
	std::unordered_map < u32, std::weak_ptr<ProjectItem>> m_rootMap;
};

} // namespace document
} // namespace cz