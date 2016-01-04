//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
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
#include "db/pgQueryResultEvent.h"
#include "utils/pgDefs.h"
#include "utils/sysLogger.h"

const wxEventType PGQueryResultEvent = wxNewEventType();

// default notice processor for the pgQueryThread
// we do assume that the argument passed will be always the
// object of pgQueryThread
void pgNoticeProcessor(void *_arg, const char *_message)
{
	wxString str(_message, wxConvUTF8);

	wxLogNotice(wxT("%s"), str.Trim().c_str());
	if (_arg)
	{
		((pgQueryThread *)_arg)->AppendMessage(str);
	}
}


// support for multiple queries support
pgQueryThread::pgQueryThread(pgConn *_conn, wxEvtHandler *_caller,
                             PQnoticeProcessor _processor, void *_noticeHandler) :
	wxThread(wxTHREAD_JOINABLE), m_currIndex(-1), m_conn(_conn),
	m_cancelled(false), m_multiQueries(true), m_useCallable(false),
	m_caller(_caller), m_processor(pgNoticeProcessor), m_noticeHandler(NULL),
	m_eventOnCancellation(true)
{
	// check if we can really use the enterprisedb callable statement and
	// required
#ifdef __WXMSW__
	if (PQiGetOutResult && PQiPrepareOut && PQiSendQueryPreparedOut)
	{
		// we do not need all of pqi stuff as90 onwards
		if (m_conn && m_conn->conn && m_conn->GetIsEdb()
		        && !m_conn->EdbMinimumVersion(9, 0))
		{
			m_useCallable = true;
		}
	}
#else // __WXMSW__
#ifdef EDB_LIBPQ
	// use callable statement only with enterprisedb advanced servers
	// we do not need all of pqi stuff as90 onwards
	if (m_conn && m_conn->conn && m_conn->GetIsEdb()
	        && !m_conn->EdbMinimumVersion(9, 0))
	{
		m_useCallable = true;
	}
#endif // EDB_LIBPQ
#endif // __WXMSW__

	if (m_conn && m_conn->conn)
	{
		PQsetnonblocking(m_conn->conn, 1);
	}

	if (_processor != NULL)
	{
		m_processor = _processor;
		if (_noticeHandler)
			m_noticeHandler = _noticeHandler;
		else
			m_noticeHandler = (void *)this;
	}
	else
	{
		m_noticeHandler = (void *)this;
	}
}

pgQueryThread::pgQueryThread(pgConn *_conn, const wxString &_qry,
                             int _resultToRetrieve, wxWindow *_caller, long _eventId, void *_data)
	: wxThread(wxTHREAD_JOINABLE), m_currIndex(-1), m_conn(_conn),
	  m_cancelled(false), m_multiQueries(false), m_useCallable(false),
	  m_caller(NULL), m_processor(pgNoticeProcessor), m_noticeHandler(NULL),
	  m_eventOnCancellation(true)
{
	if (m_conn && m_conn->conn)
	{
		PQsetnonblocking(m_conn->conn, 1);
	}
	m_queries.Add(
	    new pgBatchQuery(_qry, (pgParamsArray *)NULL, _eventId, _data, false,
	                     _resultToRetrieve));

	wxLogInfo(wxT("queueing : %s"), _qry.c_str());
	m_noticeHandler = (void *)this;

	if (_caller)
	{
		m_caller = _caller->GetEventHandler();
	}
}

void pgQueryThread::SetEventOnCancellation(bool eventOnCancelled)
{
	m_eventOnCancellation = eventOnCancelled;
}

void pgQueryThread::AddQuery(const wxString &_qry, pgParamsArray *_params,
                             long _eventId, void *_data, bool _useCallable, int _resultToRetrieve)
{
	m_queries.Add(
	    new pgBatchQuery(_qry, _params, _eventId, _data,
	                     // use callable statement only if supported
	                     m_useCallable && _useCallable, _resultToRetrieve));

	wxLogInfo(wxT("queueing (%ld): %s"), GetId(), _qry.c_str());
}


pgQueryThread::~pgQueryThread()
{
	m_conn->RegisterNoticeProcessor(0, 0);
	WX_CLEAR_ARRAY(m_queries);
}


