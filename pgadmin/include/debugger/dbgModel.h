//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dbgModel.h - Debugger Model
//
//////////////////////////////////////////////////////////////////////////

#ifndef DBGMODEL_H
#define DBGMODEL_H

#include <wx/wx.h>

#include "debugger/dbgBreakPoint.h"
#include "debugger/dbgTargetInfo.h"

class dbgCachedStack
{
public:
	dbgCachedStack() {}
	dbgCachedStack(const wxString &_pkg, const wxString &_func,
	               const wxString &_target, const wxString &_arg, const wxString &_src)
		: m_func(_func), m_pkg(_pkg), m_source(_src),
		  m_target(_target), m_arg(_arg) {}

	dbgCachedStack(const dbgCachedStack &_src)
		: m_func(_src.m_func), m_pkg(_src.m_pkg), m_source(_src.m_source),
		  m_target(_src.m_target), m_arg(_src.m_arg) {}

	dbgCachedStack &operator =(const dbgCachedStack &_src)
	{
		m_func   = _src.m_func;
		m_pkg    = _src.m_pkg;
		m_source = _src.m_source;
		m_target = _src.m_target;
		m_arg    = _src.m_arg;

		return *this;
	}

private:
	wxString    m_pkg;    // Package OID
	wxString    m_func;   // Function OID
	wxString    m_target; // Target Name
	wxString	m_arg;	  // Argument passed to the target
	wxString	m_source;  // Source code for this function

	friend class frmDebugger;
};

WX_DECLARE_STRING_HASH_MAP(dbgCachedStack, dbgSourceHash);

class dbgModel
{
public:
	dbgModel(Oid _target, pgConn *_conn);

	dbgTargetInfo *GetTarget()
	{
		return m_target;
	}
	dbgBreakPointList &GetBreakPoints()
	{
		return m_breakpoints;
	}

	wxString &GetPort()
	{
		return m_port;
	}
	wxString &GetSession()
	{
		return m_session;
	}
	wxString &GetTargetPid()
	{
		return m_targetPid;
	}

	bool GetSource(const wxString &_funcOid, dbgCachedStack *_cached = NULL);
	void ClearCachedSource();
	void AddSource(const wxString &_funcOid, const dbgCachedStack &cached);

	bool RequireDisplayUpdate()
	{
		return (m_focusedFuncOid != m_displayedFuncOid ||
		        m_displayedPkgOid != m_focusedPkgOid);
	}

	wxString &GetFocusedPackage()
	{
		return m_focusedPkgOid;
	}
	wxString &GetDisplayedPackage()
	{
		return m_displayedPkgOid;
	}
	wxString &GetFocusedFunction()
	{
		return m_focusedFuncOid;
	}
	wxString &GetDisplayedFunction()
	{
		return m_displayedFuncOid;
	}

	int      &GetCurrLineNo()
	{
		return m_currLineNo;
	}

private:
	// Target Information
	dbgTargetInfo     *m_target;

	// Break-Points
	dbgBreakPointList  m_breakpoints;

	// Debugging Port, session-handle & target-backend pid
	wxString           m_port;
	wxString           m_session;
	wxString           m_targetPid;

	// Cached source-code for the stacked functions
	dbgSourceHash      m_sourceMap;

	// Current focused function-information
	wxString           m_focusedFuncOid;
	wxString           m_focusedPkgOid;

	// Current displayed function-information
	wxString           m_displayedFuncOid;
	wxString           m_displayedPkgOid;

	// Current Line number
	int                m_currLineNo;
};

#endif
