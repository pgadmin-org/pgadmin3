//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dbgController.cpp - debugger controller
// - Flow and data controller for the debugger
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>

#include "ctl/ctlAuiNotebook.h"
#include "db/pgConn.h"
#include "db/pgQueryThread.h"
#include "db/pgQueryResultEvent.h"
#include "schema/pgObject.h"
#include "schema/pgTrigger.h"
#include "debugger/dbgConst.h"
#include "debugger/dbgBreakPoint.h"
#include "debugger/dbgController.h"
#include "debugger/dbgModel.h"
#include "debugger/ctlStackWindow.h"
#include "debugger/ctlMessageWindow.h"
#include "debugger/ctlTabWindow.h"
#include "debugger/frmDebugger.h"
#include "debugger/dlgDirectDbg.h"

#include "utils/pgDefs.h"

#include <stdexcept>

#define LOG_DBG_MUTEX_LOCKING 0

#if LOG_DBG_MUTEX_LOCKING

#define LOCKMUTEX(m) \
	wxPrintf(wxT("Mutex locking @ %s:%d\n"), wxString::FromAscii(__WXFUNCTION__).c_str(), __LINE__); \
	m.Lock(); \
	wxPrintf(wxT("Mutex locked @ %s:%d\n"), wxString::FromAscii(__WXFUNCTION__).c_str(), __LINE__);

#define UNLOCKMUTEX(m) \
	wxPrintf(wxT("Mutex unlocking @ %s:%d\n"), wxString::FromAscii(__WXFUNCTION__).c_str(), __LINE__); \
	m.Unlock(); \
	wxPrintf(wxT("Mutex unlocked @ %s:%d\n"), wxString::FromAscii(__WXFUNCTION__).c_str(), __LINE__);

#else

#define LOCKMUTEX(m) \
	m.Lock();

#define UNLOCKMUTEX(m) \
	m.Unlock();

#endif

const wxString  dbgController::ms_cmdDebugSPLV1(
    wxT("SELECT edb_oid_debug(%lu, %lu)"));
const wxString  dbgController::ms_cmdDebugSPLV2(
    wxT("SELECT edb_oid_debug(%lu)"));

const wxString  dbgController::ms_cmdDebugPLPGSQLV1(
    wxT("SELECT plpgsql_oid_debug(%lu, %lu)"));
const wxString  dbgController::ms_cmdDebugPLPGSQLV2(
    wxT("SELECT plpgsql_oid_debug(%lu)"));

const wxString dbgController::ms_cmdAttachToPort(
    wxT("SELECT * FROM pldbg_attach_to_port(%s)"));
const wxString dbgController::ms_cmdWaitForBreakpointV1(
    wxT("SELECT\n")
    wxT("	p.pkg AS pkg, p.func AS func, p.targetName AS targetName,\n")
    wxT("	p.linenumber AS linenumber, pldbg_get_source($1::INTEGER, p.pkg, p.func) AS src,\n")
    wxT("	(SELECT\n")
    wxT("		s.args\n")
    wxT("	 FROM pldbg_get_stack($1::INTEGER) s\n")
    wxT("	 WHERE s.func = p.func AND s.pkg = p.pkg) AS args\n")
    wxT("FROM pldbg_wait_for_breakpoint($1::INTEGER) p"));
const wxString dbgController::ms_cmdWaitForBreakpointV2(
    wxT("SELECT\n")
    wxT("	p.func AS func, p.targetName AS targetName, \n")
    wxT("	p.linenumber AS linenumber,\n")
    wxT("	pldbg_get_source($1::INTEGER, p.func) AS src,\n")
    wxT("	(SELECT\n")
    wxT("		s.args\n")
    wxT("	 FROM pldbg_get_stack($1::INTEGER) s\n")
    wxT("	 WHERE s.func = p.func) AS args\n")
    wxT("FROM pldbg_wait_for_breakpoint($1::INTEGER) p"));

const wxString dbgController::ms_cmdGetVars(
    wxT("SELECT\n")
    wxT("    name, varClass, value,\n")
    wxT("    pg_catalog.format_type(dtype, NULL) as dtype, isconst\n")
    wxT("FROM pldbg_get_variables(%s) ORDER BY varClass"));
