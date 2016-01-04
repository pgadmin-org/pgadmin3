//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dbgController.h - Debugger controller
//
//////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// class dbgController
//
//    A dbgController object controls the behaviour of the debugger. It stays
//    in the central of the whole debugger mechanism. It controls the flow of
//    execution and also, asks the view to update the user presentation (when
//    needed), also execute commands based on the user inputs.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef DBGCONTROLLER_H
#define DBGCONTROLLER_H

#include <wx/wx.h>
#include <wx/event.h>

#include "db/pgQueryResultEvent.h"
#include "debugger/dbgTargetInfo.h"
#include "debugger/dbgModel.h"

class frmMain;
class frmDebugger;

typedef enum
{
	DBG_SESSION_TYPE_UNKNOWN,   // Session could be in-context or direct
	DBG_SESSION_TYPE_INCONTEXT, // Session is configured for in-context debugging
	DBG_SESSION_TYPE_DIRECT     // Session is configured for direct debugging
} DebuggerSessionType;


typedef enum
{
	DEBUGGER_UNKNOWN_API = 0,
	DEBUGGER_V1_API = 1,
	DEBUGGER_V2_API = 2,
	DEBUGGER_V3_API = 3
} DebuggerApiVersion;


class dbgController : public wxEvtHandler
{
public:
	dbgController(frmMain *_main, pgObject *_obj, bool _directDebugging = false);
	~dbgController();

	dbgTargetInfo *GetTargetInfo();
	dbgModel      *GetModel()
	{
		return m_model;
	}

	bool CanRestart()
	{
		return (m_dbgConn && (m_dbgConn->GetStatus() != PGCONN_OK));
	}

	// Debugging actions (called from the frmDebugger)
	bool Start();
	void ClearBreakpoint(int _lineNo);
	void SetBreakpoint(int _lineNo);
	void Countinue();
	void StepOver();
	void StepInto();
	bool Stop();
	void DepositValue(const wxString &_name, const wxString &_val);
	bool SelectFrame(int _frameNo);
	void UpdateBreakpoints();

	bool HandleQuery(pgBatchQuery *_qry, const wxString &_err);

	// Closing Debugger
	bool CloseDebugger();
	bool ExecuteTarget();
	bool IsTerminated()
	{
		return m_terminated;
	}

	// Event functions
	void OnNoticeReceived(wxCommandEvent &);
	void OnStartDebugging(wxCommandEvent &);

	void ResultTargetComplete(pgQueryResultEvent &);
	void ResultPortAttach(pgQueryResultEvent &);
	void ResultBreakpoint(pgQueryResultEvent &);
	void ResultVarList(pgQueryResultEvent &);
	void ResultStack(pgQueryResultEvent &);
	void ResultBreakpoints(pgQueryResultEvent &);
	void ResultNewBreakpoint(pgQueryResultEvent &);
	void ResultNewBreakpointWait(pgQueryResultEvent &);
	void ResultDeletedBreakpoint(pgQueryResultEvent &);
	void ResultDepositValue(pgQueryResultEvent &);
	void ResultListenerCreated(pgQueryResultEvent &);
	void ResultTargetReady(pgQueryResultEvent &);

private:
	static void NoticeHandler(void *arg, const char *message);

private:
	const static wxString ms_cmdDebugSPLV1;
	const static wxString ms_cmdDebugSPLV2;
	const static wxString ms_cmdDebugPLPGSQLV1;
	const static wxString ms_cmdDebugPLPGSQLV2;
	const static wxString ms_cmdAttachToPort;
	const static wxString ms_cmdWaitForBreakpointV1;
	const static wxString ms_cmdWaitForBreakpointV2;
	const static wxString ms_cmdGetVars;
	const static wxString ms_cmdGetStack;
	const static wxString ms_cmdGetBreakpoints;
	const static wxString ms_cmdStepOverV1;
	const static wxString ms_cmdStepOverV2;
	const static wxString ms_cmdStepIntoV1;
	const static wxString ms_cmdStepIntoV2;
	const static wxString ms_cmdContinueV1;
	const static wxString ms_cmdContinueV2;
	const static wxString ms_cmdSetBreakpointV1;
	const static wxString ms_cmdSetBreakpointV2;
	const static wxString ms_cmdClearBreakpointV1;
	const static wxString ms_cmdClearBreakpointV2;
	const static wxString ms_cmdSelectFrameV1;
	const static wxString ms_cmdSelectFrameV2;
	const static wxString ms_cmdDepositValue;
	const static wxString ms_cmdAbortTarget;
	const static wxString ms_cmdAddBreakpointEDB;
	const static wxString ms_cmdAddBreakpointPG;
	const static wxString ms_cmdGetTargetInfo;
	const static wxString ms_cmdCreateListener;
	const static wxString ms_cmdWaitForTarget;
	const static wxString ms_cmdIsBackendRunning;

private:
	// Debugger Version for the current server
	DebuggerApiVersion  m_ver;

	// Session type
	DebuggerSessionType m_sessionType;

	// Line actually shown from this offset
	int m_lineOffset;

	// Debugging Terminated
	bool                m_terminated;
	bool                m_isStopping;

	// Main Window for the debugger
	frmDebugger        *m_frm;

	// Connection for collecting the current debugging information
	pgConn             *m_dbgConn;
	// Connetion Thread for fetching the debugging information in background
	pgQueryThread      *m_dbgThread;
	// Connection to execute the function/procedure for the direct debugging
	pgConn             *m_execConn;
	// Connection-thread to run the function in background for debugging
	pgQueryThread      *m_execConnThread;
	// Mutex for debugger thread
	wxMutex             m_dbgThreadLock;

	// Debugger Data Model
	dbgModel            *m_model;

	// In-direct Debugging on which target-pid
	wxString             m_currTargetPid;

	DECLARE_EVENT_TABLE()

};

#define MARKERINDEX_TO_MARKERMASK( MI ) ( 1 << MI )

#endif
