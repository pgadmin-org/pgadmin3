//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// wscodewindow.cpp - debugger 
//
//////////////////////////////////////////////////////////////////////////

#include "debugger.h"
#include "wsCodeWindow.h"
#include "wsVarWindow.h"
#include "wsMainFrame.h"
#include "debuggerMenu.h"
#include "wsFuncdoc.h"
#include "wsConst.h"
#include "wsResultset.h"
#include "wsBreakPoint.h"
#include "wsWaitingDialog.h"

#include "images/pgAdmin3.xpm"

#include <wx/colour.h>
#include <wx/tokenzr.h>		// wxStringTokenizer
#include <wx/msgdlg.h>		// wxMessageBox

////////////////////////////////////////////////////////////////////////////////
// NOTES:
//
//	1) In this class, we update the user interface in a lazy fashion. Instead of 
//  updating the variable list, call stack, and breakpoint markers every time
//  the target pauses, we wait for an idle period and then query the proxy for
//  the variable list, call stack, and breakpoint list. That way, the debugger 
//	doesn't bog down when you hit the 'step over' or 'step into' key repeatedly.
//
//	Lazy updating is a multi-step operation.  When the target pauses (which 
//  means that a call to pldbg_continue(), pldbg_step_over(), or pldbg_step_into()
//	has just returned), we update the source code window and then set a flag 
//	(m_updateVars) that tells us to update the variable window during the next
//	idle period.  As soon as OnIdle() is called, we update the variable window,
//	turn off the m_updateVars flag, and turn on the next flag (m_updateStack) 
//  to force the stack window to update itself during the next idle period.  
//	After the stack window updates itself, it turns on m_updateBreakpoints() to
//  force the breakpoint markers to refresh.
//
//	2) This class will issue a number of different queries to the proxy process.
//	Each query executes (one at a time) in a separate thread so that the user
//	interface remains responsive. When we queue up a query, we give an event
//	ID to the queue - when the query completes, the wsPgThread objects sends us
//  an event that includes the event ID that we provided.
//
//  All of the result-set related event names start with 'RESULT_ID_' and each
//  event is handled by an event handler function whose name starts with 'Result'.
//  For example, when we queue a query to retrieve the source code for a function,
//  we tell wsPgThread to send us a RESULT_ID_GET_SOURCE event when the query 
//	completes and we handle that event in a function named ResultSource.

IMPLEMENT_CLASS( wsCodeWindow,  wxSashLayoutWindow )

#define MARKERINDEX_TO_MARKERMASK( MI ) ( 1 << MI )

BEGIN_EVENT_TABLE( wsCodeWindow , wxSashLayoutWindow )
  EVT_MENU( MENU_ID_SET_BREAK,	 	 wsCodeWindow::OnCommand )
  EVT_MENU( MENU_ID_CLEAR_BREAK, 	 wsCodeWindow::OnCommand )
  EVT_MENU( MENU_ID_CLEAR_ALL_BREAK, wsCodeWindow::OnCommand )
  EVT_MENU( MENU_ID_SET_WATCH,	 	 wsCodeWindow::OnCommand )
  EVT_MENU( MENU_ID_CLEAR_WATCH, 	 wsCodeWindow::OnCommand )

  EVT_MENU( MENU_ID_CONTINUE,	 	 wsCodeWindow::OnCommand )
  EVT_MENU( MENU_ID_STEP_OVER,	 	 wsCodeWindow::OnCommand )
  EVT_MENU( MENU_ID_STEP_INTO,	 	 wsCodeWindow::OnCommand )
  EVT_MENU( MENU_ID_SET_PC,		 	 wsCodeWindow::OnCommand )
  EVT_MENU( MENU_ID_STOP,	 		 wsCodeWindow::OnCommand )
  EVT_MENU( MENU_ID_RESTART,	 	 wsCodeWindow::OnCommand )

  EVT_IDLE( wsCodeWindow::OnIdle )

  EVT_CLOSE( wsCodeWindow::OnClose)

  EVT_BUTTON( MENU_ID_NOTICE_RECEIVED, wsCodeWindow::OnNoticeReceived )

  EVT_SASH_DRAGGED_RANGE( WINDOW_ID_STACK, WINDOW_ID_BREAKPOINTS, wsCodeWindow::OnSashDrag )

  EVT_STC_MARGINCLICK( wxID_ANY,     wsCodeWindow::OnMarginClick )
  EVT_STC_ROMODIFYATTEMPT( wxID_ANY, wsCodeWindow::OnWriteAttempt )

  EVT_ACTIVATE( wsCodeWindow::OnActivate )

  EVT_LISTBOX( wxID_ANY, wsCodeWindow::OnSelectFrame )
  EVT_GRID_CELL_CHANGE( wsCodeWindow::OnVarChange )

  EVT_MENU( RESULT_ID_ATTACH_TO_PORT, 		wsCodeWindow::ResultPortAttach )
  EVT_MENU( RESULT_ID_BREAKPOINT,			wsCodeWindow::ResultBreakpoint )
  EVT_MENU( RESULT_ID_GET_VARS,				wsCodeWindow::ResultVarList )
  EVT_MENU( RESULT_ID_GET_STACK,			wsCodeWindow::ResultStack )
  EVT_MENU( RESULT_ID_GET_BREAKPOINTS,		wsCodeWindow::ResultBreakpoints )
  EVT_MENU( RESULT_ID_GET_SOURCE,			wsCodeWindow::ResultSource )
  EVT_MENU( RESULT_ID_NEW_BREAKPOINT,		wsCodeWindow::ResultNewBreakpoint )
  EVT_MENU( RESULT_ID_NEW_BREAKPOINT_WAIT,	wsCodeWindow::ResultNewBreakpointWait )
  EVT_MENU( RESULT_ID_DEL_BREAKPOINT,		wsCodeWindow::ResultDeletedBreakpoint )
  EVT_MENU( RESULT_ID_DEPOSIT_VALUE,		wsCodeWindow::ResultDepositValue )
  EVT_MENU( RESULT_ID_ABORT_TARGET,			wsCodeWindow::ResultAbortTarget )
  EVT_MENU( RESULT_ID_ADD_BREAKPOINT,   	wsCodeWindow::ResultAddBreakpoint )
  EVT_MENU( RESULT_ID_LAST_BREAKPOINT,  	wsCodeWindow::ResultLastBreakpoint )
  EVT_MENU( RESULT_ID_LISTENER_CREATED, 	wsCodeWindow::ResultListenerCreated )
  EVT_MENU( RESULT_ID_TARGET_READY,     	wsCodeWindow::ResultTargetReady )

  EVT_TIMER( wxID_ANY, wsCodeWindow::OnTimer )

END_EVENT_TABLE()