const wxString dbgController::ms_cmdGetStack(
    wxT("SELECT * FROM pldbg_get_stack(%s) ORDER BY level"));
const wxString dbgController::ms_cmdGetBreakpoints(
    wxT("SELECT * FROM pldbg_get_breakpoints(%s)"));

const wxString dbgController::ms_cmdStepOverV1(
    wxT("SELECT\n")
    wxT("	p.pkg AS pkg, p.func AS func, p.targetName AS targetName,\n")
    wxT("	p.linenumber AS linenumber, pldbg_get_source($1::INTEGER, p.pkg, p.func) AS src,\n")
    wxT("	(SELECT\n")
    wxT("		s.args\n")
    wxT("	 FROM pldbg_get_stack($1::INTEGER) s\n")
    wxT("	 WHERE s.func = p.func AND s.pkg = p.pkg) AS args\n")
    wxT("FROM pldbg_step_over($1::INTEGER) p"));
const wxString dbgController::ms_cmdStepOverV2(
    wxT("SELECT\n")
    wxT("	p.func, p.targetName, p.linenumber,\n")
    wxT("	pldbg_get_source($1::INTEGER, p.func) AS src,\n")
    wxT("	(SELECT\n")
    wxT("		s.args\n")
    wxT("	 FROM pldbg_get_stack($1::INTEGER) s\n")
    wxT("	 WHERE s.func = p.func) AS args\n")
    wxT("FROM pldbg_step_over($1::INTEGER) p"));
const wxString dbgController::ms_cmdStepIntoV1(
    wxT("SELECT\n")
    wxT("	p.pkg AS pkg, p.func AS func, p.targetName AS targetName,\n")
    wxT("	p.linenumber AS linenumber, pldbg_get_source($1::INTEGER, p.pkg, p.func) AS src,\n")
    wxT("	(SELECT\n")
    wxT("		s.args\n")
    wxT("	 FROM pldbg_get_stack($1::INTEGER) s\n")
    wxT("	 WHERE s.func = p.func AND s.pkg = p.pkg) AS args\n")
    wxT("FROM pldbg_step_into($1::INTEGER) p"));
const wxString dbgController::ms_cmdStepIntoV2(
    wxT("SELECT\n")
    wxT("	p.func, p.targetName, p.linenumber,\n")
    wxT("	pldbg_get_source($1::INTEGER, p.func) AS src,\n")
    wxT("	(SELECT\n")
    wxT("		s.args\n")
    wxT("	 FROM pldbg_get_stack($1::INTEGER) s\n")
    wxT("	 WHERE s.func = p.func) AS args\n")
    wxT("FROM pldbg_step_into($1::INTEGER) p"));
const wxString dbgController::ms_cmdContinueV1(
    wxT("SELECT\n")
    wxT("	p.pkg AS pkg, p.func AS func, p.targetName AS targetName,\n")
    wxT("	p.linenumber AS linenumber, pldbg_get_source($1::INTEGER, p.pkg, p.func) AS src,\n")
    wxT("	(SELECT\n")
    wxT("		s.args\n")
    wxT("	 FROM pldbg_get_stack($1::INTEGER) s\n")
    wxT("	 WHERE s.func = p.func AND s.pkg = p.pkg) AS args\n")
    wxT("FROM pldbg_continue($1::INTEGER) p"));
const wxString dbgController::ms_cmdContinueV2(
    wxT("SELECT\n")
    wxT("	p.func, p.targetName, p.linenumber,\n")
    wxT("	pldbg_get_source($1::INTEGER, p.func) AS src,\n")
    wxT("	(SELECT\n")
    wxT("		s.args\n")
    wxT("	 FROM pldbg_get_stack($1::INTEGER) s\n")
    wxT("	 WHERE s.func = p.func) AS args\n")
    wxT("FROM pldbg_continue($1::INTEGER) p"));

const wxString dbgController::ms_cmdSetBreakpointV1(
    wxT("SELECT * FROM pldbg_set_breakpoint(%s,%s,%s,%d)"));
const wxString dbgController::ms_cmdSetBreakpointV2(
    wxT("SELECT * FROM pldbg_set_breakpoint(%s,%s,%d)"));

