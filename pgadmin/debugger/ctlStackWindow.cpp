//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id: ctlStackWindow.cpp 6202 2007-04-18 11:22:49Z dpage $
// Copyright (C) 2002 - 2008, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// ctlStackWindow.cpp - debugger 
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>
#include <wx/tokenzr.h>

// App headers
#include "debugger/ctlStackWindow.h"

IMPLEMENT_CLASS(ctlStackWindow, wxListBox)

////////////////////////////////////////////////////////////////////////////////
// ctlStackWindow constructor
//
//  Initialize the grid control and clear it out....
//

ctlStackWindow::ctlStackWindow(wxWindow *parent, wxWindowID id, const wxPoint & pos, const wxSize & size, long style, const wxString &name )
  : wxListBox(parent , id, pos, size, 0, NULL, style|wxLB_HSCROLL|wxLB_NEEDED_SB )
{
    wxWindowBase::SetFont(settings->GetSystemFont());
}

////////////////////////////////////////////////////////////////////////////////
// clear()
//
//    Remove all stack frames from the display
//

void ctlStackWindow::clear()
{
    Set(0, NULL);
}

////////////////////////////////////////////////////////////////////////////////
// setStack()
//
//    Add an array of stack frames to the display
//


void ctlStackWindow::setStack(const wxArrayString & stack )
{
    for(size_t i = 0; i < stack.GetCount(); ++i)
    {
        Append(stack[i]);
    }
}
