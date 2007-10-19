//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id: ctlMessageWindow.cpp 6200 2007-04-18 10:00:20Z dpage $
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// ctlMessageWindow.cpp - debugger 
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "debugger/ctlMessageWindow.h"

IMPLEMENT_CLASS( ctlMessageWindow, wxTextCtrl )

////////////////////////////////////////////////////////////////////////////////
// ctlMessageWindow constructor
//
//  Initialize the grid control and clear it out....
//

ctlMessageWindow::ctlMessageWindow( wxWindow * parent, wxWindowID id )
    : wxTextCtrl( parent, wxID_ANY, _T(""), wxPoint(0, 0), wxSize(0, 0),
                               wxTE_MULTILINE | wxTE_READONLY)
{
    SetFont(settings->GetSQLFont());
}

////////////////////////////////////////////////////////////////////////////////
// addMessage()
//
//    Adds the message in the 'DBMS Messages' window.  
//

void ctlMessageWindow::addMessage( wxString message )
{
    AppendText(message + wxT("\n"));
}

////////////////////////////////////////////////////////////////////////////////
// delMessage()
//
//    Removes the given message from the 'DBMS Messages' window.
//

void ctlMessageWindow::delMessage( const char * name )
{
    SetValue(wxT(""));
}


wxString ctlMessageWindow::getMessage( int row )
{
    return( GetValue());
}
