//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id: ctlCodeWindow.cpp 6216 2007-04-19 11:28:08Z dpage $
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// ctlCodeWindow.cpp - debugger 
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>
#include <wx/colour.h>
#include <wx/tokenzr.h>
#include <wx/progdlg.h>

// App headers
#include "debugger/ctlCodeWindow.h"
#include "debugger/ctlVarWindow.h"
#include "debugger/frmDebugger.h"
#include "debugger/dbgConst.h"
#include "debugger/dbgPgConn.h"
#include "debugger/dbgResultset.h"
#include "debugger/dbgBreakPoint.h"

////////////////////////////////////////////////////////////////////////////////
// NOTES:
//
//    1) In this class, we update the user interface in a lazy fashion. Instead of 
//  updating the variable list, call stack, and breakpoint markers every time
//  the target pauses, we wait for an idle period and then query the proxy for
//  the variable list, call stack, and breakpoint list. That way, the debugger 
//    doesn't bog down when you hit the 'step over' or 'step into' key repeatedly.
//
//    Lazy updating is a multi-step operation.  When the target pauses (which 
//  means that a call to pldbg_continue(), pldbg_step_over(), or pldbg_step_into()
//    has just returned), we update the source code window and then set a flag 
//    (m_updateVars) that tells us to update the variable window during the next
//    idle period.  As soon as OnIdle() is called, we update the variable window,
//    turn off the m_updateVars flag, and turn on the next flag (m_updateStack) 
//  to force the stack window to update itself during the next idle period.  
//    After the stack window updates itself, it turns on m_updateBreakpoints() to
//  force the breakpoint markers to refresh.
//
//    2) This class will issue a number of different queries to the proxy process.
//    Each query executes (one at a time) in a separate thread so that the user
//    interface remains responsive. When we queue up a query, we give an event
//    ID to the queue - when the query completes, the dbgPgThread objects sends us
//  an event that includes the event ID that we provided.
//
//  All of the result-set related event names start with 'RESULT_ID_' and each
//  event is handled by an event handler function whose name starts with 'Result'.
//  For example, when we queue a query to retrieve the source code for a function,
//  we tell dbgPgThread to send us a RESULT_ID_GET_SOURCE event when the query 
//    completes and we handle that event in a function named ResultSource.

IMPLEMENT_CLASS(ctlCodeWindow,  pgFrame)

BEGIN_EVENT_TABLE(ctlCodeWindow , pgFrame)
  EVT_MENU(MENU_ID_TOGGLE_BREAK,            ctlCodeWindow::OnCommand)
  EVT_MENU(MENU_ID_CLEAR_ALL_BREAK,         ctlCodeWindow::OnCommand)

  EVT_MENU(MENU_ID_CONTINUE,                ctlCodeWindow::OnCommand)
  EVT_MENU(MENU_ID_STEP_OVER,               ctlCodeWindow::OnCommand)
  EVT_MENU(MENU_ID_STEP_INTO,               ctlCodeWindow::OnCommand)
  EVT_MENU(MENU_ID_STOP,                    ctlCodeWindow::OnCommand)

  EVT_IDLE(ctlCodeWindow::OnIdle)

  EVT_BUTTON(MENU_ID_NOTICE_RECEIVED,       ctlCodeWindow::OnNoticeReceived)

  EVT_LISTBOX(wxID_ANY,                     ctlCodeWindow::OnSelectFrame)
  EVT_GRID_CELL_CHANGE(                     ctlCodeWindow::OnVarChange)
  EVT_STC_MARGINCLICK(wxID_ANY,             ctlCodeWindow::OnMarginClick)
  EVT_STC_UPDATEUI(wxID_ANY,                ctlCodeWindow::OnPositionStc)

  EVT_MENU(RESULT_ID_ATTACH_TO_PORT,        ctlCodeWindow::ResultPortAttach)
  EVT_MENU(RESULT_ID_BREAKPOINT,            ctlCodeWindow::ResultBreakpoint)
  EVT_MENU(RESULT_ID_GET_VARS,              ctlCodeWindow::ResultVarList)
  EVT_MENU(RESULT_ID_GET_STACK,             ctlCodeWindow::ResultStack)
  EVT_MENU(RESULT_ID_GET_BREAKPOINTS,       ctlCodeWindow::ResultBreakpoints)
  EVT_MENU(RESULT_ID_GET_SOURCE,            ctlCodeWindow::ResultSource)
  EVT_MENU(RESULT_ID_NEW_BREAKPOINT,        ctlCodeWindow::ResultNewBreakpoint)
  EVT_MENU(RESULT_ID_NEW_BREAKPOINT_WAIT,   ctlCodeWindow::ResultNewBreakpointWait)
  EVT_MENU(RESULT_ID_DEL_BREAKPOINT,        ctlCodeWindow::ResultDeletedBreakpoint)
  EVT_MENU(RESULT_ID_DEPOSIT_VALUE,         ctlCodeWindow::ResultDepositValue)
  EVT_MENU(RESULT_ID_ABORT_TARGET,          ctlCodeWindow::ResultAbortTarget)
  EVT_MENU(RESULT_ID_ADD_BREAKPOINT,        ctlCodeWindow::ResultAddBreakpoint)
  EVT_MENU(RESULT_ID_LAST_BREAKPOINT,       ctlCodeWindow::ResultLastBreakpoint)
  EVT_MENU(RESULT_ID_LISTENER_CREATED,      ctlCodeWindow::ResultListenerCreated)
  EVT_MENU(RESULT_ID_TARGET_READY,          ctlCodeWindow::ResultTargetReady)

  EVT_TIMER(wxID_ANY,                       ctlCodeWindow::OnTimer)
END_EVENT_TABLE()

