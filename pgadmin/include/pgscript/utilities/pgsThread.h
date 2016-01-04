//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#ifndef PGSTHREAD_H_
#define PGSTHREAD_H_

#include "pgscript/pgScript.h"
#include "pgscript/objects/pgsVariable.h"

#include <wx/thread.h>

class pgConn;
class pgsApplication;
class pgsStmtList;

class pgsThread : public wxThread
{

private:

	/** Symbol table (memory variables). */
	pgsVarMap &m_vars;

	/** In order to have only one thread at once. */
	wxSemaphore &m_mutex;

	/** Connection to the database. */
	pgConn *m_connection;

	/** Either the file to parse or the string. */
	wxString m_data;

	/** Where to write the output. */
	pgsOutputStream &m_out;

	/** The calling application. */
	pgsApplication &m_app;

	/** If set it is the encoding used in the file to parse. */
	wxMBConv *m_conv;

	/** Location of the last error if there was one otherwise -1 */
	int m_last_error_line;

public:

	/** Parses a file with the provided encoding. */
	pgsThread(pgsVarMap &vars, wxSemaphore &mutex, pgConn *connection,
	          const wxString &file, pgsOutputStream &out,
	          pgsApplication &app, wxMBConv *conv);

	/** Parses a wxString. */
	pgsThread(pgsVarMap &vars, wxSemaphore &mutex, pgConn *connection,
	          const wxString &string, pgsOutputStream &out, pgsApplication &app);

	/** Destructor. */
	~pgsThread();

	/** Thread main code. */
	virtual void *Entry();

	/** Retrieves the connection to the database. */
	pgConn *connection();

	/** Gets a lock on the output stream. */
	void LockOutput();

	/** Releases the lock on the output stream. */
	void UnlockOutput();

	/** Set the position (line) of the last error. */
	void last_error_line(int line);

	/** Get the position (line) of the last error. */
	int last_error_line() const;

private:

	pgsThread(const pgsThread &that);

	pgsThread &operator=(const pgsThread &that);

};

#endif /*PGSTHREAD_H_*/
