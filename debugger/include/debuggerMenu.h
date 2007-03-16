//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// debuggerMenu.h - debugger 
//
//////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
// Command and Object Identifiers for the entire workstation application
//
////////////////////////////////////////////////////////////////////////////////

#ifndef MENUH
#define MENUH

enum
{
    MENU_ID_EXECUTE = 101,			// Execute command entered by user
    MENU_ID_OPEN,					// Open a file
    MENU_ID_DEBUG,					// Open the debugger window (obsolete)

    MENU_ID_SET_BREAK,				// Set breakpoint
    MENU_ID_CLEAR_BREAK,			// Clear breakpoint
	MENU_ID_CLEAR_ALL_BREAK,		// Clear all breakpoints
    MENU_ID_SET_WATCH,				// Set watchpoint
    MENU_ID_CLEAR_WATCH,			// Clear watchpoint
    MENU_ID_CONTINUE,				// Continue
    MENU_ID_STEP_OVER,				// Step over	
    MENU_ID_STEP_INTO,				// Step into
    MENU_ID_STOP,					// Stop debugging
    MENU_ID_RESTART,				// Restart debugging with the initially given values
    MENU_ID_SET_PC,					// Set program counter

	MENU_ID_SPAWN_DEBUGGER,			// Spawn a separate debugger process
	MENU_ID_NOTICE_RECEIVED,		// NOTICE received from server
    WINDOW_ID_STACK,				// Tree-control window
    WINDOW_ID_CONSOLE,				// Console window
    WINDOW_ID_TABS,					// Tab window
    WINDOW_ID_BREAKPOINTS,			// Breakpoints window
	WINDOW_ID_RESULT_GRID,			// Results window
	WINDOW_ID_COMMAND,				// Command window
    SOCKET_ID_DEBUG,				// Debugger Socket ID

	RESULT_ID_RESULT_SET_READY,		// Generic - result set received from server

	RESULT_ID_ATTACH_TO_PORT,		// Debugger - attach to port completed
	RESULT_ID_BREAKPOINT,			// Debugger - breakpoint reached
	RESULT_ID_GET_VARS,				// Debugger - variable list complete
	RESULT_ID_GET_STACK,			// Debugger - stack trace complete
	RESULT_ID_GET_BREAKPOINTS,		// Debugger - breakpoint list complete
	RESULT_ID_GET_SOURCE,       	// Debugger - source code listing complete
	RESULT_ID_NEW_BREAKPOINT,		// Debugger - set breakpoint complete
	RESULT_ID_NEW_BREAKPOINT_WAIT,	// Debugger - set breakpoint complete, wait for target progress
	RESULT_ID_DEL_BREAKPOINT,		// Debugger - drop breakpoint complete
	RESULT_ID_DEPOSIT_VALUE,		// Debugger - deposit value complete
	RESULT_ID_ABORT_TARGET,     	// Debugger - abort target (cancel function)
	RESULT_ID_ADD_BREAKPOINT,		// Debugger - target info received, now set a breakpoint
	RESULT_ID_LISTENER_CREATED,		// Debugger - global listener created
	RESULT_ID_TARGET_READY,			// Debugger - target session attached
	RESULT_ID_LAST_BREAKPOINT,		// Debugger - last breakpoint created

	RESULT_ID_DIRECT_TARGET_COMPLETE,	// DirectDebug - target function complete

	ID_DEBUG_INITIALIZER,			// Debugger - debug package initializer? checkbox
};


#endif // MENUH