const wxString dbgController::ms_cmdClearBreakpointV1(
    wxT("SELECT * FROM pldbg_drop_breakpoint(%s,%s,%s,%d)"));
const wxString dbgController::ms_cmdClearBreakpointV2(
    wxT("SELECT * FROM pldbg_drop_breakpoint(%s,%s,%d)"));

const wxString dbgController::ms_cmdSelectFrameV1(
    wxT("SELECT\n")
    wxT("	p.pkg AS pkg, p.func AS func, p.targetName AS targetName,\n")
    wxT("	p.linenumber AS linenumber,\n")
    wxT("	CASE WHEN p.func <> 0 THEN pldbg_get_source($1::INTEGER, p.func, p.pkg) ELSE '<No source available>' END AS src,\n")
    wxT("	(SELECT\n")
    wxT("		s.args\n")
    wxT("	 FROM pldbg_get_stack($1::INTEGER) s\n")
    wxT("	 WHERE s.func = p.func AND s.pkg = p.pkg) AS args\n")
    wxT("FROM pldbg_select_frame($1::INTEGER, $2::INTEGER) p"));
const wxString dbgController::ms_cmdSelectFrameV2(
    wxT("SELECT\n")
    wxT("	p.func AS func, p.targetName AS targetName, p.linenumber AS linenumber,\n")
    wxT("	CASE WHEN p.func <> 0 THEN pldbg_get_source($1::INTEGER, p.func) ELSE '<No source available>' END AS src,\n")
    wxT("	(SELECT\n")
    wxT("		s.args\n")
    wxT("	 FROM pldbg_get_stack($1::INTEGER) s\n")
    wxT("	 WHERE s.func = p.func) AS args\n")
    wxT("FROM pldbg_select_frame($1::INTEGER, $2::INTEGER) p"));

const wxString dbgController::ms_cmdDepositValue(
    wxT("SELECT * FROM pldbg_deposit_value(%s,%s,%d,%s)"));
const wxString dbgController::ms_cmdAbortTarget(
    wxT("SELECT * FROM pldbg_abort_target(%s)"));

const wxString dbgController::ms_cmdAddBreakpointEDB(
    wxT("SELECT * FROM pldbg_set_global_breakpoint(%s, %s, %s, -1, %s)"));
const wxString dbgController::ms_cmdAddBreakpointPG(
    wxT("SELECT * FROM pldbg_set_global_breakpoint(%s, %s, -1, %s)"));

const wxString dbgController::ms_cmdCreateListener(
    wxT("SELECT * from pldbg_create_listener()"));
const wxString dbgController::ms_cmdWaitForTarget(
    wxT("SELECT * FROM pldbg_wait_for_target(%s)"));

const wxString dbgController::ms_cmdIsBackendRunning(
    wxT("SELECT COUNT(*) FROM (SELECT pg_catalog.pg_stat_get_backend_idset() AS bid) AS s WHERE pg_catalog.pg_stat_get_backend_pid(s.bid) = '%d'::integer;"));

