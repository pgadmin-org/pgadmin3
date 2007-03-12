//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id: wsResultset.h 5827 2007-01-04 16:35:14 hiroshi $
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// wsResultset.h - debugger 
//
//////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// class wsResultSet
//
//	A wsResultSet object encapsulates a result set produced by executing a 
//  database command. This class is a wrapper around a PGresult handle that 
//	provides a few convenient member functions.  
//
////////////////////////////////////////////////////////////////////////////////

#ifndef WSRESULTSETH
#define WSRESULTSETH

#include <wx/string.h>
#include <libpq-fe.h>

class wsResultSet
{
public:
	wsResultSet( PGresult * handle );

	const char   * getRawErrorMessage();	// Return error message as a char *
	const wxString getErrorMessage();		// Return error message as a wxString
	const wxString getString( int column, int row = 0 );
	const wxString getString( const wxString & columnName, int row = 0 );
	long           getLong( int column, int row = 0 );
	long           getLong( const wxString & columnName, int row = 0 );
	bool		   getBool( int column, int row = 0 );
	bool		   getBool( const wxString & columnName, int row = 0 );
	int			   getRowCount() { return( PQntuples( m_handle )); }

	ExecStatusType getCommandStatus();

private:

	PGresult * m_handle;

};
#endif
