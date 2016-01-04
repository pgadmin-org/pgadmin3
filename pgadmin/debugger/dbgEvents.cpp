//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dbgEvents.cpp - debugger controller events
// - This files contains the functions related to the event handling of the
// debugger controller.
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>
#include <wx/tokenzr.h>

#include "ctl/ctlAuiNotebook.h"
#include "db/pgConn.h"
#include "db/pgQueryThread.h"
#include "db/pgQueryResultEvent.h"
#include "schema/pgObject.h"
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

BEGIN_EVENT_TABLE(dbgController, wxEvtHandler)
	EVT_BUTTON(MENU_ID_NOTICE_RECEIVED,                 dbgController::OnNoticeReceived)
	EVT_BUTTON(MENU_ID_START_DEBUGGING,                 dbgController::OnStartDebugging)

	EVT_PGQUERYRESULT(RESULT_ID_DIRECT_TARGET_COMPLETE, dbgController::ResultTargetComplete)
	EVT_PGQUERYRESULT(RESULT_ID_ATTACH_TO_PORT,         dbgController::ResultPortAttach)
	EVT_PGQUERYRESULT(RESULT_ID_LISTENER_CREATED,       dbgController::ResultListenerCreated)

	EVT_PGQUERYRESULT(RESULT_ID_TARGET_READY,           dbgController::ResultTargetReady)

	EVT_PGQUERYRESULT(RESULT_ID_DEL_BREAKPOINT,         dbgController::ResultDeletedBreakpoint)

	EVT_PGQUERYRESULT(RESULT_ID_BREAKPOINT,             dbgController::ResultBreakpoint)
	EVT_PGQUERYRESULT(RESULT_ID_NEW_BREAKPOINT,         dbgController::ResultNewBreakpoint)
	EVT_PGQUERYRESULT(RESULT_ID_NEW_BREAKPOINT_WAIT,    dbgController::ResultNewBreakpointWait)

	EVT_PGQUERYRESULT(RESULT_ID_GET_VARS,               dbgController::ResultVarList)
	EVT_PGQUERYRESULT(RESULT_ID_GET_STACK,              dbgController::ResultStack)
	EVT_PGQUERYRESULT(RESULT_ID_GET_BREAKPOINTS,        dbgController::ResultBreakpoints)

	EVT_PGQUERYRESULT(RESULT_ID_DEPOSIT_VALUE,          dbgController::ResultDepositValue)
END_EVENT_TABLE()


// Event functions
void dbgController::OnNoticeReceived(wxCommandEvent &_ev)
{
	m_frm->GetMessageWindow()->AddMessage(_ev.GetString());
	m_frm->GetTabWindow()->SelectTab(ID_MSG_PAGE);
}


void dbgController::OnStartDebugging(wxCommandEvent &_ev)
{
	LOCKMUTEX(m_dbgThreadLock);
	// Do not bother to process the result, if the debugger thread is not
	// running or not exists
	if (m_dbgThread && m_dbgThread->IsRunning())
	{
		m_dbgThread->AddQuery(
		    wxString::Format(ms_cmdAttachToPort, m_model->GetPort().c_str()),
		    NULL, RESULT_ID_ATTACH_TO_PORT);
	}
	UNLOCKMUTEX(m_dbgThreadLock);
}


