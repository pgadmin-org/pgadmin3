//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// gqbGridRestTable.h - Table implementation for Restrictions Panel Grid
//
//////////////////////////////////////////////////////////////////////////

#ifndef GQBGRIDRESTTABLE_H
#define GQBGRIDRESTTABLE_H

#include <wx/grid.h>
#include <wx/laywin.h>

// App headers
#include "gqb/gqbModel.h"
#include "gqb/gqbArrayCollection.h"

class gqbGridRestTable : public wxGridTableBase
{
public:
	gqbGridRestTable(gqbRestrictions *_restrictions);
	~gqbGridRestTable();
	int GetNumberRows();
	int GetNumberCols();
	bool IsEmptyCell( int row, int col );
	wxString GetValue( int row, int col );
	wxString GetColLabelValue( int col);
	void SetValue( int row, int col, const wxString &value );
	void AppendItem(gqbQueryRestriction *item);
	void emptyTableData();
	bool DeleteRows(size_t pos, size_t numRows);

private:
	gqbRestrictions *restrictions;
};

//
// Cell rendering utilities classes
//

class wxGridCellComboBoxRenderer : public wxGridCellStringRenderer
{
public:
	wxGridCellComboBoxRenderer(wxLayoutAlignment border = wxLAYOUT_NONE) :
		m_border(border) {}
	virtual void Draw(wxGrid &grid, wxGridCellAttr &attr, wxDC &dc, const wxRect &rect, int row, int col, bool isSelected);
	virtual wxGridCellRenderer *Clone() const
	{
		return new wxGridCellComboBoxRenderer;
	}

private:
	wxLayoutAlignment m_border;
};

class wxGridCellButtonRenderer : public wxGridCellStringRenderer
{
public:
	wxGridCellButtonRenderer(wxLayoutAlignment border = wxLAYOUT_NONE) :
		m_border(border) {}
	virtual void Draw(wxGrid &grid, wxGridCellAttr &attr, wxDC &dc, const wxRect &rect, int row, int col, bool isSelected);
	virtual wxGridCellRenderer *Clone() const
	{
		return new wxGridCellComboBoxRenderer;
	}

private:
	wxLayoutAlignment m_border;
};

// Shows a wxGridCellChoiceEditor of cell's wide
class dxGridCellSizedChoiceEditor : public wxGridCellChoiceEditor
{
public:
	dxGridCellSizedChoiceEditor(const wxArrayString &choices, bool allowOthers = false);
	dxGridCellSizedChoiceEditor(size_t count = 0, const wxString choices[] = NULL, bool allowOthers = false);

	~dxGridCellSizedChoiceEditor() {}

	virtual wxGridCellEditor  *Clone() const;
	virtual void Show(bool show, wxGridCellAttr *attr = (wxGridCellAttr *)NULL);

protected:
	int m_maxWide;

	DECLARE_NO_COPY_CLASS(dxGridCellSizedChoiceEditor)

};

// GQB-TODO: don't use gqbObjsArray, use a new one in the model because violating MVC Pattern

// GQB-TODO: this is not needed the one in gqbArrayCollections works her ?????
// WX_DEFINE_ARRAY_PTR(gqbObject *, gqbObjsArray); this is not

// Create the Data Model that will be used by wxGrid Component

#endif
