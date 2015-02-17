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
	explicit ProjectItem(ProjectItemType type_, UTF8String fullpath);
	ProjectItem(const ProjectItem&) = delete;
	virtual ~ProjectItem() {}
	UTF8String fullpath;
	UTF8String name;
	ProjectItemType type;
	ProjectItemId id = 0;
	UTF8String getDirectory() const;
};

// Represents a file in the project
struct File : public ProjectItem
{
	File(UTF8String fullname);
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
	Folder(UTF8String fullpath);
	virtual ~Folder() {}
	std::set<std::shared_ptr<ProjectItem>> items;
};

struct ProjectItemCompare
{
	bool operator() (const std::shared_ptr<ProjectItem>& a, const std::shared_ptr<ProjectItem>& b)
	{
		if (a->type < b->type)
			return -1;
		else
			a->name < b->name;
	}
};


class Project
{
public:
	Project() { }
	~Project();

	//! Populates the project with the files found in the project's folder
	void populate();
	void populate(const UTF8String& root);
	void removeLooseFile(ProjectItemId fileId);

	//! Gets the file object given the id
	File* getFile(ProjectItemId id);

	std::shared_ptr<const Folder> getRoot();

	File* addLooseFile(const UTF8String& filename);
private:

	void setRoot(const UTF8String& root);
	void populateFromDir(std::shared_ptr<Folder> root, const UTF8String& dir);
	void add(const std::shared_ptr<Folder>& root, const std::shared_ptr<ProjectItem>& item);

	cz::UTF8String m_folder;
	std::shared_ptr<Folder> m_root;
	std::unordered_map < u32, std::weak_ptr<ProjectItem>> m_rootMap;
};

} // namespace document
} // namespace cz