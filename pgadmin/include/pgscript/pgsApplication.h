//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#ifndef PGSAPP_H_
#define PGSAPP_H_

#include "pgscript/pgScript.h"
#include "pgscript/utilities/pgsThread.h"

class pgsApplication
{

public:

	static const int default_port = 5432;

private:

	/** Global symbol table shared between different runs. */
	pgsVarMap m_vars;

	/** In order to have only one thread at once. */
	wxSemaphore m_mutex;

	/** Protects stream accesses. */
	wxSemaphore m_stream;

	/** Connection to the database. */
	pgConn *m_connection;

	/** Is m_connection provided in the constructor or has it been created. */
	bool m_defined_conn;

	/** Detached thread running a pgScript (parses a file or a string). */
	pgsThread *m_thread;

	/** pgAdmin specific: post an event to this window when m_thread is done. */
	wxWindow *m_caller;

	/** pgAdmin specific: post this event when m_thread is done. */
	long m_event_id;

	/** Location of the last error if there was one. */
	int m_last_error_line;

public:

	/** Creates an application and creates a connection. */
	pgsApplication(const wxString &host, const wxString &database,
	               const wxString &user, const wxString &password, int port = default_port);

	/** Creates an application and uses an existing connection. This connection
	 * is not deleted when the application is deleted. */
	pgsApplication(pgConn *connection);

	/** Deletes custom connection if one was created (first constructor). */
	~pgsApplication();

	/** Parses a file by creating a new thread. */
	bool ParseFile(const wxString &file, pgsOutputStream &out,
	               wxMBConv *conv = &wxConvLocal);

	/** Parses a string by creating a new thread. */
	bool ParseString(const wxString &string, pgsOutputStream &out);

	/** Is m_thread running? */
	bool IsRunning();

	/** If m_thread is running then wait for it to terminate. */
	void Wait();

	/** If m_thread is running then delete it. */
	void Terminate();

	/** Called by m_thread when the thread is finished: IsRunning() becomes
	 * false and m_event_id is pushed into the event queue if m_caller exists. */
	void Complete();

	/** Uses a new database connection instead of the previous one. If the
	 * previous one was user-defined then it is deleted otherwise it is just
	 * replaced with the new one. */
	void SetConnection(pgConn *conn);

	/** Deletes everything in the symbol table. */
	void ClearSymbols();

#if !defined(PGSCLI)
	/** Used in pgAdmin integration for sending an event to the caller when the
	 * thread is finishing its task. */
	void SetCaller(wxWindow *caller, long event_id);
#endif // PGSCLI

	/** Tells whether the database connection is valid. */
	bool IsConnectionValid() const;

	/** Gets a lock on the output stream. */
	void LockOutput();

	/** Releases the lock on the output stream. */
	void UnlockOutput();

	/** Was there an error? */
	bool errorOccurred() const;

	/** Get the position (line) of the last error. */
	int errorLine() const;

private:

	/** Common method for parse_file & parse_string: runs the thread. */
	bool RunThread();

private:

	pgsApplication(const pgsApplication &that);

	pgsApplication &operator=(const pgsApplication &that);

};

#endif /*PGSAPP_H_*/
