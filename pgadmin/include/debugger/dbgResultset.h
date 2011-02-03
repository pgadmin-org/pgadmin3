//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2010, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dbgResultset.h - debugger
//
//////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// class dbgResultset
//
//	A dbgResultset object encapsulates a result set produced by executing a
//  database command. This class is a wrapper around a PGresult handle that
//	provides a few convenient member functions.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef DBGRESULTSET_H
#define DBGRESULTSET_H

#include <libpq-fe.h>

class dbgResultset
{
public:
	dbgResultset( PGresult *handle );

	const char	*getRawErrorMessage();	// Return error message as a char *
	const wxString getErrorMessage();	// Return error message as a wxString
	const wxString getString(int column, int row = 0);
	const wxString getString(const wxString &columnName, int row = 0);
	long	getLong(int column, int row = 0);
	long	getLong(const wxString &columnName, int row = 0);
	bool	getBool(int column, int row = 0);
	bool	getBool(const wxString &columnName, int row = 0);
	int	getRowCount()
	{
		return(PQntuples( m_handle));
	}
	bool	columnExists(const wxString &columnname);

	ExecStatusType getCommandStatus();

private:

	PGresult *m_handle;

};
#endif
