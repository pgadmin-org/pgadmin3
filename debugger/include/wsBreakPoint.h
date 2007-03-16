//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// wsBreakPoint.h - debugger 
//
//////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// class wsBreakpoint
//
//
////////////////////////////////////////////////////////////////////////////////

#ifndef WSBREAKPOINTH
#define WSBREAKPOINTH

#include <wx/string.h>
#include <wx/list.h>

class wsBreakpoint
{
public:

	enum eTargetType
	{
		TRIGGER,
		FUNCTION,
		PROCEDURE,
		OID
	};

	wsBreakpoint( eTargetType targetType, wxString & target, wxString & process ): m_targetType( targetType ), m_target( target ), m_targetProcess( process ) {}

	eTargetType   getTargetType() 	 { return( m_targetType ); }
	wxString    & getTarget() 		 { return( m_target ); }
	wxString    & getTargetProcess() { return( m_targetProcess ); }
private:
	eTargetType 	m_targetType;
	wxString		m_target;
	wxString		m_targetProcess;

};

WX_DECLARE_LIST( wsBreakpoint, wsBreakpointList );

#endif