////////////////////////////////////////////////////////////////////////////////
// Static data members 
////////////////////////////////////////////////////////////////////////////////
wxString wsCodeWindow::m_commandAttach( wxT( "SELECT * FROM pldbg_attach_to_port(%s)" ));
wxString wsCodeWindow::m_commandWaitForBreakpoint( wxT( "SELECT * FROM pldbg_wait_for_breakpoint(%s)" ));
wxString wsCodeWindow::m_commandGetVars( wxT( "SELECT name, varClass, value, pg_catalog.format_type( dtype, NULL ) as dtype, isconst FROM pldbg_get_variables(%s)" ));
wxString wsCodeWindow::m_commandGetStack( wxT( "SELECT targetName, args, linenumber FROM pldbg_get_stack(%s) ORDER BY level" ));
wxString wsCodeWindow::m_commandGetBreakpoints( wxT( "SELECT * FROM pldbg_get_breakpoints(%s)" ));
wxString wsCodeWindow::m_commandGetSource( wxT( "SELECT %s AS pkg, %s AS func, pldbg_get_source(%s,%s,%s) AS source" ));
wxString wsCodeWindow::m_commandStepOver( wxT( "SELECT * FROM pldbg_step_over(%s)" ));
wxString wsCodeWindow::m_commandStepInto( wxT( "SELECT * FROM pldbg_step_into(%s)" ));
wxString wsCodeWindow::m_commandContinue( wxT( "SELECT * FROM pldbg_continue(%s)" ));
wxString wsCodeWindow::m_commandSetBreakpoint( wxT( "SELECT * FROM pldbg_set_breakpoint(%s,%s,%s,%d)" ));
wxString wsCodeWindow::m_commandClearBreakpoint( wxT( "SELECT * FROM pldbg_drop_breakpoint(%s,%s,%s,%d)" ));
wxString wsCodeWindow::m_commandSelectFrame( wxT( "SELECT * FROM pldbg_select_frame(%s,%d)" ));
wxString wsCodeWindow::m_commandDepositValue( wxT( "SELECT * FROM pldbg_deposit_value(%s,'%s',%d,'%s')" ));		   	
wxString wsCodeWindow::m_commandAbortTarget( wxT( "SELECT * FROM pldbg_abort_target(%s)" ));
wxString wsCodeWindow::m_commandAddBreakpoint( wxT( "SELECT * FROM pldbg_set_global_breakpoint(%s, %s, %s, %s, %s)" ));
wxString wsCodeWindow::m_commandGetTargetInfo( wxT( "SELECT *, %s as pid FROM pldbg_get_target_info('%s', '%c')" ));
wxString wsCodeWindow::m_commandCreateListener( wxT( "SELECT * from pldbg_create_listener()" ));
wxString wsCodeWindow::m_commandWaitForTarget( wxT( "SELECT * FROM pldbg_wait_for_target(%s)" ));

////////////////////////////////////////////////////////////////////////////////
// wsCodeWindow constructor
//
//	This class implements the debugger window.  The constructor expects a string
//  that contains a TCP port number - the constructor connects to the debugger
//  server waiting at that port.  
//
//  A wsCodeWindow object creates (and manages) a toolbar and handles toolbar
//  and keystroke messages. The input messages are treated as debugger commands.
//
//	The m_view member is a wsRichWindow that displays the code for the PL
//  function that you're debugging.  The m_currentLineNumber member tracks the current 
//  line (that is, the line about to execute).  We use hilight the current line.
//  If m_currentLineNumber is -1, there is no current line.  

wsCodeWindow::wsCodeWindow( wxDocParentFrame * parent, wxWindowID id, const wsConnProp & connProps )
	:wxSashLayoutWindow(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER | wxSW_3D | wxCLIP_CHILDREN),
	  m_parent( parent ),
	  m_toolsEnabled( true ),
	  m_currentLineNumber( -1 ),
	  m_view( NULL ),
	  m_stackWindow( NULL ),
	  m_tabWindow( NULL ),
	  m_debugPort( connProps.m_debugPort ),
	  m_sessionHandle(),
	  m_updateVars( false ),
	  m_updateStack( false ),
	  m_updateBreakpoints( false ),
	  m_targetName(),
	  m_progressBar( NULL ),
	  m_progress( 0 ),
	  m_timer( this ),
	  m_targetAborted( false )
{

#if 0
	SetTitle( wxT( "" ));

	// Define the icon for this window
    SetIcons( wxIconBundle( wxIcon( pgAdmin3_xpm )));
#endif
	// A wsCodeWindow is the user interface (the client side) of the PL debugger. We
	// display the source code for a PL function (the function being debugged) in a 
	// wsRichWindow (m_view) so we'll create that window now.

    // Create the leftmost window - a tree control (currently unused)

	m_layout = (wxSashLayoutWindow*) this;

	m_stackWindow = new wsStackWindow( parent , WINDOW_ID_STACK,  wxDefaultPosition, wxDefaultSize, wxNO_BORDER | wxSW_3D | wxCLIP_CHILDREN );

	int width;
	int	height;
	
	glApp->getSettings().Read( wxT( "StackWindow/width" ), &width, 120 );

    m_stackWindow->SetDefaultSize( wxSize( width, 1000 ));
    m_stackWindow->SetOrientation( wxLAYOUT_VERTICAL );
    m_stackWindow->SetAlignment( wxLAYOUT_RIGHT );
    m_stackWindow->SetSashVisible( wxSASH_LEFT, true );
    m_stackWindow->SetBackgroundColour( wxColor( 128, 128, 128 ));

    // Create the bottom window (Notebook)
    m_tabWindow = new wsTabWindow( parent , WINDOW_ID_TABS, wxDefaultPosition, wxDefaultSize, wxNO_BORDER | wxSW_3D | wxCLIP_CHILDREN );
    
	glApp->getSettings().Read( wxT( "TabWindow/height" ), &height, 180 );

    m_tabWindow->SetDefaultSize( wxSize( 1000, height ));
    m_tabWindow->SetOrientation( wxLAYOUT_HORIZONTAL );
    m_tabWindow->SetAlignment( wxLAYOUT_BOTTOM );
    m_tabWindow->SetSashVisible( wxSASH_TOP, true );
    m_tabWindow->SetBackgroundColour( wxColor( 128,128,128 ));

	m_viewHolder = new wxSashLayoutWindow( m_layout, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER | wxSW_3D | wxCLIP_CHILDREN );

	m_viewHolder->SetDefaultSize( wxSize( 1000, 180 ) );
	m_viewHolder->SetOrientation( wxLAYOUT_VERTICAL );
	m_viewHolder->SetAlignment( wxLAYOUT_LEFT );

	m_view = new wsRichWindow( m_viewHolder, -1);
    m_view->m_parentWantsKeys = true;						// Send keystrokes to this object

	// Set up the markers that we use do indicate the current line and a breakpoint
	m_view->MarkerDefine( MARKER_CURRENT, wxSTC_MARK_ARROW , *wxGREEN, *wxGREEN );
	m_view->MarkerDefine( MARKER_CURRENT_BG, wxSTC_MARK_BACKGROUND, *wxGREEN, *wxGREEN );
	m_view->MarkerDefine( MARKER_BREAKPOINT, wxSTC_MARK_CIRCLEPLUS, *wxRED, *wxRED );

	wxFont	numfont(8, wxSWISS, wxNORMAL, wxNORMAL);
	m_view->StyleSetFont(wxSTC_STYLE_DEFAULT, numfont);
	m_view->SetMarginType(1, wxSTC_MARGIN_NUMBER); 
	m_view->SetMarginWidth( 1, 30 );
	// m_view->SetMarginWidth( 1, 16 );
/*
	wxFont	font(10, wxDEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL );
	m_view->StyleSetFont(0, font);
	m_view->StyleSetFont(1, font);
	m_view->StyleSetFont(2, font);
*/
	// Make sure that the text control tells us when the user clicks in the left margin
	m_view->SetMarginSensitive( 0, true );
	m_view->SetMarginSensitive( 1, true );
	m_view->SetMarginSensitive( 2, true );

	// Make sure the user can't edit the source code for this function...
	m_view->SetReadOnly( true );

    wxLayoutAlgorithm	layout;
    layout.LayoutWindow( m_layout, m_viewHolder );

	// Now setup the accelerators (function key mappings)
	wxAcceleratorEntry entries[8];

	entries[0].Set( wxACCEL_NORMAL, WXK_F2, MENU_ID_STEP_INTO );
	entries[1].Set( wxACCEL_NORMAL, WXK_F3, MENU_ID_STEP_OVER );
	entries[2].Set( wxACCEL_NORMAL, WXK_F4, MENU_ID_CONTINUE );
	entries[3].Set( wxACCEL_NORMAL, WXK_F5, MENU_ID_SET_BREAK );
	entries[4].Set( wxACCEL_NORMAL, WXK_F6, MENU_ID_CLEAR_BREAK );
	entries[5].Set( wxACCEL_NORMAL, WXK_F7, MENU_ID_CLEAR_ALL_BREAK );
	entries[6].Set( wxACCEL_NORMAL, WXK_F8, MENU_ID_STOP );
	entries[7].Set( wxACCEL_NORMAL, WXK_F9, MENU_ID_RESTART );

	glMainFrame->SetAcceleratorTable( wxAcceleratorTable( 8, entries ));

#if 0
	SetTitle( _( "Debugger... connecting" ));
#endif
	
	// We create a wsCodeWindow when a wsPgThread intercepts a PLDBGBREAK NOTICE
	// generated by the PostgreSQL server.   The NOTICE contains a TCP port number
	// and we connect to that port here.
	glMainFrame->manager.AddPane(m_layout, wxAuiPaneInfo().Name(wxT("view")).Caption(_("view")).Center().CaptionVisible(false).CloseButton(false).MinSize(wxSize(200,100)).BestSize(wxSize(350,200)));
	glMainFrame->manager.AddPane(m_stackWindow, wxAuiPaneInfo().Name(wxT("stackWindow")).Caption(_("stackWindow")).Right().MinSize(wxSize(100,100)).BestSize(wxSize(250,200)));
	glMainFrame->manager.AddPane(m_tabWindow, wxAuiPaneInfo().Name(wxT("tabWindow")).Caption(_("tabWindow")).Bottom().MinSize(wxSize(200,100)).BestSize(wxSize(550,300)));

	glMainFrame->manager.GetPane(wxT("view")).Caption(_("view"));
	glMainFrame->manager.GetPane(wxT("stackWindow")).Caption(_("stackWindow"));
	glMainFrame->manager.GetPane(wxT("tabWindow")).Caption(_("tabWindow"));
 
	glMainFrame->PerspectivesDef();

	glMainFrame->manager.Update();

	glMainFrame->Refresh();

	// The wsDbgConn constructor connects to the given host+port and
	// and sends events to us whenever a string arrives from the 
	// debugger server.

	glApp->getStatusBar()->SetStatusText( _( "Connecting to debugger" ), 1 );
	m_dbgConn = new wsPgConn( connProps, this );
	glApp->getStatusBar()->SetStatusText( _( "Connecting to debugger..." ), 1 );

	// Our proxy API may throw (perfectly legitimate) errors at us (for example,
	// if the target process ends while we are waiting for a breakpoint) - apparently
	// those error messages scare the user when they show up in the log, so we'll
	// just suppress logging for this session

	PQclear( m_dbgConn->waitForCommand( wxT( "SET log_min_messages TO fatal" )));

	m_sessionType = SESSION_TYPE_UNKNOWN;

	// force
	enableTools();
	wxActivateEvent   fakeActivate( 0, true );
	OnActivate( fakeActivate );

}

