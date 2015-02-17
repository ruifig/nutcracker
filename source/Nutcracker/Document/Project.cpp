#include "NutcrackerPCH.h"
#include "Project.h"


namespace cz
{
namespace document
{

//////////////////////////////////////////////////////////////////////////
// ProjectItem
//////////////////////////////////////////////////////////////////////////
ProjectItem::ProjectItem(ProjectItemType type_, UTF8String fullpath_)
	: type(type_), fullpath(std::move(fullpath_))
{
	id.val = FNVHash32::compute(fullpath.c_str(), fullpath.sizeBytes());
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
	name = fname.getBaseFilename();
	extension = ansiToLower(fname.getExtension());
	filetime = cz::FileTime::get(fullpath, FileTime::kModified);
}

//////////////////////////////////////////////////////////////////////////
// Folder
//////////////////////////////////////////////////////////////////////////
Folder::Folder(UTF8String fullpath_) : ProjectItem(ProjectItemType::Folder, std::move(fullpath_))
{
	name = Filesystem::getSingleton().pathStrip(fullpath);
}

//////////////////////////////////////////////////////////////////////////
// Project
//////////////////////////////////////////////////////////////////////////

Project::Project()
{
	m_root = std::make_shared < Folder >("");
}

Project::~Project()
{
}

void Project::add(const std::shared_ptr<Folder>& parent, const std::shared_ptr<ProjectItem>& item)
{
	CZ_CHECK(m_rootMap.find(item->id.val) == m_rootMap.end());
	parent->items.insert(item);
	m_rootMap[item->id.val] = item;
}

void Project::addFolder(const UTF8String& path)
{
	UTF8String fullpath;
	if (!Filesystem::getSingleton().fullPath(fullpath, path))
	{
		throw std::runtime_error(formatString("Error parsing folder '%s'", path.c_str()));
	}

	if (!Filesystem::getSingleton().isExistingDirectory(fullpath))
	{
		throw std::runtime_error(formatString("'%s' is not a folder", path.c_str()));
	}

	auto f = std::make_shared<Folder>(fullpath);
	auto existing = getFile(f->id);
	if (!existing)
	{
		add(m_root, f);
		populateFromDir(f);
	}
}

File* Project::addLooseFile(const UTF8String& path)
{
	UTF8String fullpath;
	if (!Filesystem::getSingleton().fullPath(fullpath, path))
	{
		throw std::runtime_error(formatString("Error parsing path '%s'", path.c_str()));
	}

	if (!Filesystem::getSingleton().isExistingFile(fullpath))
		return nullptr;

	auto f = std::make_shared<File>(fullpath);
	f->looseFile = true;

	// Check if there is a file with the same id. If so, it's the same file
	if (auto existing = getFile(f->id))
		return existing;

	add(m_root, f);
	return f.get();
}

void Project::resyncFolders()
{
	CZ_UNEXPECTED();
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
				cz::UTF8String path = folder->fullpath + f.name + "\\";
				auto item = std::make_shared<Folder>(path);
				add(folder, item);
				populateFromDir(item);
			}
			break;
			case Filesystem::Type::File:
				if (ansiToLower(Filename(f.name).getExtension()) == "nut")
				{
					auto item = std::make_shared<File>(folder->fullpath +f.name);
					add(folder, item);
				}
			break;
			default:
				CZ_UNEXPECTED();
		}
	}
}


File* Project::getFile(ProjectItemId id)
{
	auto it = m_rootMap.find(id.val);
	if (it == m_rootMap.end())
		return nullptr;

	auto item = it->second.lock().get();
	if (item->type != ProjectItemType::File)
	{
		return nullptr;
	}

	return static_cast<File*>(item);
}

std::shared_ptr<const Folder> Project::getRoot()
{
	return m_root;
}

void Project::removeLooseFile(ProjectItemId fileId)
{
	CZ_UNEXPECTED();
	/*
	auto file = getFile(fileId);
	if (!file || !file->looseFile)
		return;

	m_rootMap.erase(fileId.val);
	cz::erase_if(m_root->items,
		[&](const std::shared_ptr<ProjectItem>& item)
	{
		return item->id == fileId;
	});
	*/
}



} // namespace document
} // namespace cz

