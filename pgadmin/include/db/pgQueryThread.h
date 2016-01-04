//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// pgQueryThread.h - PostgreSQL threaded query class header
//
//////////////////////////////////////////////////////////////////////////

#ifndef PGQUERYTHREAD_H
#define PGQUERYTHREAD_H

#include "wx/wx.h"
#include "wx/event.h"
#include "db/pgConn.h"

// Forward declaration
class pgSet;
class pgQueryThread;
class pgBatchQuery;

// Support for the IN & INOUT parameters type
class pgParam : public wxObject
{
public:
	enum
	{
		PG_PARAM_IN       = 1,
		PG_PARAM_OUT      = 2,
		PG_PARAM_INOUT    = 3,
		PG_PARAM_VARIADIC = 4,
		PG_PARAM_TABLE    = 5
	};

	// Any data
	//
	// This constructor won't call the functions - 'htonl' or 'htons'
	// It will be the responsibility of the caller to take care
	// to call 'htonl' or 'htons' depending on requirements.
	//
	// NOTE:
	// This data will be owned by pgParam object and will be released.
	pgParam(Oid _type, void *_val, int _len, short mode = PG_PARAM_IN);

	// wxString data
	pgParam(Oid _type, wxString *_val, wxMBConv *_conv = NULL,
	        short mode = PG_PARAM_IN);

	~pgParam();

	// Returns 0 for text type and 1 for otherwise
	int   GetFormat();

	Oid   GetType()
	{
		return m_type;
	}
	short GetMode()
	{
		return m_mode;
	}

protected:
	Oid    m_type;
	void  *m_val;
	int    m_len;
	short  m_format;
	// Modes are required by EnterpriseDB's callable statement
	short  m_mode;

	// Do not allow copy construction and shadow-copy
	// to avoid ownership
	// Force to use an pointer
	pgParam(const pgParam &)
	{
		wxASSERT(0);
	}
	pgParam &operator= (const pgParam &)
	{
		wxASSERT(0);
		return *this;
	}

	friend class pgConn;
	friend class pgQueryThread;
};

WX_DEFINE_ARRAY_PTR(pgParam *, pgParamsArray);

class pgBatchQuery : public wxObject
{
public:
	pgBatchQuery(const wxString &_query, pgParamsArray *_params = NULL,
	             long _eventId = -1, void *_data = NULL, bool _useCallable = false,
	             int _resultToRetrieve = 0)
		: m_query(_query), m_params(_params), m_eventID(_eventId), m_data(_data),
		  m_useCallable(_useCallable), m_resToRetrieve(_resultToRetrieve),
		  m_returnCode(-1), m_resultSet(NULL), m_rowsInserted(-1), m_insertedOid(-1)
	{
		// Do not honour the empty query string
		wxASSERT(!_query.IsEmpty());
	}
	~pgBatchQuery();

	bool            Release();

	pgSet          *ResultSet()
	{
		return m_resultSet;
	}

	int             ReturnCode()
	{
		return m_returnCode;
	}

	const wxString &GetMessage()
	{
		return m_message;
	}

	long            RowInserted()
	{
		return m_rowsInserted;
	}

	const wxString &GetErrorMessage();

protected:
	wxString           m_query;         // Query
	pgParamsArray     *m_params;        // parameters
	long               m_eventID;       // Event ID
	void              *m_data;          // Data to be send with event
	bool               m_useCallable;   // Use EnterpriseDB callable statement if possible
	int                m_resToRetrieve; // Which result to be retrieved
	int                m_returnCode;    // Return code
	pgSet             *m_resultSet;     // Result-Set
	long               m_rowsInserted;  // No of rows inserted
	Oid                m_insertedOid;   // Inserted Oid
	wxString           m_message;       // Message generated during query execution
	pgError            m_err;           // Error

private:
	// Do not allow copy construction and '=' operator (shadow copying)
	// to avoid ownership of parameters and result-set
	//
	// This will force this class to be used as an pointer only.
	pgBatchQuery(const pgBatchQuery &)
	{
		wxASSERT(0);
	}
	pgBatchQuery &operator= (const pgBatchQuery &)
	{
		wxASSERT(0);
		return *this;
	}