////////////////////////////////////////////////////////////////////////////////
// wsCodeWindow destructor
//
//	Shuts down a debugger session.  At the moment, we just record window 
//	geometries and go home.

wsCodeWindow::~wsCodeWindow()
{
	int	height;
	int width;

	// Save the width of the stack window...

	m_stackWindow->GetClientSize( &width, &height );
	glApp->getSettings().Write( wxT( "StackWindow/width" ), width );

	// And the height of the tab window...

	m_tabWindow->GetClientSize( &width, &height );
	glApp->getSettings().Write( wxT( "TabWindow/height" ), height );

	glApp->getSettings().Flush();

	// NOTE: you may be tempted to delete m_dbgConn here, but don't do it.
	//		 m_dbgConn has an associated thread and that thread my be 
	//		 waiting for the server (for example, that thread may be waiting 
	//		 for a call to pldbg_continue()).  We don't want to delete the
	//		 data structures that that thread requires so we'll just leave
	//		 it alone for now.

}

////////////////////////////////////////////////////////////////////////////////
// doDebug()
//
//  This function gets things started by asking the proxy process to attach
//	to the given port (m_debugPort).

void wsCodeWindow::startLocalDebugging()
{
	m_sessionType = SESSION_TYPE_DIRECT;

  	m_dbgConn->startCommand( wxString::Format( m_commandAttach, m_debugPort.c_str()), GetEventHandler(), RESULT_ID_ATTACH_TO_PORT );
}

void wsCodeWindow::resumeLocalDebugging()
{
	// The user has asked to resume local debugging after the previous target finished.
	// We can find ourself in two different states here:
	//
	//	1) The previous target may have run to completion, in which case we are already
	//	   waiting for the target to hit a breakpoint
	//
	//	2) The user aborted the previous target, in which case we are NOT waiting for 
	//	   a breakpoint and we'd better tell the proxy to wait
	//
	// We use the m_targetAborted flag to figure out which of the two states were are 
	// in. If m_targetAborted is false, our proxy is already waiting for the target 
	// to hit a breakpoint so we don't have to do anything here.  If m_targetAborted
	// is true, the proxy is waiting for another command from us - send it a waitForBreakpoint
	// request

	if( m_targetAborted )
	{
		m_targetAborted = false;
		m_dbgConn->startCommand( wxString::Format( m_commandWaitForBreakpoint, m_sessionHandle.c_str()), GetEventHandler(), RESULT_ID_BREAKPOINT );		
	}
}

////////////////////////////////////////////////////////////////////////////////
// OnActivate()
//
// 	This event handler is called when this window is activated *or* deactivated.
// 	When we become active, we want to display the debugger toolbar.
// 	When we become inactive, we want to hide the debugger toolbar.

void wsCodeWindow::disableTools( )
{
	m_toolsEnabled = FALSE;
}

void wsCodeWindow::enableTools( )
{
	m_toolsEnabled = TRUE;
}

void wsCodeWindow::OnActivate( wxActivateEvent & event )
{
	bool activateDebug = event.GetActive();

	// If we don't have a connection to the debugger proxy, disable all
	// of the debugger controls

	if( m_dbgConn == NULL )
		activateDebug = FALSE;

	if( m_toolsEnabled == FALSE )
		activateDebug = FALSE;

	glMainFrame->manager.GetPane(wxT("Toolbar")).Caption(_("Toolbar"));

	wxToolBar * t = glMainFrame->m_toolBar;

	// We may find that our toolbar has disappeared during application shutdown -
	// It seems a little strange that OnActivate() is called during shutdown, but 
	// that does seem to happen on Win32 hosts.

	if( t == NULL )
		return;

	t->EnableTool( MENU_ID_STEP_INTO,   	activateDebug );
	t->EnableTool( MENU_ID_STEP_OVER,   	activateDebug );
	t->EnableTool( MENU_ID_CONTINUE,    	activateDebug );
	t->EnableTool( MENU_ID_SET_BREAK,   	activateDebug );
	t->EnableTool( MENU_ID_CLEAR_BREAK, 	activateDebug );
	t->EnableTool( MENU_ID_CLEAR_ALL_BREAK, activateDebug );
	t->EnableTool( MENU_ID_STOP,			activateDebug );
	
	// Restart functionality shall not be available during in-context
	// debugging so make the Restart button inactive

	if( m_sessionType == SESSION_TYPE_INCONTEXT )
		t->EnableTool( MENU_ID_RESTART, false );
	else 
		t->EnableTool( MENU_ID_RESTART, activateDebug );
	

	t->EnableTool( MENU_ID_EXECUTE,    ! activateDebug );
	t->EnableTool( wxID_CUT,   		   ! activateDebug );
	t->EnableTool( wxID_PASTE, 		   ! activateDebug );		



#if INCLUDE_WATCHPOINTS
	t->EnableTool( MENU_ID_SET_WATCH,   activateDebug );
	t->EnableTool( MENU_ID_CLEAR_WATCH, activateDebug );
	t->EnableTool( MENU_ID_SET_PC,      activateDebug );
#endif
}

////////////////////////////////////////////////////////////////////////////////
// OnIdle()
//
//	This event handler is called during 'idle time' (that is, when other events
//  are not waiting in the queue).  We use the idle period to update most parts
//  of the user interface in a lazy fashion.  That way, the debugger doesn't
//  bog down when you hit the 'step over' or 'step into' key repeatedly - we
//  don't bother updating the variable list, call stack, or breakpoint markers
//  until the user pauses for a moment.

