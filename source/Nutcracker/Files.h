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

// Base object for workspace items
struct BaseItem : std::enable_shared_from_this<BaseItem>
{
	virtual ~BaseItem();
	UTF8String fullpath;
	UTF8String name;
	UTF8String name_lower; // lower case name
	UTF8String fullpath_lower; // lower case path
	ItemType type;
	FileId id;
	UTF8String getDirectory() const;
	Files *parent=nullptr;
	int level;
protected:
	explicit BaseItem(Files* parent_, ItemType type_, UTF8String fullpath_, int level);
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
	static std::shared_ptr<File> create(Files* parent_, UTF8String fullpath_, int level);

protected:
	friend class Files;
	friend class Workspace;
	explicit File(Files* parent_, UTF8String fullpath_, int level);
	std::function<bool(const std::shared_ptr<const File>&)> m_saveFunc;
};

// Used to sort files and folders
struct ProjectItemCompare
{
	bool operator() (const std::shared_ptr<BaseItem>& a, const std::shared_ptr<BaseItem>& b) const
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
	static std::shared_ptr<Folder> create(Files* parent_, UTF8String fullpath_, int level);
protected:
	friend Files;
	explicit Folder(Files* parent_, UTF8String fullpath_, int level);
};

// Manages all the files and folders
class Files
{
public:
	Files(Workspace* outer);
	~Files();

	// Retrieves an existing file object
	std::shared_ptr<File> getFile(FileId fileId);
	std::shared_ptr<Folder> getFolder(FileId folderId);
	std::shared_ptr<BaseItem> getItem(FileId itemId);

	// Adds a new file, or retrieves an existing one
	std::shared_ptr<File> createFile(UTF8String path);
	void addFolder(const UTF8String& path);
	void removeFolder(const UTF8String& path);
	void resyncFolders();
	std::shared_ptr<const Folder> getRoot();
	void iterateFiles(std::function<bool(const std::shared_ptr<const File>&)> func);
	bool hasDirtyFiles();

protected:
	friend BaseItem;
	friend File;
	friend Folder;
	friend Workspace;
	std::unordered_map<cz::u32, std::weak_ptr<BaseItem>> m_all;

	void _clearAll();

private:
	Workspace* m_outer;
	std::shared_ptr<Folder> m_root;
	std::shared_ptr<File> createFileImpl(UTF8String path, int level);
	void populateFromDir(const std::shared_ptr<Folder>& folder);
	bool iterateFilesHelper(const std::shared_ptr<const Folder>& folder, std::function<bool(const std::shared_ptr<const File>&)>& func);
};

} // namespace nutcracker

