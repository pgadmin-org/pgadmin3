//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id: dbgDbResult.h 6136 2007-03-29 10:38:10Z hiroshi $
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// dbgDbResult.h - debugger 
//
//////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// class dbgDbResult
//
//	This class is used to convert a PGresult (a query result set) into a wxEvent
//  We create a dbgDbResult object whenever a result set arrives from the server
//  and then we send that object (which is really a wxEvent) through the normal
//  wxWidgets event handler mechanism.
//
//  The arrival of a result set thus becomes a wxEvent.  We create dbgDbResult 
//  objects in dbgPgThread.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef DBGDBRESULT_H
#define DBGDBRESULT_H

#include <libpq-fe.h>

BEGIN_DECLARE_EVENT_TYPES()
    DECLARE_EVENT_TYPE( dbgDBRESULT, wxID_HIGHEST+1 )
END_DECLARE_EVENT_TYPES()

class dbgDbResult : public wxEvent
{

public:
	dbgDbResult( PGresult * result ) : wxEvent( 0, dbgDBRESULT ), m_result( result ) { }

	wxEvent  * Clone( void ) const { return( new dbgDbResult( *this )); }
	PGresult * getResult( void )   { return( m_result ); }

private:
	PGresult	*m_result;
};

#endif
