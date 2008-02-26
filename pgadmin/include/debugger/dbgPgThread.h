//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id: dbgPgThread.h 6202 2007-04-18 11:22:49Z dpage $
// Copyright (C) 2002 - 2008, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// dbgPgThread.h - debugger 
//
//////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// class dbgPgThread
//
//    A dbgPgThread object encapsulates a separate execution thread that interacts
//  with a PostgreSQL server.  We use a separate thread to keep the main thread
//  (and thus the user interface) responsive while we're waiting for the server.
//
//    The startCommand() member function initiates a new command - this function 
//  is called by the UI thread. It stores the command text (and the caller) in 
//  the dbgPgThread object and then awakens the thread.  The dbgPgThread then sends
//  the command string to the server and waits for a reply. When the reply (a 
//  result set) arrives from the server, the Entry() member function converts
//  the result set (a Pgresult) into a wxEvent (specifically, a dbgDbResult) and
//  posts that event to the UI thread's event queue.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef DBGPGTHREAD_H
#define DBGPGTHREAD_H

// #include "debugger/dbgPgConn.h"
class dbgPgConn;
class dbgPgParams;

class dbgPgThreadCommand
{
public:
    dbgPgThreadCommand( const wxString &command, wxEvtHandler * caller, wxEventType eventType = wxEVT_NULL, dbgPgParams *params = NULL ) : m_command( command ), m_caller( caller ), m_eventType( eventType ), m_params( params ) {};

    wxString    &getCommand()   { return m_command; }
    dbgPgParams *getParams()    { return m_params; }

    wxEvtHandler    *getCaller()    { return( m_caller ); }
    wxEventType    getEventType() { return( m_eventType ); }

private:
    wxString    m_command;    // Text of the command we're supposed to execute
    wxEvtHandler    *m_caller;    // Event handler that we post results to
    wxEventType    m_eventType;    // Event to report when result set arrives
    dbgPgParams *m_params;
};

WX_DECLARE_LIST( dbgPgThreadCommand, ThreadCommandList );

class dbgPgThread : public wxThread
{

public:
    dbgPgThread(dbgPgConn &owner);

    virtual void * Entry();
    void startCommand( const wxString &command, wxEvtHandler * caller, wxEventType eventType = wxEVT_NULL, dbgPgParams *params = NULL );
    void Die();
	
private:

    static void noticeHandler( void * arg, const char * message );

    dbgPgThreadCommand * getNextCommand();    // Grab next command from queue 

    dbgPgConn &m_owner;        // Connection to the PostgreSQL server
    wxSemaphore m_queueCounter;        // Number of entries in queue (thread synchronizer)
    wxMutex m_queueMutex;        // Mutex to serialize access to m_commandQueue
    ThreadCommandList m_commandQueue;        // Queue of pending commands

    dbgPgThreadCommand *m_currentCommand;    // Currently executing command
    wxMBConv *conv;
    long run;
	bool die;
};

#endif
