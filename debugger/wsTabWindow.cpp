//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// wstabwindow.cpp - debugger 
//
//////////////////////////////////////////////////////////////////////////

#include "debugger.h"
#include "wsTabWindow.h"
#include "wsConst.h"
#include <wx/thread.h>

IMPLEMENT_CLASS( wsTabWindow, wxSashLayoutWindow )

////////////////////////////////////////////////////////////////////////////////
// wsTabWindow constructor
//
//	This constructor creates a new notebook (a tab control) and clears out the 
//  rest of the data members.
//

wsTabWindow::wsTabWindow( wxWindow * parent, wxWindowID id, const wxPoint & pos, const wxSize & size, long style, const wxString& name )
	: wxSashLayoutWindow( parent, id, pos, size, style, name ),
	m_noteBook( 0 ),
	m_resultWindow( 0 ),
	m_varWindow( 0 ),
	m_pkgVarWindow( 0 ),
	m_stackWindow( 0 ),
	m_paramWindow( 0 ),
	m_messageWindow( 0 )
{
	m_tabMap   = new wsTabHash();
	m_noteBook = new wxNotebook( this, -1, wxDefaultPosition, wxDefaultSize, wxNB_BOTTOM );

}

void wsTabWindow::selectTab( wxWindowID id )
{
	wsTabHash::iterator result = m_tabMap->find( id );

	if( result != m_tabMap->end())
	{
		m_noteBook->SetSelection( result->second );
	}
#if 0
	switch( id )
	{
		case ID_PARAMGRID:  m_noteBook->SetSelection( 0 ); break;
		case ID_VARGRID:    m_noteBook->SetSelection( 1 ); break;
		case ID_MSG_PAGE:   m_noteBook->SetSelection( 2 ); break;
		default: break;
	}
#endif
}

////////////////////////////////////////////////////////////////////////////////
// getResultWindow()
//
//	This function returns a pointer to our child result window (m_resultWindow)
//  and creates that window when we first need it.
//

wsResultGrid * wsTabWindow::getResultWindow( void )
{
	if( m_resultWindow == 0 )
	{
		// We don't have a result window yet - go ahead and create one

		m_resultWindow = new wsResultGrid( m_noteBook, -1 );
		m_noteBook->AddPage( m_resultWindow, _( "Results" ), true );
	}

	return( m_resultWindow );
}

////////////////////////////////////////////////////////////////////////////////
// getVarWindow()
//
//	This function returns a pointer to our child 'local-variables' window 
//  (m_varWindow) and creates that window when we first need it.
//

wsVarWindow * wsTabWindow::getVarWindow( bool create )
{
	if(( m_varWindow == NULL ) && create )
	{
		// We don't have a variable window yet - go ahead and create one

		(*m_tabMap)[ID_VARGRID] = m_noteBook->GetPageCount();

		m_varWindow = new wsVarWindow( m_noteBook, ID_VARGRID );
		m_noteBook->AddPage( m_varWindow, _( "Local Variables" ), true );
	}

	return( m_varWindow );
}

////////////////////////////////////////////////////////////////////////////////
// getPkgVarWindow()
//
//  This function returns a pointer to our child 'package-variables' window 
//  (m_varWindow) and creates that window when we first need it.
//

wsVarWindow * wsTabWindow::getPkgVarWindow( bool create )
{
	if(( m_pkgVarWindow == NULL ) && create )
	{
		// We don't have a variable window yet - go ahead and create one

		(*m_tabMap)[ID_PKGVARGRID] = m_noteBook->GetPageCount();

		m_pkgVarWindow = new wsVarWindow( m_noteBook, ID_PKGVARGRID );
		m_noteBook->AddPage( m_pkgVarWindow, _( "Package Variables" ), true );
	}

	return( m_pkgVarWindow );
}

////////////////////////////////////////////////////////////////////////////////
// getParamWindow()
//
//	This function returns a pointer to our child 'parameters' window 
//  (m_paramWindow) and creates that window when we first need it.
//

wsVarWindow * wsTabWindow::getParamWindow( bool create )
{
	if(( m_paramWindow == NULL ) && create )
	{
		// We don't have a variable window yet - go ahead and create one

		(*m_tabMap)[ID_PARAMGRID] = m_noteBook->GetPageCount();

		m_paramWindow = new wsVarWindow( m_noteBook, ID_PARAMGRID );
		m_noteBook->AddPage( m_paramWindow, _( "Parameters" ), true );
	}

	return( m_paramWindow );
}

////////////////////////////////////////////////////////////////////////////////
// getMessageWindow()
//
//	This function returns a pointer to our child 'messages' window 
//  (m_messageWindow) and creates that window when we first need it.
//

wsMessageWindow * wsTabWindow::getMessageWindow( void )
{
	if( m_messageWindow == 0 )
	{
		// We don't have a variable window yet - go ahead and create one

		(*m_tabMap)[ID_MSG_PAGE] = m_noteBook->GetPageCount();

		m_messageWindow = new wsMessageWindow( m_noteBook, ID_MSG_PAGE );
		m_noteBook->AddPage( m_messageWindow, _( "DBMS Messages" ), true );
	}
	return( m_messageWindow );
}

////////////////////////////////////////////////////////////////////////////////
// getStackWindow()
//
//	This function returns a pointer to our child stack-trace window 
//  (m_stackWindow) and creates that window when we first need it.
//

wsStackWindow * wsTabWindow::getStackWindow( )
{
	if( m_stackWindow == 0 )
	{
		// We don't have a stack-trace window yet - go ahead and create one
		m_stackWindow = new wsStackWindow( m_noteBook, -1 );
		m_noteBook->AddPage( m_stackWindow, _( "Stack" ), true );
	}

	return( m_stackWindow );
}

