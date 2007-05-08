//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id: ctlTabWindow.cpp 6200 2007-04-18 10:00:20Z dpage $
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// ctlTabWindow.cpp - debugger 
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>
#include <wx/thread.h>

// App headers
#include "debugger/ctlTabWindow.h"
#include "debugger/dbgConst.h"


IMPLEMENT_CLASS( ctlTabWindow, wxWindow )

////////////////////////////////////////////////////////////////////////////////
// ctlTabWindow constructor
//
//	This constructor creates a new notebook (a tab control) and clears out the 
//  rest of the data members.
//

ctlTabWindow::ctlTabWindow( wxWindow * parent, wxWindowID id, const wxPoint & pos, const wxSize & size, long style, const wxString& name )
  : wxNotebook( parent, id, pos, size, style | wxNB_BOTTOM ),
	m_resultWindow( 0 ),
	m_varWindow( 0 ),
	m_pkgVarWindow( 0 ),
	m_stackWindow( 0 ),
	m_paramWindow( 0 ),
	m_messageWindow( 0 )
{
    wxWindowBase::SetFont(settings->GetSystemFont());
	m_tabMap   = new wsTabHash();
}

void ctlTabWindow::selectTab( wxWindowID id )
{
	wsTabHash::iterator result = m_tabMap->find( id );

	if( result != m_tabMap->end())
	{
		SetSelection( result->second );
	}
}

////////////////////////////////////////////////////////////////////////////////
// getResultWindow()
//
//	This function returns a pointer to our child result window (m_resultWindow)
//  and creates that window when we first need it.
//

ctlResultGrid * ctlTabWindow::getResultWindow( void )
{
	if( m_resultWindow == 0 )
	{
		// We don't have a result window yet - go ahead and create one

		m_resultWindow = new ctlResultGrid( this, -1 );
		AddPage( m_resultWindow, _( "Results" ), true );
	}

	return( m_resultWindow );
}

////////////////////////////////////////////////////////////////////////////////
// getVarWindow()
//
//	This function returns a pointer to our child 'local-variables' window 
//  (m_varWindow) and creates that window when we first need it.
//

ctlVarWindow * ctlTabWindow::getVarWindow( bool create )
{
	if(( m_varWindow == NULL ) && create )
	{
		// We don't have a variable window yet - go ahead and create one

		(*m_tabMap)[ID_VARGRID] = GetPageCount();

		m_varWindow = new ctlVarWindow( this, ID_VARGRID );
		AddPage( m_varWindow, _( "Local Variables" ), true );
	}

	return( m_varWindow );
}

////////////////////////////////////////////////////////////////////////////////
// getPkgVarWindow()
//
//  This function returns a pointer to our child 'package-variables' window 
//  (m_varWindow) and creates that window when we first need it.
//

ctlVarWindow * ctlTabWindow::getPkgVarWindow( bool create )
{
	if(( m_pkgVarWindow == NULL ) && create )
	{
		// We don't have a variable window yet - go ahead and create one

		(*m_tabMap)[ID_PKGVARGRID] = GetPageCount();

		m_pkgVarWindow = new ctlVarWindow( this, ID_PKGVARGRID );
		AddPage( m_pkgVarWindow, _( "Package Variables" ), true );
	}

	return( m_pkgVarWindow );
}

////////////////////////////////////////////////////////////////////////////////
// getParamWindow()
//
//	This function returns a pointer to our child 'parameters' window 
//  (m_paramWindow) and creates that window when we first need it.
//

ctlVarWindow * ctlTabWindow::getParamWindow( bool create )
{
	if(( m_paramWindow == NULL ) && create )
	{
		// We don't have a variable window yet - go ahead and create one

		(*m_tabMap)[ID_PARAMGRID] = GetPageCount();

		m_paramWindow = new ctlVarWindow( this, ID_PARAMGRID );
		AddPage( m_paramWindow, _( "Parameters" ), true );
	}

	return( m_paramWindow );
}

////////////////////////////////////////////////////////////////////////////////
// getMessageWindow()
//
//	This function returns a pointer to our child 'messages' window 
//  (m_messageWindow) and creates that window when we first need it.
//

ctlMessageWindow * ctlTabWindow::getMessageWindow( void )
{
	if( m_messageWindow == 0 )
	{
		// We don't have a variable window yet - go ahead and create one

		(*m_tabMap)[ID_MSG_PAGE] = GetPageCount();

		m_messageWindow = new ctlMessageWindow( this, ID_MSG_PAGE );
		AddPage( m_messageWindow, _( "DBMS Messages" ), true );
	}
	return( m_messageWindow );
}

////////////////////////////////////////////////////////////////////////////////
// getStackWindow()
//
//	This function returns a pointer to our child stack-trace window 
//  (m_stackWindow) and creates that window when we first need it.
//

ctlStackWindow * ctlTabWindow::getStackWindow( )
{
	if( m_stackWindow == 0 )
	{
		// We don't have a stack-trace window yet - go ahead and create one
		m_stackWindow = new ctlStackWindow( this, -1 );
		AddPage( m_stackWindow, _( "Stack" ), true );
	}

	return( m_stackWindow );
}

