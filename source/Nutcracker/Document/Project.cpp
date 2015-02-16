#include "NutcrackerPCH.h"
#include "Project.h"


namespace cz
{
namespace document
{

ProjectItem::ProjectItem(ProjectItemType type_, UTF8String fullname_, UTF8String name_)
	: type(type_), fullname(fullname_), name(std::move(name_))
{
	id.val = FNVHash32::compute(fullname.c_str(), fullname.sizeBytes());
}

cz::UTF8String ProjectItem::getDirectory() const
{
	if (type == ProjectItemType::Folder)
		return fullname;
	else
		return Filename(fullname).getDirectory();
}


File::File(UTF8String fullname, UTF8String name) : ProjectItem(ProjectItemType::File, fullname, name)
{
	extension = ansiToLower(Filename(fullname).getExtension());
}

Project::Project(UTF8String folder)
{
	setRoot(folder);
}

Project::~Project()
{
}

void Project::setRoot(const UTF8String& root)
{
	if (!Filesystem::getSingleton().fullPath(m_folder, root))
	{
		throw std::runtime_error(formatString("Error parsing folder '%s'", root.c_str()));
	}
}

void Project::add(const std::shared_ptr<Folder>& root, const std::shared_ptr<ProjectItem>& item)
{
	CZ_CHECK(m_rootMap.find(item->id.val) == m_rootMap.end());
	root->items.push_back(item);
	m_rootMap[item->id.val] = item;
	item->absolutepath = item->fullname;
}


File* Project::addLooseFile(const UTF8String& filename)
{
	if (!Filesystem::getSingleton().isExistingFile(filename))
		return nullptr;

	auto file = std::make_shared<File>(filename, Filename(filename).getFilename());

	// Check if there is a file with the same id. If so, it's the same file
	if (auto existingFile = getFile(file->id))
		return existingFile;

	file->looseFile = true;
	add(m_root, file);
	return file.get();
}

void Project::populateFromDir(std::shared_ptr<Folder> root, const UTF8String& dir)
{
	auto files = Filesystem::getFilesInDirectory(dir, "*", true);

	for (auto& f : files)
	{
		switch (f.type)
		{
			case Filesystem::Type::Directory:
			{
				cz::UTF8String fullname = dir + f.name + "\\";
				auto folder = std::make_shared<Folder>(fullname, f.name);
				add(root, folder);
				populateFromDir(folder, fullname);
			}
			break;
			case Filesystem::Type::File:
				if (ansiToLower(Filename(f.name).getExtension()) == "nut")
				{
					auto file = std::make_shared<File>(dir + f.name, f.name);
					add(root, file);
				}
			break;
			default:
				CZ_UNEXPECTED();
		}
	}
}

void Project::populate()
{
	auto folder = std::make_shared<Folder>(m_folder, "Root");
	m_root = folder;
	if (m_folder == "")
		return;
	populateFromDir(folder, m_folder.c_str());
}

void Project::populate(const UTF8String& root)
{

	setRoot(root);
	auto folder = std::make_shared<Folder>(m_folder, "Root");
	m_root = folder;
	if (m_folder == "")
		return;
	populateFromDir(folder, m_folder.c_str());
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
	auto file = getFile(fileId);
	if (!file || !file->looseFile)
		return;

	m_rootMap.erase(fileId.val);
	cz::erase_if(m_root->items,
		[&](const std::shared_ptr<ProjectItem>& item)
	{
		return item->id == fileId;
	});
}


} // namespace document
} // namespace cz

