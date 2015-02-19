#include "NutcrackerPCH.h"
#include "Project.h"


namespace cz
{
namespace document
{

static std::unordered_map<u32, std::weak_ptr<ProjectItem>> gAll;
std::shared_ptr<struct File> createFile(UTF8String fullpath)
{
	// If this file is references somewhere (as for example a file editor window has it open, then use that
	// object)
	u32 id = FNVHash32::compute(fullpath.c_str(), fullpath.sizeBytes());
	auto existing = gAll.find(id);
	if (existing != gAll.end())
		return std::static_pointer_cast<File>(existing->second.lock());

	auto f = std::shared_ptr<File>(new File(std::move(fullpath)));
	gAll[f->id.val] = f;
	return f;
}

std::shared_ptr<struct Folder> createFolder(UTF8String fullpath)
{
	auto f = std::shared_ptr<Folder>(new Folder(std::move(fullpath)));
	CZ_ASSERT(gAll.find(f->id.val) == gAll.end());
	gAll[f->id.val] = f;
	return f;
}

//////////////////////////////////////////////////////////////////////////
// ProjectItem
//////////////////////////////////////////////////////////////////////////
ProjectItem::ProjectItem(ProjectItemType type_, UTF8String fullpath_)
	: type(type_), fullpath(std::move(fullpath_))
{
	id.val = FNVHash32::compute(fullpath.c_str(), fullpath.sizeBytes());
}

ProjectItem::~ProjectItem()
{
	gAll.erase(id.val);
}

cz::UTF8String ProjectItem::getDirectory() const
{
	if (type == ProjectItemType::Folder)
		return fullpath;
	else
		return Filename(fullpath).getDirectory();
}


//////////////////////////////////////////////////////////////////////////
// File
//////////////////////////////////////////////////////////////////////////
File::File(UTF8String fullpath_) : ProjectItem(ProjectItemType::File, std::move(fullpath_))
{
	auto fname = Filename(fullpath);
	name = fname.getFilename();
	extension = ansiToLower(fname.getExtension());
	filetime = cz::FileTime::get(fullpath, FileTime::kModified);
	//czDebug(ID_Log, "%s (%s) : %u", fullpath.c_str(), name.c_str(), id.val);
}

//////////////////////////////////////////////////////////////////////////
// Folder
//////////////////////////////////////////////////////////////////////////
Folder::Folder(UTF8String fullpath_) : ProjectItem(ProjectItemType::Folder, std::move(fullpath_))
{
	name = Filesystem::getSingleton().pathStrip(fullpath);
	//czDebug(ID_Log, "%s (%s) : %u", fullpath.c_str(), name.c_str(), id.val);
}

//////////////////////////////////////////////////////////////////////////
// Project
//////////////////////////////////////////////////////////////////////////

Project::Project()
{
	m_root = createFolder("");
}

Project::~Project()
{
}

void Project::add(const std::shared_ptr<Folder>& parent, const std::shared_ptr<ProjectItem>& item)
{
	parent->items.insert(item);
}

void Project::addFolder(const UTF8String& path)
{
	UTF8String fullpath;
	if (!Filesystem::getSingleton().fullPath(fullpath, Filename(path).removeBackslash()))
	{
		throw std::runtime_error(formatString("Error parsing folder '%s'", path.c_str()));
	}

	if (!Filesystem::getSingleton().isExistingDirectory(fullpath))
	{
		throw std::runtime_error(formatString("'%s' is not a folder", path.c_str()));
	}

	ProjectItemId id(FNVHash32::compute(fullpath.c_str(), fullpath.sizeBytes()));
	if (gAll.find(id.val) != gAll.end())
		return;

	auto f = createFolder(fullpath);
	auto existing = getFile(f->id);
	if (!existing)
	{
		add(m_root, f);
		populateFromDir(f);
	}
}

std::shared_ptr<File> Project::getFile(ProjectItemId fileId)
{
	auto it = gAll.find(fileId.val);
	if (it == gAll.end())
		return nullptr;

	auto item = it->second.lock();
	if (item->type != ProjectItemType::File)
		return nullptr;

	return std::static_pointer_cast<File>(item);
}

std::shared_ptr<File> Project::getFile(const UTF8String& path)
{
	UTF8String fullpath;
	if (!Filesystem::getSingleton().fullPath(fullpath, path))
		throw std::runtime_error(formatString("Error parsing path '%s'", path.c_str()));

	if (!Filesystem::getSingleton().isExistingFile(fullpath))
		return nullptr;

	ProjectItemId id(FNVHash32::compute(fullpath.c_str(), fullpath.sizeBytes()));
	// Check if this file is referenced somewhere already
	auto f = getFile(id);
	if (f)
		return f;

	// Not referenced, so it's a new file
	f = createFile(fullpath);
	return f;
}

void Project::resyncFolders()
{
	// Keep the names of first level folders, so we know what to refresh
	std::vector<UTF8String> folders;
	for (const auto n : m_root->items)
		folders.push_back(n->fullpath);

	// By doing this, we drop all our references to files/folders,
	// but other components still keep the references to any files they are using (such as files
	// being edited at the moment)
	m_root.reset();
	m_root = createFolder("");

	for (auto& n : folders)
		addFolder(n);

}

void Project::populateFromDir(const std::shared_ptr<Folder>& folder)
{
	auto files = Filesystem::getFilesInDirectory(folder->fullpath, "*", true);

	for (auto& f : files)
	{
		switch (f.type)
		{
			case Filesystem::Type::Directory:
			{
				cz::UTF8String path = folder->fullpath + "\\" + f.name;
				auto item = createFolder(path);
				add(folder, item);
				populateFromDir(item);
			}
			break;
			case Filesystem::Type::File:
				if (ansiToLower(Filename(f.name).getExtension()) == "nut")
				{
					auto item = createFile(folder->fullpath + "\\" + f.name);
					add(folder, item);
				}
			break;
			default:
				CZ_UNEXPECTED();
		}
	}
}

std::shared_ptr<const Folder> Project::getRoot()
{
	return m_root;
}

} // namespace document
} // namespace cz