void dbgController::ResultTargetComplete(pgQueryResultEvent &_ev)
{
	pgBatchQuery *qry = _ev.GetQuery();

	m_frm->EnableToolsAndMenus(false);
	m_terminated = true;
	m_frm->SetStatusText(_("Debugging Completed"));
	m_frm->CloseProgressBar();

	wxTheApp->Yield(true);

	if (m_execConnThread)
	{
		switch(qry->ReturnCode())
		{
			case pgQueryResultEvent::PGQ_CONN_LOST:
				wxLogInfo(
				    wxString::Format(
				        _("Debugger(%ld): Execution of the debugging target terminated due to connection loss.\n"),
				        m_execConnThread->GetId()));
				break;
			// This is very unlikely, unless we made mistake creating the query
			case pgQueryResultEvent::PGQ_STRING_INVALID:
				wxLogInfo(
				    wxString::Format(
				        _("Debugger(%ld): Execution of the debugging target terminated due to empty query string.\n"),
				        m_execConnThread->GetId()));
				break;
			case pgQueryResultEvent::PGQ_ERROR_PREPARE_CALLABLE:
			case pgQueryResultEvent::PGQ_ERROR_EXECUTE_CALLABLE:
			case pgQueryResultEvent::PGQ_ERROR_SEND_QUERY:
			case pgQueryResultEvent::PGQ_ERROR_CONSUME_INPUT:
				wxLogInfo(
				    wxString::Format(
				        _("Debugger(%ld): Execution of the debugging target terminated due to execution error (%d).\n"),
				        m_execConnThread->GetId(), qry->ReturnCode()));
				break;
			case pgQueryResultEvent::PGQ_EXECUTION_CANCELLED:
				m_frm->SetStatusText(_("Debugging Cancelled"));
				wxLogInfo(
				    wxString::Format(
				        _("Debugger(%ld): Execution of the debugging target cancelled.\n"),
				        m_execConnThread->GetId()));
				break;
			case pgQueryResultEvent::PGQ_RESULT_ERROR:
			{
				wxString strErr = qry->GetErrorMessage();

				m_frm->SetStatusText(_("Execution completed with an error."));
				wxLogInfo(
				    wxString::Format(
				        _("Debugger(%ld): Function/Procedure terminated with an error.\n%s"),
				        m_execConnThread->GetId(), strErr.c_str()));

				m_frm->GetMessageWindow()->AddMessage(qry->GetMessage());
				m_frm->GetMessageWindow()->SetFocus();
			}

			break;
			case PGRES_FATAL_ERROR:
			case PGRES_NONFATAL_ERROR:
			case PGRES_BAD_RESPONSE:
			{
				wxString strErr = qry->GetErrorMessage();
				m_frm->SetStatusText(_("Execution completed with an error."));
				wxLogInfo(
				    wxString::Format(
				        _("Debugger(%ld): Function/Procedure terminated with an error.\n%s"),
				        m_execConnThread->GetId(), strErr.c_str()));

				m_frm->GetMessageWindow()->AddMessage(qry->GetMessage());
				m_frm->GetMessageWindow()->SetFocus();
			}

			break;
			case PGRES_TUPLES_OK:
			{
				m_frm->SetStatusText(_("Execution completed."));
				pgSet *set = qry->ResultSet();
				m_frm->GetMessageWindow()->AddMessage(set->GetCommandStatus());

				m_frm->GetResultWindow()->FillResult(set);
			}

			break;
			default:
				m_frm->SetStatusText(_("Execution completed."));
				wxLogInfo(
				    wxString::Format(
				        _("Debugger(%ld): Execution of the debugging function/procedure completed\n"),
				        m_execConnThread->GetId()));

				m_frm->GetMessageWindow()->AddMessage(qry->GetMessage());
				m_frm->GetMessageWindow()->SetFocus();

				break;
		}

		if (m_execConnThread->IsRunning())
		{
			m_execConnThread->CancelExecution();
			m_execConnThread->Wait();
		}

		pgConn *execConn = m_execConnThread->GetConn();
		delete m_execConnThread;

		m_execConnThread = NULL;
		execConn->Close();
		delete execConn;
	}

	LOCKMUTEX(m_dbgThreadLock);
	pgQueryThread *oldDebugThread = m_dbgThread;
	m_dbgThread = NULL;
	UNLOCKMUTEX(m_dbgThreadLock);

	if (oldDebugThread)
	{
		oldDebugThread->CancelExecution();
		oldDebugThread->Wait();

		delete oldDebugThread;
	}

	// We won't keep the connection open, we will open it only when required
	if (m_dbgConn)
		m_dbgConn->Close();

	if (m_frm)
		m_frm->EnableToolsAndMenus(false);
}


