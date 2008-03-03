//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id: factory.cpp 6044 2007-03-12 20:38:50Z dpage $
// Copyright (C) 2002 - 2008, The pgAdmin Development Team
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
debuggerFactory::debuggerFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar) : actionFactory(list)
{
    mnu->Append(id, _("&Debug"), _("Debug the selected object"));
}

wxWindow *debuggerFactory::StartDialog(frmMain *form, pgObject *obj)
{
    // Check here to make sure the function still exists before proceeding.
    // There is still a very small window in which it might be dropped, but
    // that will be handled by a cache lookup failure error in the database
    // We also make sure the function name doesn't contain a : as that will
    // sent the debugger API nuts.
    pgSet *res = obj->GetConnection()->ExecuteSet(wxT("SELECT count(*) AS count, proname FROM pg_proc WHERE oid = ") + NumToStr((long)obj->GetOid()) + wxT(" GROUP BY proname"));
    if (res->GetVal(wxT("proname")).Contains(wxT(":")))
    {
        wxLogError(_("Functions with a colon in the name cannot be debugged."));
        return 0;
    }

    if (res->GetLong(wxT("count")) != 1)
    {
        wxLogError(_("The selected function could not be found."));
        ctlTree *browser = form->GetBrowser();
        wxTreeItemId item=browser->GetSelection();
        if (obj == browser->GetObject(item))
        {
            pgCollection *coll=browser->GetParentCollection(obj->GetId());
            browser->DeleteChildren(coll->GetId());
            coll->ShowTreeDetail(browser);
        }
        return 0;
    }

    // Setup the debugger frame
    frmDebugger *debugger = new frmDebugger(form, wxString::Format(_("Debugger - %s"), obj->GetFullIdentifier().c_str()));

    // Setup the connection properties to be used by the debugger
    dbgConnProp cp;
    cp.m_database = obj->GetDatabase()->GetIdentifier();
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
    if (!directDebugger->startDebugging())
    {
        ctlTree *browser = form->GetBrowser();
        wxTreeItemId item=browser->GetSelection();
        if (obj == browser->GetObject(item))
        {
            pgCollection *coll=browser->GetParentCollection(obj->GetId());
            browser->DeleteChildren(coll->GetId());
            coll->ShowTreeDetail(browser);
        }
        return 0;
    }

    // Return the debugger window to frmMain.
    return debugger;
}

bool debuggerFactory::CheckEnable(pgObject *obj)
{
    if (!obj)
        return false;

    // Can't debug catalog objects.
    if (obj->GetSchema() && obj->GetSchema()->GetMetaType() == PGM_CATALOG)
        return false;

    // Must be a super user or object owner to create breakpoints of any kind.
    if (!obj->GetServer() || !(obj->GetServer()->GetSuperUser() || obj->GetServer()->GetUsername() == obj->GetOwner()))
        return false;

    if (!obj->IsCollection())
    {
        switch (obj->GetMetaType())
        {
            case PGM_FUNCTION:
                {
                    pgFunction *func = (pgFunction *)obj;
                    if (func->GetReturnType() != wxT("trigger") && func->GetReturnType() != wxT("\"trigger\""))
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
breakpointFactory::breakpointFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar) : actionFactory(list)
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

    // Check here to make sure the function still exists before proceeding.
    // There is still a very small window in which it might be dropped, but
    // we should be able to handle most cases here without having to do this 
    // deep down in query threads.
    // We also make sure the function name doesn't contain a : as that will
    // sent the debugger API nuts.
    pgSet *res = obj->GetConnection()->ExecuteSet(wxT("SELECT count(*) AS count, proname FROM pg_proc WHERE oid = ") + dbgOid + wxT(" GROUP BY proname"));
    if (res->GetVal(wxT("proname")).Contains(wxT(":")))
    {
        wxLogError(_("Functions with a colon in the name cannot be debugged."));
        return 0;
    }

    if (res->GetLong(wxT("count")) != 1)
    {
        wxLogError(_("The selected function could not be found."));
        ctlTree *browser = form->GetBrowser();
        wxTreeItemId item=browser->GetSelection();
        if (obj == browser->GetObject(item))
        {
            pgCollection *coll=browser->GetParentCollection(obj->GetId());
            browser->DeleteChildren(coll->GetId());
            coll->ShowTreeDetail(browser);
        }
        return 0;
    }

    // Setup the debugger frame
    frmDebugger *debugger = new frmDebugger(form, wxString::Format(_("Debugger - %s"), obj->GetFullIdentifier().c_str()));
    debugger->Show(true);
    debugger->Raise();

    // Setup the connection properties to be used by the debugger
    dbgConnProp cp;
    cp.m_database = obj->GetDatabase()->GetIdentifier();
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
    if (!obj)
        return false;

    // Can't debug catalog objects.
    if (obj->GetSchema() && obj->GetSchema()->GetMetaType() == PGM_CATALOG)
        return false;

    // Must be a super user to create breakpoints of any kind.
    if (!obj->GetServer() || !obj->GetServer()->GetSuperUser())
        return false;

    if (!obj->IsCollection())
    {
        switch (obj->GetMetaType())
        {
            case PGM_FUNCTION:
                {
                    pgFunction *func = (pgFunction *)obj;
                    if (func->GetLanguage() == wxT("plpgsql") && obj->GetDatabase()->CanDebugPlpgsql())
                        return true;
                    else if (func->GetLanguage() == wxT("edbspl") && obj->GetDatabase()->CanDebugEdbspl())
                        return true;
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