void wsCodeWindow::OnIdle( wxIdleEvent & event )
{
	if( m_updateVars )
	{	
		m_updateVars = FALSE;
		m_dbgConn->startCommand( wxString::Format( m_commandGetVars, m_sessionHandle.c_str()), GetEventHandler(), RESULT_ID_GET_VARS );
		return;
	}

	if( m_updateStack )
	{
		m_updateStack = FALSE;
		m_dbgConn->startCommand( wxString::Format( m_commandGetStack, m_sessionHandle.c_str()), GetEventHandler(), RESULT_ID_GET_STACK );		
		return;
	}
	
	if( m_updateBreakpoints )
	{
		m_updateBreakpoints = FALSE;
		m_dbgConn->startCommand( wxString::Format( m_commandGetBreakpoints, m_sessionHandle.c_str()), GetEventHandler(), RESULT_ID_GET_BREAKPOINTS );
		return;
	}

}

////////////////////////////////////////////////////////////////////////////////
// ResultPortAttach()
//
//	This event handler is called when the result set of an earlier query arrives
//  from the proxy. In this case, the result set is generated by a call to 
//  pldbg_attach_to_port(). If the query succeeded, our proxy is connected to 
//  the debugger server running inside of the target process. At that point,
//  we have to wait for the target process to wait for a breakpoint so we 
//  queue up another query (a call to pldbg_wait_for_breakpoint()).

void wsCodeWindow::ResultPortAttach( wxCommandEvent & event )
{
	wsResultSet  result((PGresult *)event.GetClientData()); 

	if( result.getCommandStatus() == PGRES_TUPLES_OK )
	{
		glApp->getStatusBar()->SetStatusText( _( "Connected to debugger" ), 1 );

		m_sessionHandle = result.getString( 0 );
		
		m_dbgConn->startCommand( wxString::Format( m_commandWaitForBreakpoint, m_sessionHandle.c_str()), GetEventHandler(), RESULT_ID_BREAKPOINT );		
	}
	else
	{
		wxMessageBox( result.getErrorMessage(), _( "Connection Error" ));
		closeConnection();
	}
}


////////////////////////////////////////////////////////////////////////////////
// ResultBreakpoint()
//
//	This event handler is called when the target process pauses.  The target
//  may have reached a breakpoint or it may have just completed a step/over or
//	step/into.
//
//	In any case, we schedule an update of the user interface for the next idle
//  period by calling updateUI().

void wsCodeWindow::ResultBreakpoint( wxCommandEvent & event )
{
	wsResultSet  result((PGresult *)event.GetClientData()); 

	if( connectionLost( result ))
		closeConnection();
	else
	{
		if( result.getCommandStatus() == PGRES_TUPLES_OK )
		{
			// Change our focus
			m_focusPackageOid = result.getString( wxT( "pkg" ));
			m_focusFuncOid    = result.getString( wxT( "func" ));
#if 0
			SetTitle( result.getString( _( "targetname" )));
#endif
			// The result set contains one tuple: 
			//	packageOID, functionOID, linenumber
			glApp->getStatusBar()->SetStatusText( wxString::Format( _( "Paused at line %s" ), result.getString( wxT( "linenumber" )).c_str()), 1 );		

			updateUI( result );
		}
		else if( result.getCommandStatus() == PGRES_FATAL_ERROR )
		{
			/* 
			 * We were waiting for a breakpoint (because we just sent a step into, step over, or continue request) and
			 * the proxy sent us an error instead.  Presumably, the target process exited before reaching a breakpoint.
			 *
			 * If we have any global breakpoints, we must have a listener in the proxy... wait for the next target process.
			 */

			if( m_breakpoints.GetCount())
			{
				m_dbgConn->startCommand( wxString::Format( m_commandWaitForTarget, m_sessionHandle.c_str()), GetEventHandler(), RESULT_ID_TARGET_READY );		

				glApp->getStatusBar()->SetStatusText( _( "Waiting for a target" ), 1 );
				glApp->getStatusBar()->SetStatusText( wxT( "" ), 2 );
#if 0
				SetTitle( _( "Debugger... waiting for target" ));
#endif
				launchWaitingDialog();
			}
		}
	}
}


void wsCodeWindow::launchWaitingDialog()
{
	glApp->getStatusBar()->SetStatusText( wxString::Format( _( "Waiting for another session to invoke %s" ), m_targetName.c_str()), 1 );
#if 0
	SetTitle( wxString::Format( _( "Debugger...waiting for %s" ), m_targetName.c_str()));
#endif
	// NOTE: the waiting-dialog takes forever to appear running a remote X session so you can disable it by defining the following env. variable
	if( getenv( "SUPPRESS_WAIT_DIALOG" ))
		m_progressBar = NULL;
	else
	{
		m_progressBar = new wsWaitingDialog(  _( "Waiting for target" ), wxString::Format( _( "Waiting for breakpoint in %s" ), m_targetName.c_str()), 0, this, wxPD_CAN_ABORT | wxPD_ELAPSED_TIME );
		
		m_timer.Start( 1000 );	// One clock tick per second
	}
}

////////////////////////////////////////////////////////////////////////////////
// ResultVarList()
//
//	This event handler is called when the proxy finishes sending us a list of 
//  variables (in response to an earlier call to pldbg_get_variables()).
//
//	We extract the variable names, types, and values from the result set and
//	add them to the variable (and parameter) windows.

void wsCodeWindow::ResultVarList( wxCommandEvent & event )
{
	wsResultSet  result((PGresult *)event.GetClientData()); 

	if( connectionLost( result ))
		closeConnection();
	else
	{
		if( result.getCommandStatus() == PGRES_TUPLES_OK )
		{
			// The result set contains one tuple per variable
			for( int row = 0; row < result.getRowCount(); ++row )
			{
				wxString	varName = result.getString( wxT( "name" ), row );
				char		varClass = result.getString( wxT( "varclass" ), row )[0];

				if( varClass == 'A' )
				{
					getParamWindow( true )->addVar( varName, result.getString( wxT( "value" ), row ), result.getString( wxT( "dtype" ), row ), result.getBool( wxT( "isconst" ), row ));
				}
				else if( varClass == 'P' )
				{
					getPkgVarWindow( true )->addVar( varName, result.getString( wxT( "value" ), row ), result.getString( wxT( "dtype" ), row ), result.getBool( wxT( "isconst" ), row ));
				}
				else
				{
					getVarWindow( true )->addVar( varName, result.getString( wxT( "value" ), row ), result.getString( wxT( "dtype" ), row ), result.getBool( wxT( "isconst" ), row ));
				}
			}
		}

		// Update the next part of the user interface 
		m_updateStack = TRUE;
	}
}

////////////////////////////////////////////////////////////////////////////////
// ResultStack()
//
//	This event handler is called when the proxy finishes sending us a stack
//	trace (in response to an earlier call to pldbg_get_stack()).
//
//	We extract each frame from the result set and add it to the stack window.
//	For each frame, the proxy sends us the function name, line number, and
//  a string that holds the name and value of each argument.

void wsCodeWindow::ResultStack( wxCommandEvent & event )
{
	wsResultSet  result((PGresult *)event.GetClientData()); 

	if( connectionLost( result ))
		closeConnection();
	else
	{
		if( result.getCommandStatus() == PGRES_TUPLES_OK )
		{
			// The result set contains one tuple per frame:
			//		package, function, linenumber, args
		
			wxArrayString	stack;

			for( int row = 0; row < result.getRowCount(); ++row )
				stack.Add( wxString::Format( wxT( "%s(%s)@%s" ), result.getString( wxT( "targetName" ), row ).c_str(), result.getString( wxT( "args" ), row ).c_str(), result.getString( wxT( "linenumber" ), row ).c_str()));
		
			getStackWindow()->clear();
			getStackWindow()->setStack( stack );
										 
		}

		m_updateBreakpoints = TRUE;
	}
}