wxString pgQueryThread::GetMessagesAndClear(int idx)
{
	wxString msg;

	if (idx == -1)
		idx = m_currIndex;

	if (idx >= 0 && idx <= m_currIndex)
	{
		wxCriticalSectionLocker lock(m_criticalSection);
		msg = m_queries[idx]->m_message;
		m_queries[idx]->m_message = wxT("");
	}

	return msg;
}


void pgQueryThread::AppendMessage(const wxString &str)
{
	if (m_queries[m_currIndex]->m_message.IsEmpty())
	{
		wxCriticalSectionLocker lock(m_criticalSection);
		if (str != wxT("\n"))
			m_queries[m_currIndex]->m_message.Append(str);
	}
	else
	{
		wxCriticalSectionLocker lock(m_criticalSection);
		m_queries[m_currIndex]->m_message.Append(wxT("\n")).Append(str);
	}
}


int pgQueryThread::Execute()
{
	wxMutexLocker lock(m_queriesLock);

	PGresult       *result           = NULL;
	wxMBConv       &conv             = *(m_conn->conv);

	wxString       &query            = m_queries[m_currIndex]->m_query;
	int            &resultToRetrieve = m_queries[m_currIndex]->m_resToRetrieve;
	long           &rowsInserted     = m_queries[m_currIndex]->m_rowsInserted;
	Oid            &insertedOid      = m_queries[m_currIndex]->m_insertedOid;
	// using the alias for the pointer here, in order to save the result back
	// in the pgBatchQuery object
	pgSet         *&dataSet          = m_queries[m_currIndex]->m_resultSet;
	int            &rc               = m_queries[m_currIndex]->m_returnCode;
	pgParamsArray  *params           = m_queries[m_currIndex]->m_params;
	bool            useCallable      = m_queries[m_currIndex]->m_useCallable;
	pgError        &err              = m_queries[m_currIndex]->m_err;

	wxCharBuffer queryBuf = query.mb_str(conv);

	if (PQstatus(m_conn->conn) != CONNECTION_OK)
	{
		rc = pgQueryResultEvent::PGQ_CONN_LOST;
		err.msg_primary = _("Connection to the database server lost");

		return(RaiseEvent(rc));
	}

	if (!queryBuf && !query.IsEmpty())
	{
		rc = pgQueryResultEvent::PGQ_STRING_INVALID;
		m_conn->SetLastResultError(NULL, _("the query could not be converted to the required encoding."));
		err.msg_primary = _("Query string is empty");

		return(RaiseEvent(rc));
	}

	// Honour the parameters (if any)
	if (params && params->GetCount() > 0)
	{
		int    pCount = params->GetCount();
		int    ret    = 0,
		       idx    = 0;

		Oid         *pOids    = (Oid *)malloc(pCount * sizeof(Oid));
		const char **pParams  = (const char **)malloc(pCount * sizeof(const char *));
		int         *pLens    = (int *)malloc(pCount * sizeof(int));
		int         *pFormats = (int *)malloc(pCount * sizeof(int));
		// modes are used only by enterprisedb callable statement
#if defined (__WXMSW__) || (EDB_LIBPQ)
		int         *pModes   = (int *)malloc(pCount * sizeof(int));
#endif

		for (; idx < pCount; idx++)
		{
			pgParam *param = (*params)[idx];

			pOids[idx] = param->m_type;
			pParams[idx] = (const char *)param->m_val;
			pLens[idx] = param->m_len;
			pFormats[idx] = param->GetFormat();
#if defined (__WXMSW__) || (EDB_LIBPQ)
			pModes[idx] = param->m_mode;
#endif
		}

		if (useCallable)
		{
#if defined (__WXMSW__) || (EDB_LIBPQ)
			wxLogInfo(wxString::Format(
			              _("using an enterprisedb callable statement (queryid:%ld, threadid:%ld)"),
			              (long)m_currIndex, (long)GetId()));
			wxString stmt = wxString::Format(wxT("pgQueryThread-%ld-%ld"), this->GetId(), m_currIndex);
			PGresult *res = PQiPrepareOut(m_conn->conn, stmt.mb_str(wxConvUTF8),
			                              queryBuf, pCount, pOids, pModes);

			if( PQresultStatus(res) != PGRES_COMMAND_OK)
			{
				rc = pgQueryResultEvent::PGQ_ERROR_PREPARE_CALLABLE;
				err.SetError(res, &conv);

				PQclear(res);

				goto return_with_error;
			}

			ret = PQiSendQueryPreparedOut(m_conn->conn, stmt.mb_str(wxConvUTF8),
			                              pCount, pParams, pLens, pFormats, 1);

			if (ret != 1)
			{
				rc = pgQueryResultEvent::PGQ_ERROR_EXECUTE_CALLABLE;

				m_conn->SetLastResultError(NULL, _("Failed to run PQsendQuery in pgQueryThread"));
				err.msg_primary = wxString(PQerrorMessage(m_conn->conn), conv);

				PQclear(res);
				res = NULL;

				goto return_with_error;
			}

			PQclear(res);
			res = NULL;
#else
			rc = -1;
			wxASSERT_MSG(false,
			             _("the program execution flow must not reach to this point in pgQueryThread"));

			goto return_with_error;
#endif
		}
		else
		{
			// assumptions: we will need the results in text format only
			ret = PQsendQueryParams(m_conn->conn, queryBuf, pCount, pOids, pParams, pLens, pFormats, 0);

			if (ret != 1)
			{
				rc = pgQueryResultEvent::PGQ_ERROR_SEND_QUERY;

				m_conn->SetLastResultError(NULL,
				                           _("Failed to run PQsendQueryParams in pgQueryThread"));

				err.msg_primary = _("Failed to run PQsendQueryParams in pgQueryThread.\n") +
				                  wxString(PQerrorMessage(m_conn->conn), conv);

				goto return_with_error;
			}
		}
		goto continue_without_error;

return_with_error:
		{
			free(pOids);
			free(pParams);
			free(pLens);
			free(pFormats);
#if defined (__WXMSW__) || (EDB_LIBPQ)
			free(pModes);
#endif
			return (RaiseEvent(rc));
		}
	}
	else
	{
		// use the PQsendQuery api in case, we don't have any parameters to
		// pass to the server
		if (!PQsendQuery(m_conn->conn, queryBuf))
		{
			rc = pgQueryResultEvent::PGQ_ERROR_SEND_QUERY;

			err.msg_primary = _("Failed to run PQsendQueryParams in pgQueryThread.\n") +
			                  wxString(PQerrorMessage(m_conn->conn), conv);

			return(RaiseEvent(rc));
		}
	}

continue_without_error:
	int resultsRetrieved = 0;
	PGresult *lastResult = 0;
	bool connExecutionCancelled = false;

	while (true)
	{
		// This is a 'joinable' thread, it is not advisable to call 'delete'
		// function on this.
		// Hence - it does not make sense to use the function 'testdestroy' here.
		// We introduced the 'CancelExecution' function for the same purpose.
		//
		// We will have to call the CancelExecution function of pgConn to
		// inform the backend that the user has cancelled the execution.
		//
		// We will need to consume all the results before quiting from the thread.
		// Otherwise - the connection object will become unusable, and throw
		// error - because libpq connections expects application to consume all
		// the result, before executing another query
		//
		if (m_cancelled && rc != pgQueryResultEvent::PGQ_EXECUTION_CANCELLED)
		{
			// We shouldn't be calling cancellation multiple time
			if (!connExecutionCancelled)
			{
				m_conn->CancelExecution();
				connExecutionCancelled = true;
			}
			rc = pgQueryResultEvent::PGQ_EXECUTION_CANCELLED;

			err.msg_primary = _("Execution Cancelled");

			if (lastResult)
			{
				PQclear(lastResult);
				lastResult = NULL;
			}
		}

		if ((rc = PQconsumeInput(m_conn->conn)) != 1)
		{
			if (m_cancelled)
			{
				rc = pgQueryResultEvent::PGQ_EXECUTION_CANCELLED;
				err.msg_primary = _("Execution Cancelled");

				if (lastResult)
				{
					PQclear(lastResult);
					lastResult = NULL;
				}
				// There is nothing more to consume.
				// We can quit the thread now.
				//
				// Raise the event - if the component asked for it on
				// execution cancellation.
				if (m_eventOnCancellation)
					RaiseEvent(rc);

				return rc;
			}

			if (rc == 0)
			{
				err.msg_primary = wxString(PQerrorMessage(m_conn->conn), conv);
			}
			if (PQstatus(m_conn->conn) == CONNECTION_BAD)
			{
				err.msg_primary = _("Connection to the database server lost");
				rc = pgQueryResultEvent::PGQ_CONN_LOST;
			}
			else
			{
				rc = pgQueryResultEvent::PGQ_ERROR_CONSUME_INPUT;
			}

			return(RaiseEvent(rc));
		}

		if (PQisBusy(m_conn->conn))
		{
			Yield();
			this->Sleep(10);

			continue;
		}

		// if resultToRetrieve is given, the nth result will be returned,
		// otherwise the last result set will be returned.
		// all others are discarded
		PGresult *res = PQgetResult(m_conn->conn);

		if (!res)
			break;

		if((PQresultStatus(res) == PGRES_NONFATAL_ERROR) ||
		        (PQresultStatus(res) == PGRES_FATAL_ERROR) ||
		        (PQresultStatus(res) == PGRES_BAD_RESPONSE))
		{
			result = res;
			err.SetError(res, &conv);

			// Wait for the execution to be finished
			// We need to fetch all the results, before sending the error
			// message
			do
			{
				if (PQconsumeInput(m_conn->conn) != 1)
				{
					if (m_cancelled)
					{
						rc = pgQueryResultEvent::PGQ_EXECUTION_CANCELLED;

						// Release the result as the query execution has been cancelled by the
						// user
						if (result)
							PQclear(result);

						result = NULL;

						if (m_eventOnCancellation)
							RaiseEvent(rc);

						return rc;
					}
					goto out_of_consume_input_loop;
				}

				if ((res = PQgetResult(m_conn->conn)) == NULL)
				{
					goto out_of_consume_input_loop;
				}

				// Release the temporary results
				PQclear(res);
				res = NULL;

				if (PQisBusy(m_conn->conn))
				{
					Yield();
					this->Sleep(10);
				}
			}
			while (true);

			break;
		}

#if defined (__WXMSW__) || (EDB_LIBPQ)
		// there should be 2 results in the callable statement - the first is the
		// dummy, the second contains our out params.
		if (useCallable)
		{
			PQclear(res);
			result = PQiGetOutResult(m_conn->conn);
		}
#endif
		if (PQresultStatus(res) == PGRES_COPY_IN)
		{
			rc = PGRES_COPY_IN;
			PQputCopyEnd(m_conn->conn, "not supported by pgadmin");
		}

		if (PQresultStatus(res) == PGRES_COPY_OUT)
		{
			int copyRc;
			char *buf;
			int copyRows = 0;
			int lastCopyRc = 0;

			rc = PGRES_COPY_OUT;

			AppendMessage(_("query returned copy data:\n"));

			while((copyRc = PQgetCopyData(m_conn->conn, &buf, 1)) >= 0)
			{
				// Ignore processing the query result, when it has already been
				// cancelled by the user
				if (m_cancelled)
				{
					if (!connExecutionCancelled)
					{
						m_conn->CancelExecution();
						connExecutionCancelled = true;
					}
					continue;
				}

				if (buf != NULL)
				{
					if (copyRows < 100)
					{
						wxString str(buf, conv);
						wxCriticalSectionLocker cs(m_criticalSection);
						m_queries[m_currIndex]->m_message.Append(str);

					}
					else if (copyRows == 100)
						AppendMessage(_("Query returned more than 100 copy rows, discarding the rest...\n"));

					PQfreemem(buf);
				}
				if (copyRc > 0)
					copyRows++;

				if (lastCopyRc == 0 && copyRc == 0)
				{
					Yield();
					this->Sleep(10);
				}
				if (copyRc == 0)
				{
					if (!PQconsumeInput(m_conn->conn))
					{
						// It might be the case - it is a result of the
						// execution cancellation.
						if (m_cancelled)
						{
							rc = pgQueryResultEvent::PGQ_EXECUTION_CANCELLED;

							// Release the result as the query execution has been cancelled by the
							// user
							if (result)
								PQclear(result);

							result = NULL;

							if (m_eventOnCancellation)
								RaiseEvent(rc);

							return rc;
						}
						if (PQstatus(m_conn->conn) == CONNECTION_BAD)
						{
							err.msg_primary = _("Connection to the database server lost");
							rc = pgQueryResultEvent::PGQ_CONN_LOST;
						}
						else
						{
							rc = pgQueryResultEvent::PGQ_ERROR_CONSUME_INPUT;

							err.msg_primary = wxString(PQerrorMessage(m_conn->conn), conv);
						}
						return(RaiseEvent(rc));
					}
				}
				lastCopyRc = copyRc;
			}

			res = PQgetResult(m_conn->conn);

			if (!res)
				break;
		}

		resultsRetrieved++;

		// Save the current result, as asked by the component
		// But - only if the execution is not cancelled
		if (!m_cancelled && resultsRetrieved == resultToRetrieve)
		{
			result = res;
			insertedOid = PQoidValue(res);
			if (insertedOid && insertedOid != (Oid) - 1)
				AppendMessage(wxString::Format(_("query inserted one row with oid %d.\n"), insertedOid));
			else
				AppendMessage(wxString::Format(wxPLURAL("query result with %d row will be returned.\n", "query result with %d rows will be returned.\n",
				                                        PQntuples(result)), PQntuples(result)));
			continue;
		}

		if (lastResult)
		{
			if (!m_cancelled && PQntuples(lastResult))
				AppendMessage(wxString::Format(wxPLURAL("query result with %d row discarded.\n", "query result with %d rows discarded.\n",
				                                        PQntuples(lastResult)), PQntuples(lastResult)));
			PQclear(lastResult);
		}
		lastResult = res;
	}

out_of_consume_input_loop:
	if (m_cancelled)
	{
		rc = pgQueryResultEvent::PGQ_EXECUTION_CANCELLED;

		// Release the result as the query execution has been cancelled by the
		// user
		if (result)
			PQclear(result);

		result = NULL;

		if (m_eventOnCancellation)
			RaiseEvent(rc);

		return rc;
	}

	if (!result)
		result = lastResult;

	err.SetError(result, &conv);

	AppendMessage(wxT("\n"));

	rc = PQresultStatus(result);
	if (rc == PGRES_TUPLES_OK)
	{
		dataSet = new pgSet(result, m_conn, conv, m_conn->needColQuoting);
		dataSet->MoveFirst();
	}
	else if (rc == PGRES_COMMAND_OK)
	{
		char *s = PQcmdTuples(result);
		if (*s)
			rowsInserted = atol(s);
	}
	else if (rc == PGRES_FATAL_ERROR ||
	         rc == PGRES_NONFATAL_ERROR ||
	         rc == PGRES_BAD_RESPONSE)
	{
		if (result)
		{
			AppendMessage(wxString(PQresultErrorMessage(result), conv));
			PQclear(result);
			result = NULL;
		}
		else
		{
			AppendMessage(wxString(PQerrorMessage(m_conn->conn), conv));
		}

		return(RaiseEvent(rc));
	}

	insertedOid = PQoidValue(result);
	if (insertedOid == (Oid) - 1)
		insertedOid = 0;

	return(RaiseEvent(1));
}

