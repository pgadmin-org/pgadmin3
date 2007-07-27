//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id: dbgResultset.cpp 6202 2007-04-18 11:22:49Z dpage $
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// dbgResultset.cpp - debugger 
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "debugger/dbgResultset.h"

////////////////////////////////////////////////////////////////////////////////
// dbgResultset constructor
//
//    A dbgResultset object encapsulates a result set produced by executing a 
//  database command. This class is a wrapper around a PGresult handle that 
//    provides a few convenient member functions.  
//

dbgResultset::dbgResultset( PGresult * handle )
    : m_handle( handle )
{
}

////////////////////////////////////////////////////////////////////////////////
// getString()
//
//    This function, given a column number and a row number, returns the value in
//  that slot in the result set (in the form of a string). 
//
//  Notice that row defaults to 0, which is handy if you have a single-row result
//    set.

const wxString dbgResultset::getString( int column, int row )
{
    return( wxString( PQgetvalue( m_handle, row, column ), wxConvUTF8 ));
}

////////////////////////////////////////////////////////////////////////////////
// getString()
//
//    This function, given a column name and a row number, returns the value in
//  that slot in the result set (in the form of a string). 
//
//  Notice that row defaults to 0.

const wxString dbgResultset::getString( const wxString &columnName, int row )
{
    return( getString( PQfnumber( m_handle, columnName.mb_str( wxConvUTF8 )), row ));
}

////////////////////////////////////////////////////////////////////////////////
// getLong()
//
//    This function, given a column number and a row number, converts the value 
//  in that slot into a long and returns that long.
//
//  Notice that row defaults to 0.

long dbgResultset::getLong( int column, int row )
{
    return( atoi( PQgetvalue( m_handle, row, column )));
}

////////////////////////////////////////////////////////////////////////////////
// getLong()
//
//    This function, given a column name and a row number, converts the value 
//  in that slot into a long and returns that long.
//
//  Notice that row defaults to 0.

long dbgResultset::getLong( const wxString &columnName, int row )
{
    return( getLong( PQfnumber( m_handle, columnName.mb_str( wxConvUTF8 )), row ));
}

////////////////////////////////////////////////////////////////////////////////
// getBool()
//
//    This function, given a column number and a row number, converts the value 
//  in that slot into a bool.
//
//  Notice that row defaults to 0.

bool dbgResultset::getBool( int column, int row )
{
    return( PQgetvalue( m_handle, row, column )[0] == 't' ? true : false );
}

////////////////////////////////////////////////////////////////////////////////
// getBool()
//
//    This function, given a column name and a row number, converts the value 
//  in that slot into a bool.
//
//  Notice that row defaults to 0.

bool dbgResultset::getBool( const wxString &columnName, int row )
{
    return( getBool( PQfnumber( m_handle, columnName.mb_str( wxConvUTF8 )), row ));
}

////////////////////////////////////////////////////////////////////////////////
// getCommandStatus()
//
//    This function returns the command status contained in the result set. (The
//  command status will be a value such as PGRES_TUPLES_OK or PGRES_FATAL_ERROR)

ExecStatusType dbgResultset::getCommandStatus()
{
    return( PQresultStatus( m_handle ));
}

////////////////////////////////////////////////////////////////////////////////
// getErrorMessage()
//
//    This function returns the error message (if any) contained in the result set

const wxString dbgResultset::getErrorMessage()
{
    return( wxString( PQresultErrorMessage( m_handle ), wxConvUTF8 ));
}

////////////////////////////////////////////////////////////////////////////////
// getRawErrorMessage()
//
//    This function returns the error message (if any) contained in the result set
//
//    Use this variant (as opposed to getErrorMessage()) when you need the error
//    message in the same character encoding provided by the server

const char * dbgResultset::getRawErrorMessage()
{
    return( PQresultErrorMessage( m_handle ));
}

////////////////////////////////////////////////////////////////////////////////
// columnExists()
//
//    Check if a column exists in the resultset

bool dbgResultset::columnExists(const wxString &columnName)
{
    if(PQfnumber(m_handle, columnName.mb_str(wxConvUTF8)) != -1)
        return(true);
    else
		return(false);
}
