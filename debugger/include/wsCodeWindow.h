//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// wsCodeWindow.h - debugger 
//
//////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// class wsCodeWindow
//
//	This class implements the debugger window.  The constructor expects a string
//  that contains a TCP port number - the constructor connects to the debugger
//  server waiting at that port.  
//
//  A wsCodeWindow object creates (and manages) a toolbar and handles toolbar
//  and keystroke messages. The input messages are treated as debugger commands.
//
//	The m_view member is a wsRichWindow that displays the code for the PL
//  function that you're debugging.  The m_hilite member tracks the current 
//  line (that is, the line about to execute).  We use hilight the current line.
//  If m_hilite is -1, there is no current line.  
//
////////////////////////////////////////////////////////////////////////////////

#ifndef WSCODEWINDOWH
#define WSCODEWINDOWH

#include <wx/mdi.h>
#include <wx/toolbar.h>
#include <wx/timer.h>

#include "wsRichWindow.h"
#include "wsStackWindow.h"
#include "wsVarWindow.h"
#include "wsTabWindow.h"
#include "wsBreakPoint.h"

class wsPgConn;
class wsResultSet;
class wsConnProp;
class wsWaitingDialog;

class wsCodeCache
{
public:
	wsCodeCache() {}
	wsCodeCache( const wxString & packageOID, const wxString & funcOID, const wxString & source );

	const wxString & getSource()   { return( m_sourceCode ); }

private:
	wxString		m_packageOID;	// Package OID
	wxString		m_funcOID;		// Function OID
	wxString		m_sourceCode;	// Source code for this function

};

class wsCodeWindow : public wxMDIChildFrame
{
    DECLARE_CLASS( wsCodeWindow )

 public:
    wsCodeWindow( wxMDIParentFrame * parent, wxWindowID id, const wsConnProp & connProps );
	virtual ~wsCodeWindow();

    void startLocalDebugging();			 						 // Start debugging 
    void resumeLocalDebugging();			 					 // Start debugging, already attached to the proxy
    void startGlobalDebugging(); 								 // Start debugging 
    void OnCommand( wxCommandEvent & event );					 // Handle menu/toolbar commands
    void processResult( wxString & result );					 // Handle a message from the debugger server
	void OnWriteAttempt( wxStyledTextEvent & event );			 // Do something useful when the user types into the source code window
	void OnNoticeReceived( wxCommandEvent & event );			 // NOTICE received from server
	void OnResultSet( PGresult * result );						 // Result set received from server

	void disableTools();										 // Disable toolbar tools
	void enableTools();										 	 // Enable toolbar tools

	wsBreakpointList & getBreakpointList();

	WX_DECLARE_STRING_HASH_MAP( wsCodeCache, sourceHash );

 private:

	wsStackWindow   * getStackWindow()   { return( m_stackWindow ); }
	wsMessageWindow * getMessageWindow() { return( m_tabWindow->getMessageWindow()); }

	wsVarWindow     * getVarWindow( bool create )     { return( m_tabWindow->getVarWindow( create )); }
	wsVarWindow     * getParamWindow( bool create )   { return( m_tabWindow->getParamWindow( create )); }
	wsVarWindow		* getPkgVarWindow( bool create )  { return( m_tabWindow->getPkgVarWindow( create )); } 
	wsResultGrid    * getResultWindow()               { return( m_tabWindow->getResultWindow()); }

    void 	OnSashDrag( wxSashEvent & event );					// Handle geometry changes
    void 	OnActivate( wxActivateEvent & event );				// Display/Remove debugger toolbar
	void	OnMarginClick( wxStyledTextEvent & event );			// Set/clear breakpoint on margin click
	void	OnSelectFrame( wxCommandEvent & event );			// Select a different stack frame
	void    OnVarChange( wxGridEvent & event );					// User changed a variable
	void	OnIdle( wxIdleEvent & event );						// Idle processor
	void	OnTimer( wxTimerEvent & event );					// Clock tick

	int		getLineNo( );										// Compute line number for current cursor position
	void 	closeConnection();									// Closes proxy connection
	void	updateUI( wsResultSet & breakpoint );				// Update the lazy parts of the UI
	void	updateSourceCode( wsResultSet & breakpoint );		// Update the source code window
	bool	connectionLost( wsResultSet & resultSet );			// Returns TRUE if proxy lost it's connection
	bool	gotFatalError( wsResultSet & resultSet );			// Returns TRUE if result set indicates a fatal error has occurred
	void 	popupError( wsResultSet & resultSet, wxString title);
	void	addBreakpoint( wsBreakpoint * breakpoint, wxEventType nextStep );

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