	friend class pgQueryThread;
};
WX_DEFINE_ARRAY_PTR(pgBatchQuery *, pgBatchQueryArray);

class pgQueryThread : public wxThread
{
public:
	// For running a single query (Used by few components)
	pgQueryThread(pgConn *_conn, const wxString &qry, int resultToRetrieve = -1,
	              wxWindow *_caller = 0, long eventId = 0, void *_data = 0);

	// Support for multiple queries support
	pgQueryThread(pgConn *_conn, wxEvtHandler *_caller = NULL,
	              PQnoticeProcessor _processor = NULL, void *_noticeHandler = NULL);

	~pgQueryThread();

	bool HasMultipleQueriesSupport()
	{
		return m_multiQueries;
	}

	bool SupportCallableStatement()
	{
		return m_useCallable;
	}

	void SetEventOnCancellation(bool eventOnCancelled);

	void AddQuery(
	    const wxString &_qry, pgParamsArray *_params = NULL,
	    long _eventId = 0, void *_data = NULL, bool _useCallable = false,
	    int _resultToRetrieve = -1);

	virtual void *Entry();
	bool DataValid(int _idx = -1) const
	{
		if (_idx == -1)
			_idx = m_currIndex;
		return (_idx >= 0 && _idx > m_currIndex ? false : (m_queries[_idx]->m_resultSet != NULL));
	}

	pgConn *GetConn()
	{
		return m_conn;
	}

	pgSet *DataSet(int _idx = -1)
	{
		if (_idx == -1)
			_idx = m_currIndex;
		return (_idx >= 0 && _idx > m_currIndex ? NULL : m_queries[_idx]->m_resultSet);
	}

	int ReturnCode(int _idx = -1) const
	{
		if (_idx == -1)
			_idx = m_currIndex;
		return (_idx >= 0 && _idx > m_currIndex ? -1 : m_queries[_idx]->m_returnCode);
	}

	long RowsInserted(int _idx = -1) const
	{
		if (_idx == -1)
			_idx = m_currIndex;
		return (_idx >= 0 && _idx > m_currIndex ? -1L : m_queries[_idx]->m_rowsInserted);
	}

	Oid InsertedOid(int _idx = -1) const
	{
		if (_idx == -1)
			_idx = m_currIndex;
		return (_idx >= 0 && _idx > m_currIndex ? -1L : m_queries[_idx]->m_insertedOid);
	}

	inline void CancelExecution()
	{
		m_cancelled = true;
	}

	inline size_t GetNumberQueries()
	{
		return m_queries.GetCount();
	}

	size_t QueriesExecuted()
	{
		return m_currIndex + 1;
	}

	wxString GetMessagesAndClear(int _idx = -1);
	void AppendMessage(const wxString &_str);

	int DeleteReleasedQueries();

	pgError GetResultError(int idx = -1);

private:
	int Execute();
	int RaiseEvent(int _retval = 0);

	// Queries to be executed
	pgBatchQueryArray  m_queries;
	// Current running query index
	int                m_currIndex;
	// Connection object
	pgConn            *m_conn;
	// Execution cancelled?
	bool               m_cancelled;
	// Raise events even when cancelled the execution
	bool               m_eventOnCancellation;
	// Does this thread support multiple queries
	bool               m_multiQueries;
	// Use EDB callable statement (if available and require)
	bool               m_useCallable;
	// Is executing a query
	bool               m_executing;
	// Queries are being accessed at this time
	wxMutex            m_queriesLock;
	// When one thread is accesing messages, other should not be able to access it
	wxCriticalSection  m_criticalSection;
	// Event Handler
	wxEvtHandler      *m_caller;
	// Database server notice-processor
	PQnoticeProcessor  m_processor;
	// Notice Handler
	void              *m_noticeHandler;

};

#endif
