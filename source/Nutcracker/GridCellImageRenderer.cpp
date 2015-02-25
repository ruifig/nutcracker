#include "NutcrackerPCH.h"
#include "GridCellImageRenderer.h"


namespace nutcracker
{

GridCellImageRenderer::GridCellImageRenderer(wxBitmap img)
{
	m_img = img;
}

void GridCellImageRenderer::Draw(wxGrid& grid, wxGridCellAttr& attr, wxDC& dc, const wxRect& rect, int row, int col, bool isSelected)
{
	wxGridCellStringRenderer::Draw(grid, attr, dc, rect, row, col, isSelected);
	if (m_img.IsOk())
	{
		int x = rect.x;
		int y = rect.y;
		if (attr.HasAlignment())
		{
			int h = 0, v = 0;
			attr.GetAlignment(&h, &v);
			if (h & wxALIGN_LEFT)
				x = rect.x;
			else if (h & wxALIGN_CENTER)
				x = rect.x + (rect.width - m_img.GetSize().x) / 2;
			else if (h & wxALIGN_RIGHT)
				x = rect.x + rect.width - m_img.GetSize().x;

			if (v & wxALIGN_TOP)
				y = rect.y;
			else if (v & wxALIGN_CENTER)
				y = rect.y + (rect.height - m_img.GetSize().y) / 2;
			else if (v & wxALIGN_BOTTOM)
				y = rect.y + rect.height - m_img.GetSize().y;
		}

		dc.DrawBitmap(m_img, x, y, true);
	}
}

void GridCellImageRenderer::setImage(wxBitmap img)
{
	m_img = img;
}

wxBitmap GridCellImageRenderer::getImage() const
{
	return m_img;
}

} // namespace nutcracker
