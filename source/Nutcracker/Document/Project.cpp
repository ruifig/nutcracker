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


File::File(UTF8String fullname, UTF8String name) : ProjectItem(ProjectItemType::File, fullname, name)
{
	extension = ansiToLower(Filename(fullname).getExtension());
}

Project::Project(UTF8String folder)
	: m_folder(std::move(folder))
{
}

Project::~Project()
{
}


void Project::populateFromDir(std::shared_ptr<Folder> root, const UTF8String& dir)
{
	auto files = Filesystem::getFilesInDirectory(dir, "*", true);

	auto addItem = [&](const std::shared_ptr<ProjectItem>& item)
	{
		CZ_CHECK(m_rootMap.find(item->id.val) == m_rootMap.end());
		root->items.push_back(item);
		m_rootMap[item->id.val] = item;
	};

	for (auto& f : files)
	{
		switch (f.type)
		{
			case Filesystem::Type::Directory:
			{
				cz::UTF8String fullname = dir + f.name + "\\";
				auto folder = std::make_shared<Folder>(fullname, f.name);
				addItem(folder);
				populateFromDir(folder, fullname);
			}
			break;
			case Filesystem::Type::File:
				if (ansiToLower(Filename(f.name).getExtension()) == "nut")
				{
					auto file = std::make_shared<File>(dir + f.name, f.name);
					file->absolutepath = file->fullname;
					addItem(file);
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
	populateFromDir(folder, m_folder.c_str());
	m_root = folder;
}

File* Project::getFile(ProjectItemId id)
{
	auto it = m_rootMap.find(id.val);
	if (it == m_rootMap.end())
	{
		CZ_UNEXPECTED();
		return nullptr;
	}

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



} // namespace document
} // namespace cz