void dbgController::ResultPortAttach(pgQueryResultEvent &_ev)
{
	pgBatchQuery *qry = _ev.GetQuery();

	if (!HandleQuery(qry, _("Error attaching the proxy session.")))
	{
		return;
	}

	if (qry->ReturnCode() != PGRES_TUPLES_OK)
	{
		Stop();

		return;
	}

	m_frm->EnableToolsAndMenus(false);

	wxString strSession = qry->ResultSet()->GetVal(0);
	m_model->GetSession() = strSession;

	pgParamsArray *params = new pgParamsArray;

	params->Add(new pgParam(PGOID_TYPE_INT4, &strSession));

	LOCKMUTEX(m_dbgThreadLock);
	if (m_dbgThread && m_dbgThread->IsRunning())
	{
		if (m_ver <= DEBUGGER_V2_API)
		{
			m_dbgThread->AddQuery(ms_cmdWaitForBreakpointV1, params, RESULT_ID_BREAKPOINT);
		}
		else
		{
			m_dbgThread->AddQuery(ms_cmdWaitForBreakpointV2, params, RESULT_ID_BREAKPOINT);
		}
		qry->Release();

		// Now create any breakpoints that the user created in last execution. We
		// start by asking the server to resolve the breakpoint name into an OID (or,
		// a pair of OID's if the target is defined in a package). As each
		// (targetInof) result arrives, we add a break-point at the resulting OID.
		dbgBreakPointList breakpoints = m_model->GetBreakPoints();

		for (dbgBreakPointList::Node *node = breakpoints.GetFirst(); node;
		        node = node->GetNext())
		{
			dbgBreakPoint *breakpoint = node->GetData();
			/*
			 * In EnterpriseDB versions <= 9.1 the
			 * pldbg_set_global_breakpoint function took five arguments,
			 * the 2nd argument being the package's OID, if any. Starting
			 * with 9.2, the package OID argument is gone, and the function
			 * takes four arguments like the community version has always
			 * done.
			 */
			if (m_dbgConn->GetIsEdb() && !m_dbgConn->BackendMinimumVersion(9, 2))
			{
				m_dbgThread->AddQuery(
				    wxString::Format(
				        ms_cmdSetBreakpointV1, m_model->GetSession().c_str(),
				        breakpoint->GetPackageOid().c_str(),
				        breakpoint->GetFunctionOid().c_str(),
				        breakpoint->GetLineNo()),
				    NULL, RESULT_ID_NEW_BREAKPOINT);
			}
			else
			{
				m_dbgThread->AddQuery(
				    wxString::Format(
				        ms_cmdSetBreakpointV2, m_model->GetSession().c_str(),
				        breakpoint->GetFunctionOid().c_str(),
				        breakpoint->GetLineNo()),
				    NULL, RESULT_ID_NEW_BREAKPOINT);
			}
		}
	}
	UNLOCKMUTEX(m_dbgThreadLock);
}


