//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id: ctlStackWindow.h 6199 2007-04-17 16:12:00Z dpage $
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// ctlStackWindow.h - debugger 
//
//////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//	class ctlStackWindow
//
//	This class implements the window that displays the current call stack at 
//  bottom of the debugger window.  When we create a ctlStackWindow, the parent
//	is a ctlTabWindow (the ctlStackWindow becomes a tab in a tab control).
//
//	It is a simple grid control - the grid contains two columns:
//		the RowLabel column displays the stack level 
//		column 0 displays the function name, line number and argument list
//
////////////////////////////////////////////////////////////////////////////////

#ifndef CTLSTACKWINDOW_H
#define CTLSTACKWINDOW_H

#include <wx/grid.h>
#include <wx/laywin.h>
#include <wx/listbox.h>

class ctlStackWindow : public wxListBox
{
    DECLARE_CLASS( ctlVarWindow )

public:

	ctlStackWindow(wxWindow *parent, wxWindowID id, const wxPoint & pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxCLIP_CHILDREN | wxSW_3D, const wxString& name = wxT( "stackWindow" ));
	void clear();											// Remove all frames from the stack trace
	void setStack(const wxArrayString &stack);			// Add an array of frames to the stack trace 
};

#endif
