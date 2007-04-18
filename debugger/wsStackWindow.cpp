//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// wsstackwindow.cpp - debugger 
//
//////////////////////////////////////////////////////////////////////////

#include "wsStackWindow.h"
#include "debugger.h"

#include <wx/log.h>
#include <wx/tokenzr.h>

IMPLEMENT_CLASS( wsStackWindow, wxListBox )

////////////////////////////////////////////////////////////////////////////////
// wsStackWindow constructor
//
//  Initialize the grid control and clear it out....
//

wsStackWindow::wsStackWindow( wxWindow * parent, wxWindowID id, const wxPoint & pos, const wxSize & size, long style, const wxString & name )
	: wxListBox( parent , id, pos, size, 0, NULL, style|wxLB_HSCROLL|wxLB_NEEDED_SB )
{
    wxWindowBase::SetFont(glApp->GetSystemFont());
}

////////////////////////////////////////////////////////////////////////////////
// clear()
//
//	Remove all stack frames from the display
//

void wsStackWindow::clear( )
{
	Set( 0, NULL );
}

////////////////////////////////////////////////////////////////////////////////
// setStack()
//
//	Add an array of stack frames to the display
//


void wsStackWindow::setStack( const wxArrayString & stack )
{
	for( size_t i = 0; i < stack.GetCount(); ++i )
	{
		Append( stack[i] );
	}
}