int pgQueryThread::RaiseEvent(int _retval)
{
#if !defined(PGSCLI)
	if (m_caller)
	{
		pgQueryResultEvent resultEvent(GetId(), m_queries[m_currIndex], m_queries[m_currIndex]->m_eventID);

		// client data
		resultEvent.SetClientData(m_queries[m_currIndex]->m_data);
		resultEvent.SetInt(_retval);

		m_caller->AddPendingEvent(resultEvent);
	}
#endif
	return _retval;
}


void *pgQueryThread::Entry()
{
	do
	{
		if (m_currIndex < (((int)m_queries.GetCount()) - 1))
		{
			// Create the PGcancel object to enable cancelling the running
			// query
			m_conn->SetConnCancel();
			m_currIndex++;

			m_queries[m_currIndex]->m_returnCode = -2;
			m_queries[m_currIndex]->m_rowsInserted = -1l;

			wxLogSql(wxT("Thread executing query (%d:%s:%d): %s"),
			         m_currIndex + 1, m_conn->GetHost().c_str(), m_conn->GetPort(),
			         m_queries[m_currIndex]->m_query.c_str());

			// register the notice processor for the current query
			m_conn->RegisterNoticeProcessor(m_processor, m_noticeHandler);

			// execute the current query now
			Execute();

			// remove the notice processor now
			m_conn->RegisterNoticeProcessor(0, 0);

			// reset the PGcancel object
			m_conn->ResetConnCancel();
		}

		if (!m_multiQueries || m_cancelled)
			break;

		wxThread::Sleep(10);
	}
	while (true);

	return(NULL);
}