dbgController::dbgController(frmMain *main, pgObject *_obj, bool _directDebugging)
	: m_ver(DEBUGGER_UNKNOWN_API), m_sessionType(DBG_SESSION_TYPE_UNKNOWN),
	  m_lineOffset(1), m_terminated(false), m_frm(NULL), m_dbgConn(NULL),
	  m_dbgThread(NULL), m_execConnThread(NULL), m_model(NULL),
	  m_isStopping(false)
{
	// Create the connection for listening the debugger port and doing the
	// debugging operations.
	// i.e.
	// - step in, step over, continue, fetch stack-frames, fetch variables, etc.
	m_dbgConn = _obj->GetConnection()->Duplicate(
	                wxT("pgAdmin Debugger - Global Listener"));

	if (!m_dbgConn->IsAlive())
	{
		wxLogError(_("Couldn't create a connection for the debugger"));

		delete m_dbgConn;
		m_dbgConn = NULL;

		throw (std::runtime_error("Couldn't create a connection for the debugger"));
	}

	m_dbgConn->ExecuteVoid(wxT("SET client_min_messages TO error"));

	if (!m_dbgConn->BackendMinimumVersion(9, 1))
		m_lineOffset = 0;

	OID target;
	if (_obj->GetMetaType() == PGM_TRIGGER)
	{
		target = ((pgTrigger *)_obj)->GetFunctionOid();
	}
	else
	{
		target = _obj->GetOid();
	}

	// Fetch the target information
	try
	{
		m_model = new dbgModel((Oid)target, m_dbgConn);
	}
	catch (const std::runtime_error &error)
	{
		// Catching the runtime error thrown by dbgTargetInfo, so that we can
		// delete the created connection object and then rethrow it
		m_dbgConn->Close();
		delete m_dbgConn;
		m_dbgConn = NULL;

		// Rethrow the error
		throw error;
	}

	if (m_model->GetTarget()->HasVariadic() &&
	        m_model->GetTarget()->GetLanguage() == wxT("edbspl"))
	{
		wxLogError(
		    _("An 'edbspl' target with a variadic argument is not supported by this version of pgAdmin debugger!"));

		throw (std::runtime_error(
		           "Unsupported target specified!"));
	}

	// Fetch the debugger version
	wxString strCntProxyInfo = m_dbgConn->ExecuteScalar(
	                               wxT("SELECT COUNT(*) FROM pg_catalog.pg_proc p\n")
	                               wxT("	LEFT JOIN pg_catalog.pg_namespace n ON p.pronamespace = n.oid\n")
	                               wxT("WHERE n.nspname = ANY(current_schemas(false))\n")
	                               wxT("	AND p.proname = 'pldbg_get_proxy_info'"), false);

	if(!strCntProxyInfo.IsEmpty() && m_dbgConn->GetLastResultStatus() == PGRES_TUPLES_OK)
	{
		if (strCntProxyInfo == wxT("0"))
		{
			m_ver = DEBUGGER_V1_API;
		}
		wxString strVer = m_dbgConn->ExecuteScalar(
		                      wxT("SELECT proxyapiver FROM pldbg_get_proxy_info()"), false);

		if (!strVer.IsEmpty() && m_dbgConn->GetLastResultStatus() == PGRES_TUPLES_OK)
		{
			long ver;
			strVer.ToLong(&ver);

			switch (ver)
			{
				case DEBUGGER_V3_API:
				case DEBUGGER_V2_API:
					m_ver = (DebuggerApiVersion)ver;
					break;
				default:
					wxLogError(
					    wxString::Format(
					        _("pgAdmin does not support this version of the debugger plugin (v:%ld)"),
					        ver));

					m_dbgConn->Close();
					delete m_dbgConn;
					m_dbgConn = NULL;

					delete m_model;
					m_model = NULL;

					throw (std::runtime_error(
					           "Unsupported version of debugger plugin installed"));
					break;
			}
		}
		else
		{
			wxLogError(
			    wxString::Format(
			        _("Couldn't determine the debugger plugin version.\n%s"),
			        m_dbgConn->GetLastError().c_str()));

			m_dbgConn->Close();
			delete m_dbgConn;
			m_dbgConn = NULL;

			delete m_model;
			m_model = NULL;

			throw (std::runtime_error(
			           "Couldn't determine the debugger version (function execution error)."));
		}
	}
	else
	{
		wxLogError(
		    wxString::Format(
		        _("Couldn't determine the debugger plugin version.\n%s"),
		        m_dbgConn->GetLastError().c_str()));

		m_dbgConn->Close();
		delete m_dbgConn;
		m_dbgConn = NULL;

		delete m_model;
		m_model = NULL;

		throw (std::runtime_error(
		           "Couldn't determine the debugger version (function check)"));
	}

	// Store debugging type
	if (_directDebugging)
	{
		m_sessionType = DBG_SESSION_TYPE_DIRECT;
		m_frm = new frmDebugger(
		    main, this, wxString::Format(
		        _("Debugger - %s"),
		        m_model->GetTarget()->GetQualifiedName().c_str()));
	}
	else
	{
		m_sessionType = DBG_SESSION_TYPE_INCONTEXT;
		m_frm = new frmDebugger(
		    main, this, _("Global Debugger"));
	}

	if (!Start())
		m_frm->EnableToolsAndMenus(false);
}


