#include "NutcrackerPCH.h"
#include "FindFileDlg.h"

namespace nutcracker
{

BEGIN_EVENT_TABLE(FindFileDlg, FindFileDlg_Auto)
    EVT_TIMER(wxID_ANY,FindFileDlg::OnTimer)
END_EVENT_TABLE()

FindFileDlg::FindFileDlg(wxWindow* parent)
	: FindFileDlg_Auto(parent), m_timer(this)
{
	m_grid->SetColLabelValue(0, "File");
	m_grid->SetColLabelValue(1, "Project");
	m_grid->SetColLabelValue(2, "Path");

	m_grid->Freeze();
	if (m_grid->GetNumberRows())
		m_grid->DeleteRows(0, m_grid->GetNumberRows());
	m_grid->Thaw();

	Connect(wxEVT_CHAR_HOOK, wxKeyEventHandler(FindFileDlg::OnCharHook));
	m_txt->SetFocus();
}

void FindFileDlg::updateGrid()
{
	m_needsUpdate = false;
	m_files.clear();

	std::string txt = m_txt->GetValue();

	gWorkspace->iterateFiles([&](const std::shared_ptr<const File>& file)
	{
		std::string name = file->name.c_str();
		auto res = cz::ci_find_substr(name, txt);
		if (res == std::string::npos)
			return;

		int penalty = res;
		bool exactMatch = (res==0 && name == txt);
		if (!exactMatch)
			penalty++;
		int postfixPenalty = name.size() - (res + txt.size());
		penalty += postfixPenalty;
		m_files.emplace_back(penalty, file);
	});

	auto pairCompare =
		[](const std::pair<int, std::shared_ptr<const File>>& firstElem, const std::pair<int, std::shared_ptr<const File>>& secondElem)
	{
		return firstElem.first < secondElem.first;
	};
	std::sort(m_files.begin(), m_files.end(), pairCompare);

	m_grid->Freeze();
	if (m_grid->GetNumberRows())
		m_grid->DeleteRows(0, m_grid->GetNumberRows());

	for (auto& it : m_files)
	{
		m_grid->AppendRows();
		int r = m_grid->GetNumberRows() - 1;

		m_grid->SetCellValue(r, 0, it.second->name.c_str());
		m_grid->SetCellValue(r, 1, "NA");
		m_grid->SetCellValue(r, 2, it.second->fullpath.c_str());
		for (int c=0; c<m_grid->GetNumberCols(); c++)
			m_grid->SetReadOnly(r, c, true);
	}

	if (m_grid->GetNumberRows())
		m_grid->SelectRow(0);

	m_grid->AutoSize();
	m_grid->Thaw();
	this->Layout();
}

void FindFileDlg::OnTextctrlTextUpdated(wxCommandEvent& event)
{
	m_needsUpdate = true;
	m_timer.Start(500, true);
}

void FindFileDlg::doPick(int row)
{
	if (row < 0 && row >= (int)m_files.size())
		return;
	m_picked = m_files[row].second;
	Close();
}

void FindFileDlg::OnGridCellDClick(wxGridEvent& event)
{
	doPick(event.GetRow());
}

void FindFileDlg::OnTimer(wxTimerEvent& event)
{
	if (m_needsUpdate && m_txt->GetValue().Trim() != "")
		updateGrid();
}

void FindFileDlg::OnCharHook(wxKeyEvent& event)
{
	if (m_grid->GetNumberRows() == 0)
	{
		event.Skip();
		return;
	}

	auto keycode = event.GetKeyCode();
	auto getCurrentRow = [&]()
	{
		if (m_needsUpdate)
			updateGrid();
		wxArrayInt rows = m_grid->GetSelectedRows();
		if (rows.size() == 0)
			return -1;
		else
			return rows[0];
	};
	auto setRow = [&](int row)
	{
		int newRow = std::max(0, std::min(row, m_grid->GetNumberRows()-1));
		m_grid->SelectRow(newRow);
		m_grid->MakeCellVisible(newRow, 0);
	};

	switch (keycode)
	{
		case WXK_UP:
			setRow(getCurrentRow() - 1);
			break;
		case WXK_DOWN:
			setRow(getCurrentRow() + 1);
			break;
		case WXK_RETURN:
		{
			wxArrayInt rows = m_grid->GetSelectedRows();
			if (rows.size())
			{
				doPick(rows[0]);
			}
		}
		break;
		default:
			event.Skip();
	}

	if (event.GetModifiers() == 0 && !m_txt->HasFocus())
	{
		m_txt->SetFocus();
	}
}

std::shared_ptr<const File> FindFileDlg::getResult()
{
	return m_picked;
}

} // namespace nutcracker
