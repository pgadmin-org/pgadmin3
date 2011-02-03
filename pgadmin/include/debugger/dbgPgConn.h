//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2010, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dbgPgConn.h - debugger
//
//////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// class dbgPgConn
//
//    A dbgPgConn object encapsulates the connection to a PostgreSQL server.  This
//  class is a wrapper around a Pgconn that provides a convenient constructor
//  and a few member functions.
//
//    This class doesn't do much - instead, the real work happens in a dbgPgThread
//  (a separate thread) that initiates commands and processes result sets.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef DBGPGCONN_H
#define DBGPGCONN_H

#include <libpq-fe.h>

#include "debugger/frmDebugger.h"
#include "debugger/dbgPgThread.h"
#include "debugger/dbgConnProp.h"

enum DebuggerApiVersions
{
	DEBUGGER_UNKNOWN_API = 0,
	DEBUGGER_V1_API = 1,
	DEBUGGER_V2_API = 2,
	DEBUGGER_V3_API = 3
};

class dbgPgParams
{
public:
	int nParams;
	Oid *paramTypes;
	char **paramValues;
	int *paramModes;
};

class dbgPgConn
{
public:

	dbgPgConn( frmDebugger *frame,
	           const wxString &server   = wxT( "" ),
	           const wxString &database = wxT( "" ),
	           const wxString &username = wxT( "" ),
	           const wxString &password = wxT( "" ),
	           const wxString &port     = wxT( "5432" ),
	           int sslmode               = 0,
	           const wxString &applicationname = wxT( "" ));

	dbgPgConn( frmDebugger *frame, const dbgConnProp &props, bool startThread = true );

	~dbgPgConn();

	bool BackendMinimumVersion(int major, int minor);
	bool EdbMinimumVersion(int major, int minor);
	bool GetIsEdb();
	bool GetIsGreenplum();
	DebuggerApiVersions DebuggerApiVersion();
	wxString GetVersionString();
	bool isConnected() const;               // Returns true if the connection attempt succeeded
	const wxString  getName() const;        // Returns human-friendly name for this connection
	const wxString  getHost() const;        // Returns the host-name (or IP address) for this connection
	const wxString  getDatabase() const;    // Returns the name of the database that we're connected to
	PGconn *getConnection();                // Returns the libpq connection handle
	void Close();                           // Close this connection
	void Cancel();                          // Cancel any ongoing queries

	void startCommand( const wxString &command, wxEvtHandler *caller, wxEventType eventType = wxEVT_NULL, dbgPgParams *params = NULL );     // Starts executing a command
	void setNoticeHandler( PQnoticeProcessor handler, void *arg );  // Registers a NOTICE handler
	PGresult *waitForCommand( const wxString &command );            // Starts a command and waits for completion

private:

	void Init( const wxString &server, const wxString &database, const wxString &userName, const wxString &password, const wxString &port, int sslmode, const wxString &applicationname, bool startThread );

	PGconn *m_pgConn;               // libpq connection handler
	dbgPgThread *m_workerThread;    // Worker thread (this thread interacts with the server)
	frmDebugger *m_frame;
	int m_minorVersion, m_majorVersion;
	bool m_isEdb;
	bool m_isGreenplum;
	DebuggerApiVersions m_debuggerApiVersion;
};

#endif