dbgController::~dbgController()
{
	if (m_dbgConn)
	{
		m_dbgConn->Close();
		delete m_dbgConn;
		m_dbgConn = NULL;
	}

	if (m_model)
	{
		delete m_model;
		m_model = NULL;
	}
}


bool dbgController::Start()
{
	m_frm->EnableToolsAndMenus(false);
	m_frm->Show(true);

	if(m_frm->GetTabWindow()->GetPageCount()) // Clearing message window, result grid, and re-setting the status message during the module re-exeuction.
	{
		m_frm->GetMessageWindow()->Clear();
		m_frm->GetResultWindow()->ClearGrid();
		m_frm->LaunchWaitingDialog(_("Initializing..."));
	}

	if (!m_dbgConn->IsAlive())
	{
		if (!m_dbgConn->Reconnect())
		{
			// Unable to re-establish the connection for debugger
			return false;
		}
	}

	LOCKMUTEX(m_dbgThreadLock);
	m_dbgThread = new pgQueryThread(
	    m_dbgConn, this, &(dbgController::NoticeHandler), this);
	m_dbgThread->SetEventOnCancellation(false);

	if (m_dbgThread->Create() != wxTHREAD_NO_ERROR)
	{
		delete m_dbgThread;
		m_dbgThread = NULL;
		UNLOCKMUTEX(m_dbgThreadLock);

		return false;
	}

	m_dbgThread->Run();
	UNLOCKMUTEX(m_dbgThreadLock);

	wxTheApp->Yield(true);

	m_terminated = false;
	m_isStopping = false;

	if (m_sessionType == DBG_SESSION_TYPE_DIRECT)
	{
		pgConn *conn = m_dbgConn->Duplicate(
		                   wxT("pgAdmin Debugger - Target Invoker"));

		if (m_model->GetTarget()->RequireUserInput())
		{
			dlgDirectDbg dlg(m_frm, this, conn);

			if (dlg.ShowModal() != wxID_OK)
			{
				// Close the connection for the debugging listener
				m_dbgConn->Close();

				// Close the connection for the debugging executor
				conn->Close();
				delete conn;
				conn = NULL;

				// Declare that execution has been cancelled
				m_terminated = true;
				m_frm->EnableToolsAndMenus(false);
				m_frm->CloseProgressBar();

				return false;
			}
		}

		m_execConnThread = new pgQueryThread(
		    conn, this, &(dbgController::NoticeHandler), this);
		m_execConnThread->SetEventOnCancellation(false);

		if (m_execConnThread->Create() != wxTHREAD_NO_ERROR)
		{
			delete m_execConnThread;
			m_execConnThread = NULL;

			conn->Close();
			delete conn;
			conn = NULL;

			Stop();

			return false;
		}
		return ExecuteTarget();
	}
	else if (m_sessionType == DBG_SESSION_TYPE_INCONTEXT)
	{
		// TODO:: Ask for the target session and different targets
		dbgBreakPointList &breakpoints = m_model->GetBreakPoints();

		WX_CLEAR_ARRAY(breakpoints);

		dbgTargetInfo *target = m_model->GetTarget();
		breakpoints.Append(new dbgBreakPoint(
		                       wxString::Format(wxT("%lu"), target->GetOid()),
		                       wxString::Format(wxT("%lu"), target->GetPkgOid())));

		LOCKMUTEX(m_dbgThreadLock);
		m_dbgThread->AddQuery(ms_cmdCreateListener, NULL, (long)RESULT_ID_LISTENER_CREATED);
		UNLOCKMUTEX(m_dbgThreadLock);
	}

	return true;
}


