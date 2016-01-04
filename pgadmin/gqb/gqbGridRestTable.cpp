//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// gqbGridRestTable.cpp - Table implementation for Restrictions Panel Grid
//
//////////////////////////////////////////////////////////////////////////

// App headers
#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>
#include <wx/settings.h>
#include <wx/utils.h>
#include <wx/notebook.h>

// App headers
#include "gqb/gqbGridRestTable.h"
#include "gqb/gqbColumn.h"
#include "gqb/gqbQueryObjs.h"
#include "gqb/gqbViewPanels.h"

gqbGridRestTable::gqbGridRestTable(gqbRestrictions *_restrictions):
	wxGridTableBase()
{
	restrictions = _restrictions;
}


gqbGridRestTable::~gqbGridRestTable()
{
}


int gqbGridRestTable::GetNumberRows()
{
	return (restrictions->restrictionsCount());
}


int gqbGridRestTable::GetNumberCols()
{
	return 4;
}


bool gqbGridRestTable::IsEmptyCell( int row, int col )
{

	int count = restrictions->restrictionsCount();
	if(row + 1 <= count)
		return false;
	else
		return true;
}


wxString gqbGridRestTable::GetValue( int row, int col )
{
	switch(col)
	{
		case 0:
			return ((gqbQueryRestriction *)restrictions->getRestrictionAt(row))->getLeft();
			break;
		case 1:
			return ((gqbQueryRestriction *)restrictions->getRestrictionAt(row))->getRestriction();
			break;
		case 2:
			return ((gqbQueryRestriction *)restrictions->getRestrictionAt(row))->getValue_s();
			break;
		case 3:
			return ((gqbQueryRestriction *)restrictions->getRestrictionAt(row))->getConnector();
			break;
	};

	return wxT("");
}


wxString gqbGridRestTable::GetColLabelValue( int col)
{
	switch(col)
	{
		case 0:
			return _("Restricted Value");
			break;
		case 1:
			return _("Operator");
			break;
		case 2:
			return _("Value");
			break;
		case 3:
			return _("Connector");
			break;
	};
	return wxT("");
}


void gqbGridRestTable::SetValue( int row, int col, const wxString &value )
{
	switch(col)
	{
		case 0:
			((gqbQueryRestriction *)restrictions->getRestrictionAt(row))->setLeft(value);
			break;
		case 1:
			((gqbQueryRestriction *)restrictions->getRestrictionAt(row))->setRestriction(value);
			break;
		case 2:
			((gqbQueryRestriction *)restrictions->getRestrictionAt(row))->setValue_s(value);
			break;
		case 3:
			((gqbQueryRestriction *)restrictions->getRestrictionAt(row))->setConnector(value);
			break;
	}
}


void gqbGridRestTable::AppendItem(gqbQueryRestriction *item)
{
	bool notify = true;

	restrictions->addRestriction(item);

	if (notify && GetView() )
	{
		wxGridTableMessage msg( this,
		                        wxGRIDTABLE_NOTIFY_ROWS_INSERTED,
		                        (restrictions->restrictionsCount() - 1),
		                        1 );
		GetView()->ProcessTableMessage( msg );
	}

}


bool gqbGridRestTable::DeleteRows(size_t pos = 0, size_t numRows = 1)
{
	if((pos < (size_t)GetNumberRows()) && numRows == 1)
	{
		gqbQueryRestriction *r = restrictions->getRestrictionAt(pos);
		restrictions->removeRestriction(r);

		// Notify Grid about the change
		if ( GetView() )
		{
			wxGridTableMessage msg( this,
			                        wxGRIDTABLE_NOTIFY_ROWS_DELETED,
			                        pos,
			                        1 );
			GetView()->ProcessTableMessage( msg );
		}

		delete r;
		return true;
	}
	else
		return false;
}


// Removes all items from gqbGridTable
void gqbGridRestTable::emptyTableData()
{

	int count = restrictions->restrictionsCount();
	restrictions->deleteAllRestrictions();

	if ( GetView() )                              //Notify Grid about the change
	{
		wxGridTableMessage msg( this,
		                        wxGRIDTABLE_NOTIFY_ROWS_DELETED,
		                        1,
		                        count);
		GetView()->ProcessTableMessage( msg );
	}
}


//
// Cell rendering utilities classes
//

