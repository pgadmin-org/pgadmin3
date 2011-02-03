//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2010, The pgAdmin Development Team
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

	enum eTargetType
	{
		TRIGGER,
		FUNCTION,
		PROCEDURE,
		OID
	};

	dbgBreakPoint(eTargetType targetType, const wxString &target, const wxString &process ): m_targetType(targetType), m_target(target), m_targetProcess(process) {}

	eTargetType   getTargetType()
	{
		return( m_targetType );
	}
	wxString     &getTarget()
	{
		return( m_target );
	}
	wxString     &getTargetProcess()
	{
		return( m_targetProcess );
	}
private:
	eTargetType 	m_targetType;
	wxString	m_target;
	wxString	m_targetProcess;

};

WX_DECLARE_LIST( dbgBreakPoint, dbgBreakPointList );

#endif
