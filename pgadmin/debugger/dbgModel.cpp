//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dbgModel.cpp - debugger model
// - It contains the data and information related the debugging session
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>

#include "db/pgConn.h"
#include "db/pgQueryThread.h"
#include "db/pgQueryResultEvent.h"
#include "debugger/dbgModel.h"


dbgModel::dbgModel(Oid _target, pgConn *_conn)
	: m_target(NULL), m_currLineNo(-1), m_targetPid(wxT("NULL"))
{
	m_target = new dbgTargetInfo(_target, _conn);
}


bool dbgModel::GetSource(const wxString &_funcOid, dbgCachedStack *_cached)
{
	dbgSourceHash::iterator match = m_sourceMap.find(_funcOid);

	if (match == m_sourceMap.end())
		return false;
	else
	{
		if (_cached)
		{
			*_cached = match->second;
		}

		return true;
	}
}


void dbgModel::ClearCachedSource()
{
	m_sourceMap.clear();

	// Put a dummy entry for invalid function OID to the cache. This is
	// displayed at least for inline code blocks, as we currently have no way
	// to fetch the source for those
	m_sourceMap[wxT("0")] = dbgCachedStack(wxT("0"), wxT("0"), wxT(""), wxT(""), _("<source not available>"));
}


void dbgModel::AddSource(const wxString &_funcOid, const dbgCachedStack &_source)
{
	m_sourceMap[_funcOid] = _source;
}
