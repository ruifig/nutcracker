/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com
	
	purpose:
	
*********************************************************************/

#pragma once

#include "TreeCtrlUtilCode.h"
#include "AppEvents.h"
#include "Document/Project.h"

namespace cz
{

namespace view
{

class WorkspaceWnd : public wxPanel, public AppEventListener
{
public:
	WorkspaceWnd(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style);
	virtual ~WorkspaceWnd();

	void locateFile(document::ProjectItemId fileId);

private:
	DECLARE_EVENT_TABLE()
	void OnItemActivated(wxTreeEvent& event);
	void OnOpenContainingFolder(wxCommandEvent& event);
	void OnCompileFile(wxCommandEvent& event);
	void OnItemMenu(wxTreeEvent& event);

	std::shared_ptr<TreeCtrlUtil::TreeItemData> updateState(const std::shared_ptr<TreeCtrlUtil::TreeItemData>& parent,
															const std::shared_ptr<const document::ProjectItem>& item);
	void updateState();
	wxTreeItemId findFileTreeItem(document::ProjectItemId fileId);

	virtual void onAppEvent(const AppEvent& evt) override;
	wxTreeCtrl* m_treeCtrl;
	TreeCtrlUtil::TreeCtrlData m_treeData;
	document::ProjectItemId m_selectedFileId;
};

} // namespace view
} // namespace cz