bool dbgController::HandleQuery(pgBatchQuery *_qry, const wxString &_err)
{
	if (!_qry)
		return false;

	LOCKMUTEX(m_dbgThreadLock);

	// It is possible that we found one error, while running the previous query
	// and, called Stop function from it, because of an error found.
	// That may have released the debugger thread
	//
	// In this case, we should exit this flow gracefully instead showing any
	// error
	if (!m_dbgThread || !m_dbgThread->IsRunning())
	{
		UNLOCKMUTEX(m_dbgThreadLock);
		return false;
	}

	switch(_qry->ReturnCode())
	{
		case pgQueryResultEvent::PGQ_CONN_LOST:
		// This is very unlikely, unless we made mistake creating the query
		case pgQueryResultEvent::PGQ_STRING_INVALID:
		case PGRES_EMPTY_QUERY:
		case pgQueryResultEvent::PGQ_ERROR_PREPARE_CALLABLE:
		case pgQueryResultEvent::PGQ_ERROR_EXECUTE_CALLABLE:
		case pgQueryResultEvent::PGQ_ERROR_SEND_QUERY:
		case pgQueryResultEvent::PGQ_ERROR_CONSUME_INPUT:
		case pgQueryResultEvent::PGQ_RESULT_ERROR:
		case PGRES_BAD_RESPONSE:
		case PGRES_NONFATAL_ERROR:
		case PGRES_FATAL_ERROR:
			break;

		// This is unlikely, we do not generate an event, when execution is
		// cancelled
		case pgQueryResultEvent::PGQ_EXECUTION_CANCELLED:
			break;
		case PGRES_COMMAND_OK:
		case PGRES_TUPLES_OK:
			UNLOCKMUTEX(m_dbgThreadLock);
			return true;
		// Hmm - where did it come from?
		// We never call a function, which results into these results
		// Anyway - we will return true as we have the result
		case PGRES_COPY_IN:
		case PGRES_COPY_OUT:
			UNLOCKMUTEX(m_dbgThreadLock);
			return true;
	}

	m_frm->EnableToolsAndMenus(false);
	wxTheApp->Yield(true);

	if (!m_dbgConn->IsAlive())
	{
		UNLOCKMUTEX(m_dbgThreadLock);
		wxMessageBox(
		    _("Connection to the database server lost, debugging cannot continue!"),
		    _("Connection Lost"), wxICON_ERROR | wxOK);
	}
	else
	{
		wxString strErr;
		if (!_err.IsEmpty())
			strErr = _err + wxT("\n\n");
		strErr += _qry->GetErrorMessage();
		UNLOCKMUTEX(m_dbgThreadLock);

		if (_qry->ReturnCode() == PGRES_FATAL_ERROR)
		{
			// We will start start listening for new in-context session, if the
			// current session is closed. On which, the query/target was
			// running.
			//
			// This allows us to have the same behaviour as the old one.
			//
			if (m_sessionType == DBG_SESSION_TYPE_INCONTEXT && m_currTargetPid != wxT(""))
			{
				// Let's check if the target pid has stopped or exited after
				// successful debugging, let's move on and wait for the next
				// target to hit.
				wxString isTargetRunning = m_dbgConn->ExecuteScalar(wxString::Format(ms_cmdIsBackendRunning, m_currTargetPid.c_str()));

				if (isTargetRunning == wxT("0"))
				{
					// Reset the current backend-pid of the target
					m_currTargetPid = wxT("");
					m_dbgThread->AddQuery(
					    wxString::Format(
					        ms_cmdWaitForTarget, m_model->GetSession().c_str()),
					    NULL, RESULT_ID_TARGET_READY);

					m_frm->LaunchWaitingDialog();

					return false;
				}
			}
			else
			{
				wxMessageBox(_("The calling connection was closed or lost."), _("Connection Lost"), wxICON_ERROR | wxOK);
			}
		}
		else
		{
			wxMessageBox(strErr, _("Execution Error"), wxICON_ERROR | wxOK);
		}

		wxLogQuietError(strErr);
	}

	Stop();
	m_terminated = true;

	m_frm->SetStatusText(_("Debugging aborting..."));

	return false;
}