////////////////////////////////////////////////////////////////////////////////
// Static data members 
////////////////////////////////////////////////////////////////////////////////
wxString ctlCodeWindow::m_commandAttach(wxT("SELECT * FROM pldbg_attach_to_port(%s)"));
wxString ctlCodeWindow::m_commandWaitForBreakpoint( wxT( "SELECT * FROM pldbg_wait_for_breakpoint(%s)"));
wxString ctlCodeWindow::m_commandGetVars(wxT("SELECT name, varClass, value, pg_catalog.format_type( dtype, NULL ) as dtype, isconst FROM pldbg_get_variables(%s)"));
wxString ctlCodeWindow::m_commandGetStack(wxT("SELECT targetName, args, linenumber FROM pldbg_get_stack(%s) ORDER BY level"));
wxString ctlCodeWindow::m_commandGetBreakpoints(wxT("SELECT * FROM pldbg_get_breakpoints(%s)"));
wxString ctlCodeWindow::m_commandGetSourceV1(wxT("SELECT %s AS pkg, %s AS func, pldbg_get_source(%s,%s,%s) AS source, targetName, args FROM pldbg_get_stack(%s) ORDER BY level LIMIT 1"));
wxString ctlCodeWindow::m_commandGetSourceV2(wxT("SELECT %s AS func, pldbg_get_source(%s,%s) AS source, targetName, args FROM pldbg_get_stack(%s) ORDER BY level LIMIT 1"));
wxString ctlCodeWindow::m_commandStepOver(wxT("SELECT * FROM pldbg_step_over(%s)"));
wxString ctlCodeWindow::m_commandStepInto(wxT("SELECT * FROM pldbg_step_into(%s)"));
wxString ctlCodeWindow::m_commandContinue(wxT("SELECT * FROM pldbg_continue(%s)"));
wxString ctlCodeWindow::m_commandSetBreakpointV1(wxT("SELECT * FROM pldbg_set_breakpoint(%s,%s,%s,%d)"));
wxString ctlCodeWindow::m_commandSetBreakpointV2(wxT("SELECT * FROM pldbg_set_breakpoint(%s,%s,%d)"));
wxString ctlCodeWindow::m_commandClearBreakpointV1(wxT("SELECT * FROM pldbg_drop_breakpoint(%s,%s,%s,%d)"));
wxString ctlCodeWindow::m_commandClearBreakpointV2(wxT("SELECT * FROM pldbg_drop_breakpoint(%s,%s,%d)"));
wxString ctlCodeWindow::m_commandSelectFrame(wxT("SELECT * FROM pldbg_select_frame(%s,%d)"));
wxString ctlCodeWindow::m_commandDepositValue(wxT("SELECT * FROM pldbg_deposit_value(%s,'%s',%d,'%s')"));               
wxString ctlCodeWindow::m_commandAbortTarget(wxT("SELECT * FROM pldbg_abort_target(%s)"));
wxString ctlCodeWindow::m_commandAddBreakpointEDB(wxT("SELECT * FROM pldbg_set_global_breakpoint(%s, %s, %s, %s, %s)"));
wxString ctlCodeWindow::m_commandAddBreakpointPG(wxT("SELECT * FROM pldbg_set_global_breakpoint(%s, %s, %s, %s)"));
wxString ctlCodeWindow::m_commandGetTargetInfo(wxT("SELECT *, %s as pid FROM pldbg_get_target_info('%s', '%c')"));
wxString ctlCodeWindow::m_commandCreateListener(wxT("SELECT * from pldbg_create_listener()"));
wxString ctlCodeWindow::m_commandWaitForTarget(wxT("SELECT * FROM pldbg_wait_for_target(%s)"));

////////////////////////////////////////////////////////////////////////////////
// ctlCodeWindow constructor
//
//    This class implements the debugger window.  The constructor expects a string
//  that contains a TCP port number - the constructor connects to the debugger
//  server waiting at that port.  
//
//  A ctlCodeWindow object creates (and manages) a toolbar and handles toolbar
//  and keystroke messages. The input messages are treated as debugger commands.
//
//    The m_view member is a ctlSQLBox that displays the code for the PL
//  function that you're debugging.  The m_currentLineNumber member tracks the current 
//  line (that is, the line about to execute).  We use hilight the current line.
//  If m_currentLineNumber is -1, there is no current line.  

ctlCodeWindow::ctlCodeWindow( frmDebugger *parent, wxWindowID id, const dbgConnProp & connProps )
    :pgFrame(NULL, wxEmptyString),
      m_debugPort(connProps.m_debugPort),
      m_parent(parent),
      m_currentLineNumber(-1),
      m_updateVars(false),
      m_updateStack(false),
      m_updateBreakpoints(false),
      m_progressBar(NULL),
      m_timer(this)
{
      m_targetComplete = false;
      m_targetAborted = false;

    wxWindowBase::SetFont(settings->GetSystemFont());

    m_stackWindow = new ctlStackWindow(parent , WINDOW_ID_STACK,  wxDefaultPosition, wxDefaultSize, 0);
    m_tabWindow = new ctlTabWindow(parent , WINDOW_ID_TABS, wxDefaultPosition, wxDefaultSize, 0);
    m_view = new ctlSQLBox(parent, -1);

    // Set up the markers that we use do indicate the current line and a breakpoint
    m_view->MarkerDefine( MARKER_CURRENT, wxSTC_MARK_ARROW , *wxGREEN, *wxGREEN );
    m_view->MarkerDefine( MARKER_CURRENT_BG, wxSTC_MARK_BACKGROUND, *wxGREEN, *wxGREEN );
    m_view->MarkerDefine( MARKER_BREAKPOINT, wxSTC_MARK_CIRCLEPLUS, *wxRED, *wxRED );

    m_view->SetMarginWidth(1, ConvertDialogToPixels(wxPoint(16, 0)).x);

    // Make sure that the text control tells us when the user clicks in the left margin
    m_view->SetMarginSensitive( 0, true );
    m_view->SetMarginSensitive( 1, true );
    m_view->SetMarginSensitive( 2, true );

    // Make sure the user can't edit the source code for this function...
    m_view->SetReadOnly( true );

    // We create a ctlCodeWindow when a dbgPgThread intercepts a PLDBGBREAK NOTICE
    // generated by the PostgreSQL server.   The NOTICE contains a TCP port number
    // and we connect to that port here.
    m_parent->manager.AddPane(m_view, wxAuiPaneInfo().Name(wxT("sourcePane")).Caption(_("Source pane")).Center().CaptionVisible(false).CloseButton(false).MinSize(wxSize(200,100)).BestSize(wxSize(350,200)));
    m_parent->manager.AddPane(m_stackWindow, wxAuiPaneInfo().Name(wxT("stackPane")).Caption(_("Stack pane")).Right().MinSize(wxSize(100,100)).BestSize(wxSize(250,200)));
    m_parent->manager.AddPane(m_tabWindow, wxAuiPaneInfo().Name(wxT("outputPane")).Caption(_("Output pane")).Bottom().MinSize(wxSize(200,100)).BestSize(wxSize(550,300)));

    // Now (re)load the layout
    wxString perspective;
    settings->Read(wxT("Debugger/frmDebugger/Perspective-") + VerFromRev(FRMDEBUGGER_PERPSECTIVE_VER), &perspective, FRMDEBUGGER_DEFAULT_PERSPECTIVE);
    m_parent->manager.LoadPerspective(perspective, true);

    // And reset the captions
    m_parent->manager.GetPane(wxT("sourcePane")).Caption(_("Source pane"));
    m_parent->manager.GetPane(wxT("stackPane")).Caption(_("Stack pane"));
    m_parent->manager.GetPane(wxT("outputPane")).Caption(_("Output pane"));

    // Sync the View menu options
    m_parent->m_viewMenu->Check(MENU_ID_VIEW_STACKPANE, m_parent->manager.GetPane(wxT("stackPane")).IsShown());
    m_parent->m_viewMenu->Check(MENU_ID_VIEW_OUTPUTPANE, m_parent->manager.GetPane(wxT("outputPane")).IsShown());

    // Enable the options for these controls
    m_parent->m_viewMenu->Enable(MENU_ID_VIEW_OUTPUTPANE, true);
    m_parent->m_viewMenu->Enable(MENU_ID_VIEW_STACKPANE,  true);

    m_parent->manager.Update();

    // The wsDbgConn constructor connects to the given host+port and
    // and sends events to us whenever a string arrives from the 
    // debugger server.

    m_parent->getStatusBar()->SetStatusText( _( "Connecting to debugger" ), 1 );
    m_dbgConn = new dbgPgConn(m_parent, connProps, (this != 0 ? true : false) );
    m_parent->getStatusBar()->SetStatusText( _( "Connecting to debugger..." ), 1 );

    // Our proxy API may throw (perfectly legitimate) errors at us (for example,
    // if the target process ends while we are waiting for a breakpoint) - apparently
    // those error messages scare the user when they show up in the log, so we'll
    // just suppress logging for this session

    PQclear( m_dbgConn->waitForCommand( wxT( "SET log_min_messages TO fatal" )));

    m_sessionType = SESSION_TYPE_UNKNOWN;

    // force
    enableTools();
}

