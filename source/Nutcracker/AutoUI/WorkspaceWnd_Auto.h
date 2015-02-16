/////////////////////////////////////////////////////////////////////////////
// Name:        WorkspaceWnd_Auto.h
// Purpose:     
// Author:      Rui Figueira
// Modified by: 
// Created:     16/02/2015 19:25:39
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef _WORKSPACEWND_AUTO_H_
#define _WORKSPACEWND_AUTO_H_


/*!
 * Includes
 */

////@begin includes
#include "wx/filepicker.h"
#include "wx/treectrl.h"
////@end includes

/*!
 * Forward declarations
 */

////@begin forward declarations
class wxDirPickerCtrl;
class wxTreeCtrl;
////@end forward declarations

/*!
 * Control identifiers
 */

////@begin control identifiers
#define SYMBOL_WORKSPACEWND_AUTO_STYLE wxTAB_TRAVERSAL
#define SYMBOL_WORKSPACEWND_AUTO_TITLE _("WorkspaceWnd_Auto")
#define SYMBOL_WORKSPACEWND_AUTO_IDNAME ID_WORKSPACEWND_AUTO
#define SYMBOL_WORKSPACEWND_AUTO_SIZE wxSize(400, 300)
#define SYMBOL_WORKSPACEWND_AUTO_POSITION wxDefaultPosition
////@end control identifiers


/*!
 * WorkspaceWnd_Auto class declaration
 */

class WorkspaceWnd_Auto: public wxPanel
{    
	DECLARE_DYNAMIC_CLASS( WorkspaceWnd_Auto )
	DECLARE_EVENT_TABLE()

public:
	/// Constructors
	WorkspaceWnd_Auto();
	WorkspaceWnd_Auto( wxWindow* parent, wxWindowID id = SYMBOL_WORKSPACEWND_AUTO_IDNAME, const wxPoint& pos = SYMBOL_WORKSPACEWND_AUTO_POSITION, const wxSize& size = SYMBOL_WORKSPACEWND_AUTO_SIZE, long style = SYMBOL_WORKSPACEWND_AUTO_STYLE );

	/// Creation
	bool Create( wxWindow* parent, wxWindowID id = SYMBOL_WORKSPACEWND_AUTO_IDNAME, const wxPoint& pos = SYMBOL_WORKSPACEWND_AUTO_POSITION, const wxSize& size = SYMBOL_WORKSPACEWND_AUTO_SIZE, long style = SYMBOL_WORKSPACEWND_AUTO_STYLE );

	/// Destructor
	~WorkspaceWnd_Auto();

	/// Initialises member variables
	void Init();

	/// Creates the controls and sizers
	void CreateControls();

////@begin WorkspaceWnd_Auto event handler declarations

	/// wxEVT_DIRPICKER_CHANGED event handler for ID_DIRPICKERCTRL
	virtual void OnDirpickerctrlDirPickerChanged( wxFileDirPickerEvent& event );

	/// wxEVT_COMMAND_TREE_ITEM_ACTIVATED event handler for ID_TREECTRL
	virtual void OnTreeItemActivated( wxTreeEvent& event );

	/// wxEVT_COMMAND_TREE_ITEM_MENU event handler for ID_TREECTRL
	virtual void OnTreeItemMenu( wxTreeEvent& event );

////@end WorkspaceWnd_Auto event handler declarations

////@begin WorkspaceWnd_Auto member function declarations

	/// Retrieves bitmap resources
	wxBitmap GetBitmapResource( const wxString& name );

	/// Retrieves icon resources
	wxIcon GetIconResource( const wxString& name );
////@end WorkspaceWnd_Auto member function declarations

	/// Should we show tooltips?
	static bool ShowToolTips();

////@begin WorkspaceWnd_Auto member variables
	wxDirPickerCtrl* m_workspaceRoot;
	wxTreeCtrl* m_treeCtrl;
	/// Control identifiers
	enum {
		ID_WORKSPACEWND_AUTO = 10000,
		ID_DIRPICKERCTRL = 10002,
		ID_TREECTRL = 10001
	};
////@end WorkspaceWnd_Auto member variables
};

#endif
	// _WORKSPACEWND_AUTO_H_