void dbgController::ResultBreakpoint(pgQueryResultEvent &_ev)
{
	if (m_frm)
		m_frm->CloseProgressBar();

	pgBatchQuery *qry = _ev.GetQuery();

	if (!HandleQuery(qry, wxEmptyString))
		return;

	LOCKMUTEX(m_dbgThreadLock);
	// Do not bother to process the result, if the debugger thread is not
	// running or not exists
	if (m_dbgThread && m_dbgThread->IsRunning())
	{
		if (qry->ReturnCode() == PGRES_TUPLES_OK)
		{
			wxString func,
			         pkg = wxT("0");

			pgSet *set = qry->ResultSet();

			if (set->HasColumn(wxT("pkg")))
			{
				pkg = set->GetVal(wxT("pkg"));
			}
			func = set->GetVal(wxT("func"));

			m_model->GetFocusedPackage() = pkg;
			m_model->GetFocusedFunction() = func;

			int lineNo = (int)(set->GetLong(wxT("linenumber")) - 1);

			if (lineNo < 0)
				lineNo = -1;
			else
				lineNo -= m_lineOffset;
			m_model->GetCurrLineNo() = lineNo;

			wxString strSource = set->GetVal(wxT("src"));

			if (strSource.IsEmpty())
				strSource = _("<source not available>");

			dbgCachedStack src(pkg, func, set->GetVal(wxT("targetname")),
			                   set->GetVal(wxT("args")), strSource);

			m_model->AddSource(func, src);
			m_frm->DisplaySource(src);

			m_dbgThread->AddQuery(
			    wxString::Format(ms_cmdGetStack, m_model->GetSession().c_str()),
			    NULL, RESULT_ID_GET_STACK);

			m_frm->EnableToolsAndMenus(true);
		}

		// Release the result-set
		qry->Release();
	}
	UNLOCKMUTEX(m_dbgThreadLock);
}


void dbgController::ResultVarList(pgQueryResultEvent &_ev)
{
	pgBatchQuery *qry = _ev.GetQuery();

	if (!HandleQuery(qry, _("Error fetching variables.")))
		return;

	LOCKMUTEX(m_dbgThreadLock);
	// Do not bother to process the result, if the debugger thread is not
	// running or not exists
	if (m_dbgThread && m_dbgThread->IsRunning())
	{
		if (qry->ReturnCode() == PGRES_TUPLES_OK)
		{
			ctlVarWindow *paramWin = NULL, *pkgVarWin = NULL, *varWin = NULL;
			pgSet *set = qry->ResultSet();

			while (!set->Eof())
			{
				switch((char)(set->GetVal(wxT("varclass"))[0]))
				{
					case 'A':
					{
						if (paramWin == NULL)
							paramWin = m_frm->GetParamWindow(true);

						paramWin->AddVar(
						    set->GetVal(wxT("name")),
						    set->GetVal(wxT("value")),
						    set->GetVal(wxT("dtype")),
						    set->GetBool(wxT("isconst")));

						break;
					}
					case 'P':
					{
						if (pkgVarWin == NULL)
							pkgVarWin = m_frm->GetPkgVarWindow(true);

						pkgVarWin->AddVar(
						    set->GetVal(wxT("name")),
						    set->GetVal(wxT("value")),
						    set->GetVal(wxT("dtype")),
						    set->GetBool(wxT("isconst")));

						break;
					}
					default:
					{
						if (varWin == NULL)
							varWin = m_frm->GetVarWindow(true);

						varWin->AddVar(
						    set->GetVal(wxT("name")),
						    set->GetVal(wxT("value")),
						    set->GetVal(wxT("dtype")),
						    set->GetBool(wxT("isconst")));

						break;
					}
				}
				set->MoveNext();
			}
		}

		// Release the result-set
		qry->Release();
	}
	UNLOCKMUTEX(m_dbgThreadLock);
}


