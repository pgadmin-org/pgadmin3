//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#include "pgAdmin3.h"
#include "pgscript/pgsApplication.h"

#include "db/pgConn.h"
#include "pgscript/expressions/pgsAssign.h"
#include "pgscript/expressions/pgsIdent.h"
#include "pgscript/objects/pgsString.h"
#include "pgscript/utilities/pgsThread.h"

pgsApplication::pgsApplication(const wxString &host, const wxString &database,
                               const wxString &user, const wxString &password, int port) :
	m_mutex(1, 1), m_stream(1, 1), m_connection(pnew pgConn(host, wxEmptyString, wxEmptyString, database, user,
	        password, port)), m_defined_conn(true), m_thread(0), m_caller(0)
{
	if (m_connection->GetStatus() != PGCONN_OK)
	{
		wxLogError(wxT("PGSCRIPT: Cannot connect to database %s:%d/%s with ")
		           wxT("credentials '%s'/'%s'"), host.c_str(), port, database.c_str(),
		           user.c_str(), password.c_str());
	}

	wxLogScript(wxT("Application created"));
}

pgsApplication::pgsApplication(pgConn *connection) :
	m_mutex(1, 1), m_stream(1, 1), m_connection(connection),
	m_defined_conn(false), m_thread(0), m_caller(0)
{
	wxLogScript(wxT("Application created"));
}

pgsApplication::~pgsApplication()
{
	if (m_defined_conn)
	{
		pdelete(m_connection);
	}

	wxLogScript(wxT("Application destroyed"));
}

bool pgsApplication::ParseFile(const wxString &file, pgsOutputStream &out,
                               wxMBConv *conv)
{
	if (!IsRunning())
	{
		m_last_error_line = -1;
		m_thread = new pgsThread(m_vars, m_mutex, m_connection,
		                         file, out, *this, conv);
		return RunThread();
	}
	else
	{
		return false;
	}
}

bool pgsApplication::ParseString(const wxString &string,
                                 pgsOutputStream &out)
{
	if (!IsRunning())
	{
		m_last_error_line = -1;
		m_thread = new pgsThread(m_vars, m_mutex, m_connection,
		                         string, out, *this);
		return RunThread();
	}
	else
	{
		return false;
	}
}

bool pgsApplication::RunThread()
{
	bool created = false;

	if (m_thread != 0 && m_thread->Create() == wxTHREAD_NO_ERROR)
	{
		m_thread->SetPriority(WXTHREAD_MIN_PRIORITY);

		if (m_thread->Run() == wxTHREAD_NO_ERROR)
		{
			created = true;
		}
	}

	if (created)
	{
		wxLogScript(wxT("Running..."));
		return true;
	}
	else
	{
		wxLogError(wxT("PGSCRIPT: Thread error"));
		pdelete(m_thread);
		return false;
	}
}

bool pgsApplication::IsRunning()
{
	if (m_mutex.TryWait() == wxSEMA_NO_ERROR)
	{
		m_mutex.Post();
		return false;
	}
	else
	{
		return true;
	}
}

void pgsApplication::Wait()
{
	if (IsRunning())
	{
		wxLogScript(wxT("Waiting for pgScript"));
		m_mutex.Wait();
		m_mutex.Post();
	}
}

void pgsApplication::Terminate()
{
	if (IsRunning())
	{
		wxLogScript(wxT("Deleting pgScript"));
		m_thread->Delete();
	}
}

void pgsApplication::Complete()
{
	// If last_error_line() == -1 then there was no error
	// Else get the line number where the error occurred
	m_last_error_line = m_thread->last_error_line();

#if !defined(PGSCLI)
	if (m_caller != 0)
	{
		wxCommandEvent resultEvent(wxEVT_COMMAND_MENU_SELECTED, m_event_id);

#if wxCHECK_VERSION(2, 9, 0)
		m_caller->GetEventHandler()->AddPendingEvent(resultEvent);
#else
		m_caller->AddPendingEvent(resultEvent);
#endif
	}
#endif // PGSCLI

	wxLogScript(wxT("Execution completed"));
}

void pgsApplication::SetConnection(pgConn *conn)
{
	if (m_defined_conn)
	{
		pdelete(m_connection);
		m_defined_conn = false;
	}

	m_connection = conn;
}

void pgsApplication::ClearSymbols()
{
	if (!IsRunning())
	{
		m_vars.clear();
	}
}

#if !defined(PGSCLI)
void pgsApplication::SetCaller(wxWindow *caller, long event_id)
{
	m_caller = caller;
	m_event_id = event_id;
}
#endif // PGSCLI

bool pgsApplication::IsConnectionValid() const
{
	return (m_connection->GetStatus() == PGCONN_OK);
}

void pgsApplication::LockOutput()
{
	m_stream.Wait();
}

void pgsApplication::UnlockOutput()
{
	m_stream.Post();
}

bool pgsApplication::errorOccurred() const
{
	return (m_last_error_line != -1);
}

int pgsApplication::errorLine() const
{
	return m_last_error_line;
}
