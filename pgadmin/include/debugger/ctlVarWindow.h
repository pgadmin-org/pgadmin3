//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// ctlVarWindow.h - debugger
//
//////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//	class ctlVarWindow
//
//	This class implements the window that displays PL variable values at the
//  bottom of the debugger window.  When we create a ctlVarWindow, the parent
//	is a ctlTabWindow (the ctlVarWindow becomes a tab in a tab control).
//
//	It is a simple grid control - the grid contains three columns:
//		the RowLabel column displays the name of each variable
//		column 0 displays the value of each variable
//		column 1 displays the data type of each variable
//
//	Each ctlVarWindow contains a hash map that can locate a grid cell given a
//  variable name
//
////////////////////////////////////////////////////////////////////////////////

#ifndef CTLVARWINDOW_H
#define CTLVARWINDOW_H

#include <wx/hashmap.h>
#include <wx/hashset.h>
#include <wx/grid.h>

class ctlVarWindow : public wxGrid
{
	DECLARE_CLASS(ctlVarWindow)

public:
	ctlVarWindow(wxWindow *parent, wxWindowID id);

	// Add a variable to the window
	void AddVar(wxString name, wxString value, wxString type, bool readOnly);
	// Remove a variable from the window
	void DelVar(wxString name = wxEmptyString);

	wxString GetVarName(int row);
	wxString GetVarValue(int row);

private:

	// The content of a grid cell is defined by the gridCell structure
	typedef struct
	{
		int      m_row;   // Row number for this variable/grid cell
		wxString m_value; // Variable value
		wxString m_type;  // Variable type
	} gridCell;

	enum
	{
		COL_NAME = 0,		// Column 0 contains the variable name
		COL_TYPE,		// This column contains the variable type
		COL_VALUE		// This column contains the variable value
	};

	// The m_cells hash translates variable names into gridCell references
public:
	WX_DECLARE_STRING_HASH_MAP(gridCell, wsCellHash);
	WX_DECLARE_HASH_SET(wxString, wxStringHash, wxStringEqual, wsStringSet);

private:
	wsStringSet	m_hiddenNames;	// List of hidden variable names
	wsStringSet	m_hiddenTypes;	// List of hidden variable types
	wsCellHash	*m_cells;	// name-to-gridCell map
	wxFont		m_nameFont;	// Font used to display field names
};

#endif
