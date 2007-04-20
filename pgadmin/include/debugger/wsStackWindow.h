//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id: wsStackWindow.h 6199 2007-04-17 16:12:00Z dpage $
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// wsStackWindow.h - debugger 
//
//////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//	class wsStackWindow
//
//	This class implements the window that displays the current call stack at 
//  bottom of the debugger window.  When we create a wsStackWindow, the parent
//	is a wsTabWindow (the wsStackWindow becomes a tab in a tab control).
//
//	It is a simple grid control - the grid contains two columns:
//		the RowLabel column displays the stack level 
//		column 0 displays the function name, line number and argument list
//
////////////////////////////////////////////////////////////////////////////////

#ifndef WSSTACKWINDOWH
#define WSSTACKWINDOWH

#include <wx/grid.h>
#include <wx/laywin.h>
#include <wx/listbox.h>

class wsStackWindow : public wxListBox
{
    DECLARE_CLASS( wsVarWindow )

public:

	wsStackWindow(wxWindow *parent, wxWindowID id, const wxPoint & pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxCLIP_CHILDREN | wxSW_3D, const wxString& name = wxT( "stackWindow" ));
	void clear();											// Remove all frames from the stack trace
	void setStack(const wxArrayString &stack);			// Add an array of frames to the stack trace 
};

#endif
