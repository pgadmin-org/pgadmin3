//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id: wsVarWindow.h 6136 2007-03-29 10:38:10Z hiroshi $
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// wsVarWindow.h - debugger 
//
//////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//	class wsVarWindow
//
//	This class implements the window that displays PL variable values at the 
//  bottom of the debugger window.  When we create a wsVarWindow, the parent
//	is a wsTabWindow (the wsVarWindow becomes a tab in a tab control).
//
//	It is a simple grid control - the grid contains three columns:
//		the RowLabel column displays the name of each variable
//		column 0 displays the value of each variable
//		column 1 displays the data type of each variable
//
//	Each wsVarWindow contains a hash map that can locate a grid cell given a
//  variable name
//
////////////////////////////////////////////////////////////////////////////////

#ifndef WSVARWINDOWH
#define WSVARWINDOWH

#include <wx/hashmap.h>
#include <wx/hashset.h>
#include <wx/grid.h>

class wsVarWindow : public wxGrid
{
    DECLARE_CLASS( wsVarWindow )

public:
	wsVarWindow( wxWindow * parent, wxWindowID id );

	void	addVar( wxString name, wxString value, wxString type, bool readOnly );	// Add a variable to the window
	void	delVar( wxString name = wxEmptyString);								    // Remove a variable from the window
	wxString	getVarName( int row );
	wxString	getVarValue( int row );

private:

	// The content of a grid cell is defined by the gridCell structure

    typedef struct
    {
	int		m_row;	 // Row number for this variable/grid cell
	wxString	m_value; // Variable value
	wxString	m_type;	 // Variable type
    } gridCell;

	enum
	{
		COL_NAME = 0,		// Column 0 contains the variable name
		COL_TYPE,		// This column contains the variable type
		COL_VALUE		// This column contains the variable value
	};

	// The m_cells hash translates variable names into gridCell references
public:
    WX_DECLARE_STRING_HASH_MAP( gridCell, wsCellHash );
	WX_DECLARE_HASH_SET( wxString, wxStringHash, wxStringEqual, wsStringSet );

private:
	wsStringSet	m_hiddenNames;	// List of hidden variable names
	wsStringSet	m_hiddenTypes;	// List of hidden variable types
	wsCellHash	*m_cells;	// name-to-gridCell map
	wxFont		m_nameFont;	// Font used to display field names
};

#endif
