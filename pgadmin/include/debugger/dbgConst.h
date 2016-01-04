//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dbgConst.h - debugger
//
//////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
// Constants and enumerator Identifiers for the entire debugger
//
////////////////////////////////////////////////////////////////////////////////

#ifndef DBGCONST_H
#define DBGCONST_H

const int      ID_BTNNEXT     = 1800;
const int      ID_GRDFUNCARGS = 1810;
const int      ID_TXTMESSAGE  = 1820;
const int      ID_TIMER       = 1830;
const int      ID_BTNCANCEL   = 1840;

const int      ID_PARAMGRID   = 1000;
const int      ID_VARGRID     = 1001;
const int      ID_MSG_PAGE    = 1002;
const int      ID_PKGVARGRID  = 1003;

enum
{
	MENU_ID_EXECUTE = 10001,            // Execute command entered by user

	MENU_ID_TOGGLE_BREAK,               // Set/Unset breakpoint
	MENU_ID_CLEAR_ALL_BREAK,            // Clear all breakpoints
	MENU_ID_CONTINUE,                   // Continue
	MENU_ID_STEP_OVER,                  // Step over
	MENU_ID_STEP_INTO,                  // Step into
	MENU_ID_STOP,                       // Stop debugging

	MENU_ID_START_DEBUGGING,            // Spawn a separate debugger process
	MENU_ID_NOTICE_RECEIVED,            // NOTICE received from server
	WINDOW_ID_STACK,                    // Tree-control window
	WINDOW_ID_CONSOLE,                  // Console window
	WINDOW_ID_TABS,                     // Tab window
	WINDOW_ID_BREAKPOINTS,              // Breakpoints window
	WINDOW_ID_RESULT_GRID,              // Results window
	WINDOW_ID_COMMAND,                  // Command window
	SOCKET_ID_DEBUG,                    // Debugger Socket ID

	MENU_ID_VIEW_TOOLBAR,               // View menu options
	MENU_ID_VIEW_STACKPANE,
	MENU_ID_VIEW_OUTPUTPANE,
	MENU_ID_VIEW_DEFAULTVIEW,

	RESULT_ID_ATTACH_TO_PORT,           // Debugger - attach to port completed
	RESULT_ID_BREAKPOINT,               // Debugger - breakpoint reached
	RESULT_ID_GET_VARS,                 // Debugger - variable list complete
	RESULT_ID_GET_STACK,                // Debugger - stack trace complete
	RESULT_ID_GET_BREAKPOINTS,          // Debugger - breakpoint list complete
	RESULT_ID_NEW_BREAKPOINT,           // Debugger - set breakpoint complete
	RESULT_ID_NEW_BREAKPOINT_WAIT,      // Debugger - set breakpoint complete, wait for target progress
	RESULT_ID_DEL_BREAKPOINT,           // Debugger - drop breakpoint complete
	RESULT_ID_DEPOSIT_VALUE,            // Debugger - deposit value complete
	RESULT_ID_ADD_BREAKPOINT,           // Debugger - target info received, now set a breakpoint
	RESULT_ID_LISTENER_CREATED,         // Debugger - global listener created
	RESULT_ID_TARGET_READY,             // Debugger - target session attached
	RESULT_ID_LAST_BREAKPOINT,          // Debugger - last breakpoint created
	RESULT_ID_ARGS_UPDATED,             // Debugger - Values are evaluated and been updated in the arguments
	RESULT_ID_ARGS_UPDATE_ERROR,        // Debugger - Error while evaluateling the value for the arguments

	RESULT_ID_DIRECT_TARGET_COMPLETE,   // DirectDebug - target function complete

	ID_DEBUG_INITIALIZER               // Debugger - debug package initializer? checkbox

};

#endif
