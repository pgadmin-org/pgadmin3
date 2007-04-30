//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id: wsDirectdbg.h 6202 2007-04-18 11:22:49Z dpage $
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// wsDirectdbg.h - debugger 
//
//////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// class wsDirectDbg
//
//	This class implements 'direct-debugging'. In direct-debugging, the user 
//  provides a function signature, procedure signature, or OID on the command
//  line (this identifies the debug target).  We query the server for the 
//  names, types, and in/out modes for each target parameter and then prompt
//	the user to enter a value for each of the IN (and IN/OUT) parameters.
//
//  When the user fills in the parameter values and clicks OK, we set a 
//  breakpoint at the target and then execute a SELECT statement or an 
//  EXEC statement that invokes the target (with the parameter values 
//  provided by the user).
//
//  A wsDirectDbg object is typically a child of the wsMainFrame object
//
////////////////////////////////////////////////////////////////////////////////

#ifndef WSDIRECTDBGH
#define WSDIRECTDBGH

#include "dlg/dlgClasses.h"
#include "debugger/wsBreakPoint.h"
#include "debugger/wsConnProp.h"
#include "debugger/wsMainFrame.h"

class wsTargetInfo;
class wsPgConn;
class wsCodeWindow;

class wsDirectDbg : public pgDialog
{
	DECLARE_CLASS( wsDirectDbg )

public:

	wsDirectDbg( wsMainFrame *parent, wxWindowID id, const wsConnProp & connProp );
	wsBreakpointList & getBreakpointList();
    void setupParamWindow();
	void startDebugging();

private:

	enum
	{
		COL_NAME = 0,	// Column 0 contains the variable name
		COL_TYPE,	// This column contains the variable type
		COL_VALUE	// This column contains the variable value
	};

	wxString	m_target;	// Target name (function/procedure signature or OID)
	bool		m_isFunc;
	const wsConnProp &m_connProp;	// Connection properties (used to connect to the server)
	wsTargetInfo     *m_targetInfo;	// Detailed information about the target (like argument types, name, ...)
	wsPgConn         *m_conn;		// The connection to the server
	wsCodeWindow     *m_codeWindow;		// A pointer to the debugger window that we'll create
	wsBreakpointList m_breakpoints;		// List of initial breakpoints to create
	wsMainFrame		 *m_parent;

	void loadTargetInfo( const wxString &target, const wsConnProp & connProp, char targetType );
	void populateParamGrid();
	void OnOk( wxCommandEvent & event );
	void OnCancel( wxCommandEvent & event );
	void OnClose( wxCloseEvent & event );
	void OnResultReady( wxCommandEvent & event );
	void OnTargetComplete( wxCommandEvent & event );
	void OnDebug( wxCommandEvent & event );
	void OnNoticeReceived( wxCommandEvent & event );
	bool activateDebugger( );

	void saveSettings();
	void loadSettings();
	void setBreakpoint( long pkgOid, long funcOid );
	void invokeTarget();

    DECLARE_EVENT_TABLE()

};

#endif