void ctlCodeWindow::OnClose(wxCloseEvent& event)
{
    if (event.CanVeto() && !m_targetAborted && !m_targetComplete)
    {
        if (wxMessageBox(_("Are you sure you wish to abort the debugging session?\nThis will abort the function currently being debugged."), _("Close debugger"), wxICON_QUESTION | wxYES_NO) == wxNO)
        {
            event.Veto();
            return;
        }
    }
    // If we're global debugging, we may be waiting for a
    // breakpoint if we're waiting for the function to be
    // called a second, third, fourth etc. time
    if (!m_parent->m_standaloneDirectDbg)
    {
        m_targetAborted = true;
        if (m_dbgConn)
        {
            m_dbgConn->Cancel();
            stopDebugging();
        }
    }
    else
    {
        // If we haven't completed or aborted, stop debugging.
        if (!m_targetComplete && !m_targetAborted)
            stopDebugging();
    }

    // Wait for the abort to complete.
    while (!m_targetComplete && !m_targetAborted)
        wxTheApp->Yield(true);

    closeConnection();
    m_timer.Stop();

    event.Skip();
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
      
void ctlCodeWindow::OnMarginClick( wxStyledTextEvent& event )      
{      
    int lineNumber = m_view->LineFromPosition(event.GetPosition());      
      
    if (!lineNumber)
        return;

    // If we already have a breakpoint at the clickpoint, disable it, otherwise      
    // create a new breakpoint.      
      
    if(m_view->MarkerGet(lineNumber) &MARKERINDEX_TO_MARKERMASK(MARKER_BREAKPOINT))      
        clearBreakpoint(lineNumber, true);      
    else      
        setBreakpoint(lineNumber);      
}

////////////////////////////////////////////////////////////////////////////////      
// OnPositionStc()      
//      
//  Update the current line number etc in the status bar.
      
void ctlCodeWindow::OnPositionStc( wxStyledTextEvent& event )      
{      
    wxString pos; 
    pos.Printf(_("Ln %d Col %d Ch %d"), m_view->LineFromPosition(m_view->GetCurrentPos()), m_view->GetColumn(m_view->GetCurrentPos()) + 1, m_view->GetCurrentPos() + 1);
    m_parent->getStatusBar()->SetStatusText(pos, 2);  
}


////////////////////////////////////////////////////////////////////////////////
// doDebug()
//
//  This function gets things started by asking the proxy process to attach
//    to the given port (m_debugPort).

void ctlCodeWindow::startLocalDebugging()
{
    m_sessionType = SESSION_TYPE_DIRECT;

      m_dbgConn->startCommand( wxString::Format( m_commandAttach, m_debugPort.c_str()), GetEventHandler(), RESULT_ID_ATTACH_TO_PORT );
}

void ctlCodeWindow::resumeLocalDebugging()
{
    // The user has asked to resume local debugging after the previous target finished.
    // We can find ourself in two different states here:
    //
    //    1) The previous target may have run to completion, in which case we are already
    //       waiting for the target to hit a breakpoint
    //
    //    2) The user aborted the previous target, in which case we are NOT waiting for 
    //       a breakpoint and we'd better tell the proxy to wait
    //
    // We use the m_targetAborted flag to figure out which of the two states were are 
    // in. If m_targetAborted is false, our proxy is already waiting for the target 
    // to hit a breakpoint so we don't have to do anything here.  If m_targetAborted
    // is true, the proxy is waiting for another command from us - send it a waitForBreakpoint
    // request

    // Clear the source cache in case someone updated the function in another session
    m_sourceCodeMap.clear();
    m_displayedFuncOid = wxT("-1");
    m_displayedPackageOid = wxT("-1");

    if( m_targetAborted )
    {
        m_targetAborted = false;
        m_dbgConn->startCommand( wxString::Format( m_commandWaitForBreakpoint, m_sessionHandle.c_str()), GetEventHandler(), RESULT_ID_BREAKPOINT );        
    }
}

////////////////////////////////////////////////////////////////////////////////
// OnActivate()
//
//     This event handler is called when this window is activated *or* deactivated.
//     When we become active, we want to display the debugger toolbar.
//     When we become inactive, we want to hide the debugger toolbar.

void ctlCodeWindow::disableTools()
{
    setTools(false);
}

void ctlCodeWindow::enableTools()
{
    setTools(true);
}

void ctlCodeWindow::setTools(bool enable)
{
    // If we don't have a connection to the debugger proxy, disable all
    // of the debugger controls
    bool activateDebug = true;

    if( m_dbgConn == NULL )
        activateDebug = false;

    if( enable == false )
        activateDebug = false;

    wxToolBar *t = m_parent->m_toolBar;
    wxMenu *m = m_parent->m_debugMenu;

    // We may find that our toolbar has disappeared during application shutdown -
    // It seems a little strange that OnActivate() is called during shutdown, but 
    // that does seem to happen on Win32 hosts.

    if (t)
    {
        t->EnableTool( MENU_ID_STEP_INTO,       activateDebug );
        t->EnableTool( MENU_ID_STEP_OVER,       activateDebug );
        t->EnableTool( MENU_ID_CONTINUE,        activateDebug );
        t->EnableTool( MENU_ID_TOGGLE_BREAK,       activateDebug );
        t->EnableTool( MENU_ID_CLEAR_ALL_BREAK, activateDebug );
        t->EnableTool( MENU_ID_STOP,            activateDebug );
    }

    if (m)
    {
        m->Enable( MENU_ID_STEP_INTO,       activateDebug );
        m->Enable( MENU_ID_STEP_OVER,       activateDebug );
        m->Enable( MENU_ID_CONTINUE,        activateDebug );
        m->Enable( MENU_ID_TOGGLE_BREAK,       activateDebug );
        m->Enable( MENU_ID_CLEAR_ALL_BREAK, activateDebug );
        m->Enable( MENU_ID_STOP,            activateDebug );
    }

    /* Activate hook */
    m_parent->Show( true );
}

////////////////////////////////////////////////////////////////////////////////
// OnIdle()
//
//    This event handler is called during 'idle time' (that is, when other events
//  are not waiting in the queue).  We use the idle period to update most parts
//  of the user interface in a lazy fashion.  That way, the debugger doesn't
//  bog down when you hit the 'step over' or 'step into' key repeatedly - we
//  don't bother updating the variable list, call stack, or breakpoint markers
//  until the user pauses for a moment.

void ctlCodeWindow::OnIdle( wxIdleEvent & event )
{
    if (m_targetAborted)
        return;

    if( m_updateVars )
    {    
        m_updateVars = false;
        m_dbgConn->startCommand( wxString::Format( m_commandGetVars, m_sessionHandle.c_str()), GetEventHandler(), RESULT_ID_GET_VARS );
        return;
    }

    if( m_updateStack )
    {
        m_updateStack = false;
        m_dbgConn->startCommand( wxString::Format( m_commandGetStack, m_sessionHandle.c_str()), GetEventHandler(), RESULT_ID_GET_STACK );        
        return;
    }
    
    if( m_updateBreakpoints )
    {
        m_updateBreakpoints = false;
        m_dbgConn->startCommand( wxString::Format( m_commandGetBreakpoints, m_sessionHandle.c_str()), GetEventHandler(), RESULT_ID_GET_BREAKPOINTS );
        return;
    }

}

////////////////////////////////////////////////////////////////////////////////
// ResultPortAttach()
//
//    This event handler is called when the result set of an earlier query arrives
//  from the proxy. In this case, the result set is generated by a call to 
//  pldbg_attach_to_port(). If the query succeeded, our proxy is connected to 
//  the debugger server running inside of the target process. At that point,
//  we have to wait for the target process to wait for a breakpoint so we 
//  queue up another query (a call to pldbg_wait_for_breakpoint()).

void ctlCodeWindow::ResultPortAttach( wxCommandEvent & event )
{
    dbgResultset  result((PGresult *)event.GetClientData()); 

    if( result.getCommandStatus() == PGRES_TUPLES_OK )
    {
        m_parent->getStatusBar()->SetStatusText( _( "Connected to debugger" ), 1 );

        m_sessionHandle = result.getString( 0 );
        
        m_dbgConn->startCommand( wxString::Format( m_commandWaitForBreakpoint, m_sessionHandle.c_str()), GetEventHandler(), RESULT_ID_BREAKPOINT );        
    }
    else
    {
        wxLogError( result.getErrorMessage(), _( "Connection Error" ));
        closeConnection();
    }
}


////////////////////////////////////////////////////////////////////////////////
// ResultBreakpoint()
//
//    This event handler is called when the target process pauses.  The target
//  may have reached a breakpoint or it may have just completed a step/over or
//    step/into.
//
//    In any case, we schedule an update of the user interface for the next idle
//  period by calling updateUI().

void ctlCodeWindow::ResultBreakpoint( wxCommandEvent & event )
{
    dbgResultset  result((PGresult *)event.GetClientData()); 

    if( connectionLost( result ))
        closeConnection();
    else
    {
        if( result.getCommandStatus() == PGRES_TUPLES_OK )
        {
            // Change our focus
            if (result.columnExists(wxT("pkg")))
                m_focusPackageOid = result.getString(wxT( "pkg"));
            else
                m_focusPackageOid = wxT("0");

            m_focusFuncOid    = result.getString(wxT("func"));

            // The result set contains one tuple: 
            //    packageOID, functionOID, linenumber
            m_parent->getStatusBar()->SetStatusText(wxString::Format(_( "Paused at line %d"), atoi(result.getString(wxT("linenumber")).ToAscii()) - 1), 1);    
            updateUI(result);

            /* break point markup line number */
            unhilightCurrentLine();

            int current_line = atoi(result.getString( wxT("linenumber")).ToAscii()) - 1;
            if ( current_line < 0) 
                current_line = 1;
            m_currentLineNumber = current_line;

            m_view->SetAnchor(m_view->PositionFromLine(!current_line ? 0 : current_line -1));
            m_view->SetCurrentPos(m_view->PositionFromLine(!current_line ? 0 : current_line -1));
            m_view->MarkerAdd(current_line -1, MARKER_CURRENT);
            m_view->MarkerAdd(current_line -1, MARKER_CURRENT_BG);
            m_view->EnsureCaretVisible();
            enableTools();

        }
        else if(result.getCommandStatus() == PGRES_FATAL_ERROR)
        {
            if (!m_targetAborted)
            { 
                // We were waiting for a breakpoint (because we just sent a step into, step over, or continue request) and
                // the proxy sent us an error instead.  Presumably, the target process exited before reaching a breakpoint.
                //
                // If we have any global breakpoints, we must have a listener in the proxy... wait for the next target process.

                if( m_breakpoints.GetCount())
                {
                    m_dbgConn->startCommand( wxString::Format(m_commandWaitForTarget, m_sessionHandle.c_str()), GetEventHandler(), RESULT_ID_TARGET_READY);        

                    m_parent->getStatusBar()->SetStatusText(_("Waiting for a target"), 1 );
                    m_parent->getStatusBar()->SetStatusText(wxT(""), 2 );

                    launchWaitingDialog();
                }
            }
        }
    }
}


void ctlCodeWindow::launchWaitingDialog()
{
    m_parent->getStatusBar()->SetStatusText(wxString::Format( _( "Waiting for another session to invoke %s" ), m_targetName.c_str()), 1 );

    // NOTE: the waiting-dialog takes forever to appear running a remote X session so you can disable it by defining the following env. variable
    if( getenv( "SUPPRESS_WAIT_DIALOG" ))
        m_progressBar = NULL;
    else
    {
        m_progressBar = new wxProgressDialog(_( "Waiting for target" ), wxString::Format( _( "Waiting for breakpoint in %s" ), m_targetName.c_str()), 100, m_parent, wxPD_SMOOTH | wxPD_CAN_ABORT | wxPD_ELAPSED_TIME );
        
        m_timer.Start( 100 );    // 10 clock ticks per second
    }
}

////////////////////////////////////////////////////////////////////////////////
// ResultVarList()
//
//    This event handler is called when the proxy finishes sending us a list of 
//  variables (in response to an earlier call to pldbg_get_variables()).
//
//    We extract the variable names, types, and values from the result set and
//    add them to the variable (and parameter) windows.

void ctlCodeWindow::ResultVarList( wxCommandEvent & event )
{
    dbgResultset  result((PGresult *)event.GetClientData()); 

    if( connectionLost( result ))
        closeConnection();
    else
    {
        if( result.getCommandStatus() == PGRES_TUPLES_OK )
        {
            // The result set contains one tuple per variable
            for( int row = 0; row < result.getRowCount(); ++row )
            {
                wxString    varName = result.getString( wxT( "name" ), row );
                char        varClass = result.getString( wxT( "varclass" ), row )[0];

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
        m_updateStack = true;
    }
}

////////////////////////////////////////////////////////////////////////////////
// ResultStack()
//
//    This event handler is called when the proxy finishes sending us a stack
//    trace (in response to an earlier call to pldbg_get_stack()).
//
//    We extract each frame from the result set and add it to the stack window.
//    For each frame, the proxy sends us the function name, line number, and
//  a string that holds the name and value of each argument.

void ctlCodeWindow::ResultStack( wxCommandEvent & event )
{
    dbgResultset  result((PGresult *)event.GetClientData()); 

    if( connectionLost( result ))
        closeConnection();
    else
    {
        if( result.getCommandStatus() == PGRES_TUPLES_OK )
        {
            // The result set contains one tuple per frame:
            //        package, function, linenumber, args
        
            wxArrayString    stack;

            for(int row = 0; row < result.getRowCount(); ++row)
                stack.Add(wxString::Format(wxT( "%s(%s)@%s" ), result.getString(wxT("targetName"), row ).c_str(), result.getString(wxT("args"), row).c_str(), result.getString(wxT("linenumber"), row).c_str()));
        
            getStackWindow()->clear();
            getStackWindow()->setStack( stack );
                                         
        }

        m_updateBreakpoints = true;
    }
}

////////////////////////////////////////////////////////////////////////////////
// ResultBreakpoints()
//
//    This event handler is called when the proxy finishes sending us a list of 
//  breakpoints (in response to an earlier SHOW BREAKPOINTS command).
//
//    We clear out the old breakpoint markers and then display a new marker
//    for each breakpoint defined in the current function.

void ctlCodeWindow::ResultBreakpoints(wxCommandEvent & event)
{
    dbgResultset  result((PGresult *)event.GetClientData()); 

    if( connectionLost(result))
        closeConnection();
    else
    {
        if(result.getCommandStatus() == PGRES_TUPLES_OK)
        {
            clearBreakpointMarkers();

            // The result set contains one tuple per breakpoint:
            //        packageOID, functionOID, linenumber

            for(int row = 0; row < result.getRowCount(); ++row)
            {
                m_view->MarkerAdd(result.getLong(wxT("linenumber"), row) - 1, MARKER_BREAKPOINT);
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// ResultSource()
//
//    This event handler is called when the proxy finishes sending us the source
//    code for a function (in response to an earlier call to pldbg_get_source()).
//
//    We keep a client-side cache of source code listings to avoid an extra 
//  round trip for each step. In this function, we add the source code to 
//  the cache and then display the source code in the source window.

void ctlCodeWindow::ResultSource(wxCommandEvent & event)
{
    dbgResultset  result((PGresult *)event.GetClientData()); 

    if(connectionLost(result))
        closeConnection();
    else
    {
        wxString pkg = wxT("0");
        if (result.columnExists(wxT("pkg")))
            pkg = result.getString(wxT("pkg"));

        cacheSource(pkg, result.getString(wxT("func")), result.getString(wxT("source")), wxString::Format(wxT("%s(%s)"), result.getString(wxT("targetName")).c_str(), result.getString(wxT("args")).c_str()));
        displaySource(pkg, result.getString(wxT("func")));
    }
}

////////////////////////////////////////////////////////////////////////////////
// ResultDeletedBreakpoint()
//
//    This event handler is called when the proxy finishes executing a DROP
//    BREAKPOINT command on our behalf.
//
//    If the DROP BREAKPOINT command succeeded, we display a mesasge in the
//  status bar.

void ctlCodeWindow::ResultDeletedBreakpoint( wxCommandEvent & event )
{
    dbgResultset  result((PGresult *)event.GetClientData()); 

    if( connectionLost( result ))
        closeConnection();
    else
    {
        if( result.getBool( 0 ))
        {
            m_updateBreakpoints = true;
            m_parent->getStatusBar()->SetStatusText( _( "Breakpoint dropped" ), 1 );        
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// ResultNewBreakpoint()
//
//    This event handler is called when the proxy finishes executing a CREATE
//    BREAKPOINT command on our behalf.
//
//    We schedule a refresh of our breakpoint markers for the next idle period.

void ctlCodeWindow::ResultNewBreakpoint( wxCommandEvent & event )
{
    dbgResultset  result((PGresult *)event.GetClientData()); 

    if( connectionLost( result ))
        closeConnection();
    else if( gotFatalError( result ))
        popupError( result, _( "Can't set breakpoint" ));
    else
    {
    }
}

void ctlCodeWindow::ResultNewBreakpointWait( wxCommandEvent & event )
{
    dbgResultset  result((PGresult *)event.GetClientData()); 

    if( connectionLost( result ))
        closeConnection();
    else if( gotFatalError( result ))
        popupError( result, _( "Can't set breakpoint" ));
    else
    {
        setTools(false);
        m_dbgConn->startCommand( wxString::Format( m_commandWaitForTarget, m_sessionHandle.c_str()), GetEventHandler(), RESULT_ID_TARGET_READY );        

        launchWaitingDialog();
    }
}
////////////////////////////////////////////////////////////////////////////////
// ResultDepositValue()
//
//    This event handler is called when the proxy completes a 'deposit' operation
//    (in response to an earlier call to pldbg_deposit_value()).
//
//    We schedule a refresh of our variable window(s) for the next idle period.

void ctlCodeWindow::ResultDepositValue( wxCommandEvent & event )
{
    dbgResultset  result((PGresult *)event.GetClientData()); 

    if( connectionLost( result ))
        closeConnection();
    else
    {
        if( result.getBool( 0 ))
        {
            m_parent->getStatusBar()->SetStatusText( _( "Value changed" ), 1 );        
            m_updateVars = true;
        }
        else
        {
            wxLogError(wxT( "Could not deposit the new value." ));
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// ResultAbortTarget()
//
//    This event handler is called when the proxy completes an 'abort target' 
//  operation (in response to an earlier call to pldbg_abort_target()).
//

void ctlCodeWindow::ResultAbortTarget( wxCommandEvent & event )
{
    dbgResultset  result((PGresult *)event.GetClientData()); 

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

            m_parent->getStatusBar()->SetStatusText( _( "Execution Canceled" ), 1 );        

            // Remove the current-line indicator
            unhilightCurrentLine();

            // And force the toolbar to refresh itself (to disable the debugger-related tools)
            setTools(false);
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
//    This function checks the given result set to determine whether the connection
//  to the debugger proxy is still viable.  For the moment, we assume that any
//  fatal error is... fatal.  We should probably PQstatus() if we get a fatal
//    error.

bool ctlCodeWindow::connectionLost( dbgResultset & resultSet )
{
    if (!m_dbgConn)
        return true;

    if( m_dbgConn->isConnected())
        return false;
    else
        return true;
}

bool ctlCodeWindow::gotFatalError( dbgResultset & resultSet )
{
    if( resultSet.getCommandStatus() == PGRES_FATAL_ERROR )
        return( true );
    else
        return( false );
}

void ctlCodeWindow::popupError( dbgResultset & resultSet, wxString title )
{
    wxMessageBox( resultSet.getErrorMessage(), title );

    // Now close the entire application since we can't continue debugging
    m_timer.Stop();
    m_parent->Close();

}

////////////////////////////////////////////////////////////////////////////////
// closeConnection()
//
//    This member function closes the connection to the debugger and changes the
//  user interface to let the user know what just happened. In particular, we 
//    remove the breakpoint markers (they may be obsolete) and disable the 
//    debugger-related tools on the toolbar.

void ctlCodeWindow::closeConnection()
{
    // Close the debugger (proxy) connection
    if (m_dbgConn)
        m_dbgConn->Close();
    m_dbgConn = NULL;
 
    // Let the user know what happened
    m_parent->getStatusBar()->SetStatusText( _( "Debugger connection terminated (session complete)" ), 1 );        

    // Remove the current-line indicator
    unhilightCurrentLine();

    // And force the toolbar to refresh itself (to disable the debugger-related tools)
    setTools(false);
}

////////////////////////////////////////////////////////////////////////////////
// updateSourceCode()
//
//    This function is invoked whenever the target process pauses (either because
//    it reached a breakpoint or because it just completed a step over/into).  In 
//  this function, we update the source code window. 
//
//    The caller gives us a result set (that had better contain a breakpoint tuple)
//    that gives us the OID of the function that we're paused in.  We search our
//    cache for the source code for that function. If we don't already have the 
//    source code for the function, we send a request for the code to the proxy and
//    (asynchronously) wait for the result set (ResultSource() is called when the
//    result set arrives). If we have the required source code (in the cache), we
//  display the code in the source window (if not already displayed).

void ctlCodeWindow::updateSourceCode(dbgResultset &breakpoint)
{
    wxString packageOID, funcOID, lineNumber;

    if (breakpoint.columnExists(wxT("pkg")))
        packageOID = breakpoint.getString( wxT("pkg"));
    else
        packageOID = wxT("0");

    funcOID = breakpoint.getString(wxT("func"));
    lineNumber = breakpoint.getString(wxT("linenumber"));

    m_currentLineNumber = atoi((const char *)lineNumber.c_str());

    if(!findSourceInCache(packageOID, funcOID))
    {
        getSource(packageOID, funcOID);
    }
    else
    {
        displaySource(packageOID, funcOID);
    }
}

////////////////////////////////////////////////////////////////////////////////
// updateUI()
//
//    This function is called each time the target pauses (either because it reached
//  a breakpoint or because it just completed a step over/into). We update some 
//    parts of the user interface in a lazy fashion so that we remain responsive if
//  the user repeatedly hits the step over/into key.  updateUI() turns off the lazy
//  update flags and the updates the source code window - when the source code 
//  refresh completes, we schedule a variable refresh for the next idle period. 
//    When the variable refresh completes, it schedules a stack refresh...

void ctlCodeWindow::updateUI(dbgResultset & breakpoint)
{
    // Arrange for the lazy parts of our UI to be updated
    // during the next IDLE time
    m_updateVars    = false;
    m_updateStack    = false;
    m_updateBreakpoints = false;

    updateSourceCode(breakpoint);
}

////////////////////////////////////////////////////////////////////////////////
// clearBreakpointMarkers()
//
//    This is a helper function that clears all of the breakpoint markers currently
//    displayed in the source code window (m_view).

void ctlCodeWindow::clearBreakpointMarkers()
{
    int    lineNo = 0;

    while(( lineNo = m_view->MarkerNext( lineNo, MARKERINDEX_TO_MARKERMASK( MARKER_BREAKPOINT ))) != -1 )
        m_view->MarkerDelete( lineNo++, MARKER_BREAKPOINT );
}

////////////////////////////////////////////////////////////////////////////////
// findSourceInCache()
//
//    A ctlCodeWindow can display the source code for many different functions (if
//  you step from one function into another function, the same ctlCodeWindow will
//  display the source code for each function). 
//
//    To avoid constantly re-sending the source code for each function over the
//  network we keep a cache (m_sourceCodeMap) of the source code for each 
//  function that we've seen. The cache is indexed by function OID.  We keep
//  track of the transaction and command ID for each function too so we know
//  when our cached copy becomes stale.
//  
//  This function searches the cache for an entry that matches the given 
//  function ID.  Note that we simply return true or false to indicate 
//  whether the function exists in the cache - to retreive the actual source
//  code for a function, call getSource()

bool ctlCodeWindow::findSourceInCache(const wxString &packageOID, const wxString &funcOID)
{
    sourceHash::iterator match = m_sourceCodeMap.find(funcOID);

    if (match == m_sourceCodeMap.end())
        return(false);
    else
    {
        // FIXME: compare the xid and cid here, throw out out the cached copy (and return false) if they don't match
        return(true);
    }
}

////////////////////////////////////////////////////////////////////////////////
// cacheSource()
//
//    This function adds the source code for a given function to the cache.  See
//  findSourceInCache() for more details.

void ctlCodeWindow::cacheSource(const wxString &packageOID, const wxString &funcOID, const wxString &sourceCode, const wxString &signature)
{
    // Throw out any stale version
    m_sourceCodeMap.erase(funcOID);

    // And add the new version to the cache.
    m_sourceCodeMap[funcOID] = wsCodeCache(packageOID, funcOID, sourceCode, signature);

}

////////////////////////////////////////////////////////////////////////////////
// getSource()
//
//    This function retrieves the source code for a given function from the 
//  PostgreSQL server. We don't actually wait for completionm, we just 
//  schedule the request.

void ctlCodeWindow::getSource(const wxString &packageOID, const wxString &funcOID)
{
    if (m_dbgConn->DebuggerApiVersion() == DEBUGGER_V1_API)
        m_dbgConn->startCommand(wxString::Format(m_commandGetSourceV1, packageOID.c_str(), funcOID.c_str(), m_sessionHandle.c_str(), packageOID.c_str(), funcOID.c_str(), m_sessionHandle.c_str()), GetEventHandler(), RESULT_ID_GET_SOURCE);
    else
        m_dbgConn->startCommand(wxString::Format(m_commandGetSourceV2, funcOID.c_str(), m_sessionHandle.c_str(), funcOID.c_str(), m_sessionHandle.c_str()), GetEventHandler(), RESULT_ID_GET_SOURCE);
}

////////////////////////////////////////////////////////////////////////////////
// displaySource()
//
//    This function loads the source code for the given funcID into the view.
//  If the requested function is already loaded, we just return without doing
//  any extra work.

void ctlCodeWindow::displaySource(const wxString &packageOID, const wxString &funcOID)
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
        
        m_displayedFuncOid       = funcOID;
        m_displayedPackageOid = packageOID;

        wsCodeCache &codeCache = m_sourceCodeMap[funcOID];

        // Now erase any old code and write out the new listing
        m_view->SetReadOnly( false );

        // Strip the leading blank line from the source as it looks ugly
        wxString src = codeCache.getSource();
        src.Replace(wxT("\r"), wxT(""));
 
        if (src.StartsWith(wxT("\n")))
            src = src.AfterFirst('\n');

        m_view->SetText(src);

        m_view->Colourise(0, src.Length());
        m_view->SetReadOnly(true);
    }

    // Clear the current-line indicator 
    int lineNo = m_view->MarkerNext(0, MARKERINDEX_TO_MARKERMASK( MARKER_CURRENT));
    int current_line = m_currentLineNumber;

    if (lineNo != -1)
    {
        m_view->MarkerDelete(lineNo, MARKER_CURRENT);
        m_view->MarkerDelete(lineNo, MARKER_CURRENT_BG);
    }

    // Adjustment of the next position
    if (current_line > 1) 
        current_line--;

    // Add the current-line indicator to the current line of code
    m_view->MarkerAdd(current_line, MARKER_CURRENT);
    m_view->MarkerAdd(current_line, MARKER_CURRENT_BG);

    // Scroll the source code listing (if required) to make sure
    // that this line of code is visible 
    //
    // (note: we set the anchor and the caret to the same position to avoid
    // creating a selection region)
    m_view->SetAnchor(m_view->PositionFromLine(current_line));
    m_view->SetCurrentPos(m_view->PositionFromLine(current_line));
    m_view->EnsureCaretVisible();

    // Update the next lazy part of the user interface (the variable list)
    m_updateVars = true;
}

////////////////////////////////////////////////////////////////////////////////
// getLineNo()
//
//     Figure out which line (in the source code listing) contains the cursor
//    (actually, the insertion point)

int ctlCodeWindow::getLineNo( )
{
    return(m_view->LineFromPosition( m_view->GetCurrentPos( )));
}

////////////////////////////////////////////////////////////////////////////////
// OnCommand()
//
//     This event handler is called when the user clicks a button in the debugger
//    toolbar.

void ctlCodeWindow::OnCommand( wxCommandEvent & event )
{

    switch( event.GetId())
    {
        case MENU_ID_TOGGLE_BREAK:
        {
            // The user wants to set or clear a breakpoint at the line that
            // contains the insertion point (the caret)
            if (isBreakpoint(getLineNo()))
                clearBreakpoint(getLineNo(), true);
            else
                setBreakpoint(getLineNo());
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
            m_parent->getStatusBar()->SetStatusText( _( "Waiting for target (continue)..." ), 1 );
            unhilightCurrentLine();
            disableTools();
            break;
        }

        case MENU_ID_STEP_OVER:
        {
            // The user wants to step-over a function invocation (or
            // just single-step). Unhilite all variables and tell the
            // debugger server to step-over
            m_dbgConn->startCommand( wxString::Format( m_commandStepOver, m_sessionHandle.c_str()), GetEventHandler(), RESULT_ID_BREAKPOINT );        
            m_parent->getStatusBar()->SetStatusText( _( "Waiting for target (step over)..." ), 1 );
            unhilightCurrentLine();
            disableTools();
            break;
        }

        case MENU_ID_STEP_INTO:
        {
            // The user wants to step-into a function invocation (or
            // just single-step). Unhilite all variables and tell the
            // debugger server to step-into
            m_dbgConn->startCommand( wxString::Format( m_commandStepInto, m_sessionHandle.c_str()), GetEventHandler(), RESULT_ID_BREAKPOINT );        
            m_parent->getStatusBar()->SetStatusText( _( "Waiting for target (step into)..." ), 1 );
            unhilightCurrentLine();
            disableTools();
            break;
        }

        case MENU_ID_STOP:
        {
            stopDebugging();
            if (!m_parent->m_standaloneDirectDbg)
                closeConnection();
            unhilightCurrentLine();
            break;
        }

        default:
            break;
    }
}

void ctlCodeWindow::OnNoticeReceived( wxCommandEvent & event )
{   
    getMessageWindow()->AppendText( event.GetString());
    m_tabWindow->selectTab( ID_MSG_PAGE );
}

void ctlCodeWindow::OnResultSet( PGresult * result )
{
    getResultWindow()->fillGrid( result );
}

////////////////////////////////////////////////////////////////////////////////
// setBreakpoint()
//
//    This function creates a breakpoint at the given line number (actually, it
//  sends a request to the debugger server to create the breakpoint - the server
//  will send back a reply if everything worked).

void ctlCodeWindow::setBreakpoint(int lineNumber)
{
    if (m_dbgConn->DebuggerApiVersion() == DEBUGGER_V1_API)
        m_dbgConn->startCommand(wxString::Format(m_commandSetBreakpointV1, m_sessionHandle.c_str(), m_displayedPackageOid.c_str(), m_displayedFuncOid.c_str(), lineNumber + 1), GetEventHandler(), RESULT_ID_NEW_BREAKPOINT);
    else
        m_dbgConn->startCommand(wxString::Format(m_commandSetBreakpointV2, m_sessionHandle.c_str(), m_displayedFuncOid.c_str(), lineNumber + 1), GetEventHandler(), RESULT_ID_NEW_BREAKPOINT);

    m_updateBreakpoints = true;
}

////////////////////////////////////////////////////////////////////////////////
// clearBreakpoint()
//
//    This function clears a breakpoint at the given line number (actually, it
//  sends a request to the debugger server to clear the breakpoint)

void ctlCodeWindow::clearBreakpoint( int lineNumber, bool requestUpdate )
{
    if (m_dbgConn->DebuggerApiVersion() == DEBUGGER_V1_API)
        m_dbgConn->startCommand(wxString::Format(m_commandClearBreakpointV1, m_sessionHandle.c_str(), m_displayedPackageOid.c_str(), m_displayedFuncOid.c_str(), lineNumber + 1), GetEventHandler(), RESULT_ID_NEW_BREAKPOINT);
    else
        m_dbgConn->startCommand(wxString::Format(m_commandClearBreakpointV2, m_sessionHandle.c_str(), m_displayedFuncOid.c_str(), lineNumber + 1), GetEventHandler(), RESULT_ID_NEW_BREAKPOINT);

    if (requestUpdate)
        m_updateBreakpoints = true;   
}

void ctlCodeWindow::clearAllBreakpoints()
{
    int    lineNo = 0;

    if (m_view)
    {
        while(( lineNo = m_view->MarkerNext( lineNo, MARKERINDEX_TO_MARKERMASK( MARKER_BREAKPOINT ))) != -1 )
            clearBreakpoint( lineNo++, false );

        m_updateBreakpoints = true;
    }
}

void ctlCodeWindow::stopDebugging()
{
    m_dbgConn->startCommand( wxString::Format( m_commandAbortTarget, m_sessionHandle.c_str()), GetEventHandler(), RESULT_ID_ABORT_TARGET );
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

void ctlCodeWindow::OnSelectFrame( wxCommandEvent & event )
{
	if( event.GetSelection() != -1 )
	{
        if (!m_targetComplete && !m_targetAborted)
            m_dbgConn->startCommand( wxString::Format( m_commandSelectFrame, m_sessionHandle.c_str(), event.GetSelection()), GetEventHandler(), RESULT_ID_BREAKPOINT );        
    }
}

void ctlCodeWindow::OnVarChange( wxGridEvent & event )
{
    ctlVarWindow      *  window;

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

void ctlCodeWindow::startGlobalDebugging( )
{
    m_sessionType = SESSION_TYPE_INCONTEXT;

    m_dbgConn->startCommand(m_commandCreateListener, GetEventHandler(), RESULT_ID_LISTENER_CREATED);
}

////////////////////////////////////////////////////////////////////////////////
// ResultListenerCreated()
//
//    This event handler is called when the result set of an earlier query arrives
//  from the proxy. In this case, the result set is generated by a call to 
//  pldbg_create_listener(). If the query succeeded, our proxy has created a 
//  global listener that's listening for a debugger target.  At this point, the
//  listener is *not* waiting for a target, it's just created the socket.
//  Now we'll issue another query (to the proxy) that will force it to wait
//  for a debugger target.

void ctlCodeWindow::ResultListenerCreated( wxCommandEvent & event )
{
    dbgResultset  result((PGresult *)event.GetClientData()); 

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

        unsigned int    index = 1;

        for( dbgBreakPointList::Node * node = m_breakpoints.GetFirst(); node; node = node->GetNext(), ++index )
        {
            dbgBreakPoint * breakpoint = node->GetData();

            if( index < m_breakpoints.GetCount())
                addBreakpoint( breakpoint, RESULT_ID_ADD_BREAKPOINT );
            else
                addBreakpoint( breakpoint, RESULT_ID_LAST_BREAKPOINT );
        }    
    }
}

void ctlCodeWindow::ResultTargetReady(  wxCommandEvent & event )
{
    dbgResultset  result((PGresult *)event.GetClientData()); 

    if(m_progressBar)
    {
        m_progressBar->Close();
        delete m_progressBar;
        m_progressBar = NULL;
    }

    if( connectionLost( result ))
        closeConnection();
    else
    {
        setTools(true);
        m_dbgConn->startCommand( wxString::Format( m_commandWaitForBreakpoint, m_sessionHandle.c_str()), GetEventHandler(), RESULT_ID_BREAKPOINT );        
    }
}

void ctlCodeWindow::addBreakpoint( dbgBreakPoint * breakpoint, wxEventType nextStep )
{
    // The user want's to add a (global) breakpoint on a function, procedure, oid, or trigger
    //
    // First, ask the proxy to resolve the target name into an OID (or two OID's if the 
    // target happens to reside in a package).  When the target info arrives, we'll get a
    // RESULT_ID_ADD_BREAKPOINT message.

    char    targetType=0;

    switch( breakpoint->getTargetType())
    {
        case dbgBreakPoint::FUNCTION:  targetType = 'f'; break;
        case dbgBreakPoint::PROCEDURE: targetType = 'p'; break;
        case dbgBreakPoint::OID:       targetType = 'o'; break;
        case dbgBreakPoint::TRIGGER:   targetType = 't'; break;
    }

    m_dbgConn->startCommand( wxString::Format( m_commandGetTargetInfo, breakpoint->getTargetProcess().c_str(), breakpoint->getTarget().c_str(), targetType ), GetEventHandler(), nextStep );
}

void ctlCodeWindow::ResultAddBreakpoint( wxCommandEvent & event )
{
    dbgResultset  result((PGresult *)event.GetClientData()); 

    if( connectionLost( result ))
        closeConnection();
    else if( gotFatalError( result ))
        popupError( result, _( "Error" ));
    else
    {
        if (m_dbgConn->GetIsEdb())
            m_dbgConn->startCommand(wxString::Format(m_commandAddBreakpointEDB, m_sessionHandle.c_str(), result.getString(wxT("pkg")).c_str(), result.getString(wxT("target")).c_str(), wxT("NULL"), result.getString(wxT( "pid")).c_str()), GetEventHandler(), RESULT_ID_NEW_BREAKPOINT);
        else
            m_dbgConn->startCommand(wxString::Format(m_commandAddBreakpointPG, m_sessionHandle.c_str(), result.getString(wxT("target")).c_str(), wxT("NULL"), result.getString(wxT( "pid")).c_str()), GetEventHandler(), RESULT_ID_NEW_BREAKPOINT);

        if( m_targetName.IsEmpty() == false )
            m_targetName.Append( wxT( ", " ));

        m_targetName.Append( result.getString( wxT( "fqname" )));
    }
}

void ctlCodeWindow::ResultLastBreakpoint( wxCommandEvent & event )
{
    dbgResultset  result((PGresult *)event.GetClientData()); 

    if( connectionLost( result ))
        closeConnection();
    else if( gotFatalError( result ))
        popupError( result, _( "Error" ));
    else
    {
        if (m_dbgConn->GetIsEdb())
            m_dbgConn->startCommand(wxString::Format(m_commandAddBreakpointEDB, m_sessionHandle.c_str(), result.getString(wxT("pkg")).c_str(), result.getString(wxT("target")).c_str(), wxT("NULL"), result.getString(wxT("pid")).c_str()), GetEventHandler(), RESULT_ID_NEW_BREAKPOINT_WAIT);
        else
            m_dbgConn->startCommand(wxString::Format(m_commandAddBreakpointPG, m_sessionHandle.c_str(), result.getString(wxT("target")).c_str(), wxT("NULL"), result.getString(wxT("pid")).c_str()), GetEventHandler(), RESULT_ID_NEW_BREAKPOINT_WAIT);

        if( m_targetName.IsEmpty() == false )
            m_targetName.Append( wxT( ", " ));

        m_targetName.Append( result.getString( wxT( "fqname" )));
    }
}

////////////////////////////////////////////////////////////////////////////////
// getBreakpointList()
//
//    This function returns a non-const reference to our breakpoint list.  The
//  caller typically populates this list before calling startDebugging() - we
//  set a breakpoint for each member of the list

dbgBreakPointList & ctlCodeWindow::getBreakpointList()
{ 
    return( m_breakpoints ); 
}

void ctlCodeWindow::unhilightCurrentLine()
{
    
    int    lineNo = m_view->MarkerNext( 0, MARKERINDEX_TO_MARKERMASK( MARKER_CURRENT ));

    if( lineNo != -1 )
    {
        m_view->MarkerDelete( lineNo, MARKER_CURRENT );
        m_view->MarkerDelete( lineNo, MARKER_CURRENT_BG );
    }
    
}

void ctlCodeWindow::OnTimer( wxTimerEvent & event )
{
    if( m_progressBar )
    {
        if( m_progressBar->Pulse() == false )
        {
            closeConnection();
            m_targetAborted = true;
            m_parent->Close();
        }
    }
    else
    {
        // If we're indirect debugging from the query tool, the query thread
        // might be preventing Idle events being fired, so we do it
        // manually here.
        if (!m_parent->m_standaloneDirectDbg)
        {
            wxIdleEvent evt;
            OnIdle(evt);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// wsCodeCache constructor
//
//    Each entry in our code cache (ctlCodeWindow::m_sourceCodeMap) is an object 
//  of class wsCodeCache. 

wsCodeCache::wsCodeCache(const wxString &packageOID, const wxString &funcOID, const wxString &source, const wxString &signature)
  : m_packageOID(packageOID), m_funcOID(funcOID), m_sourceCode(source), m_signature(signature)
{
}