int pgQueryThread::DeleteReleasedQueries()
{
	int res = 0,
	    idx = 0;

	if (m_queriesLock.TryLock() == wxMUTEX_BUSY)
		return res;

	for (; idx <= m_currIndex; idx++)
	{
		if (m_queries[idx]->m_resultSet != NULL)
		{
			pgSet *set = m_queries[idx]->m_resultSet;
			m_queries[idx]->m_resultSet = NULL;
			delete set;
			set = NULL;

			res++;
		}
	}
	m_queriesLock.Unlock();

	return res;
}


pgError pgQueryThread::GetResultError(int idx)
{
	wxMutexLocker lock(m_queriesLock);

	if (idx == -1)
		idx = m_currIndex;

	return m_queries[idx]->m_err;
}


const wxString &pgBatchQuery::GetErrorMessage()
{
	return m_err.msg_primary;
}

pgBatchQuery::~pgBatchQuery()
{
	if (m_resultSet)
	{
		delete m_resultSet;
		m_resultSet = NULL;
	}

	if (m_params)
	{
		WX_CLEAR_ARRAY((*m_params));
		delete m_params;
		m_params = NULL;
	}
}

bool pgBatchQuery::Release()
{
	bool res = false;

	if (m_resultSet != NULL)
	{
		res = true;

		pgSet *set = m_resultSet;
		m_resultSet = NULL;

		if (set)
			delete set;
		set = NULL;
	}

	if (m_params)
	{
		res = true;

		WX_CLEAR_ARRAY((*m_params));
		delete m_params;
		m_params = NULL;
	}

	return res;
}

