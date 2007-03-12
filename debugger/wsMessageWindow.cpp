//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id: wsmessagewindow.cpp 5827 2007-01-04 16:35:14 hiroshi $
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// wsmessagewindow.cpp - debugger 
//
//////////////////////////////////////////////////////////////////////////

#include "wsMessageWindow.h"
#include "debugger.h"
#include <wx/log.h>

IMPLEMENT_CLASS( wsMessageWindow, wxTextCtrl )

////////////////////////////////////////////////////////////////////////////////
// wsMessageWindow constructor
//
//  Initialize the grid control and clear it out....
//

wsMessageWindow::wsMessageWindow( wxWindow * parent, wxWindowID id )
	: wxTextCtrl( parent, wxID_ANY, _T(""), wxPoint(0, 0), wxSize(0, 0),
                               wxTE_MULTILINE | wxTE_READONLY)
{
	wxString fontName;

	if( glApp->getSettings().Read( wxT( "Font" ), &fontName ))
	{
		wxFont	font( fontName );
	}
	else
	{
		wxFont	font( 10, wxTELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL );
		SetDefaultStyle(wxTextAttr (wxNullColour,wxNullColour,font,wxTEXT_ALIGNMENT_DEFAULT));

		// Note: we should not have to call SetFont() here, but under Win32, it's required otherwise
		//	     we get a proportional font

		SetFont( font );
	}
}

////////////////////////////////////////////////////////////////////////////////
// addMessage()
//
//	Adds the message in the 'DBMS Messages' window.  
//

void wsMessageWindow::addMessage( wxString message )
{
	AppendText(message + wxT("\n"));
}

////////////////////////////////////////////////////////////////////////////////
// delMessage()
//
//	Removes the given message from the 'DBMS Messages' window.
//

void wsMessageWindow::delMessage( const char * name )
{
	SetValue(wxT(""));
}


wxString wsMessageWindow::getMessage( int row )
{
	return( GetValue());
}
