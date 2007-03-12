//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id: wsresultset.cpp 5827 2007-01-04 16:35:14 hiroshi $
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// wsresultset.cpp - debugger 
//
//////////////////////////////////////////////////////////////////////////

#include "wsResultset.h"

////////////////////////////////////////////////////////////////////////////////
// wsResultSet constructor
//
//	A wsResultSet object encapsulates a result set produced by executing a 
//  database command. This class is a wrapper around a PGresult handle that 
//	provides a few convenient member functions.  
//

wsResultSet::wsResultSet( PGresult * handle )
	: m_handle( handle )
{
}

////////////////////////////////////////////////////////////////////////////////
// getString()
//
//	This function, given a column number and a row number, returns the value in
//  that slot in the result set (in the form of a string). 
//
//  Notice that row defaults to 0, which is handy if you have a single-row result
//	set.

const wxString wsResultSet::getString( int column, int row )
{
	return( wxString( PQgetvalue( m_handle, row, column ), wxConvUTF8 ));
}

////////////////////////////////////////////////////////////////////////////////
// getString()
//
//	This function, given a column name and a row number, returns the value in
//  that slot in the result set (in the form of a string). 
//
//  Notice that row defaults to 0.

const wxString wsResultSet::getString( const wxString & columnName, int row )
{
	return( getString( PQfnumber( m_handle, columnName.mb_str( wxConvUTF8 )), row ));
}

////////////////////////////////////////////////////////////////////////////////
// getLong()
//
//	This function, given a column number and a row number, converts the value 
//  in that slot into a long and returns that long.
//
//  Notice that row defaults to 0.

long wsResultSet::getLong( int column, int row )
{
	return( atoi( PQgetvalue( m_handle, row, column )));
}

////////////////////////////////////////////////////////////////////////////////
// getLong()
//
//	This function, given a column name and a row number, converts the value 
//  in that slot into a long and returns that long.
//
//  Notice that row defaults to 0.

long wsResultSet::getLong( const wxString & columnName, int row )
{
	return( getLong( PQfnumber( m_handle, columnName.mb_str( wxConvUTF8 )), row ));
}

////////////////////////////////////////////////////////////////////////////////
// getBool()
//
//	This function, given a column number and a row number, converts the value 
//  in that slot into a bool.
//
//  Notice that row defaults to 0.

bool wsResultSet::getBool( int column, int row )
{
	return( PQgetvalue( m_handle, row, column )[0] == 't' ? true : false );
}

////////////////////////////////////////////////////////////////////////////////
// getBool()
//
//	This function, given a column name and a row number, converts the value 
//  in that slot into a bool.
//
//  Notice that row defaults to 0.

bool wsResultSet::getBool( const wxString & columnName, int row )
{
	return( getBool( PQfnumber( m_handle, columnName.mb_str( wxConvUTF8 )), row ));
}

////////////////////////////////////////////////////////////////////////////////
// getCommandStatus()
//
//	This function returns the command status contained in the result set. (The
//  command status will be a value such as PGRES_TUPLES_OK or PGRES_FATAL_ERROR)

ExecStatusType wsResultSet::getCommandStatus()
{
	return( PQresultStatus( m_handle ));
}

////////////////////////////////////////////////////////////////////////////////
// getErrorMessage()
//
//	This function returns the error message (if any) contained in the result set

const wxString wsResultSet::getErrorMessage()
{
	return( wxString( PQresultErrorMessage( m_handle ), wxConvUTF8 ));
}

////////////////////////////////////////////////////////////////////////////////
// getRaErrorMessage()
//
//	This function returns the error message (if any) contained in the result set
//
//	Use this variant (as opposed to getErrorMessage()) when you need the error
//	message in the same character encoding provided by the server

const char * wsResultSet::getRawErrorMessage()
{
	return( PQresultErrorMessage( m_handle ));
}