bool dbgController::ExecuteTarget()
{
	dbgTargetInfo *target = m_model->GetTarget();
	wxString strDebugCmdPkgInitializer, strDebugCmdTarget;

	if (m_ver <= DEBUGGER_V2_API)
	{
		if (target->GetLanguage() == wxT("edbspl"))
		{
			strDebugCmdPkgInitializer = wxString::Format(
			                                ms_cmdDebugSPLV1, target->GetPkgOid(), target->GetPkgInitOid());
			strDebugCmdTarget = wxString::Format(
			                        ms_cmdDebugSPLV1, target->GetPkgOid(), target->GetOid());
		}
		else
		{
			strDebugCmdPkgInitializer = wxString::Format(
			                                ms_cmdDebugPLPGSQLV1, target->GetPkgOid(), target->GetPkgInitOid());
			strDebugCmdTarget = wxString::Format(
			                        ms_cmdDebugPLPGSQLV1, target->GetPkgOid(), target->GetOid());
		}
	}
	else
	{
		if (target->GetLanguage() == wxT("edbspl"))
		{
			strDebugCmdPkgInitializer = wxString::Format(
			                                ms_cmdDebugSPLV2, target->GetPkgInitOid());
			strDebugCmdTarget = wxString::Format(ms_cmdDebugSPLV2, target->GetOid());
		}
		else
		{
			strDebugCmdPkgInitializer = wxString::Format(
			                                ms_cmdDebugPLPGSQLV2, target->GetPkgInitOid());
			strDebugCmdTarget = wxString::Format(ms_cmdDebugPLPGSQLV2, target->GetOid());
		}
	}
	if (target->DebugPackageConstructor())
	{
		m_execConnThread->AddQuery(strDebugCmdPkgInitializer);
	}
	m_execConnThread->AddQuery(strDebugCmdTarget);

	if (target->AddForExecution(m_execConnThread))
	{
		// Start the execution thread
		m_execConnThread->Run();
	}
	else
	{
		wxLogError(_("Couldn't determine how to execute the selected target."));

		Stop();

		return false;
	}
	return true;
}


void dbgController::NoticeHandler(void *_arg, const char *_msg)
{
	dbgController *controller = (dbgController *)_arg;

	if (controller->m_terminated)
		return;

	wxMBConv *conv = controller->m_dbgConn->GetConv();

	wxString strMsg(_msg, *conv);

	if (strMsg.EndsWith(wxT("\n")))
	{
		strMsg.RemoveLast();
	}

	if (strMsg.Find(wxT("PLDBGBREAK")) != wxNOT_FOUND)
	{
		// NOTICE: PLDBGBREAK 1
		wxStringTokenizer tokens(strMsg, wxT(":\n"));

		tokens.GetNextToken(); // NOTICE:
		tokens.GetNextToken(); // PLDBGBREAK

		controller->m_model->GetPort() = tokens.GetNextToken(); // <port number>

		wxCommandEvent btnEvt(wxEVT_COMMAND_BUTTON_CLICKED, MENU_ID_START_DEBUGGING);
		controller->AddPendingEvent(btnEvt);
	}
	else
	{
		wxCommandEvent btnEvt(wxEVT_COMMAND_BUTTON_CLICKED, MENU_ID_NOTICE_RECEIVED);

		btnEvt.SetString(strMsg);
		controller->AddPendingEvent(btnEvt);
	}
}


// Debugging actions (called from the frmDebugger)
void dbgController::ClearBreakpoint(int _lineNo)
{
	if (m_terminated || m_isStopping)
		return;

	if (m_ver <= DEBUGGER_V2_API)
	{
		LOCKMUTEX(m_dbgThreadLock);
		m_dbgThread->AddQuery(
		    wxString::Format(
		        ms_cmdClearBreakpointV1, m_model->GetSession().c_str(),
		        m_model->GetDisplayedPackage().c_str(),
		        m_model->GetDisplayedFunction().c_str(), _lineNo + 1),
		    NULL, (long) RESULT_ID_NEW_BREAKPOINT);
		UNLOCKMUTEX(m_dbgThreadLock);
	}
	else
	{
		LOCKMUTEX(m_dbgThreadLock);
		m_dbgThread->AddQuery(
		    wxString::Format(
		        ms_cmdClearBreakpointV2, m_model->GetSession().c_str(),
		        m_model->GetDisplayedFunction().c_str(), _lineNo + 1),
		    NULL, RESULT_ID_NEW_BREAKPOINT);
		UNLOCKMUTEX(m_dbgThreadLock);
	}
}


