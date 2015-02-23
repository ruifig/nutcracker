#include "NutcrackerPCH.h"
#include "BaseWatchesWnd.h"

IMPLEMENT_DYNAMIC_CLASS( BaseWatchesWnd, wxPanel )

BEGIN_EVENT_TABLE( BaseWatchesWnd, wxPanel )
	EVT_SIZE( BaseWatchesWnd::OnSize )
END_EVENT_TABLE()


BaseWatchesWnd::BaseWatchesWnd()
{
	Init();
}

BaseWatchesWnd::BaseWatchesWnd( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
{
	Init();
	Create(parent, id, pos, size, style);
}


bool BaseWatchesWnd::Create( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
{
	wxPanel::Create( parent, id, pos, size, style );

	CreateControls();
	if (GetSizer())
	{
		GetSizer()->SetSizeHints(this);
	}
	Centre();
	return true;
}


BaseWatchesWnd::~BaseWatchesWnd()
{
}


void BaseWatchesWnd::Init()
{
	m_tree = NULL;
}


void BaseWatchesWnd::CreateControls()
{    
	BaseWatchesWnd* itemPanel1 = this;

	wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
	itemPanel1->SetSizer(itemBoxSizer2);

	m_tree = new wxTreeListCtrl(itemPanel1, ID_TREE, wxDefaultPosition, wxSize(200, 150), 0);
	itemBoxSizer2->Add(m_tree, 1, wxGROW|wxALL|wxADJUST_MINSIZE, 0);
}


/*
 * Should we show tooltips?
 */
bool BaseWatchesWnd::ShowToolTips()
{
	return true;
}

/*
 * Get bitmap resources
 */
wxBitmap BaseWatchesWnd::GetBitmapResource( const wxString& name )
{
	wxUnusedVar(name);
	return wxNullBitmap;
}

/*
 * Get icon resources
 */
wxIcon BaseWatchesWnd::GetIconResource( const wxString& name )
{
	wxUnusedVar(name);
	return wxNullIcon;
}

void BaseWatchesWnd::OnSize( wxSizeEvent& event )
{
	event.Skip();
}