void wxGridCellComboBoxRenderer::Draw(wxGrid &grid, wxGridCellAttr &attr, wxDC &dc,
                                      const wxRect &rectCell, int row, int col, bool isSelected)
{
	wxGridCellRenderer::Draw(grid, attr, dc, rectCell, row, col, isSelected);

	// first calculate button size
	// don't draw outside the cell
	int nButtonWidth = 17;
	if (rectCell.height < 2) return;
	wxRect rectButton;
	rectButton.x = rectCell.x + rectCell.width - nButtonWidth;
	rectButton.y = rectCell.y + 1;
	int cell_rows, cell_cols;
	attr.GetSize(&cell_rows, &cell_cols);
	rectButton.width = nButtonWidth;
	if (cell_rows == 1)
		rectButton.height = rectCell.height - 2;
	else
		rectButton.height = nButtonWidth;

	SetTextColoursAndFont(grid, attr, dc, isSelected);
	int hAlign, vAlign;
	attr.GetAlignment(&hAlign, &vAlign);

	// Leave room for button
	wxRect rect = rectCell;
	rect.SetWidth(rectCell.GetWidth() - rectButton.GetWidth() - 2);
	rect.Inflate(-1);
	grid.DrawTextRectangle(dc, grid.GetCellValue(row, col), rect, hAlign, vAlign);

	// Don't bother drawing if the cell is too small
	if (rectButton.height < 4 || rectButton.width < 4)
		return;

	// Draw 3-d button
	wxColour colourBackGround = wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE);
	dc.SetBrush(wxBrush(colourBackGround, wxSOLID));
	dc.SetPen(wxPen(colourBackGround, 1, wxSOLID));
	dc.DrawRectangle(rectButton);
	dc.SetPen(wxPen(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNTEXT), 1, wxSOLID));
	dc.DrawLine(rectButton.GetLeft(), rectButton.GetBottom(),
	            rectButton.GetRight(), rectButton.GetBottom());
	dc.DrawLine(rectButton.GetRight(), rectButton.GetBottom(),
	            rectButton.GetRight(), rectButton.GetTop() - 1);
	dc.SetPen(wxPen(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNSHADOW),
	                1, wxSOLID));
	dc.DrawLine(rectButton.GetLeft() + 1, rectButton.GetBottom() - 1,
	            rectButton.GetRight() - 1, rectButton.GetBottom() - 1);
	dc.DrawLine(rectButton.GetRight() - 1, rectButton.GetBottom() - 1,
	            rectButton.GetRight() - 1, rectButton.GetTop());
	dc.SetPen(wxPen(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNHIGHLIGHT),
	                1, wxSOLID));
	dc.DrawLine(rectButton.GetRight() - 2, rectButton.GetTop() + 1,
	            rectButton.GetLeft() + 1, rectButton.GetTop() + 1);
	dc.DrawLine(rectButton.GetLeft() + 1, rectButton.GetTop() + 1,
	            rectButton.GetLeft() + 1, rectButton.GetBottom() - 1);

	// Draw little triangle
	int nTriWidth = 7;
	int nTriHeight = 4;
	wxPoint point[3];
	point[0] = wxPoint(rectButton.GetLeft() + (rectButton.GetWidth() - nTriWidth) / 2,
	                   rectButton.GetTop() + (rectButton.GetHeight() - nTriHeight) / 2);
	point[1] = wxPoint(point[0].x + nTriWidth - 1, point[0].y);
	point[2] = wxPoint(point[0].x + 3, point[0].y + nTriHeight - 1);
	dc.SetBrush(wxBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNTEXT), wxSOLID));
	dc.SetPen(wxPen(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNTEXT), 1, wxSOLID));
	dc.DrawPolygon(3, point);
	if (m_border == wxLAYOUT_TOP)
	{
		dc.SetPen(wxPen(*wxBLACK, 1, wxDOT));
		dc.DrawLine(rectCell.GetRight(), rectCell.GetTop(),
		            rectCell.GetLeft(), rectCell.GetTop());
	}
}


