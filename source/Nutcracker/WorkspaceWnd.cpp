/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com
	
	purpose:
	
*********************************************************************/

#include "NutcrackerPCH.h"
#include "UIDefs.h"
#include "WorkspaceWnd.h"
#include "FileEditorGroupWnd.h"
#include "Interpreter.h"

namespace nutcracker
{

using namespace cz;

/*
Control ids
*/
	enum
	{
		ID_RunScriptFile,
		ID_OpenContainingFolder
	};

BEGIN_EVENT_TABLE(WorkspaceWnd, WorkspaceWnd_Auto)
	EVT_MENU(ID_OpenContainingFolder, WorkspaceWnd::OnOpenContainingFolder)
	EVT_MENU(ID_RunScriptFile, WorkspaceWnd::OnRunScriptFile)
END_EVENT_TABLE()


WorkspaceWnd::WorkspaceWnd(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
	: WorkspaceWnd_Auto(parent, id, pos, size, style)
{
	CZ_ASSERT(parent->IsKindOf(wxCLASSINFO(wxFrame)));
	m_treeData = TreeCtrlUtil::TreeCtrlData(static_cast<wxFrame*>(parent), m_treeCtrl);
	
	// SetImageList doesn't give ownership of the image list to the control, so we can share it with
	// other controls
	m_treeCtrl->SetImageList(&gImageListSmall);

	m_refreshBtn->SetBitmap(gImageListSmall.GetBitmap(SMALLIMG_IDX_REFRESH), wxLEFT);

	gWorkspaceWnd = this;
}

WorkspaceWnd::~WorkspaceWnd()
{
}

void WorkspaceWnd::onAppEvent(const AppEvent& evt)
{
	switch (evt.id)
	{
		case AppEventID::OpenWorkspace:
		case AppEventID::AddedFolder:
			updateState();
			break;
	}
}

// Helper to update recursively
std::shared_ptr<TreeCtrlUtil::TreeItemData> WorkspaceWnd::updateState(
	const std::shared_ptr<TreeCtrlUtil::TreeItemData>& parent, const std::shared_ptr<const BaseItem>& item)
{
	int imgIndex = -1;
	if (item->type == ItemType::Folder)
	{
		imgIndex = SMALLIMG_IDX_FOLDER;
	}
	else if (item->type == ItemType::File)
	{
		auto fileItem = static_cast<const File*>(item.get());
		if (fileItem->extension=="nut")
			imgIndex = SMALLIMG_IDX_FILE_NUT;
		else
			imgIndex = SMALLIMG_IDX_FILE_OTHER;
	}
	else
	{
		CZ_UNEXPECTED();
	}

	std::shared_ptr<TreeCtrlUtil::TreeItemData> data;
	if (parent)
		data = m_treeData.addItem(parent, TreeCtrlUtil::TreeItemID((u32)item->type, item->id.val), item->name, imgIndex);
	else
		data = m_treeData.setupRoot(TreeCtrlUtil::TreeItemID((u32)item->type, item->id.val), -1);

	if (item->type == ItemType::Folder)
	{
		auto folder = static_cast<const Folder*>(item.get());
		for (auto& f : folder->items)
			updateState(data, f);
	}

	return data;
}

void WorkspaceWnd::updateState()
{
	m_treeData.startRefresh();
	auto root = gProject->files.getRoot();
	auto data = updateState(nullptr, gProject->files.getRoot());
	m_treeData.endRefresh();
	//data->expand();
}

void WorkspaceWnd::OnTreeItemActivated(wxTreeEvent& event)
{
	wxTreeItemId itemId = event.GetItem();
	auto itemData = m_treeData.findByWxTreeItemId(event.GetItem());
	auto item = gProject->files.getFile(FileId(itemData->getItemId().id[0]));
	if (item == nullptr)
		return;

	gFileEditorGroupWnd->gotoFile(item, -1);
}

void WorkspaceWnd::OnTreeItemMenu(wxTreeEvent& event)
{
	wxTreeItemId itemId = event.GetItem();
	auto itemData = m_treeData.findByWxTreeItemId(itemId);

    wxPoint clientpt = event.GetPoint();
    wxPoint screenpt = ClientToScreen(clientpt);
	wxString title;

	switch (ItemType(itemData->getItemId().itemtype))
	{
		case ItemType::Folder:
			break;

		case ItemType::File:
		{
			//title = "File";
			wxMenu menu;
			wxMenu* addmenu = new wxMenu;
			FileId projectItemId(itemData->getItemId().id[0]);
			auto file = gProject->files.getFile(projectItemId);
			bool nutFile = file->extension=="nut";

			if (nutFile)
				menu.Append(ID_RunScriptFile, wxT("Run"));
			menu.Append(ID_OpenContainingFolder, wxT("Open containing folder"));

			m_selectedFileId = projectItemId;
			PopupMenu(&menu, clientpt);
		}
		break;
	}

	event.Skip();
}

void WorkspaceWnd::OnOpenContainingFolder(wxCommandEvent& event)
{
	auto file = gProject->files.getFile(m_selectedFileId);
	if (!file)
		return;

	UTF8String res;
	Filesystem::getSingleton().fullPath(res, file->getDirectory());
	wxExecute(wxString("explorer \"") + res.widen() + "\"");
}

void WorkspaceWnd::OnRunScriptFile(wxCommandEvent& /*event*/)
{
	auto file = gProject->files.getFile(m_selectedFileId);
	if (!file)
		return;

	Variables vars;
	vars.set("%FILE%", [&]()
	{
		return UTF8String("\"") + file->fullpath + "\"";
	});
	gUIState->currentInterpreter->launch(vars, file->fullpath, false);
}

wxTreeItemId WorkspaceWnd::findFileTreeItem(FileId fileId)
{
	wxTreeItemId res;
	TreeCtrlUtil::iterateTreeItems<TreeCtrlUtil::TreeItemBase>(
		m_treeData.getCtrl(), m_treeData.getCtrl()->GetRootItem(),
		[&](wxTreeItemId id, TreeCtrlUtil::TreeItemBase* treeitem)
	{
		if (treeitem->data->getItemId().id[0] == fileId.val)
			res = id;
	});

	return res;
}

void WorkspaceWnd::locateFile(FileId fileId)
{
	wxTreeItemId itemId = findFileTreeItem(fileId);
	if (!itemId.IsOk())
		return;

	m_treeData.getCtrl()->EnsureVisible(itemId);
	m_treeData.getCtrl()->SelectItem(itemId);
}

void WorkspaceWnd::OnRefreshClick(wxCommandEvent& event)
{
	gProject->files.resyncFolders();
	updateState();
}

} // namespace nutcracker
