//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id: wsDirectdbg.h 5827 2007-01-04 16:35:14 hiroshi $
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

#include	<wx/mdi.h>
#include    "wsConnProp.h"
#include    "wsBreakPoint.h"
class wsTargetInfo;
class wsPgConn;
class wsCodeWindow;

class wsDirectDbg : public wxMDIChildFrame	// FIXME: shouldn't this be a wxDialog?
{
	DECLARE_CLASS( wsDirectDbg )

public:

	wsDirectDbg( wxMDIParentFrame * parent, wxWindowID id, const wsConnProp & connProp );
	wsBreakpointList & getBreakpointList();
	void startDebugging();

private:

	enum
	{
		COL_NAME = 0,				// Column 0 contains the variable name
		COL_TYPE,					// This column contains the variable type
		COL_VALUE					// This column contains the variable value
	};

	wxString     	   m_target;			// Target name (function/procedure signature or OID)
	bool			   m_isFunc;
	const wsConnProp & m_connProp;			// Connection properties (used to connect to the server)
	wsTargetInfo     * m_targetInfo;		// Detailed information about the target (like argument types, name, ...)
	wxGrid           * m_grid;				// A grid that we use to prompt the user for parameter values
	wsPgConn         * m_conn;				// The connection to the server
	wsCodeWindow     * m_codeWindow;		// A pointer to the debugger window that we'll create
	wsBreakpointList   m_breakpoints;		// List of initial breakpoints to create
	wxCheckBox		 * m_debugInitializer;	// Checkbox for "Debug package initializer?"

	void loadTargetInfo( const wxString & target, const wsConnProp & connProp, char targetType );
	void setupParamWindow();
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
