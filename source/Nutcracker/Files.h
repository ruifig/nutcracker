/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com
	
	purpose:
	Manages all the workspace files
	
*********************************************************************/
#pragma once

namespace nutcracker
{

enum class ItemType
{
	Folder,
	File
};

typedef FNVHash32 FileId;
class Files;
struct File;

struct Location
{
	std::shared_ptr<File> file;
	int line=0;
};

// Base object for workspace items
struct BaseItem : std::enable_shared_from_this<BaseItem>
{
	virtual ~BaseItem();
	UTF8String fullpath;
	UTF8String name;
	ItemType type;
	FileId id;
	UTF8String getDirectory() const;
	Files *parent=nullptr;
protected:
	explicit BaseItem(Files* parent_, ItemType type_, UTF8String fullpath_);
	BaseItem(const BaseItem&) = delete;
	void addToParent();
};

// Represents a file in the project
struct File : public BaseItem
{
	virtual ~File();
	UTF8String extension;
	FileTime filetime;
	bool dirty = false;

	struct ErrorInfo
	{
		int line;
		bool isError;
		UTF8String desc;
	};
	std::vector<ErrorInfo> errorLines;
	static std::shared_ptr<File> create(Files* parent_, UTF8String fullpath_);

protected:
	friend class Files;
	explicit File(Files* parent_, UTF8String fullpath_);
};

// Used to sort files and folders
struct ProjectItemCompare
{
	bool operator() (const std::shared_ptr<BaseItem>& a, const std::shared_ptr<BaseItem>& b)
	{
		if (a->type != b->type)
			return a->type < b->type;
		else
			return a->name < b->name;
	}
};

// A folder in the workspace
struct Folder : public BaseItem
{
	virtual ~Folder() {}
	std::set<std::shared_ptr<BaseItem>, ProjectItemCompare> items;
	static std::shared_ptr<Folder> create(Files* parent_, UTF8String fullpath_);
protected:
	friend Files;
	explicit Folder(Files* parent_, UTF8String fullpath_);
};

// Manages all the files and folders
class Files
{
public:
	Files();
	~Files();

	// Retrieves an existing file object
	std::shared_ptr<File> getFile(FileId fileId);

	// Adds a new file, or retrieves an existing one
	std::shared_ptr<File> createFile(UTF8String path);
	void addFolder(const UTF8String& path);
	void resyncFolders();
	std::shared_ptr<const Folder> getRoot();

protected:
	friend BaseItem;
	friend File;
	friend Folder;
	std::unordered_map<cz::u32, std::weak_ptr<BaseItem>> m_all;

private:
	std::shared_ptr<Folder> m_root;
	void populateFromDir(const std::shared_ptr<Folder>& folder);
};

} // namespace nutcracker