pgQueryResultEvent::pgQueryResultEvent(
    unsigned long _thrdId, pgBatchQuery *_qry, int _id) :
	wxCommandEvent(PGQueryResultEvent, _id), m_thrdId(_thrdId),
	m_query(_qry) { }

pgQueryResultEvent::pgQueryResultEvent(const pgQueryResultEvent &_ev)
	: wxCommandEvent(_ev), m_thrdId(_ev.m_thrdId), m_query(_ev.m_query) { }


pgParam::pgParam(Oid _type, void *_val, int _len, short _mode)
	: m_type(_type), m_val(_val), m_len(_len), m_mode(_mode)
{
	switch(m_type)
	{
		case PGOID_TYPE_CHAR:
		case PGOID_TYPE_NAME:
		case PGOID_TYPE_TEXT:
		case PGOID_TYPE_VARCHAR:
		case PGOID_TYPE_CSTRING:
			m_format = 0;
		default:
			m_format = 1;
	}
}

// wxString data
pgParam::pgParam(Oid _oid, wxString *_val, wxMBConv *_conv, short _mode)
	: m_mode(_mode)
{
	if (m_mode == PG_PARAM_OUT || !_val)
	{
		m_len = 0;
	}
	else
	{
		m_len = _val->Len();
	}
	if (_val)
	{
		char *str = (char *)malloc(m_len + 1);
		if (!_val->IsEmpty() && _mode != PG_PARAM_OUT)
		{
			strncpy(str,
			        (const char *)_val->mb_str(
			            *(_conv != NULL ? _conv : &wxConvLocal)), m_len);
			str[m_len] = '\0';
		}
		else
		{
			str[0] = '\0';
		}
		m_val = (void *)(str);
	}
	else
	{
		m_val = NULL;
	}
	m_type = _oid;

	// text format
	m_format = 0;
}


pgParam::~pgParam()
{
	if (m_val)
		free(m_val);
	m_val = NULL;
}


int pgParam::GetFormat()
{
	return m_format;
}
