//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// debugger.cpp - Debugger factories
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "frm/frmMain.h"
#include "debugger/debugger.h"
#include "debugger/dbgController.h"
#include "schema/pgFunction.h"
#include "schema/pgTrigger.h"
#include "schema/edbPackageFunction.h"

#include <stdexcept>

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
	pgSet *res = obj->GetConnection()->ExecuteSet(wxT("SELECT proname FROM pg_proc WHERE oid = ") + NumToStr((long)obj->GetOid()));

	if (res->NumRows() != 1)
	{
		wxLogError(_("The selected function could not be found."));
		ctlTree *browser = form->GetBrowser();
		wxTreeItemId item = browser->GetSelection();
		if (obj == browser->GetObject(item))
		{
			form->Refresh(obj);
		}
		delete res;
		return (wxWindow *)NULL;
	}

	if (res->GetVal(wxT("proname")).Contains(wxT(":")))
	{
		wxLogError(_("Functions with a colon in the name cannot be debugged."));
		delete res;
		return (wxWindow *)NULL;
	}
	try
	{
		new dbgController(form, obj, true);
	}
	catch (const std::runtime_error &)
	{
		// just ignore this errors, we already logged them in native messages to
		// the end-user
	}
	delete res;
	return (wxWindow *)NULL;
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

	// EnterpriseDB 8.3 or earlier does not support debugging by the non-superuser
	if (!obj->GetServer()->GetSuperUser() && !obj->GetConnection()->EdbMinimumVersion(8, 4) && obj->GetConnection()->GetIsEdb())
		return false;

	if (!obj->IsCollection())
	{
		switch (obj->GetMetaType())
		{
			case PGM_FUNCTION:
			{
				pgFunction *func = (pgFunction *)obj;

				// If this is an EDB wrapped function, no debugging allowed
				if (obj->GetConnection()->GetIsEdb() && func->GetSource().Trim(false).StartsWith(wxT("$__EDBwrapped__$")))
					return false;

				if (func->GetReturnType() != wxT("trigger") &&
				        func->GetReturnType() != wxT("\"trigger\""))
				{
					if (func->GetLanguage() == wxT("plpgsql") &&
					        obj->GetDatabase()->CanDebugPlpgsql())
						return true;
					else if (func->GetLanguage() == wxT("edbspl") &&
					         obj->GetDatabase()->CanDebugEdbspl())
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
				        ((edbPackageFunction *)obj)->GetSource() != wxEmptyString &&
				        (!((edbPackageFunction *)obj)->GetSource().Trim(false).StartsWith(wxT("$__EDBwrapped__$"))))
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
	pgSet *res = obj->GetConnection()->ExecuteSet(
	                 wxT("SELECT count(*) AS count, proname FROM pg_proc WHERE oid = ") + dbgOid + wxT(" GROUP BY proname"));
	if (res->GetVal(wxT("proname")).Contains(wxT(":")))
	{
		wxLogError(_("Functions with a colon in the name cannot be debugged."));
		delete res;
		return (wxWindow *)NULL;
	}

	if (res->GetLong(wxT("count")) != 1)
	{
		wxLogError(_("The selected function could not be found."));

		ctlTree *browser = form->GetBrowser();
		wxTreeItemId item = browser->GetSelection();
		if (obj == browser->GetObject(item))
		{
			pgCollection *coll = browser->GetParentCollection(obj->GetId());
			browser->DeleteChildren(coll->GetId());
			coll->ShowTreeDetail(browser);
		}
		delete res;
		return (wxWindow *)NULL;
	}

	try
	{
		new dbgController(form, obj, false);
	}
	catch (const std::runtime_error &)
	{
		// just ignore this errors, we already logged them in native messages to
		// the end-user
	}

	delete res;
	return (wxWindow *)NULL;
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

				// If this is an EDB wrapped function, no debugging allowed
				if (obj->GetConnection()->GetIsEdb() &&
				        func->GetSource().Trim(false).StartsWith(wxT("$__EDBwrapped__$")))
					return false;

				if (func->GetLanguage() == wxT("plpgsql") &&
				        obj->GetDatabase()->CanDebugPlpgsql())
					return true;
				else if (func->GetLanguage() == wxT("edbspl") &&
				         obj->GetDatabase()->CanDebugEdbspl())
					return true;
				else
					return false;
			}
			break;

			case EDB_PACKAGEFUNCTION:
				if (obj->GetDatabase()->GetConnection()->EdbMinimumVersion(8, 2) &&
				        obj->GetDatabase()->CanDebugEdbspl() &&
				        obj->GetName() != wxT("cons") &&
				        ((edbPackageFunction *)obj)->GetSource() != wxEmptyString &&
				        (!((edbPackageFunction *)obj)->GetSource().Trim(false).StartsWith(wxT("$__EDBwrapped__$"))))
					return true;
				break;

			case PGM_TRIGGER:
			{
				pgTrigger *trig = (pgTrigger *)obj;

				// If this is an EDB wrapped function, no debugging allowed
				if (obj->GetConnection()->GetIsEdb() &&
				        trig->GetSource().Trim(false).StartsWith(wxT("$__EDBwrapped__$")))
					return false;

				if (trig->GetLanguage() == wxT("plpgsql") &&
				        obj->GetDatabase()->CanDebugPlpgsql())
					return true;
				else if (trig->GetLanguage() == wxT("edbspl") &&
				         obj->GetDatabase()->CanDebugEdbspl())
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




