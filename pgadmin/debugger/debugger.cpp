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
#include "debugger/dbgBreakPoint.h"
#include "debugger/ctlCodeWindow.h"
#include "debugger/dlgDirectDbg.h"
#include "debugger/frmDebugger.h"
#include "schema/pgFunction.h"
#include "schema/pgTrigger.h"
#include "schema/edbPackageFunction.h"

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
    frmDebugger *debugger = new frmDebugger(form, wxString::Format(_("Debugger - %s"), obj->GetFullIdentifier().c_str()));

    // Setup the connection properties to be used by the debugger
    dbgConnProp cp;
    cp.m_database = obj->GetDatabase()->GetQuotedIdentifier();
    cp.m_host = obj->GetServer()->GetName();
    cp.m_password = obj->GetDatabase()->GetServer()->GetPassword();
    cp.m_port = NumToStr((long)obj->GetServer()->GetPort());
    cp.m_sslMode = obj->GetServer()->GetSSL();
    cp.m_userName = obj->GetServer()->GetUsername();

    // Setup the debugging session
    dlgDirectDbg *directDebugger = NULL;
    directDebugger = debugger->addDirectDbg(cp);

    dbgBreakPointList &breakpoints = directDebugger->getBreakpointList();
    breakpoints.Append(new dbgBreakPoint(dbgBreakPoint::OID, NumToStr((long)obj->GetOid()), wxT("'NULL'")));
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
                if (obj->GetDatabase()->GetConnection()->EdbMinimumVersion(8, 2) && 
                    obj->GetDatabase()->CanDebugEdbspl() && 
                    obj->GetName() != wxT("cons") &&
                    ((edbPackageFunction *)obj)->GetSource() != wxEmptyString)
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
    frmDebugger *debugger = new frmDebugger(form, wxString::Format(_("Debugger - %s"), obj->GetFullIdentifier().c_str()));
    debugger->Show(true);
    debugger->Raise();

    // Setup the connection properties to be used by the debugger
    dbgConnProp cp;
    cp.m_database = obj->GetDatabase()->GetQuotedIdentifier();
    cp.m_host = obj->GetServer()->GetName();
    cp.m_password = obj->GetDatabase()->GetServer()->GetPassword();
    cp.m_port = NumToStr((long)obj->GetServer()->GetPort());
    cp.m_sslMode = obj->GetServer()->GetSSL();
    cp.m_userName = obj->GetServer()->GetUsername();

    // Setup the debugging session
    ctlCodeWindow *globalDebugger = NULL;
    globalDebugger = debugger->addDebug(cp);

    dbgBreakPointList &breakpoints = globalDebugger->getBreakpointList();
    breakpoints.Append(new dbgBreakPoint(dbgBreakPoint::OID, dbgOid, wxT("'NULL'")));
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
                if (obj->GetDatabase()->GetConnection()->EdbMinimumVersion(8, 2) && 
                    obj->GetDatabase()->CanDebugEdbspl() && 
                    obj->GetName() != wxT("cons") &&
                    ((edbPackageFunction *)obj)->GetSource() != wxEmptyString)
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

