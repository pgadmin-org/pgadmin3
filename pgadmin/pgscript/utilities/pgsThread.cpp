//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#include "pgAdmin3.h"
#include "pgscript/utilities/pgsThread.h"

#include "pgscript/pgsApplication.h"
#include "pgscript/statements/pgsProgram.h"
#include "pgscript/utilities/pgsContext.h"
#include "pgscript/utilities/pgsDriver.h"

pgsThread::pgsThread(pgsVarMap &vars, wxSemaphore &mutex,
                     pgConn *connection, const wxString &file, pgsOutputStream &out,
                     pgsApplication &app, wxMBConv *conv) :
	wxThread(wxTHREAD_DETACHED), m_vars(vars), m_mutex(mutex),
	m_connection(connection), m_data(file), m_out(out),
	m_app(app), m_conv(conv), m_last_error_line(-1)
{
	wxLogScript(wxT("Starting thread"));
	m_mutex.Wait();
}

pgsThread::pgsThread(pgsVarMap &vars, wxSemaphore &mutex,
                     pgConn *connection, const wxString &string, pgsOutputStream &out,
                     pgsApplication &app) :
	wxThread(wxTHREAD_DETACHED), m_vars(vars), m_mutex(mutex),
	m_connection(connection), m_data(string), m_out(out),
	m_app(app), m_conv(0), m_last_error_line(-1)
{
	wxLogScript(wxT("Starting thread"));
	m_mutex.Wait();
}

pgsThread::~pgsThread()
{
	wxLogScript(wxT("Finishing thread"));
	m_app.Complete();
	m_mutex.Post();
	wxLogScript(wxT("Thread  finished"));
}

void *pgsThread::Entry()
{
	pgsProgram program(m_vars);
	pgsContext context(m_out);
	pgscript::pgsDriver driver(context, program, *this);

	if (m_conv)
	{
		wxLogScript(wxT("Parsing file"));
		driver.parse_file(m_data, *m_conv);
		wxLogScript(wxT("File  parsed"));
	}
	else
	{
		wxLogScript(wxT("Parsing string"));
		driver.parse_string(m_data);
		wxLogScript(wxT("String  parsed"));
	}

	return 0;
}

pgConn *pgsThread::connection()
{
	return m_connection;
}

void pgsThread::LockOutput()
{
	m_app.LockOutput();
}

void pgsThread::UnlockOutput()
{
	m_app.UnlockOutput();
}

void pgsThread::last_error_line(int line)
{
	m_last_error_line = line;
}

int pgsThread::last_error_line() const
{
	return m_last_error_line;
}
