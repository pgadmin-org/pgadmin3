//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dbgBreakPoint.h - debugger
//
//////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// class dbgBreakPoint
//
//
////////////////////////////////////////////////////////////////////////////////

#ifndef DBGBREAKPOINT_H
#define DBGBREAKPOINT_H

class dbgBreakPoint
{
public:
	dbgBreakPoint(const wxString &_funcOid, const wxString &_pkgOid = wxT("-1"),
	              const int &_lineNo = -1)
		: m_func(_funcOid), m_pkg(_pkgOid), m_lineNo(_lineNo) {}

	wxString &GetFunctionOid()
	{
		return m_func;
	}
	wxString &GetPackageOid()
	{
		return m_pkg;
	}
	int      &GetLineNo()
	{
		return m_lineNo;
	}

private:
	wxString m_func;
	wxString m_pkg;
	int      m_lineNo;
};

WX_DECLARE_LIST(dbgBreakPoint, dbgBreakPointList);

#endif
