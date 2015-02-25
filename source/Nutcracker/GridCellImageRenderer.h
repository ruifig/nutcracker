#pragma once


namespace nutcracker
{

class GridCellImageRenderer : public wxGridCellStringRenderer
{
public:
	explicit GridCellImageRenderer(wxBitmap img=wxNullBitmap);
	virtual void Draw(wxGrid& grid, wxGridCellAttr& attr, wxDC& dc, const wxRect& rect, int row, int col, bool isSelected);
	void removeImage();
	void setImage(wxBitmap img);
	wxBitmap getImage() const;

private:
	wxBitmap m_img;
};

}