void dbgController::SetBreakpoint(int _lineNo)
{
	if (m_terminated || m_isStopping)
		return;

	if (m_ver <= DEBUGGER_V2_API)
	{
		LOCKMUTEX(m_dbgThreadLock);
		m_dbgThread->AddQuery(
		    wxString::Format(
		        ms_cmdSetBreakpointV1, m_model->GetSession().c_str(),
		        m_model->GetDisplayedPackage().c_str(),
		        m_model->GetDisplayedFunction().c_str(), _lineNo + 1),
		    NULL, RESULT_ID_NEW_BREAKPOINT);
		UNLOCKMUTEX(m_dbgThreadLock);
	}
	else
	{
		LOCKMUTEX(m_dbgThreadLock);
		m_dbgThread->AddQuery(
		    wxString::Format(
		        ms_cmdSetBreakpointV2, m_model->GetSession().c_str(),
		        m_model->GetDisplayedFunction().c_str(), _lineNo + 1),
		    NULL, RESULT_ID_NEW_BREAKPOINT);
		UNLOCKMUTEX(m_dbgThreadLock);
	}
}


void dbgController::Countinue()
{
	if (m_terminated || m_isStopping)
		return;

	pgParamsArray *params = new pgParamsArray;
	params->Add(new pgParam(PGOID_TYPE_INT4, &(m_model->GetSession())));

	if (m_ver <= DEBUGGER_V2_API)
	{
		m_dbgThread->AddQuery(ms_cmdContinueV1, params, RESULT_ID_BREAKPOINT);
	}
	else
	{
		m_dbgThread->AddQuery(ms_cmdContinueV2, params, RESULT_ID_BREAKPOINT);
	}

	// Do not allow any action at this time
	m_frm->EnableToolsAndMenus(false);
}


void dbgController::StepOver()
{
	if (m_terminated || m_isStopping)
		return;

	pgParamsArray *params = new pgParamsArray;
	params->Add(new pgParam(PGOID_TYPE_INT4, &(m_model->GetSession())));

	if (m_ver <= DEBUGGER_V2_API)
	{
		LOCKMUTEX(m_dbgThreadLock);
		m_dbgThread->AddQuery(ms_cmdStepOverV1, params, RESULT_ID_BREAKPOINT);
		UNLOCKMUTEX(m_dbgThreadLock);
	}
	else
	{
		LOCKMUTEX(m_dbgThreadLock);
		m_dbgThread->AddQuery(ms_cmdStepOverV2, params, RESULT_ID_BREAKPOINT);
		UNLOCKMUTEX(m_dbgThreadLock);
	}

	// Do not allow any action at this time
	m_frm->EnableToolsAndMenus(false);
}


void dbgController::StepInto()
{
	if (m_terminated || m_isStopping)
		return;

	pgParamsArray *params = new pgParamsArray;
	params->Add(new pgParam(PGOID_TYPE_INT4, &(m_model->GetSession())));

	if (m_ver <= DEBUGGER_V2_API)
	{
		LOCKMUTEX(m_dbgThreadLock);
		m_dbgThread->AddQuery(ms_cmdStepIntoV1, params, RESULT_ID_BREAKPOINT);
		UNLOCKMUTEX(m_dbgThreadLock);
	}
	else
	{
		LOCKMUTEX(m_dbgThreadLock);
		m_dbgThread->AddQuery(ms_cmdStepIntoV2, params, RESULT_ID_BREAKPOINT);
		UNLOCKMUTEX(m_dbgThreadLock);
	}

	// Do not allow any action at this time
	m_frm->EnableToolsAndMenus(false);
}


