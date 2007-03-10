//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id: wsstackwindow.cpp 5827 2007-01-04 16:35:14 hiroshi $
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// wsstackwindow.cpp - debugger 
//
//////////////////////////////////////////////////////////////////////////

#include "wsstackwindow.h"
#include "wsapp.h"

#include <wx/log.h>
#include <wx/tokenzr.h>

IMPLEMENT_CLASS( wsStackWindow, wxSashLayoutWindow )

////////////////////////////////////////////////////////////////////////////////
// wsStackWindow constructor
//
//  Initialize the grid control and clear it out....
//

wsStackWindow::wsStackWindow( wxWindow * parent, wxWindowID id, const wxPoint & pos, const wxSize & size, long style, const wxString & name )
	: wxSashLayoutWindow( parent, id, pos, size, style, name ),
	  m_grid( new wxListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_HSCROLL|wxLB_NEEDED_SB ))
{
	wxString fontName;

    if( glApp->getSettings().Read( wxT( "Font" ), &fontName ))
        m_grid->SetFont( wxFont( fontName ));
	else
		m_grid->SetFont( wxFont(  10, wxTELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL ));

	m_grid->SetBackgroundColour( *wxWHITE );
}

////////////////////////////////////////////////////////////////////////////////
// clear()
//
//	Remove all stack frames from the display
//

void wsStackWindow::clear( )
{
	m_grid->Set( 0, NULL );
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
		m_grid->Append( stack[i] );
	}
}