void wxGridCellButtonRenderer::Draw(wxGrid &grid, wxGridCellAttr &attr, wxDC &dc,
                                    const wxRect &rectCell, int row, int col, bool isSelected)
{
	wxGridCellRenderer::Draw(grid, attr, dc, rectCell, row, col, isSelected);

	// First calculate button size
	// don't draw outside the cell
	int nButtonWidth = 17;
	if (rectCell.height < 2) return;
	wxRect rectButton;
	rectButton.x = rectCell.x + rectCell.width - nButtonWidth;
	rectButton.y = rectCell.y + 1;
	int cell_rows, cell_cols;
	attr.GetSize(&cell_rows, &cell_cols);
	rectButton.width = nButtonWidth;
	if (cell_rows == 1)
		rectButton.height = rectCell.height - 2;
	else
		rectButton.height = nButtonWidth;

	SetTextColoursAndFont(grid, attr, dc, isSelected);
	int hAlign, vAlign;
	attr.GetAlignment(&hAlign, &vAlign);

	// Leave room for button
	wxRect rect = rectCell;
	rect.SetWidth(rectCell.GetWidth() - rectButton.GetWidth() - 2);
	rect.Inflate(-1);
	grid.DrawTextRectangle(dc, grid.GetCellValue(row, col), rect, hAlign, vAlign);


	// Don't bother drawing if the cell is too small draw 3-d button
	if (rectButton.height < 4 || rectButton.width < 4)
		return;

	wxColour colourBackGround = wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE);
	dc.SetBrush(wxBrush(colourBackGround, wxSOLID));
	dc.SetPen(wxPen(colourBackGround, 1, wxSOLID));
	dc.DrawRectangle(rectButton);
	dc.SetPen(wxPen(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNTEXT), 1, wxSOLID));
	dc.DrawLine(rectButton.GetLeft(), rectButton.GetBottom(),
	            rectButton.GetRight(), rectButton.GetBottom());
	dc.DrawLine(rectButton.GetRight(), rectButton.GetBottom(),
	            rectButton.GetRight(), rectButton.GetTop() - 1);
	dc.SetPen(wxPen(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNSHADOW),
	                1, wxSOLID));
	dc.DrawLine(rectButton.GetLeft() + 1, rectButton.GetBottom() - 1,
	            rectButton.GetRight() - 1, rectButton.GetBottom() - 1);
	dc.DrawLine(rectButton.GetRight() - 1, rectButton.GetBottom() - 1,
	            rectButton.GetRight() - 1, rectButton.GetTop());
	dc.SetPen(wxPen(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNHIGHLIGHT),
	                1, wxSOLID));
	dc.DrawLine(rectButton.GetRight() - 2, rectButton.GetTop() + 1,
	            rectButton.GetLeft() + 1, rectButton.GetTop() + 1);
	dc.DrawLine(rectButton.GetLeft() + 1, rectButton.GetTop() + 1,
	            rectButton.GetLeft() + 1, rectButton.GetBottom() - 1);

	// Draw little plus symbol
	dc.SetBrush(wxBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNTEXT), wxSOLID));
	dc.SetPen(wxPen(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNTEXT), 1, wxSOLID));
	int nPlusWidth = 7;
	int nPlusHeight = 7;
	wxPoint point[4];
	point[0] = wxPoint(rectButton.GetLeft() + (rectButton.GetWidth() - nPlusWidth) / 2, rectButton.GetTop() + (rectButton.GetHeight() / 2) - 1);
	point[1] = wxPoint(point[0].x + nPlusWidth, point[0].y);
	point[2] = wxPoint(rectButton.GetLeft() + (rectButton.GetWidth()) / 2, rectButton.GetTop() + (rectButton.GetHeight() - nPlusHeight) / 2);
	point[3] = wxPoint(point[2].x, point[2].y + nPlusHeight);
	dc.DrawLine(point[0], point[1]);
	dc.DrawLine(point[2], point[3]);

	if (m_border == wxLAYOUT_TOP)
	{
		dc.SetPen(wxPen(*wxBLACK, 1, wxDOT));
		dc.DrawLine(rectCell.GetRight(), rectCell.GetTop(),
		            rectCell.GetLeft(), rectCell.GetTop());
	}
}


//
// Cell editing utilities classes
//
dxGridCellSizedChoiceEditor::dxGridCellSizedChoiceEditor(const wxArrayString &choices, bool allowOthers)
	: wxGridCellChoiceEditor(choices, allowOthers)
{
}


dxGridCellSizedChoiceEditor::dxGridCellSizedChoiceEditor(size_t count, const wxString choices[], bool allowOthers)
	: wxGridCellChoiceEditor(count, choices, allowOthers)
{
}


wxGridCellEditor *dxGridCellSizedChoiceEditor::Clone() const
{
	dxGridCellSizedChoiceEditor *editor = new
	dxGridCellSizedChoiceEditor();
	return editor;
}


void dxGridCellSizedChoiceEditor::Show(bool show, wxGridCellAttr *attr)
{
	wxGridCellEditor::Show(show, attr);
}
