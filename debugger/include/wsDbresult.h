//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// wsDbresult.h - debugger 
//
//////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// class wsDBResult
//
//	This class is used to convert a PGresult (a query result set) into a wxEvent
//  We create a wsDBResult object whenever a result set arrives from the server
//  and then we send that object (which is really a wxEvent) through the normal
//  wxWidgets event handler mechanism.
//
//  The arrival of a result set thus becomes a wxEvent.  We create wsDBResult 
//  objects in wsPgThread.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef WSDBRESULTH
#define WSDBRESULTH

#include <wx/event.h>
#include <wx/string.h>
#include <libpq-fe.h>

BEGIN_DECLARE_EVENT_TYPES()
    DECLARE_EVENT_TYPE( wsDBRESULT, wxID_HIGHEST+1 )
    DECLARE_EVENT_TYPE( wsDBGEVENT, wxID_HIGHEST+2 )
END_DECLARE_EVENT_TYPES()

class wsDBResult : public wxEvent
{

public:
	wsDBResult( PGresult * result ) : wxEvent( 0, wsDBRESULT ), m_result( result ) { }

	wxEvent  * Clone( void ) const { return( new wsDBResult( *this )); }
	PGresult * getResult( void )   { return( m_result ); }

private:
	PGresult	*m_result;
};

#endif
