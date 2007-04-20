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
    extern wxString debuggerExecutable;

    // Store the password in the environment if it is not stored. 
    // We don't set it on the command line because thats insecure 
    // on some platforms.
    if (!obj->GetDatabase()->GetServer()->GetPasswordIsStored())
        wxSetEnv(wxT("PGPASSWORD"), obj->GetDatabase()->GetServer()->GetPassword());

    // Setup the debugger command
    wxString cmd = debuggerExecutable;

    cmd += wxT(" -d ") + obj->GetDatabase()->GetQuotedIdentifier();
    cmd += wxT(" -p ") + NumToStr((long)obj->GetServer()->GetPort());
    cmd += wxT(" -U ") + obj->GetServer()->GetUsername();
    cmd += wxT(" -h ") + obj->GetServer()->GetName();
    cmd += wxT(" -o ") + NumToStr((long)obj->GetOid());
    cmd += wxT(" -i");

    // Fire off the debugger...
    wxExecute(cmd);

    return 0;
}

bool debuggerFactory::CheckEnable(pgObject *obj)
{
    extern wxString debuggerExecutable;

    if (debuggerExecutable.IsEmpty())
        return false;

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
    extern wxString debuggerExecutable;

    // Store the password in the environment if it is not stored. 
    // We don't set it on the command line because thats insecure 
    // on some platforms.
    if (!obj->GetDatabase()->GetServer()->GetPasswordIsStored())
        wxSetEnv(wxT("PGPASSWORD"), obj->GetDatabase()->GetServer()->GetPassword());

    // Setup the debugger command
    wxString cmd = debuggerExecutable;

    cmd += wxT(" -d ") + obj->GetDatabase()->GetQuotedIdentifier();
    cmd += wxT(" -p ") + NumToStr((long)obj->GetServer()->GetPort());
    cmd += wxT(" -U ") + obj->GetServer()->GetUsername();
    cmd += wxT(" -h ") + obj->GetServer()->GetName();

    if (obj->GetMetaType() == PGM_TRIGGER)
        cmd += wxT(" -o ") + NumToStr((long)((pgTrigger *)obj)->GetFunctionOid());
    else
        cmd += wxT(" -o ") + NumToStr((long)obj->GetOid());

    // Fire off the debugger...
    wxExecute(cmd);

    return 0;
}

bool breakpointFactory::CheckEnable(pgObject *obj)
{
    extern wxString debuggerExecutable;

    if (debuggerExecutable.IsEmpty())
        return false;

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

