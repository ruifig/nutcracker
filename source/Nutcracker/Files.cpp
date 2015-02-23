/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com
	
	purpose:
	
*********************************************************************/

#include "NutcrackerPCH.h"
#include "Files.h"

namespace nutcracker
{

using namespace cz;

FileId computeId(const UTF8String& str)
{
	return FileId((void*)str.c_str(), str.sizeBytes());
}

//////////////////////////////////////////////////////////////////////////
// BaseItem
//////////////////////////////////////////////////////////////////////////
BaseItem::BaseItem(Files* parent_, ItemType type_, UTF8String fullpath_)
	: parent(parent_), type(type_), fullpath(std::move(fullpath_))
{
	id.val = FNVHash32::compute(fullpath.c_str(), fullpath.sizeBytes());
}

BaseItem::~BaseItem()
{
	parent->m_all.erase(id.val);
}

cz::UTF8String BaseItem::getDirectory() const
{
	if (type == ItemType::Folder)
		return fullpath;
	else
		return Filename(fullpath).getDirectory();
}

void BaseItem::addToParent()
{
	CZ_ASSERT(parent->m_all.find(id.val) == parent->m_all.end());
	parent->m_all[id.val] = shared_from_this();
}

//////////////////////////////////////////////////////////////////////////
// File
//////////////////////////////////////////////////////////////////////////
File::File(Files* parent_, UTF8String fullpath_)
	: BaseItem(parent_, ItemType::File, std::move(fullpath_))
{
	auto fname = Filename(fullpath);
	name = fname.getFilename();
	extension = ansiToLower(fname.getExtension());
	filetime = cz::FileTime::get(fullpath, FileTime::kModified);
	//czDebug(ID_Log, "%s (%s) : %u", fullpath.c_str(), name.c_str(), id.val);
}

File::~File()
{
}

std::shared_ptr<File> File::create(Files* parent_, UTF8String fullpath_)
{
	auto ptr = std::shared_ptr<File>(new File(parent_, std::move(fullpath_)));
	ptr->addToParent();
	return std::move(ptr);
}

//////////////////////////////////////////////////////////////////////////
// Folder
//////////////////////////////////////////////////////////////////////////
Folder::Folder(Files* parent_, UTF8String fullpath_)
	: BaseItem(parent_, ItemType::Folder, std::move(fullpath_))
{
	name = Filesystem::getSingleton().pathStrip(fullpath);
}

std::shared_ptr<Folder> Folder::create(Files* parent_, UTF8String fullpath_)
{
	auto ptr = std::shared_ptr<Folder>(new Folder(parent_, std::move(fullpath_)));
	ptr->addToParent();
	return std::move(ptr);
}

//////////////////////////////////////////////////////////////////////////
// Files
//////////////////////////////////////////////////////////////////////////

Files::Files()
{
	m_root = Folder::create(this, "");
}

Files::~Files()
{
}

std::shared_ptr<File> Files::getFile(FileId fileId)
{
	auto it = m_all.find(fileId.val);
	if (it == m_all.end())
		return nullptr;

	auto item = it->second.lock();
	if (item->type != ItemType::File)
		return nullptr;

	return std::static_pointer_cast<File>(item);
}

void Files::addFolder(const UTF8String& path)
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

	FileId id(FNVHash32::compute(fullpath.c_str(), fullpath.sizeBytes()));
	if (m_all.find(id.val) != m_all.end())
		return;

	auto existing = getFile(computeId(fullpath));
	if (!existing)
	{
		auto f = Folder::create(this, fullpath);
		m_root->items.insert(f);
		populateFromDir(f);
	}
}

std::shared_ptr<File> Files::createFile(UTF8String path)
{
	UTF8String fullpath;
	if (!Filesystem::getSingleton().fullPath(fullpath, path))
		throw std::runtime_error(formatString("Error parsing path '%s'", path.c_str()));

	if (!Filesystem::getSingleton().isExistingFile(fullpath))
		return nullptr;

	// If this file is references somewhere (as for example a file editor window has it open, then use that
	// object)
	u32 id = FNVHash32::compute(fullpath.c_str(), fullpath.sizeBytes());
	auto existing = m_all.find(id);
	if (existing != m_all.end())
		return std::static_pointer_cast<File>(existing->second.lock());

	auto f = File::create(this, std::move(fullpath));
	m_all[f->id.val] = f;
	return f;
}

void Files::populateFromDir(const std::shared_ptr<Folder>& folder)
{
	auto files = Filesystem::getFilesInDirectory(folder->fullpath, "*", true);

	for (auto& f : files)
	{
		switch (f.type)
		{
			case Filesystem::Type::Directory:
			{
				cz::UTF8String path = folder->fullpath + "\\" + f.name;
				auto item = Folder::create(this, path);
				folder->items.insert(item);
				populateFromDir(item);
			}
			break;
			case Filesystem::Type::File:
			{
				if (ansiToLower(Filename(f.name).getExtension()) == "nut")
				{
					auto item = createFile(folder->fullpath + "\\" + f.name);
					folder->items.insert(item);
				}
			}
			break;
			default:
				CZ_UNEXPECTED();
		}
	}
}

void Files::resyncFolders()
{
	// Keep the names of first level folders, so we know what to refresh
	std::vector<UTF8String> folders;
	for (const auto n : m_root->items)
		folders.push_back(n->fullpath);

	// By doing this, we drop all our references to files/folders,
	// but other components still keep the references to any files they are using (such as files
	// being edited at the moment)
	m_root.reset();
	m_root = Folder::create(this, "");

	for (auto& n : folders)
		addFolder(n);
}

std::shared_ptr<const Folder> Files::getRoot()
{
	return m_root;
}


} // namespace nutcracker

