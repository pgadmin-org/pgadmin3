//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id: factory.cpp 6044 2007-03-12 20:38:50Z dpage $
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// debugger.cpp - Debugger factories
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "debugger/debugger.h"
#include "debugger/wsBreakPoint.h"
#include "debugger/wsCodeWindow.h"
#include "debugger/wsDirectdbg.h"
#include "debugger/wsMainFrame.h"
#include "schema/pgFunction.h"
#include "schema/pgTrigger.h"

///////////////////////////////////////////////////
// Debugger factory
///////////////////////////////////////////////////
debuggerFactory::debuggerFactory(menuFactoryList *list, wxMenu *mnu, wxToolBar *toolbar) : actionFactory(list)
{
    mnu->Append(id, _("&Debug"), _("Debug the selected object"));
}

wxWindow *debuggerFactory::StartDialog(frmMain *form, pgObject *obj)
{
    // Setup the debugger frame
    wsMainFrame *debugger = new wsMainFrame(form, wxString::Format(_("Debugger - %s"), obj->GetFullIdentifier().c_str()));

    // Setup the connection properties to be used by the debugger
    wsConnProp cp;
    cp.m_database = obj->GetDatabase()->GetQuotedIdentifier();
    cp.m_host = obj->GetServer()->GetName();
    cp.m_password = obj->GetDatabase()->GetServer()->GetPassword();
    cp.m_port = NumToStr((long)obj->GetServer()->GetPort());
    cp.m_sslMode = obj->GetServer()->GetSSL();
    cp.m_userName = obj->GetServer()->GetUsername();

    // Setup the debugging session
	wsDirectDbg *directDebugger = NULL;
    directDebugger = debugger->addDirectDbg(cp);

    wsBreakpointList &breakpoints = directDebugger->getBreakpointList();
    breakpoints.Append(new wsBreakpoint(wsBreakpoint::OID, NumToStr((long)obj->GetOid()), wxT("'NULL'")));
    directDebugger->startDebugging();

    // Return the debugger window to frmMain.
    return debugger;
}

bool debuggerFactory::CheckEnable(pgObject *obj)
{
    if (obj && !obj->IsCollection())
    {
        switch (obj->GetMetaType())
        {
            case PGM_FUNCTION:
                {
                    pgFunction *func = (pgFunction *)obj;
                    if (func->GetReturnType() != wxT("trigger"))
                    {
                        if (func->GetLanguage() == wxT("plpgsql") && obj->GetDatabase()->CanDebugPlpgsql())
                            return true;
                        else if (func->GetLanguage() == wxT("edbspl") && obj->GetDatabase()->CanDebugEdbspl())
                            return true;
                        else
                            return false;
                    }
                    else
                        return false;
                }
                break;

            case EDB_PACKAGEFUNCTION:
                if (obj->GetDatabase()->GetConnection()->EdbMinimumVersion(8, 2) && obj->GetDatabase()->CanDebugEdbspl())
                    return true;
                break;

            default:
                break;
        }
    }
    return false;
}

///////////////////////////////////////////////////
// Breakpoint factory
///////////////////////////////////////////////////
breakpointFactory::breakpointFactory(menuFactoryList *list, wxMenu *mnu, wxToolBar *toolbar) : actionFactory(list)
{
    mnu->Append(id, _("&Set breakpoint"), _("Set a breakpoint on the selected object"));
}

wxWindow *breakpointFactory::StartDialog(frmMain *form, pgObject *obj)
{
    wxString dbgOid;
    if (obj->GetMetaType() == PGM_TRIGGER)
        dbgOid = NumToStr((long)((pgTrigger *)obj)->GetFunctionOid());
    else
        dbgOid = NumToStr((long)obj->GetOid());

    // Setup the debugger frame
    wsMainFrame *debugger = new wsMainFrame(form, wxString::Format(_("Debugger - %s"), obj->GetFullIdentifier().c_str()));
    debugger->Show(true);
    debugger->Raise();

    // Setup the connection properties to be used by the debugger
    wsConnProp cp;
    cp.m_database = obj->GetDatabase()->GetQuotedIdentifier();
    cp.m_host = obj->GetServer()->GetName();
    cp.m_password = obj->GetDatabase()->GetServer()->GetPassword();
    cp.m_port = NumToStr((long)obj->GetServer()->GetPort());
    cp.m_sslMode = obj->GetServer()->GetSSL();
    cp.m_userName = obj->GetServer()->GetUsername();

    // Setup the debugging session
	wsCodeWindow *globalDebugger = NULL;
    globalDebugger = debugger->addDebug(cp);

    wsBreakpointList &breakpoints = globalDebugger->getBreakpointList();
    breakpoints.Append(new wsBreakpoint(wsBreakpoint::OID, dbgOid, wxT("'NULL'")));
    globalDebugger->startGlobalDebugging();

    // Return the debugger window to frmMain.
    return debugger;
}

bool breakpointFactory::CheckEnable(pgObject *obj)
{
    if (obj && !obj->IsCollection())
    {
        switch (obj->GetMetaType())
        {
            case PGM_FUNCTION:
                {
                    pgFunction *func = (pgFunction *)obj;
                    if (func->GetReturnType() != wxT("trigger"))
                    {
                        if (func->GetLanguage() == wxT("plpgsql") && obj->GetDatabase()->CanDebugPlpgsql())
                            return true;
                        else if (func->GetLanguage() == wxT("edbspl") && obj->GetDatabase()->CanDebugEdbspl())
                            return true;
                        else
                            return false;
                    }
                    else
                        return false;
                }
                break;

            case EDB_PACKAGEFUNCTION:
                if (obj->GetDatabase()->GetConnection()->EdbMinimumVersion(8, 2) && obj->GetDatabase()->CanDebugPlpgsql())
                    return true;
                break;

            case PGM_TRIGGER:
                {
                    pgTrigger *trig = (pgTrigger *)obj;
                    if (trig->GetLanguage() == wxT("plpgsql") && obj->GetDatabase()->CanDebugPlpgsql())
                        return true;
                    else if (trig->GetLanguage() == wxT("edbspl") && obj->GetDatabase()->CanDebugEdbspl())
                        return true;
                    else
                        return false;
                }
                break;

            default:
                break;
        }
    }
    return false;
}

