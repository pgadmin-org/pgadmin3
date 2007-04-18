//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// wsPgthread.h - debugger 
//
//////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// class wsPgThread
//
//	A wsPgThread object encapsulates a separate execution thread that interacts
//  with a PostgreSQL server.  We use a separate thread to keep the main thread
//  (and thus the user interface) responsive while we're waiting for the server.
//
//	The startCommand() member function initiates a new command - this function 
//  is called by the UI thread. It stores the command text (and the caller) in 
//  the wsPgThread object and then awakens the thread.  The wsPgThread then sends
//  the command string to the server and waits for a reply. When the reply (a 
//  result set) arrives from the server, the Entry() member function converts
//  the result set (a Pgresult) into a wxEvent (specifically, a wsDBResult) and
//  posts that event to the UI thread's event queue.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef	WSPGTHREADH
#define WSPGTHREADH

#include <wx/thread.h>
#include <wx/string.h>
#include <wx/event.h>
#include <wx/list.h>

class wsPgConn;

class wsPgThreadCommand
{
public:
	wsPgThreadCommand( const wxString &command, wxEvtHandler * caller, wxEventType eventType = wxEVT_NULL ) : m_command( command ), m_caller( caller ), m_eventType( eventType ) {};

	wxString	&getCommand()   { return( m_command ); }
	wxEvtHandler	*getCaller()    { return( m_caller ); }
	wxEventType	getEventType() { return( m_eventType ); }
private:
	wxString	m_command;	// Text of the command we're supposed to execute
	wxEvtHandler	*m_caller;	// Event handler that we post results to
	wxEventType	m_eventType;	// Event to report when result set arrives
};

WX_DECLARE_LIST( wsPgThreadCommand, ThreadCommandList );

class wsPgThread : public wxThread
{

public:
	wsPgThread( wsPgConn & owner );

	virtual void   * Entry();
	void	startCommand( const wxString &command, wxEvtHandler * caller, wxEventType eventType = wxEVT_NULL );

private:

	static void	noticeHandler( void * arg, const char * message );

	wsPgThreadCommand * getNextCommand();	// Grab next command from queue 

	wsPgConn	&m_owner;		// Connection to the PostgreSQL server
	wxSemaphore	m_queueCounter;		// Number of entries in queue (thread synchronizer)
	wxMutex		m_queueMutex;		// Mutex to serialize access to m_commandQueue
	ThreadCommandList	m_commandQueue;		// Queue of pending commands

	wsPgThreadCommand *m_currentCommand;	// Currently executing command
};

#endif
