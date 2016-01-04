//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// ctlVarWindow.cpp - debugger
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>
#include <wx/log.h>

// App headers
#include "debugger/ctlVarWindow.h"

IMPLEMENT_CLASS(ctlVarWindow, wxGrid)

////////////////////////////////////////////////////////////////////////////////
// ctlVarWindow constructor
//
//  Initialize the grid control and clear it out....
//
ctlVarWindow::ctlVarWindow(wxWindow *parent, wxWindowID id)
	: wxGrid(parent, id),
	  m_cells(NULL),
	  m_nameFont(GetDefaultCellFont())
{
	SetFont(settings->GetSystemFont());

	// Create the grid control
	CreateGrid(0, 0);
	SetRowLabelSize(0);	// Turn off the row labels

	// Set up three columns: name, value, and data type
	AppendCols(3);
	SetColLabelValue(COL_NAME,  _("Name"));
	SetColLabelValue(COL_TYPE,  _("Type"));
	SetColLabelValue(COL_VALUE, _("Value"));

	EnableDragGridSize(true);

	// EDB wants to hide certain PL variables.  To do that, we
	// keep a hash of hidden names and a hash of hidden types...
	m_hiddenNames.insert(wxT("found"));
	m_hiddenNames.insert(wxT("rowcount"));
	m_hiddenNames.insert(wxT("sqlcode"));
	m_hiddenNames.insert(wxT("sqlerrm"));
	m_hiddenNames.insert(wxT("_found"));
	m_hiddenNames.insert(wxT("_rowcount"));
	m_hiddenNames.insert(wxT("sqlstate"));

	m_hiddenTypes.insert(wxT("refcursor"));

}

////////////////////////////////////////////////////////////////////////////////
// AddVar()
//
//    Adds (or updates) the given variable in the 'local variables' window.  If
//  we find a variabled named 'name' in the window, we simply update the value,
//  otherwise, we create a new entry in the grid
//
void ctlVarWindow::AddVar(wxString name, wxString value, wxString type, bool readOnly)
{
	// If this is a 'hidden' variable, just ignore it

	if (m_hiddenNames.find(name) != m_hiddenNames.end())
		return;

	if (m_hiddenTypes.find(type) != m_hiddenTypes.end())
		return;

	if (m_cells == NULL)
	{
		// This is the first variable we're adding to this grid,
		// layout the grid and set the column headers.

		m_cells = new wsCellHash;
	}

	// Try to find an existing grid cell for this variable...
	wxString	key(name);

	wsCellHash::iterator cell = m_cells->find(key);

	if (cell == m_cells->end())
	{
		// Can't find this variable in the grid, go ahead and add it

		gridCell	newCell;

		newCell.m_row   = m_cells->size();
		newCell.m_type  = type;
		newCell.m_value = value;

		AppendRows(1);

		SetRowLabelValue(newCell.m_row, key);

		SetCellValue(newCell.m_row, COL_NAME,  key);
		SetCellValue(newCell.m_row, COL_TYPE,  type);
		SetCellValue(newCell.m_row, COL_VALUE, value);

		SetCellFont(newCell.m_row, COL_NAME, m_nameFont);

		SetReadOnly(newCell.m_row, COL_NAME,  true);
		SetReadOnly(newCell.m_row, COL_TYPE,  true);
		SetReadOnly(newCell.m_row, COL_VALUE, readOnly);

		(*m_cells)[key] = newCell;
	}
	else
	{
		// This variable is already in the grid, update the value
		// and hilite it so the user knows that it has changed.

		cell->second.m_value = value;

		if (GetCellValue(cell->second.m_row, COL_VALUE).IsSameAs(value))
			SetCellTextColour(cell->second.m_row, COL_VALUE, *wxBLACK);
		else
			SetCellTextColour(cell->second.m_row, COL_VALUE, *wxRED);

		SetCellValue(cell->second.m_row, COL_VALUE, value);

		// FIXME: why is this part conditional?
		// FIXME: why do we need this code? can the type ever change?

		if (GetCellValue(cell->second.m_row, COL_TYPE) == wxT(""))
		{
			SetCellValue(cell->second.m_row, COL_TYPE, type);
		}
	}

	// AutoSizeColumns(false);
}

////////////////////////////////////////////////////////////////////////////////
// delVar()
//
//    Removes the given variable from the 'local variables' window.
//

void ctlVarWindow::DelVar(wxString name)
{
	if (name.IsEmpty())
	{
		delete m_cells;
		m_cells = NULL;

		if (GetNumberRows())
			DeleteRows(0, GetNumberRows());
	}
	else
	{
		for (int row = 0; row < GetNumberRows(); row++)
		{
			if (GetCellValue(row, COL_NAME) == name)
			{
				DeleteRows(row, 1);
				break;
			}
		}
	}
}


wxString ctlVarWindow::GetVarName(int row)
{
	return(GetCellValue(row, COL_NAME));

}

wxString ctlVarWindow::GetVarValue(int row)
{
	return(GetCellValue(row, COL_VALUE));
}