void dbgController::ResultStack(pgQueryResultEvent &_ev)
{
	pgBatchQuery *qry = _ev.GetQuery();

	if (!HandleQuery(qry, _("Error fetching the call stack.")))
		return;

	LOCKMUTEX(m_dbgThreadLock);
	// Do not bother to process the result, if the debugger thread is not
	// running or not exists
	if (m_dbgThread && m_dbgThread->IsRunning())
	{
		if (qry->ReturnCode() == PGRES_TUPLES_OK)
		{
			pgSet *set = qry->ResultSet();

			dbgStackFrameList stacks;
			ctlStackWindow    *stackWin = m_frm->GetStackWindow();

			// Fetched the stack, now fetch the break-points
			m_dbgThread->AddQuery(
			    wxString::Format(ms_cmdGetBreakpoints, m_model->GetSession().c_str()),
			    NULL, RESULT_ID_GET_BREAKPOINTS);

			stackWin->ClearStack();

			int selected = 0,
			    levelCol = set->ColNumber(wxT("level")),
			    pkgCol = set->HasColumn(wxT("pkg")) ? set->ColNumber(wxT("level")) : -1,
			    funCol = set->ColNumber(wxT("func")),
			    targetCol = set->ColNumber(wxT("targetname")),
			    argsCol = set->ColNumber(wxT("args")),
			    lineCol = set->ColNumber(wxT("linenumber"));

			while (!set->Eof())
			{
				// The result set contains one tuple per frame:
				//        package, function, linenumber, args
				dbgStackFrame *frame = new dbgStackFrame(
				    set->GetVal(levelCol),
				    pkgCol != -1 ? set->GetVal(wxT("pkg")) : wxT("0"),
				    set->GetVal(funCol),
				    wxString::Format(
				        wxT("%s(%s)@%s"),
				        set->GetVal(targetCol).c_str(),
				        set->GetVal(argsCol).c_str(),
				        set->GetVal(lineCol).c_str()));

				// Select this one in the stack window
				if (frame->GetFunction() == m_model->GetDisplayedFunction() &&
				        frame->GetPackage() == m_model->GetDisplayedPackage())
				{
					selected = set->CurrentPos() - 1;
				}

				stacks.Append(frame);
				set->MoveNext();
			}
			stackWin->SetStack(stacks, selected);
		}

		// Release the result-set
		qry->Release();
	}
	UNLOCKMUTEX(m_dbgThreadLock);
}


void dbgController::ResultBreakpoints(pgQueryResultEvent &_ev)
{
	pgBatchQuery *qry = _ev.GetQuery();

	if (!HandleQuery(qry, _("Error fetching breakpoints.")))
		return;

	LOCKMUTEX(m_dbgThreadLock);
	// Do not bother to process the result, if the debugger thread is not
	// running or not exists
	if (m_dbgThread && m_dbgThread->IsRunning())
	{
		if (qry->ReturnCode() == PGRES_TUPLES_OK)
		{
			pgSet *set = qry->ResultSet();

			int pkgCol = -1, funcCol = set->ColNumber(wxT("func")),
			    lineCol = set->ColNumber(wxT("linenumber"));

			if (set->HasColumn(wxT("pkg")))
			{
				pkgCol = set->ColNumber(wxT("pkg"));
			}

			m_dbgThread->AddQuery(
			    wxString::Format(ms_cmdGetVars, m_model->GetSession().c_str()),
			    NULL, RESULT_ID_GET_VARS);

			m_frm->ClearBreakpointMarkers();
			dbgBreakPointList &breakpoints = m_model->GetBreakPoints();
			WX_CLEAR_ARRAY(breakpoints);

			while (!set->Eof())
			{
				// The result set contains one tuple per breakpoint:
				//   pkg, func, linenumber, target
				//   or,
				//   func, linenumber, target
				wxString pkg = (pkgCol == -1) ? wxT("0") : set->GetVal(pkgCol);
				wxString func = set->GetVal(funcCol);
				int lineNumber = (int)set->GetLong(lineCol);

				// Save this break-points in break-point list
				breakpoints.Append(new dbgBreakPoint(func, pkg, lineNumber));

				// Mark the break-point in the viewer
				if (pkg == m_model->GetDisplayedPackage() &&
				        func == m_model->GetDisplayedFunction())
				{
					m_frm->MarkBreakpoint(lineNumber - 1);
				}
				set->MoveNext();
			}
		}

		// Release the result-set
		qry->Release();
	}
	UNLOCKMUTEX(m_dbgThreadLock);
}


void dbgController::ResultNewBreakpoint(pgQueryResultEvent &_ev)
{
	// We will wait for the last new break-point, which will be handled in
	// dbgController::ResultNewBreakpointWait function
	if (!HandleQuery(_ev.GetQuery(), _("Could not create the breakpoint.")))
		return;

	LOCKMUTEX(m_dbgThreadLock);
	// Do not bother to process the result, if the debugger thread is not
	// running or not exists
	if (m_dbgThread && m_dbgThread->IsRunning())
	{
		// Release the result-set
		_ev.GetQuery()->Release();
	}
	UNLOCKMUTEX(m_dbgThreadLock);
}


