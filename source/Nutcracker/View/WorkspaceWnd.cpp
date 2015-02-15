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

namespace cz
{
namespace view
{

/*
Control ids
*/
	enum
	{
		ID_TreeCtrl = wxID_HIGHEST + 1
	};

BEGIN_EVENT_TABLE(WorkspaceWnd, wxPanel)
	EVT_TREE_ITEM_ACTIVATED(wxID_ANY, WorkspaceWnd::OnItemActivated)
END_EVENT_TABLE()


/*
struct WorkspaceTreeItemBase : public TreeCtrlUtil::TreeItemCustomData
{
	WorkspaceTreeItemBase() {}
	virtual ~WorkspaceTreeItemBase() {}
	document::ProjectItemId id;
};
*/

WorkspaceWnd::WorkspaceWnd(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
	: wxPanel(parent, id, pos, size, style)
{
	m_treeCtrl = new wxTreeCtrl(this, ID_TreeCtrl, wxPoint(0, 0), wxSize(160, 250),
								wxTR_DEFAULT_STYLE | wxNO_BORDER | wxTR_HIDE_ROOT);

	CZ_ASSERT(parent->IsKindOf(wxCLASSINFO(wxFrame)));
	m_treeData = TreeCtrlUtil::TreeCtrlData(static_cast<wxFrame*>(parent), m_treeCtrl);
	
	// SetImageList doesn't give ownership of the image list to the control, so we can share it with
	// other controls
	m_treeCtrl->SetImageList(&gImageListSmall);

	wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
	sizer->Add(m_treeCtrl, wxSizerFlags(1).Expand());
	sizer->SetMinSize(wxSize(160, 250));
	SetSizer(sizer);

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
			updateState();
			break;
	}
}

// Helper to update recursively
std::shared_ptr<TreeCtrlUtil::TreeItemData> WorkspaceWnd::updateState(
	const std::shared_ptr<TreeCtrlUtil::TreeItemData>& parent, const std::shared_ptr<const document::ProjectItem>& item)
{
	using namespace document;
	int imgIndex = -1;
	if (item->type == ProjectItemType::Folder)
	{
		imgIndex = SMALLIMG_IDX_FOLDER;
	}
	else if (item->type == ProjectItemType::File)
	{
		const UTF8String& extension = static_cast<const document::File*>(item.get())->extension;
		if (extension == "nut")
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

	if (item->type == ProjectItemType::Folder)
	{
		auto folder = static_cast<const document::Folder*>(item.get());
		for (auto& f : folder->items)
			updateState(data, f);
	}

	return data;
}

void WorkspaceWnd::updateState()
{
	m_treeData.startRefresh();
	auto root = gProject->getRoot();
	auto data = updateState(nullptr, gProject->getRoot());
	m_treeData.endRefresh();
	//data->expand();
}

void WorkspaceWnd::OnItemActivated(wxTreeEvent& event)
{
	using namespace document;
	wxTreeItemId itemId = event.GetItem();
	auto itemData = m_treeData.findByWxTreeItemId(event.GetItem());
	auto item = gProject->getFile(ProjectItemId(itemData->getItemId().id[0]));
	if (item == nullptr)
		return;

	gFileEditorGroupWnd->gotoFile(item, -1);
}

wxTreeItemId WorkspaceWnd::findFileTreeItem(document::ProjectItemId fileId)
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

void WorkspaceWnd::locateFile(document::ProjectItemId fileId)
{
	wxTreeItemId itemId = findFileTreeItem(fileId);
	if (!itemId.IsOk())
		return;

	m_treeData.getCtrl()->EnsureVisible(itemId);
	m_treeData.getCtrl()->SelectItem(itemId);
}

} // namespace view
} // namespace cz
