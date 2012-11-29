//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2012, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// pgQueryThread.cpp - PostgreSQL threaded query class
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>

// PostgreSQL headers
#include <libpq-fe.h>

// App headers
#include "db/pgSet.h"
#include "db/pgConn.h"
#include "db/pgQueryThread.h"
#include "utils/sysLogger.h"

static void pgNoticeProcessor(void *arg, const char *message)
{
	wxString str(message, wxConvUTF8);

	wxLogNotice(wxT("%s"), str.Trim().c_str());
	((pgQueryThread *)arg)->appendMessage(str);
}

pgQueryThread::pgQueryThread(pgConn *_conn, const wxString &qry, int _resultToRetrieve, wxWindow *_caller, long _eventId, void *_data)
	: wxThread(wxTHREAD_JOINABLE)
{
	query = qry;
	conn = _conn;
	dataSet = 0;
	result = 0;
	resultToRetrieve = _resultToRetrieve;
	rc = -1;
	insertedOid = (OID) - 1;
	caller = _caller;
	eventId = _eventId;
	data = _data;

	wxLogSql(wxT("Thread query (%s:%d): %s"), conn->GetHost().c_str(), conn->GetPort(), qry.c_str());

	conn->RegisterNoticeProcessor(pgNoticeProcessor, this);
	if (conn->conn)
		PQsetnonblocking(conn->conn, 1);
}

pgQueryThread::~pgQueryThread()
{
	conn->RegisterNoticeProcessor(0, 0);
	if (dataSet)
		delete dataSet;
}


wxString pgQueryThread::GetMessagesAndClear()
{
	wxString msg;

	{
		wxCriticalSectionLocker cs(criticalSection);
		msg = messages;
		messages.Empty();
	}

	return msg;
}


void pgQueryThread::appendMessage(const wxString &str)
{
	if (messages.IsEmpty())
	{
		if (str != wxT("\n"))
			appendMessageRaw(str);
	}
	else
		appendMessageRaw(wxT("\n") + str);
}

void pgQueryThread::appendMessageRaw(const wxString &str)
{
	wxCriticalSectionLocker cs(criticalSection);
	messages.Append(str);
}


int pgQueryThread::execute()
{
	rowsInserted = -1L;

	if (!conn->conn)
		return(raiseEvent(0));

	wxCharBuffer queryBuf = query.mb_str(*conn->conv);
	if (!queryBuf && !query.IsEmpty())
	{
		conn->SetLastResultError(NULL, _("The query could not be converted to the required encoding."));
		return(raiseEvent(0));
	}

	if (!PQsendQuery(conn->conn, queryBuf))
	{
		conn->SetLastResultError(NULL);
		conn->IsAlive();
		return(raiseEvent(0));
	}
	int resultsRetrieved = 0;
	PGresult *lastResult = 0;
	while (true)
	{
		if (TestDestroy())
		{
			if (rc != -3)
			{
				if (!PQrequestCancel(conn->conn)) // could not abort; abort failed.
					return(raiseEvent(-1));

				rc = -3;
			}
		}
		if (!PQconsumeInput(conn->conn))
			return(raiseEvent(0));
		if (PQisBusy(conn->conn))
		{
			Yield();
			this->Sleep(10);
			continue;
		}

		// If resultToRetrieve is given, the nth result will be returned,
		// otherwise the last result set will be returned.
		// all others are discarded
		PGresult *res = PQgetResult(conn->conn);

		if (!res)
			break;

		if (PQresultStatus(res) == PGRES_COPY_IN)
		{
			PQputCopyEnd(conn->conn, "not supported by pgAdmin");
		} 
		if (PQresultStatus(res) == PGRES_COPY_OUT)
		{
			int copyrc;
			char *buf;
			int copyrows = 0;
			int lastcopyrc = 0;

			appendMessage(_("Query returned COPY data:\n"));

			while((copyrc = PQgetCopyData(conn->conn, &buf, 1)) >= 0)
			{
				if (buf != NULL)
				{
					if (copyrows < 100)
					{
						wxString str(buf, wxConvUTF8);
						appendMessageRaw(str);
					} else if (copyrows == 100)
						appendMessage(_("Query returned more than 100 COPY rows, discarding the rest...\n"));

					PQfreemem(buf);
				}
				if (copyrc > 0)
					copyrows++;
				if (TestDestroy() && rc != -3)
				{
					if (!PQrequestCancel(conn->conn)) // could not abort; abort failed.
						return(raiseEvent(-1));
					rc = -3;
				}
				if (lastcopyrc == 0 && copyrc == 0)
				{
					Yield();
					this->Sleep(10);
				}
				if (copyrc == 0)
				{
					if (!PQconsumeInput(conn->conn))
						return(raiseEvent(0));
				}
				lastcopyrc = copyrc;
			}
			res = PQgetResult(conn->conn);
			if (!res)
				break;
		}

		resultsRetrieved++;
		if (resultsRetrieved == resultToRetrieve)
		{
			result = res;
			insertedOid = PQoidValue(res);
			if (insertedOid && insertedOid != (OID) - 1)
				appendMessage(wxString::Format(_("Query inserted one row with OID %d.\n"), insertedOid));
			else
				appendMessage(wxString::Format(wxPLURAL("Query result with %d row will be returned.\n", "Query result with %d rows will be returned.\n",
				                                        PQntuples(result)), PQntuples(result)));
			continue;
		}
		if (lastResult)
		{
			if (PQntuples(lastResult))
				appendMessage(wxString::Format(wxPLURAL("Query result with %d row discarded.\n", "Query result with %d rows discarded.\n",
				                                        PQntuples(lastResult)), PQntuples(lastResult)));
			PQclear(lastResult);
		}
		lastResult = res;
	}

	if (!result)
		result = lastResult;

	conn->SetLastResultError(result);

	appendMessage(wxT("\n"));
	rc = PQresultStatus(result);
	insertedOid = PQoidValue(result);
	if (insertedOid == (OID) - 1)
		insertedOid = 0;

	if (rc == PGRES_TUPLES_OK)
	{
		dataSet = new pgSet(result, conn, *conn->conv, conn->needColQuoting);
		dataSet->MoveFirst();
	}
	else if (rc == PGRES_COMMAND_OK)
	{
		char *s = PQcmdTuples(result);
		if (*s)
			rowsInserted = atol(s);
	}
	else if (rc == PGRES_FATAL_ERROR)
	{
		appendMessage(conn->GetLastError() + wxT("\n"));
	}
	return(raiseEvent(1));
}


int pgQueryThread::raiseEvent(int retval)
{
	if (caller)
	{
#if !defined(PGSCLI)
		wxCommandEvent resultEvent(wxEVT_COMMAND_MENU_SELECTED, eventId);
		resultEvent.SetClientData(data);
#if wxCHECK_VERSION(2, 9, 0)
		caller->GetEventHandler()->AddPendingEvent(resultEvent);
#else
		caller->AddPendingEvent(resultEvent);
#endif

#endif
	}
	return retval;
}


void *pgQueryThread::Entry()
{
	rc = -2;
	execute();

	return(NULL);
}