void dbgController::ResultNewBreakpointWait(pgQueryResultEvent &_ev)
{
	pgBatchQuery *qry = _ev.GetQuery();

	if (!HandleQuery(qry, _("Could not create the breakpoint.")))
		return;

	LOCKMUTEX(m_dbgThreadLock);
	// Do not bother to process the result, if the debugger thread is not
	// running or not exists
	if (m_dbgThread && m_dbgThread->IsRunning())
	{
		if (qry->ReturnCode() == PGRES_TUPLES_OK)
		{
			pgSet *set = _ev.GetQuery()->ResultSet();
			m_frm->EnableToolsAndMenus(false);

			m_dbgThread->AddQuery(
			    wxString::Format(
			        ms_cmdWaitForTarget, m_model->GetSession().c_str()),
			    NULL, RESULT_ID_TARGET_READY);

			m_frm->LaunchWaitingDialog();
		}

		// Release the result-set
		qry->Release();
	}
	UNLOCKMUTEX(m_dbgThreadLock);
}


void dbgController::ResultDeletedBreakpoint(pgQueryResultEvent &_ev)
{
	pgBatchQuery *qry = _ev.GetQuery();

	if (!HandleQuery(qry, _("Could not drop the breakpoint.")))
		return;

	LOCKMUTEX(m_dbgThreadLock);
	// Do not bother to process the result, if the debugger thread is not
	// running or not exists
	if (m_dbgThread && m_dbgThread->IsRunning())
	{
		if (qry->ReturnCode() == PGRES_TUPLES_OK)
		{
			pgSet *set = qry->ResultSet();

			if (set->GetBool(0))
			{
				m_frm->SetStatusText(_("Breakpoint dropped"));
			}
		}

		// Release the result-set
		qry->Release();
	}
	UNLOCKMUTEX(m_dbgThreadLock);
}


////////////////////////////////////////////////////////////////////////////////
// ResultDepositValue()
//
//    This event handler is called when the proxy completes a 'deposit' operation
//    (in response to an earlier call to pldbg_deposit_value()).
//
//    We schedule a refresh of our variable window(s) for the next idle period.
//
void dbgController::ResultDepositValue(pgQueryResultEvent &_ev)
{
	pgBatchQuery *qry = _ev.GetQuery();

	if (!HandleQuery(qry, _("Could not deposit the new value.")))
		return;

	LOCKMUTEX(m_dbgThreadLock);
	// Do not bother to process the result, if the debugger thread is not
	// running or not exists
	if (m_dbgThread && m_dbgThread->IsRunning())
	{
		if (qry->ReturnCode() == PGRES_TUPLES_OK)
		{
			pgSet *set = qry->ResultSet();

			if (set->GetBool(0))
			{
				m_frm->SetStatusText(_("Value changed"));

				m_dbgThread->AddQuery(
				    wxString::Format(ms_cmdGetVars, m_model->GetSession().c_str()),
				    NULL, RESULT_ID_GET_VARS);
			}
			else
			{
				wxLogError(_("Could not deposit the new value."));
			}
		}

		// Release the result-set
		qry->Release();
	}
	UNLOCKMUTEX(m_dbgThreadLock);
}


