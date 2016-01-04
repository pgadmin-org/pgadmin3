//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// pgQueryResultEvent.h - Query Result Event from the pgQueryThread
//
//////////////////////////////////////////////////////////////////////////

#ifndef PGQUERYRESULTEVENT_H
#define PGQUERYRESULTEVENT_H

#include "wx/wx.h"
#include "wx/event.h"

class pgBatchQuery;

extern const wxEventType PGQueryResultEvent;


class pgQueryResultEvent : public wxCommandEvent
{
public:
	pgQueryResultEvent(unsigned long _thrdId, pgBatchQuery *_qry, int _id = 0);
	pgQueryResultEvent(const pgQueryResultEvent &_ev);

	// Required for sending with wxPostEvent()
	wxEvent *Clone() const
	{
		return new pgQueryResultEvent(*this);
	}

	pgBatchQuery *GetQuery()
	{
		return m_query;
	}
	unsigned long GetThreadID()
	{
		return m_thrdId;
	}

	enum
	{
		PGQ_RESULT_ERROR = -8,
		PGQ_EXECUTION_CANCELLED = -7,
		PGQ_ERROR_CONSUME_INPUT = -6,
		PGQ_ERROR_SEND_QUERY = -5,
		PGQ_ERROR_EXECUTE_CALLABLE = -4,
		PGQ_ERROR_PREPARE_CALLABLE = -3,
		PGQ_STRING_INVALID = -2,
		PGQ_CONN_LOST = -1,
	};

private:
	pgBatchQuery  *m_query;
	// Thread Id (pgQueryThread)
	unsigned long m_thrdId;
};

typedef void (wxEvtHandler::*pgQueryResultEventFunc)(pgQueryResultEvent &);

// This #define simplifies the one below, and makes the syntax less
// ugly if you want to use Connect() instead of an event table.
#define pgQueryResultEventHandler(func)                              \
	(wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction) \
	wxStaticCastEvent(pgQueryResultEventFunc, &func)

// Define the event table entry. Yes, it really *does* end in a comma.
#define EVT_PGQUERYRESULT(id, fn)                                \
	DECLARE_EVENT_TABLE_ENTRY(PGQueryResultEvent, id, wxID_ANY,  \
	pgQueryResultEventHandler(fn), (wxObject*) NULL),

#define EVT_PGQUERYRESULT_RANGE(id1, id2, fn)               \
	DECLARE_EVENT_TABLE_ENTRY(PGQueryResultEvent, id1, id2, \
	pgQueryResultEventHandler(fn), (wxObject*) NULL),

#endif // PGQUERYRESULTEVENT_H