bool dbgController::Stop()
{
	if (m_terminated)
		return true;

	if (m_isStopping)
		return false;

	LOCKMUTEX(m_dbgThreadLock);

	m_frm->EnableToolsAndMenus(false);
	m_frm->LaunchWaitingDialog(_("Waiting for target stop execution..."));
	m_isStopping = true;

	switch(m_sessionType)
	{
		case DBG_SESSION_TYPE_DIRECT:
		{
			// We must send the abort target command to the executor
			// session
			if (m_execConnThread)
			{
				if (m_execConnThread->IsRunning())
				{
					// Ask the direct debugging executor to not to handle any of the error or
					// result any more.
					m_execConnThread->CancelExecution();
					if (m_dbgConn->GetStatus() != CONNECTION_BAD && m_model->GetSession() != wxEmptyString)
					{
						// And then, we will ask the backend to abort the target, so that the
						// target backend will wait for any commands from debugging proxy.
						pgConn *conn = m_dbgThread->GetConn();
						pgSet *set = conn->ExecuteSet(
						                 wxString::Format(ms_cmdAbortTarget, m_model->GetSession().c_str()));

						if (set)
							delete set;

					}
					while (m_execConnThread && m_execConnThread->IsRunning())
					{
						wxTheApp->Yield(true);
						wxMilliSleep(3);
					}
					if (m_execConnThread)
						m_execConnThread->Wait();
				}
				if (m_execConnThread)
				{
					// We also need to deallocate the momory for the connection
					pgConn *conn = m_execConnThread->GetConn();

					delete m_execConnThread;
					m_execConnThread = NULL;

					conn->Close();
					delete conn;
					conn = NULL;
				}
			}
		}
		break;
		default:
			break;
	}

	if (m_dbgThread)
	{
		if (m_dbgThread->IsRunning())
		{
			m_dbgThread->CancelExecution();
			m_dbgThread->Wait();
		}

		delete m_dbgThread;
		m_dbgThread = NULL;
	}

	// Disconnect the debugger connection
	m_dbgConn->Close();
	m_frm->CloseProgressBar();
	m_terminated = true;
	m_isStopping = false;

	UNLOCKMUTEX(m_dbgThreadLock);

	return true;
}


void dbgController::DepositValue(const wxString &_name, const wxString &_val)
{
	if (m_terminated || m_isStopping)
		return;

	LOCKMUTEX(m_dbgThreadLock);
	m_dbgThread->AddQuery(
	    wxString::Format(
	        ms_cmdDepositValue, m_model->GetSession().c_str(),
	        m_dbgConn->qtDbString(_name).c_str(), -1,
	        m_dbgConn->qtDbString(_val).c_str()),
	    NULL, RESULT_ID_DEPOSIT_VALUE);
	UNLOCKMUTEX(m_dbgThreadLock);
}


bool dbgController::SelectFrame(int _frameNo)
{
	LOCKMUTEX(m_dbgThreadLock);
	if (m_terminated || m_isStopping)
	{
		UNLOCKMUTEX(m_dbgThreadLock);
		return true;
	}

	ctlStackWindow *stackWin = m_frm->GetStackWindow();
	dbgStackFrame *frame = (dbgStackFrame *)stackWin->GetClientObject(_frameNo);

	if (!frame || frame->GetFunction() == wxT("0"))
	{
		UNLOCKMUTEX(m_dbgThreadLock);
		wxMessageBox(
		    _("Cannot fetch information about the anonymous block!"),
		    _("Invalid Stack"), wxICON_ERROR | wxOK);
		return false;
	}

	wxString strLevel = frame->GetLevel();
	pgParamsArray *params = new pgParamsArray;

	params->Add(new pgParam(PGOID_TYPE_INT4, &(m_model->GetSession())));
	params->Add(new pgParam(PGOID_TYPE_INT4, &strLevel));

	if (m_ver <= DEBUGGER_V2_API)
	{
		m_dbgThread->AddQuery(ms_cmdSelectFrameV1, params, RESULT_ID_BREAKPOINT);
	}
	else
	{
		m_dbgThread->AddQuery(ms_cmdSelectFrameV2, params, RESULT_ID_BREAKPOINT);
	}
	UNLOCKMUTEX(m_dbgThreadLock);

	return true;
}

void dbgController::UpdateBreakpoints()
{
	if (m_terminated || m_isStopping)
		return;

	LOCKMUTEX(m_dbgThreadLock);
	m_dbgThread->AddQuery(
	    wxString::Format(ms_cmdGetBreakpoints, m_model->GetSession().c_str()),
	    NULL, RESULT_ID_GET_BREAKPOINTS);
	UNLOCKMUTEX(m_dbgThreadLock);
}


// Closing Debugger
bool dbgController::CloseDebugger()
{
	if (Stop())
	{
		if (m_dbgConn)
		{
			delete m_dbgConn;

			m_dbgConn = NULL;
		}

		return true;
	}
	return false;
}

dbgTargetInfo *dbgController::GetTargetInfo()
{
	return m_model->GetTarget();
}