void dbgController::ResultListenerCreated(pgQueryResultEvent &_ev)
{
	pgBatchQuery *qry = _ev.GetQuery();

	if (!HandleQuery(qry,
	                 _("Could not create the proxy listener for in-process debugging.")))
		return;

	LOCKMUTEX(m_dbgThreadLock);
	// Do not bother to process the result, if the debugger thread is not
	// running or not exists
	if (m_dbgThread && m_dbgThread->IsRunning())
	{
		if (qry->ReturnCode() == PGRES_TUPLES_OK)
		{
			pgSet *set = qry->ResultSet();

			// We now have a global listener and a session handle. Grab the session
			// handle (we'll need it for just about everything else).
			m_model->GetSession() = set->GetVal(0);

			// Release the result-set
			qry->Release();

			// Now create any global breakpoints that the user requested. We start
			// by asking the server to resolve the breakpoint name into an OID (or,
			// a pair of OID's if the target is defined in a package). As each
			// (targetInof) result arrives, we add a break-point at the resulting
			// OID.
			unsigned int idx = 1;
			long resultId;

			dbgBreakPointList breakpoints = m_model->GetBreakPoints();
			for (dbgBreakPointList::Node *node = breakpoints.GetFirst(); node;
			        node = node->GetNext(), ++idx)
			{
				dbgBreakPoint *breakpoint = node->GetData();

				if(idx < breakpoints.GetCount())
				{
					resultId = RESULT_ID_NEW_BREAKPOINT;
				}
				else
				{
					resultId = RESULT_ID_NEW_BREAKPOINT_WAIT;
				}

				/*
				 * In EnterpriseDB versions <= 9.1 the
				 * pldbg_set_global_breakpoint function took five arguments,
				 * the 2nd argument being the package's OID, if any. Starting
				 * with 9.2, the package OID argument is gone, and the function
				 * takes four arguments like the community version has always
				 * done.
				 */
				if (m_dbgConn->GetIsEdb() && !m_dbgConn->BackendMinimumVersion(9, 2))
				{
					m_dbgThread->AddQuery(
					    wxString::Format(
					        ms_cmdAddBreakpointEDB, m_model->GetSession().c_str(),
					        breakpoint->GetPackageOid().c_str(),
					        breakpoint->GetFunctionOid().c_str(),
					        m_model->GetTargetPid().c_str()),
					    NULL, resultId);
				}
				else
				{
					m_dbgThread->AddQuery(
					    wxString::Format(
					        ms_cmdAddBreakpointPG, m_model->GetSession().c_str(),
					        breakpoint->GetFunctionOid().c_str(),
					        m_model->GetTargetPid().c_str()),
					    NULL, resultId);
				}
			}
			UNLOCKMUTEX(m_dbgThreadLock);
		}
		else
		{
			// Release the result-set
			qry->Release();
			UNLOCKMUTEX(m_dbgThreadLock);
		}
	}
	else
	{
		UNLOCKMUTEX(m_dbgThreadLock);
	}
}


void dbgController::ResultTargetReady(pgQueryResultEvent &_ev)
{
	pgBatchQuery *qry = _ev.GetQuery();

	if (m_frm)
	{
		m_frm->CloseProgressBar();
		m_frm->EnableToolsAndMenus(false);
	}

	if (!HandleQuery(qry,
	                 _("Error fetching target information.")))
		return;

	LOCKMUTEX(m_dbgThreadLock);
	// Do not bother to process the result, if the debugger thread is not
	// running or not exists
	if (m_dbgThread && m_dbgThread->IsRunning())
	{
		bool goAhead = false;
		goAhead = (qry->ReturnCode() == PGRES_TUPLES_OK);
		pgSet *set = qry->ResultSet();

		// Save the current running target pid
		m_currTargetPid = set->GetVal(0);

		// Next line release the actual result-set
		set = NULL;

		// Release the result-set
		qry->Release();
		UNLOCKMUTEX(m_dbgThreadLock);

		pgParamsArray *params = new pgParamsArray;

		params->Add(new pgParam(PGOID_TYPE_INT4, &(m_model->GetSession())));
		if (m_ver <= DEBUGGER_V2_API)
		{
			m_dbgThread->AddQuery(ms_cmdWaitForBreakpointV1, params, RESULT_ID_BREAKPOINT);
		}
		else
		{
			m_dbgThread->AddQuery(ms_cmdWaitForBreakpointV2, params, RESULT_ID_BREAKPOINT);
		}
	}
	else
	{
		UNLOCKMUTEX(m_dbgThreadLock);
	}
}
