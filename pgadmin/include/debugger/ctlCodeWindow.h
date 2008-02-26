//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id: ctlCodeWindow.h 6216 2007-04-19 11:28:08Z dpage $
// Copyright (C) 2002 - 2008, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// ctlCodeWindow.h - debugger 
//
//////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// class ctlCodeWindow
//
//	This class implements the debugger window.  The constructor expects a string
//  that contains a TCP port number - the constructor connects to the debugger
//  server waiting at that port.  
//
//  A ctlCodeWindow object creates (and manages) a toolbar and handles toolbar
//  and keystroke messages. The input messages are treated as debugger commands.
//
//	The m_view member is a ctlSQLBox that displays the code for the PL
//  function that you're debugging.  The m_hilite member tracks the current 
//  line (that is, the line about to execute).  We use hilight the current line.
//  If m_hilite is -1, there is no current line.  
//
////////////////////////////////////////////////////////////////////////////////

#ifndef CTLCODEWINDOW_H
#define CTLCODEWINDOW_H

#include <wx/progdlg.h>

#include "ctl/ctlSQLBox.h"
#include "debugger/frmDebugger.h"
#include "debugger/dbgBreakPoint.h"
#include "debugger/ctlTabWindow.h"

class dbgPgConn;
class dbgResultset;
class dbgConnProp;
class wsWaitingDialog;
class ctlStackWindow;
class ctlMessageWindow;
class ctlVarWindow;
class ctlResultGrid;


#define MARKERINDEX_TO_MARKERMASK( MI ) ( 1 << MI )

class wsCodeCache
{
public:
	wsCodeCache() {}
	wsCodeCache(const wxString &packageOID, const wxString &funcOID, const wxString &source, const wxString &signature);

	const wxString &getSource()   { return( m_sourceCode ); }
	const wxString &getSignature(){ return( m_signature ); }

private:
	wxString	m_packageOID;	// Package OID
	wxString	m_funcOID;	// Function OID
	wxString	m_sourceCode;	// Source code for this function
    wxString    m_signature; // Function sig

};

class ctlCodeWindow : public pgFrame  
{
    DECLARE_CLASS( ctlCodeWindow )

 public:
	ctlCodeWindow( frmDebugger *parent, wxWindowID id, const dbgConnProp & connProps );
    
    void OnClose(wxCloseEvent& event);
	void startLocalDebugging();	 	                    // Start debugging 
	void resumeLocalDebugging();		                // Start debugging, already attached to the proxy
	void startGlobalDebugging(); 		                // Start debugging 
	void OnCommand( wxCommandEvent & event );		    // Handle menu/toolbar commands
    void OnSelectFrame( wxCommandEvent & event );	    // Select a different stack frame
    void OnMarginClick( wxStyledTextEvent & event );    // Set/clear breakpoint on margin click
    void OnPositionStc( wxStyledTextEvent & event );    // update the status bar text
    void OnVarChange( wxGridEvent & event );		    // User changed a variable
	void processResult( wxString & result );		    // Handle a message from the debugger server
	void OnNoticeReceived( wxCommandEvent & event );    // NOTICE received from server
	void OnResultSet( PGresult * result );			    // Result set received from server
	void disableTools();			                    // Disable toolbar tools
	void enableTools();		 	                        // Enable toolbar tools

	frmDebugger *GetFrame() { return m_parent; }

    bool	m_targetAborted;		    // Have we aborted the target? (true) or are we waiting for a breakpoint? (false)
    bool	m_targetComplete;		    // Is the target complete? (true) or is it still running (or aborted)? (false)
	dbgBreakPointList & getBreakpointList();

	WX_DECLARE_STRING_HASH_MAP( wsCodeCache, sourceHash );

 private:

    bool isBreakpoint(int lineNumber) { return (m_view->MarkerGet( lineNumber ) & MARKERINDEX_TO_MARKERMASK( MARKER_BREAKPOINT ) ? true : false); }
	void clearBreakpoint( int lineNumber, bool requestUpdate );
	void setBreakpoint( int lineNumber );

	ctlStackWindow   *getStackWindow()   { return( m_stackWindow ); }
	ctlMessageWindow *getMessageWindow() { return( m_tabWindow->getMessageWindow()); }

	ctlVarWindow     *getVarWindow( bool create )     { return( m_tabWindow->getVarWindow( create )); }
	ctlVarWindow     *getParamWindow( bool create )   { return( m_tabWindow->getParamWindow( create )); }
	ctlVarWindow	 *getPkgVarWindow( bool create )  { return( m_tabWindow->getPkgVarWindow( create )); } 
	ctlResultGrid    *getResultWindow()               { return( m_tabWindow->getResultWindow()); }

	void	setTools(bool enable);		            // Enable/disable debugger options
	void	OnIdle( wxIdleEvent & event );			// Idle processor
	void	OnTimer( wxTimerEvent & event );		// Clock tick

	int		getLineNo( );				                    // Compute line number for current cursor position
	void 	closeConnection();								// Closes proxy connection
	void	updateUI( dbgResultset & breakpoint );		    // Update the lazy parts of the UI
	void	updateSourceCode( dbgResultset & breakpoint );	// Update the source code window
	bool	connectionLost( dbgResultset & resultSet );	    // Returns true if proxy lost it's connection
	bool	gotFatalError( dbgResultset & resultSet );	    // Returns true if result set indicates a fatal error has occurred
	void 	popupError( dbgResultset & resultSet, wxString title);
	void	addBreakpoint( dbgBreakPoint * breakpoint, wxEventType nextStep );