////////////////////////////////////////////////////////////////////////////////
// ResultBreakpoints()
//
//	This event handler is called when the proxy finishes sending us a list of 
//  breakpoints (in response to an earlier SHOW BREAKPOINTS command).
//
//	We clear out the old breakpoint markers and then display a new marker
//	for each breakpoint defined in the current function.

void wsCodeWindow::ResultBreakpoints( wxCommandEvent & event )
{
	wsResultSet  result((PGresult *)event.GetClientData()); 

	if( connectionLost( result ))
		closeConnection();
	else
	{
		if( result.getCommandStatus() == PGRES_TUPLES_OK )
		{
			clearBreakpointMarkers();

			// The result set contains one tuple per breakpoint:
			//		packageOID, functionOID, linenumber

			for( int row = 0; row < result.getRowCount(); ++row )
			{
				m_view->MarkerAdd( result.getLong( wxT( "linenumber" ), row ), MARKER_BREAKPOINT );
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
// ResultSource()
//
//	This event handler is called when the proxy finishes sending us the source
//	code for a function (in response to an earlier call to pldbg_get_source()).
//
//	We keep a client-side cache of source code listings to avoid an extra 
//  round trip for each step. In this function, we add the source code to 
//  the cache and then display the source code in the source window.

void wsCodeWindow::ResultSource( wxCommandEvent & event )
{
	wsResultSet  result((PGresult *)event.GetClientData()); 

	if( connectionLost( result ))
		closeConnection();
	else
	{
		cacheSource( result.getString( wxT( "pkg" )), result.getString( wxT( "func" )), result.getString( wxT( "source" )));
		displaySource( result.getString( wxT( "pkg" )), result.getString( wxT( "func" )));
	}
}

////////////////////////////////////////////////////////////////////////////////
// ResultDeletedBreakpoint()
//
//	This event handler is called when the proxy finishes executing a DROP
//	BREAKPOINT command on our behalf.
//
//	If the DROP BREAKPOINT command succeeded, we display a mesasge in the
//  status bar.

void wsCodeWindow::ResultDeletedBreakpoint( wxCommandEvent & event )
{
	wsResultSet  result((PGresult *)event.GetClientData()); 

	if( connectionLost( result ))
		closeConnection();
	else
	{
		if( result.getBool( 0 ))
		{
			m_updateBreakpoints = TRUE;
			glApp->getStatusBar()->SetStatusText( _( "Breakpoint dropped" ), 1 );		
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
// ResultNewBreakpoint()
//
//	This event handler is called when the proxy finishes executing a CREATE
//	BREAKPOINT command on our behalf.
//
//	We schedule a refresh of our breakpoint markers for the next idle period.

void wsCodeWindow::ResultNewBreakpoint( wxCommandEvent & event )
{
	wsResultSet  result((PGresult *)event.GetClientData()); 

	if( connectionLost( result ))
		closeConnection();
	else if( gotFatalError( result ))
		popupError( result, _( "Can't set breakpoint" ));
	else
	{
	}
}

void wsCodeWindow::ResultNewBreakpointWait( wxCommandEvent & event )
{
	wsResultSet  result((PGresult *)event.GetClientData()); 

	if( connectionLost( result ))
		closeConnection();
	else if( gotFatalError( result ))
		popupError( result, _( "Can't set breakpoint" ));
	else
	{
		m_dbgConn->startCommand( wxString::Format( m_commandWaitForTarget, m_sessionHandle.c_str()), GetEventHandler(), RESULT_ID_TARGET_READY );		

		launchWaitingDialog();
	}
}
////////////////////////////////////////////////////////////////////////////////
// ResultDepositValue()
//
//	This event handler is called when the proxy completes a 'deposit' operation
//	(in response to an earlier call to pldbg_deposit_value()).
//
//	We schedule a refresh of our variable window(s) for the next idle period.

void wsCodeWindow::ResultDepositValue( wxCommandEvent & event )
{
	wsResultSet  result((PGresult *)event.GetClientData()); 

	if( connectionLost( result ))
		closeConnection();
	else
	{
		if( result.getBool( 0 ))
		{
			glApp->getStatusBar()->SetStatusText( _( "Value changed" ), 1 );		
			m_updateVars = TRUE;
		}
		else
		{
			// FIXME: display error message here...
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
// ResultAbortTarget()
//
//	This event handler is called when the proxy completes an 'abort target' 
//  operation (in response to an earlier call to pldbg_abort_target()).
//

void wsCodeWindow::ResultAbortTarget( wxCommandEvent & event )
{
	wsResultSet  result((PGresult *)event.GetClientData()); 

	if( connectionLost( result ))
		closeConnection();
	else
	{
		if( result.getBool( 0 ))
		{
			// Make a note of the fact that we've aborted the target, that way we
			// can know (should we start debugging again) that we are *not* waiting
			// for a breakpoint.
			m_targetAborted = true;

			glApp->getStatusBar()->SetStatusText( _( "Execution Canceled" ), 1 );		

			// Remove the current-line indicator
			unhilightCurrentLine();

			// And force the toolbar to refresh itself (to disable the debugger-related tools)
			wxActivateEvent   fakeActivate( 0, false );

			OnActivate( fakeActivate );
		}
		else
		{
			// FIXME: display error message here...
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
// connectionLost()
//
//	This function checks the given result set to determine whether the connection
//  to the debugger proxy is still viable.  For the moment, we assume that any
//  fatal error is... fatal.  We should probably PQstatus() if we get a fatal
//	error.

bool wsCodeWindow::connectionLost( wsResultSet & resultSet )
{
	if( m_dbgConn->isConnected())
		return( false );
	else
		return( true );
}

bool wsCodeWindow::gotFatalError( wsResultSet & resultSet )
{
	if( resultSet.getCommandStatus() == PGRES_FATAL_ERROR )
		return( true );
	else
		return( false );
}

void wsCodeWindow::popupError( wsResultSet & resultSet, wxString title )
{
	wxMessageBox( resultSet.getErrorMessage(), title );

	// Now close the entire application since we can't continue debugger
	m_timer.Stop();
	glMainFrame->Close();

}

////////////////////////////////////////////////////////////////////////////////
// closeConnection()
//
//	This member function closes the connection to the debugger and changes the
//  user interface to let the user know what just happened. In particular, we 
//	remove the breakpoint markers (they may be obsolete) and disable the 
//	debugger-related tools on the toolbar.

void wsCodeWindow::closeConnection()
{
	// Close the debugger (proxy) connection

	m_dbgConn->close();
	m_dbgConn = NULL;
 
	// Let the user know what happened
	glApp->getStatusBar()->SetStatusText( _( "Debugger connection terminated (session complete)" ), 1 );		

	// Remove the current-line indicator
	int	lineNo = m_view->MarkerNext( 0, MARKERINDEX_TO_MARKERMASK( MARKER_CURRENT ));

	if( lineNo != -1 )
	{
		m_view->MarkerDelete( lineNo, MARKER_CURRENT );
		m_view->MarkerDelete( lineNo, MARKER_CURRENT_BG );
	}

	// And force the toolbar to refresh itself (to disable the debugger-related tools)
	wxActivateEvent   fakeActivate;

	OnActivate( fakeActivate );


}

////////////////////////////////////////////////////////////////////////////////
// updateSourceCode()
//
//	This function is invoked whenever the target process pauses (either because
//	it reached a breakpoint or because it just completed a step over/into).  In 
//  this function, we update the source code window. 
//
//	The caller gives us a result set (that had better contain a breakpoint tuple)
//	that gives us the OID of the function that we're paused in.  We search our
//	cache for the source code for that function. If we don't already have the 
//	source code for the function, we send a request for the code to the proxy and
//	(asynchronously) wait for the result set (ResultSource() is called when the
//	result set arrives). If we have the required source code (in the cache), we
//  display the code in the source window (if not already displayed).

void wsCodeWindow::updateSourceCode( wsResultSet & breakpoint )
{
	wxString	packageOID( breakpoint.getString( wxT( "pkg" )));
	wxString	funcOID( breakpoint.getString( wxT( "func" )));
	wxString	lineNumber( breakpoint.getString( wxT( "linenumber" )));

	m_currentLineNumber = atoi((const char *)lineNumber.c_str());

	if( !findSourceInCache( packageOID, funcOID ))
	{
		getSource( packageOID, funcOID );
	}
	else
	{
		displaySource( packageOID, funcOID );
	}
}

////////////////////////////////////////////////////////////////////////////////
// updateUI()
//
//	This function is called each time the target pauses (either because it reached
//  a breakpoint or because it just completed a step over/into). We update some 
//	parts of the user interface in a lazy fashion so that we remain responsive if
//  the user repeatedly hits the step over/into key.  updateUI() turns off the lazy
//  update flags and the updates the source code window - when the source code 
//  refresh completes, we schedule a variable refresh for the next idle period. 
//	When the variable refresh completes, it schedules a stack refresh...

void wsCodeWindow::updateUI( wsResultSet & breakpoint )
{
	// Arrange for the lazy parts of our UI to be updated
	// during the next IDLE time
	m_updateVars  	    = FALSE;
	m_updateStack 		= FALSE;
	m_updateBreakpoints = FALSE;

	updateSourceCode( breakpoint );
}

////////////////////////////////////////////////////////////////////////////////
// clearBreakpointMarkers()
//
//	This is a helper function that clears all of the breakpoint markers currently
//	displayed in the source code window (m_view).

void wsCodeWindow::clearBreakpointMarkers()
{
	int	lineNo = 0;

	while(( lineNo = m_view->MarkerNext( lineNo, MARKERINDEX_TO_MARKERMASK( MARKER_BREAKPOINT ))) != -1 )
		m_view->MarkerDelete( lineNo++, MARKER_BREAKPOINT );
}

////////////////////////////////////////////////////////////////////////////////
// findSourceInCache()
//
//	A wsCodeWindow can display the source code for many different functions (if
//  you step from one function into another function, the same wsCodeWindow will
//  display the source code for each function). 
//
//	To avoid constantly re-sending the source code for each function over the
//  network we keep a cache (m_sourceCodeMap) of the source code for each 
//  function that we've seen. The cache is indexed by function OID.  We keep
//  track of the transaction and command ID for each function too so we know
//  when our cached copy becomes stale.
//  
//  This function searches the cache for an entry that matches the given 
//  function ID.  Note that we simply return true or false to indicate 
//  whether the function exists in the cache - to retreive the actual source
//  code for a function, call getSource()

bool wsCodeWindow::findSourceInCache(  const wxString & packageOID, const wxString & funcOID )
{
	sourceHash::iterator match = m_sourceCodeMap.find( funcOID );

	if( match == m_sourceCodeMap.end())
		return( false );
	else
	{
		// FIXME: compare the xid and cid here, throw out out the cached copy (and return false) if they don't match
		return( true );
	}
}

////////////////////////////////////////////////////////////////////////////////
// cacheSource()
//
//	This function adds the source code for a given function to the cache.  See
//  findSourceInCache() for more details.

void wsCodeWindow::cacheSource( const wxString & packageOID, const wxString & funcOID, const wxString & sourceCode )
{
	// Throw out any stale version
	m_sourceCodeMap.erase( funcOID );

	// And add the new version to the cache.
	m_sourceCodeMap[funcOID] = wsCodeCache( packageOID, funcOID, sourceCode );

}

////////////////////////////////////////////////////////////////////////////////
// getSource()
//
//	This function retrieves the source code for a given function from the 
//  PostgreSQL server. We don't actually wait for completionm, we just 
//  schedule the request.

void wsCodeWindow::getSource( const wxString & packageOID, const wxString & funcOID )
{
	m_dbgConn->startCommand( wxString::Format( m_commandGetSource, packageOID.c_str(), funcOID.c_str(), m_sessionHandle.c_str(), packageOID.c_str(), funcOID.c_str()), GetEventHandler(), RESULT_ID_GET_SOURCE );
}

////////////////////////////////////////////////////////////////////////////////
// displaySource()
//
//	This function loads the source code for the given funcID into the view.
//  If the requested function is already loaded, we just return without doing
//  any extra work.

void wsCodeWindow::displaySource( const wxString & packageOID, const wxString & funcOID )
{

	// We're about to display the source code for the target, give the keyboard
	// focus to the view (so that function keys will work).  This seems like a 
	// reasonable point in time to grab the focus since we're just doing something
	// rather visual.
	m_view->SetFocus();

	// If we've already loaded the source code for this function, just update the current-line marker
	if( m_displayedFuncOid != funcOID || m_displayedPackageOid != packageOID )
	{
		// We're looking at a different target now, delete all of the local, package, and parameter variables
		// so we can add the variables defined by the new target.
		if( getVarWindow( false ))
			getVarWindow( false )->delVar();

		if( getParamWindow( false ))
			getParamWindow( false )->delVar();

		if( getPkgVarWindow( false ))
			getPkgVarWindow( false )->delVar();
		
		m_displayedFuncOid 	  = funcOID;
		m_displayedPackageOid = packageOID;

		wsCodeCache & codeCache = m_sourceCodeMap[funcOID];

	    // Now erase any old code and write out the new listing
	    m_view->SetReadOnly( false );
		m_view->SetText( codeCache.getSource());
		m_view->Colourise( 0, codeCache.getSource().Length());
		m_view->SetReadOnly( true );
	}

	// Clear the current-line indicator

	int	lineNo = m_view->MarkerNext( 0, MARKERINDEX_TO_MARKERMASK( MARKER_CURRENT ));

	if( lineNo != m_currentLineNumber )
	{
		if( lineNo != -1 )
		{
			m_view->MarkerDelete( lineNo, MARKER_CURRENT );
			m_view->MarkerDelete( lineNo, MARKER_CURRENT_BG );
		}

		// Add the current-line indicator to the current line of code
		m_view->MarkerAdd( m_currentLineNumber, MARKER_CURRENT );
		m_view->MarkerAdd( m_currentLineNumber, MARKER_CURRENT_BG );
	}

	// Scroll the source code listing (if required) to make sure
	// that this line of code is visible 
	//
	// (note: we set the anchor and the caret to the same position to avoid
	// creating a selection region)

	m_view->SetAnchor( m_view->PositionFromLine( m_currentLineNumber ));
	m_view->SetCurrentPos( m_view->PositionFromLine( m_currentLineNumber ));
	m_view->EnsureCaretVisible();

	// Update the next lazy part of the user interface (the variable list)
	m_updateVars = TRUE;

}

////////////////////////////////////////////////////////////////////////////////
// getLineNo()
//
// 	Figure out which line (in the source code listing) contains the cursor
//	(actually, the insertion point)

int wsCodeWindow::getLineNo( )
{
	return( m_view->LineFromPosition( m_view->GetCurrentPos( )));
}

////////////////////////////////////////////////////////////////////////////////
// OnCommand()
//
// 	This event handler is called when the user clicks a button in the debugger
//	toolbar.

void wsCodeWindow::OnCommand( wxCommandEvent & event )
{

    switch( event.GetId())
    {
		case MENU_ID_SET_BREAK:
		{
			// The user wants to set a breakpoint at the line that
			// contains the insertion point (the caret)

			setBreakpoint( getLineNo());
			break;
		}

		case MENU_ID_CLEAR_BREAK:
		{
			// The user wants to clear the breakpoint at the line that
			// contains the insertion point (the caret)

			clearBreakpoint( getLineNo(), true );
			break;
		}

		case MENU_ID_CLEAR_ALL_BREAK:
		{
			// The user wants to clear all the breakpoint

 			clearAllBreakpoints();
 			break;
		}	

		case MENU_ID_CONTINUE:
		{
			// The user wants to continue execution (as opposed to
			// single-stepping through the code).  Unhilite all 
			// variables and tell the debugger server to continue.

			m_dbgConn->startCommand( wxString::Format( m_commandContinue, m_sessionHandle.c_str()), GetEventHandler(), RESULT_ID_BREAKPOINT );		
			glApp->getStatusBar()->SetStatusText( _( "waiting for target (continue)" ), 1 );
			unhilightCurrentLine();

			break;
		}

		case MENU_ID_STEP_OVER:
		{
			// The user wants to step-over a function invocation (or
			// just single-step). Unhilite all variables and tell the
			// debugger server to step-over

			m_dbgConn->startCommand( wxString::Format( m_commandStepOver, m_sessionHandle.c_str()), GetEventHandler(), RESULT_ID_BREAKPOINT );		
			glApp->getStatusBar()->SetStatusText( _( "waiting for target (step over)" ), 1 );
			unhilightCurrentLine();

			break;
		}

		case MENU_ID_STEP_INTO:
		{
			// The user wants to step-into a function invocation (or
			// just single-step). Unhilite all variables and tell the
			// debugger server to step-into

			m_dbgConn->startCommand( wxString::Format( m_commandStepInto, m_sessionHandle.c_str()), GetEventHandler(), RESULT_ID_BREAKPOINT );		
			glApp->getStatusBar()->SetStatusText( _( "waiting for target (step into)" ), 1 );
			unhilightCurrentLine();

			break;
		}

		case MENU_ID_STOP:
		{
			stopDebugging();
			break;
		}
		
		case MENU_ID_RESTART:
		{
			restartDebugging();	 
			break;
		}

		case MENU_ID_SET_PC:
		case MENU_ID_SET_WATCH:
		case MENU_ID_CLEAR_WATCH:
		default:
		{
			// The user wants to change the program counter (that is, jump to
			// a different point in this function), set a variable watchpoint,
			// or clear a variable watchpoint).
			// 
			// These functions are not yet complete.

			::wxMessageBox( _( "This function not yet complete" ), _( "Sorry..." ));
			break;
		}
    }
}

////////////////////////////////////////////////////////////////////////////////
// OnWriteAttempt()
//
//	This event handler is invoked when the user tries to type into the source
//  code listing. We can't let the user modify this view since we always want 
//  to show the real source code that the user is stepping through.  Instead,
//  we create a new window and let the user modify a copy of the function source
//  code.  We use the document/view mechanism to handle the tough work.

void wsCodeWindow::OnWriteAttempt( wxStyledTextEvent & event )
{
#if INCLUDE_FUNCTION_EDITOR
	wsFuncDoc::OpenDoc( m_view->GetText(), GetTitle() + wxT( "*" ), m_displayedFuncOid, m_view->GetCurrentPos());
#endif
}

////////////////////////////////////////////////////////////////////////////////
// OnSashDrag()
//
//	wxWidgets calls this member function when the user drags the sash that 
//  separates the tree window from the other windows

void wsCodeWindow::OnSashDrag( wxSashEvent & event )
{
    if( event.GetDragStatus() == wxSASH_STATUS_OUT_OF_RANGE )
        return;

    switch (event.GetId())
    {
		case WINDOW_ID_STACK:
			m_stackWindow->SetDefaultSize( wxSize( event.GetDragRect().width, 10 ));
			break;

		case WINDOW_ID_TABS:
			m_tabWindow->SetDefaultSize( wxSize( 60, event.GetDragRect().height ));
			break;
    }

    wxLayoutAlgorithm	layout;
    layout.LayoutWindow( m_layout, m_viewHolder );
}

void wsCodeWindow::OnNoticeReceived( wxCommandEvent & event )
{   
	getMessageWindow()->AppendText( event.GetString());
	m_tabWindow->selectTab( ID_MSG_PAGE );
}

void wsCodeWindow::OnResultSet( PGresult * result )
{
	getResultWindow()->fillGrid( result );
}

////////////////////////////////////////////////////////////////////////////////
// setBreakpoint()
//
//	This function creates a breakpoint at the given line number (actually, it
//  sends a request to the debugger server to create the breakpoint - the server
//  will send back a reply if everything worked).

void wsCodeWindow::setBreakpoint( int lineNumber )
{
	m_dbgConn->startCommand( wxString::Format( m_commandSetBreakpoint, m_sessionHandle.c_str(), m_displayedPackageOid.c_str(), m_displayedFuncOid.c_str(), lineNumber ), GetEventHandler(), RESULT_ID_NEW_BREAKPOINT );
	m_updateBreakpoints = TRUE;

}

////////////////////////////////////////////////////////////////////////////////
// clearBreakpoint()
//
//	This function clears a breakpoint at the given line number (actually, it
//  sends a request to the debugger server to clear the breakpoint)

void wsCodeWindow::clearBreakpoint( int lineNumber, bool requestUpdate )
{
	m_dbgConn->startCommand( wxString::Format( m_commandClearBreakpoint, m_sessionHandle.c_str(), m_displayedPackageOid.c_str(), m_displayedFuncOid.c_str(), lineNumber ), GetEventHandler(), RESULT_ID_NEW_BREAKPOINT );

	if( requestUpdate )
		m_updateBreakpoints = TRUE;
		
}

void wsCodeWindow::clearAllBreakpoints( )
{
	int	lineNo = 0;

	while(( lineNo = m_view->MarkerNext( lineNo, MARKERINDEX_TO_MARKERMASK( MARKER_BREAKPOINT ))) != -1 )
		clearBreakpoint( lineNo++, false );

	m_updateBreakpoints = TRUE;
}

void wsCodeWindow::stopDebugging()
{
	m_dbgConn->startCommand( wxString::Format( m_commandAbortTarget, m_sessionHandle.c_str()), GetEventHandler(), RESULT_ID_ABORT_TARGET );
}

void wsCodeWindow::restartDebugging()
{
	wxMessageBox( _( "Restart not yet implemented" ), _( "Not Yet Implemented" ));
}
////////////////////////////////////////////////////////////////////////////////
// OnMarginClick()
// 
//  This event handler is called when the user clicks in the margin to the left
//  of a line of source code. We use the margin to display breakpoint indicators
//  so it makes sense that if you click on an breakpoint indicator, we will clear
//  that breakpoint.  If you click on a spot that does not contain a breakpoint
//  indicator (but it's still in the margin), we create a new breakpoint at that
//  line.

void wsCodeWindow::OnMarginClick( wxStyledTextEvent& event ) 
{
	int lineNumber = m_view->LineFromPosition( event.GetPosition());

	// If we already have a breakpoint at the clickpoint, disable it, otherwise
	// create a new breakpoint.

	if( m_view->MarkerGet( lineNumber ) & MARKERINDEX_TO_MARKERMASK( MARKER_BREAKPOINT ))
		clearBreakpoint( lineNumber, true );
	else
		setBreakpoint( lineNumber );
}

////////////////////////////////////////////////////////////////////////////////
// OnSelectFrame()
//
//  This event handler is called when the user clicks on a frame in the stack-
//  trace window.  We ask the debugger server to switch to that frame, update
//  the breakpoint markers, and send a list of variables that are in-scope in 
//  the selected frame. 
//
//  Note: when the debugger server sees a '^' command, it automatically sends
//        is a "current-statement-location" message and we'll update the source
//        code listing when we receive that message.
//

void wsCodeWindow::OnSelectFrame( wxCommandEvent & event )
{
	m_dbgConn->startCommand( wxString::Format( m_commandSelectFrame, m_sessionHandle.c_str(), event.GetSelection()), GetEventHandler(), RESULT_ID_BREAKPOINT );		
}

void wsCodeWindow::OnVarChange( wxGridEvent & event )
{
	wsVarWindow      *  window;

	if( event.GetId() == ID_PARAMGRID )
		window = getParamWindow( false );
	else if( event.GetId() == ID_VARGRID )
		window = getVarWindow( false );
	else
		window = getPkgVarWindow( false );
			
	wxString varName  = window->getVarName( event.GetRow());
	wxString varValue = window->getVarValue( event.GetRow());

	if( event.GetId() == ID_PKGVARGRID )
		varName.Prepend( wxT( "@" ));

	m_dbgConn->startCommand( wxString::Format( m_commandDepositValue, m_sessionHandle.c_str(), varName.c_str(), -1, varValue.c_str()), GetEventHandler(), RESULT_ID_DEPOSIT_VALUE );		


}

void wsCodeWindow::startGlobalDebugging( )
{
	m_sessionType = SESSION_TYPE_INCONTEXT;

  	m_dbgConn->startCommand( m_commandCreateListener, GetEventHandler(), RESULT_ID_LISTENER_CREATED );
}

////////////////////////////////////////////////////////////////////////////////
// ResultListenerCreated()
//
//	This event handler is called when the result set of an earlier query arrives
//  from the proxy. In this case, the result set is generated by a call to 
//  pldbg_create_listener(). If the query succeeded, our proxy has created a 
//  global listener that's listening for a debugger target.  At this point, the
//  listener is *not* waiting for a target, it's just created the socket.
//  Now we'll issue another query (to the proxy) that will force it to wait
//  for a debugger target.

void wsCodeWindow::ResultListenerCreated( wxCommandEvent & event )
{
	wsResultSet  result((PGresult *)event.GetClientData()); 

	if( connectionLost( result ))
		closeConnection();
	else
	{
		// We now have a global listener and a session handle.
		// Grab the session handle (we'll need it for just about
		// everything else).

		m_sessionHandle = result.getString( 0 );

		// Now create any global breakpoints that the user requested.
		// We start by asking the server to resolve the breakpoint name
		// into an OID (or a pair of OID's if the target is defined in a
		// package).  As each (targetInfo) result arrives, we add a 
		// breakpoint at the resulting OID.

		int	index = 1;

		for( wsBreakpointList::Node * node = m_breakpoints.GetFirst(); node; node = node->GetNext(), ++index )
		{
			wsBreakpoint * breakpoint = node->GetData();

			if( index < m_breakpoints.GetCount())
				addBreakpoint( breakpoint, RESULT_ID_ADD_BREAKPOINT );
			else
				addBreakpoint( breakpoint, RESULT_ID_LAST_BREAKPOINT );
		}	
	}
}

void wsCodeWindow::ResultTargetReady(  wxCommandEvent & event )
{
	wsResultSet  result((PGresult *)event.GetClientData()); 

	if( m_progressBar )
	{
		m_progressBar->Close();
		delete m_progressBar;
		m_progressBar = NULL;
	}

	if( connectionLost( result ))
		closeConnection();
	else
	{
		glApp->getStatusBar()->SetStatusText( wxString::Format( _( "Connected to process %s" ), result.getString( 0 ).c_str()), 2 );
		m_dbgConn->startCommand( wxString::Format( m_commandWaitForBreakpoint, m_sessionHandle.c_str()), GetEventHandler(), RESULT_ID_BREAKPOINT );		
	}
}

void wsCodeWindow::addBreakpoint( wsBreakpoint * breakpoint, wxEventType nextStep )
{
	// The user want's to add a (global) breakpoint on a function, procedure, oid, or trigger
	//
	// First, ask the proxy to resolve the target name into an OID (or two OID's if the 
	// target happens to reside in a package).  When the target info arrives, we'll get a
	// RESULT_ID_ADD_BREAKPOINT message.

	char	targetType;

	switch( breakpoint->getTargetType())
	{
		case wsBreakpoint::FUNCTION:  targetType = 'f'; break;
		case wsBreakpoint::PROCEDURE: targetType = 'p'; break;
		case wsBreakpoint::OID:       targetType = 'o'; break;
		case wsBreakpoint::TRIGGER:   targetType = 't'; break;
	}

	m_dbgConn->startCommand( wxString::Format( m_commandGetTargetInfo, breakpoint->getTargetProcess().c_str(), breakpoint->getTarget().c_str(), targetType ), GetEventHandler(), nextStep );
}

void wsCodeWindow::ResultAddBreakpoint( wxCommandEvent & event )
{
	wsResultSet  result((PGresult *)event.GetClientData()); 

	if( connectionLost( result ))
		closeConnection();
	else if( gotFatalError( result ))
		popupError( result, _( "Error" ));
	else
	{
		m_dbgConn->startCommand( wxString::Format( m_commandAddBreakpoint, m_sessionHandle.c_str(), result.getString( wxT( "pkg" )).c_str(), result.getString( wxT( "target" )).c_str(), wxT( "NULL" ), result.getString( wxT( "pid" )).c_str()), GetEventHandler(), RESULT_ID_NEW_BREAKPOINT );

		if( m_targetName.IsEmpty() == false )
			m_targetName.Append( wxT( ", " ));

		m_targetName.Append( result.getString( wxT( "fqname" )));
	}
}

void wsCodeWindow::ResultLastBreakpoint( wxCommandEvent & event )
{
	wsResultSet  result((PGresult *)event.GetClientData()); 

	if( connectionLost( result ))
		closeConnection();
	else if( gotFatalError( result ))
		popupError( result, _( "Error" ));
	else
	{
		m_dbgConn->startCommand( wxString::Format( m_commandAddBreakpoint, m_sessionHandle.c_str(), result.getString( wxT( "pkg" )).c_str(), result.getString( wxT( "target" )).c_str(), wxT( "NULL" ), result.getString( wxT( "pid" )).c_str()), GetEventHandler(), RESULT_ID_NEW_BREAKPOINT_WAIT );

		if( m_targetName.IsEmpty() == false )
			m_targetName.Append( wxT( ", " ));

		m_targetName.Append( result.getString( wxT( "fqname" )));
	}
}

////////////////////////////////////////////////////////////////////////////////
// getBreakpointList()
//
//	This function returns a non-const reference to our breakpoint list.  The
//  caller typically populates this list before calling startDebugging() - we
//  set a breakpoint for each member of the list

wsBreakpointList & wsCodeWindow::getBreakpointList()
{ 
	return( m_breakpoints ); 
}

void wsCodeWindow::unhilightCurrentLine()
{
	int	lineNo = m_view->MarkerNext( 0, MARKERINDEX_TO_MARKERMASK( MARKER_CURRENT ));

	if( lineNo != -1 )
	{
		m_view->MarkerDelete( lineNo, MARKER_CURRENT );
		m_view->MarkerDelete( lineNo, MARKER_CURRENT_BG );
	}
}


////////////////////////////////////////////////////////////////////////////////
// OnClose()
//
//	wxWidgets invokes this event handler when the user closes the parameter
//	window. We close the connection with server and raise close event for 
//	MainFrame.

void wsCodeWindow::OnClose( wxCloseEvent & event )
{
	// This close event will make the mainFrame close
	glMainFrame->Close();
}

void wsCodeWindow::OnTimer( wxTimerEvent & event )
{
	if( m_progressBar )
	{
		if( m_progress >= 9 )
			m_progress = 0;
		else
			m_progress++;

#if 1
		m_progress = -1;
#endif

		if( m_progressBar->Update( m_progress ) == false )
		{
			m_timer.Stop();
			glMainFrame->Close();
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
// wsCodeCache constructor
//
//	Each entry in our code cache (wsCodeWindow::m_sourceCodeMap) is an object 
//  of class wsCodeCache. 

wsCodeCache::wsCodeCache( const wxString & packageOID, const wxString & funcOID,  const wxString & source )
  : m_packageOID( packageOID ), m_funcOID( funcOID ), m_sourceCode( source )
{
}