    wsPgConn		 	*	m_dbgConn;				// Network connection to debugger server
	bool					m_toolsEnabled;			// Should toolbar buttons be enabled?
	wxString				m_debugPort;			// Port at which debugger server is listening
    wxToolBar		 	*	m_toolBar;				// Debugger toolbar
    wxMDIParentFrame 	*	m_parent;				// Parent window
    int				    	m_currentLineNumber;	// Current line number

    wsRichWindow	 	*	m_view;					// Window that displays function source code
    wsStackWindow		*	m_stackWindow;			// Stack Window
    wsTabWindow			*	m_tabWindow;			// Tab Window
	wxSashLayoutWindow  *   m_layout;
	wxSashLayoutWindow  *   m_viewHolder;

	typedef enum
	{
		SESSION_TYPE_UNKNOWN,						// Session could be in-context or direct
		SESSION_TYPE_INCONTEXT,						// Session is configured for in-context debugging
		SESSION_TYPE_DIRECT							// Session is configured for direct debugging
	} eSessionType;

    eSessionType			m_sessionType;			// Debugging mode is in-context or direct?
	bool					m_updateVars;			// Update variable window in next idle period?
	bool				    m_updateStack;			// Update stack window in next idle period?
	bool					m_updateBreakpoints;	// Update breakpoints in next idle period?
	wsBreakpointList	    m_breakpoints;			// List of initial breakpoints to create

	enum
	{
		MARKER_CURRENT    = 0x02,		// Current line marker
		MARKER_CURRENT_BG = 0x04,		// Current line marker - background hilight
		MARKER_BREAKPOINT = 0x01,		// Breakpoint marker
	};

	sourceHash				m_sourceCodeMap;

	wxString				m_focusPackageOid;		// Which package has the debug focus?
	wxString				m_focusFuncOid;			// Which function has the debug focus?
	wxString				m_displayedFuncOid;	  	// Which function are we currently displaying? (function OID component)
	wxString				m_displayedPackageOid;	// Which function are we currently displaying? (package OID component)
	wxString				m_sessionHandle;		// Handle to proxy's server session
	wxString				m_targetName;			// User-friendly target name

	wsWaitingDialog 	  * m_progressBar;			// "Waiting for target" dialog
	int						m_progress;				// Simple counter for advancing m_progressBar
	wxTimer					m_timer;
	bool					m_targetAborted;		// Have we aborted the target? (true) or are we waiting for a breakpoint? (false)
	bool			 findSourceInCache( const wxString & packageOID, const wxString & funcOID );
	void			 getSource(  const wxString & packageOID, const wxString & funcOID );
	void			 cacheSource( const wxString & packageOID, const wxString & funcOID, const wxString & sourceCode );
	void		     displaySource( const wxString & packageOID, const wxString & funcID );
	void 			 unhilightCurrentLine();
	void			 launchWaitingDialog();

	void 			 clearBreakpoint( int lineNumber, bool requestUpdate );
	void			 setBreakpoint( int lineNumber );
	void			 clearAllBreakpoints();
	void			 clearBreakpointMarkers();
	void			 stopDebugging();
	void			 restartDebugging();

	static wxString	m_commandAttach;
	static wxString m_commandWaitForBreakpoint;
	static wxString m_commandGetVars;
	static wxString m_commandGetStack;
	static wxString m_commandGetBreakpoints;
	static wxString m_commandGetSource;
	static wxString m_commandStepOver;
	static wxString m_commandStepInto;
	static wxString m_commandContinue;
	static wxString m_commandSetBreakpoint;
	static wxString m_commandClearBreakpoint;
	static wxString m_commandSelectFrame;
	static wxString m_commandDepositValue;
	static wxString m_commandAbortTarget;
	static wxString m_commandGetTargetInfo;
	static wxString m_commandAddBreakpoint;
	static wxString m_commandCreateListener;
	static wxString m_commandWaitForTarget;
	
	void wsCodeWindow::OnClose( wxCloseEvent & event );

    DECLARE_EVENT_TABLE()
};

#endif // WSCODEWINDOWH