	void	ResultPortAttach( wxCommandEvent & event );			// Attach to debugger port complete
	void	ResultBreakpoint( wxCommandEvent & event );			// Breakpoint encountered
	void	ResultVarList( wxCommandEvent & event );			// Variable list complete
	void	ResultStack( wxCommandEvent & event );				// Stack trace retrieval complete
	void	ResultSource( wxCommandEvent & event );				// Source code retrieval complete
	void 	ResultBreakpoints( wxCommandEvent & event );		// Breakpoint list retrieval complete
	void	ResultNewBreakpoint( wxCommandEvent & event );		// Set Breakpoint command complete
	void	ResultNewBreakpointWait( wxCommandEvent & event );	// Set Breakpoint command complete, wait for a target process
	void	ResultDeletedBreakpoint( wxCommandEvent & event );	// Drop Breakpoint command complete
	void	ResultDepositValue( wxCommandEvent & event );		// Deposit Value command complete
	void	ResultAbortTarget( wxCommandEvent & event );		// Abort target command complete
	void	ResultAddBreakpoint( wxCommandEvent & event );		// getTargetInfo() complete, add a breakpoint
	void	ResultListenerCreated( wxCommandEvent & event );	// Global listener created, ready to wait for a target
	void	ResultTargetReady( wxCommandEvent & event );		// Target session attached, ready to wait for a breakpoint
	void	ResultLastBreakpoint( wxCommandEvent & event );		// Adding last breakpoint 

	dbgPgConn	*m_dbgConn;	    // Network connection to debugger server
	wxString	m_debugPort;	// Port at which debugger server is listening

	frmDebugger 	*m_parent;		// Parent window
	int	m_currentLineNumber;	// Current line number

	ctlSQLBox       *m_view;	    // Window that displays function source code
	ctlStackWindow	*m_stackWindow;	// Stack Window
	ctlTabWindow	*m_tabWindow;	    // Tab Window

	typedef enum
	{
		SESSION_TYPE_UNKNOWN,	// Session could be in-context or direct
		SESSION_TYPE_INCONTEXT,	// Session is configured for in-context debugging
		SESSION_TYPE_DIRECT		// Session is configured for direct debugging
	} eSessionType;

	eSessionType	m_sessionType;		// Debugging mode is in-context or direct?
	bool	m_updateVars;			    // Update variable window in next idle period?
	bool    m_updateStack;			    // Update stack window in next idle period?
	bool	m_updateBreakpoints;	    // Update breakpoints in next idle period?
	dbgBreakPointList    m_breakpoints;	// List of initial breakpoints to create

	enum
	{
		MARKER_CURRENT    = 0x02,		// Current line marker
		MARKER_CURRENT_BG = 0x04,		// Current line marker - background hilight
		MARKER_BREAKPOINT = 0x01,		// Breakpoint marker
	};

	sourceHash	m_sourceCodeMap;

	wxString	m_focusPackageOid;	    // Which package has the debug focus?
	wxString	m_focusFuncOid;		    // Which function has the debug focus?
	wxString	m_displayedFuncOid;	    // Which function are we currently displaying? (function OID component)
	wxString	m_displayedPackageOid;	// Which function are we currently displaying? (package OID component)
	wxString	m_sessionHandle;	    // Handle to proxy's server session
	wxString	m_targetName;		    // User-friendly target name

	wxProgressDialog *m_progressBar;	// "Waiting for target" dialog
	wxTimer	m_timer;
	bool	findSourceInCache( const wxString &packageOID, const wxString &funcOID);
	void	getSource(const wxString &packageOID, const wxString &funcOID);
	void	cacheSource(const wxString &packageOID, const wxString &funcOID, const wxString &sourceCode, const wxString &signature);
	void	displaySource(const wxString &packageOID, const wxString &funcID);
	void	unhilightCurrentLine();
	void	launchWaitingDialog();

	void	clearAllBreakpoints();
	void	clearBreakpointMarkers();
	void	stopDebugging();

	static wxString	m_commandAttach;
	static wxString m_commandWaitForBreakpoint;
	static wxString m_commandGetVars;
	static wxString m_commandGetStack;
	static wxString m_commandGetBreakpoints;
	static wxString m_commandGetSourceV1;
    static wxString m_commandGetSourceV2;
	static wxString m_commandStepOver;
	static wxString m_commandStepInto;
	static wxString m_commandContinue;
	static wxString m_commandSetBreakpointV1;
	static wxString m_commandSetBreakpointV2;
	static wxString m_commandClearBreakpointV1;
    static wxString m_commandClearBreakpointV2;
	static wxString m_commandSelectFrame;
	static wxString m_commandDepositValue;
	static wxString m_commandAbortTarget;
	static wxString m_commandGetTargetInfo;
	static wxString m_commandAddBreakpointEDB;
    static wxString m_commandAddBreakpointPG;
	static wxString m_commandCreateListener;
	static wxString m_commandWaitForTarget;

    DECLARE_EVENT_TABLE()
};

#endif
