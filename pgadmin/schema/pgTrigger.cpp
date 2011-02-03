//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2010, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// pgTrigger.cpp - Trigger class
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "frm/frmMain.h"
#include "utils/misc.h"
#include "utils/pgDefs.h"
#include "schema/pgObject.h"
#include "schema/pgTrigger.h"
#include "schema/pgFunction.h"


pgTrigger::pgTrigger(pgTable *newTable, const wxString &newName)
	: pgTableObject(newTable, triggerFactory, newName)
{
	triggerFunction = 0;
}


pgTrigger::~pgTrigger()
{
	if (!expandedKids && triggerFunction)
	{
		// triggerFunction wasn't appended to tree, so we
		// need to delete it manually
		delete triggerFunction;
	}
}

bool pgTrigger::IsUpToDate()
{
	wxString sql = wxT("SELECT xmin FROM pg_trigger WHERE oid = ") + this->GetOidStr();
	if (!this->GetDatabase()->GetConnection() || this->GetDatabase()->ExecuteScalar(sql) != NumToStr(GetXid()))
		return false;
	else
		return true;
}

bool pgTrigger::DropObject(wxFrame *frame, ctlTree *browser, bool cascaded)
{
	wxString sql = wxT("DROP TRIGGER ") + GetQuotedIdentifier() + wxT(" ON ") + GetQuotedFullTable();
	if (cascaded)
		sql += wxT(" CASCADE");
	return GetDatabase()->ExecuteVoid(sql);
}


void pgTrigger::SetEnabled(const bool b)
{
	if (GetQuotedFullTable().Len() > 0 && ((enabled && !b) || (!enabled && b)))
	{
		wxString sql = wxT("ALTER TABLE ") + GetQuotedFullTable() + wxT(" ");
		if (enabled && !b)
			sql += wxT("DISABLE");
		else if (!enabled && b)
			sql += wxT("ENABLE");
		sql += wxT(" TRIGGER ") + GetQuotedIdentifier();
		GetDatabase()->ExecuteVoid(sql);
	}

	enabled = b;
}


void pgTrigger::SetDirty()
{
	if (expandedKids)
		triggerFunction = 0;
	pgObject::SetDirty();
}


wxString pgTrigger::GetSql(ctlTree *browser)
{
	if (sql.IsNull() && (this->triggerFunction || GetLanguage() == wxT("edbspl")))
	{
		sql = wxT("-- Trigger: ") + GetName() + wxT(" on ") + GetQuotedFullTable() + wxT("\n\n")
		      + wxT("-- DROP TRIGGER ") + qtIdent(GetName())
		      + wxT(" ON ") + GetQuotedFullTable() + wxT(";\n\n");

		if (GetLanguage() == wxT("edbspl"))
			sql += wxT("CREATE OR REPLACE TRIGGER ") + qtIdent(GetName());
		else
			sql += wxT("CREATE TRIGGER ") + qtIdent(GetName());

		sql += wxT("\n  ") + GetFireWhen()
		       + wxT(" ") + GetEvent();

		sql += wxT("\n  ON ") + GetQuotedFullTable()
		       + wxT("\n  FOR EACH ") + GetForEach();

		if (GetConnection()->BackendMinimumVersion(8, 5)
		        && !GetWhen().IsEmpty())
			sql += wxT("\n  WHEN (") + GetWhen() + wxT(")");

		if (GetLanguage() == wxT("edbspl"))
		{
			sql += wxT("\n") + GetSource();
			if (!sql.Trim().EndsWith(wxT(";")))
				sql = sql.Trim() + wxT(";");
			sql += wxT("\n");
		}
		else
		{
			sql += wxT("\n  EXECUTE PROCEDURE ") + triggerFunction->GetQuotedFullIdentifier()
			       + wxT("(") + GetArguments() + wxT(")")
			       + wxT(";\n");
		}

		if (!GetComment().IsEmpty())
			sql += wxT("COMMENT ON TRIGGER ") + GetQuotedIdentifier() + wxT(" ON ") + GetQuotedFullTable()
			       +  wxT(" IS ") + qtDbString(GetComment()) + wxT(";\n");
	}

	return sql;
}


wxString pgTrigger::GetFireWhen() const
{
	return (triggerType & TRIGGER_TYPE_BEFORE) ? wxT("BEFORE") : wxT("AFTER");
}


wxString pgTrigger::GetEvent() const
{
	wxString event;
	if (triggerType & TRIGGER_TYPE_INSERT)
		event = wxT("INSERT");
	if (triggerType & TRIGGER_TYPE_UPDATE)
	{
		if (!event.IsNull())
			event += wxT(" OR ");
		event += wxT("UPDATE");
		if (!GetQuotedColumns().IsEmpty())
			event += wxT(" OF ") + GetQuotedColumns();
	}
	if (triggerType & TRIGGER_TYPE_DELETE)
	{
		if (!event.IsNull())
			event += wxT(" OR ");
		event += wxT("DELETE");
	}
	if (triggerType & TRIGGER_TYPE_TRUNCATE)
	{
		if (!event.IsNull())
			event += wxT(" OR ");
		event += wxT("TRUNCATE");
	}
	return event;
}

wxString pgTrigger::GetForEach() const
{
	return (triggerType & TRIGGER_TYPE_ROW) ? wxT("ROW") : wxT("STATEMENT");
}



void pgTrigger::ReadColumnDetails()
{
	if (!expandedKids && GetLanguage() != wxT("edbspl"))
	{
		expandedKids = true;

		if (GetConnection()->BackendMinimumVersion(8, 5))
		{
			pgSet *res = ExecuteSet(
			                 wxT("SELECT attname\n")
			                 wxT("FROM pg_attribute,\n")
			                 wxT("(SELECT tgrelid, unnest(tgattr) FROM pg_trigger\n")
			                 wxT(" WHERE oid=") + GetOidStr() + wxT(") AS columns(tgrelid, colnum)\n")
			                 wxT("WHERE colnum=attnum AND tgrelid=attrelid"));

			// Allocate memory to store column def
			if (res->NumRows() > 0) columnList.Alloc(res->NumRows());

			long i = 1;
			columns = wxT("");
			quotedColumns = wxT("");

			while (!res->Eof())
			{
				if (i > 1)
				{
					columns += wxT(", ");
					quotedColumns += wxT(", ");
				}

				columns += res->GetVal(wxT("attname"));
				quotedColumns += res->GetVal(wxT("attname"));
				columnList.Add(res->GetVal(wxT("attname")));

				i++;
				res->MoveNext();
			}
		}
		else
		{
			columns = wxT("");
			quotedColumns = wxT("");
		}
	}
}


void pgTrigger::ShowTreeDetail(ctlTree *browser, frmMain *form, ctlListView *properties, ctlSQLBox *sqlPane)
{
	if (!expandedKids && GetLanguage() != wxT("edbspl"))
	{
		ReadColumnDetails();

		if (browser)
		{
			// if no browser present, function will not be appended to tree
			expandedKids = true;
		}
		if (triggerFunction)
			delete triggerFunction;

		// append function here
		triggerFunction = functionFactory.AppendFunctions(this, GetSchema(), browser, wxT(
		                      "WHERE pr.oid=") + NumToStr(functionOid) + wxT("::oid\n"));
		if (triggerFunction)
		{
			iSetFunction(triggerFunction->GetName());
		}
	}

	if (properties)
	{
		CreateListColumns(properties);

		properties->AppendItem(_("Name"), GetName());
		properties->AppendItem(_("OID"), GetOid());
		properties->AppendItem(_("Fires"), GetFireWhen());
		properties->AppendItem(_("Event"), GetEvent());
		if (!GetQuotedColumns().IsEmpty())
		{
			properties->AppendItem(_("Columns"), GetColumns());
		}
		properties->AppendItem(_("For each"), GetForEach());
		if (GetLanguage() != wxT("edbspl"))
			properties->AppendItem(_("Function"), GetFunction() + wxT("(") + GetArguments() + wxT(")"));
		if (GetConnection()->BackendMinimumVersion(8, 5))
			properties->AppendItem(_("When?"), GetWhen());
		properties->AppendItem(_("Enabled?"), GetEnabled());
		properties->AppendItem(_("System trigger?"), GetSystemObject());
		properties->AppendItem(_("Comment"), firstLineOnly(GetComment()));
	}
}



pgObject *pgTrigger::Refresh(ctlTree *browser, const wxTreeItemId item)
{
	pgObject *trigger = 0;
	pgCollection *coll = browser->GetParentCollection(item);
	if (coll)
	{
		wxString restr = wxT("\n   AND t.tgname=") + qtDbString(GetName()) +
		                 wxT(" AND cl.oid=") + GetTable()->GetOidStr() +
		                 wxT("::oid AND cl.relnamespace=") + GetSchema()->GetOidStr() + wxT("::oid");
		trigger = triggerFactory.CreateObjects(coll, 0, restr);
	}

	return trigger;
}



pgObject *pgTriggerFactory::CreateObjects(pgCollection *coll, ctlTree *browser, const wxString &restriction)
{
	pgTableObjCollection *collection = (pgTableObjCollection *)coll;
	pgTrigger *trigger = 0;

	wxString trig_sql;
	trig_sql = wxT("SELECT t.oid, t.xmin, t.*, relname, nspname, des.description, l.lanname, p.prosrc, \n")
	           wxT("  trim(substring(pg_get_triggerdef(t.oid), 'WHEN (.*) EXECUTE PROCEDURE'), '()') AS whenclause\n")
	           wxT("  FROM pg_trigger t\n")
	           wxT("  JOIN pg_class cl ON cl.oid=tgrelid\n")
	           wxT("  JOIN pg_namespace na ON na.oid=relnamespace\n")
	           wxT("  LEFT OUTER JOIN pg_description des ON des.objoid=t.oid\n")
	           wxT("  LEFT OUTER JOIN pg_proc p ON p.oid=t.tgfoid\n")
	           wxT("  LEFT OUTER JOIN pg_language l ON l.oid=p.prolang\n")
	           wxT(" WHERE ");
	if (collection->GetDatabase()->connection()->BackendMinimumVersion(9, 0))
		trig_sql += wxT("NOT tgisinternal\n");
	else
		trig_sql += wxT("NOT tgisconstraint\n");
	if (restriction.IsEmpty())
		trig_sql += wxT(" AND tgrelid = ") + collection->GetOidStr() + wxT("\n");
	else
		trig_sql += restriction + wxT("\n");
	trig_sql += wxT(" ORDER BY tgname");
	pgSet *triggers = collection->GetDatabase()->ExecuteSet(trig_sql);

	if (triggers)
	{
		while (!triggers->Eof())
		{
			trigger = new pgTrigger(collection->GetTable(), triggers->GetVal(wxT("tgname")));

			trigger->iSetOid(triggers->GetOid(wxT("oid")));
			trigger->iSetXid(triggers->GetOid(wxT("xmin")));
			trigger->iSetComment(triggers->GetVal(wxT("description")));
			trigger->iSetFunctionOid(triggers->GetOid(wxT("tgfoid")));

			if (collection->GetDatabase()->connection()->BackendMinimumVersion(8, 3))
			{
				if (triggers->GetVal(wxT("tgenabled")) != wxT("D"))
					trigger->iSetEnabled(true);
				else
					trigger->iSetEnabled(false);
			}
			else
				trigger->iSetEnabled(triggers->GetBool(wxT("tgenabled")));

			trigger->iSetTriggerType(triggers->GetLong(wxT("tgtype")));
			trigger->iSetLanguage(triggers->GetVal(wxT("lanname")));
			trigger->iSetSource(triggers->GetVal(wxT("prosrc")));
			trigger->iSetQuotedFullTable(collection->GetDatabase()->GetQuotedSchemaPrefix(triggers->GetVal(wxT("nspname"))) + qtIdent(triggers->GetVal(wxT("relname"))));
			wxString arglist = wxEmptyString;
			if (triggers->GetLong(wxT("tgnargs")) > 0)
				arglist = triggers->GetVal(wxT("tgargs"));
			wxString args = wxEmptyString;

			while (!arglist.IsEmpty())
			{
				int pos = arglist.Find(wxT("\\000"));
				if (pos != 0)
				{
					wxString arg;
					if (pos > 0)
						arg = arglist.Left(pos);
					else
						arg = arglist;

					if (!args.IsEmpty())
						args += wxT(", ");
					if (NumToStr(StrToLong(arg)) == arg)
						args += arg;
					else
						args += collection->GetDatabase()->GetConnection()->qtDbString(arg);
				}
				if (pos >= 0)
					arglist = arglist.Mid(pos + 4);
				else
					break;
			}
			trigger->iSetArguments(args);

			if (collection->GetDatabase()->connection()->BackendMinimumVersion(8, 5))
				trigger->iSetWhen(triggers->GetVal(wxT("whenclause")));

			if (browser)
			{
				browser->AppendObject(collection, trigger);
				triggers->MoveNext();
			}
			else
				break;
		}

		delete triggers;
	}
	return trigger;
}


/////////////////////////////

#include "images/trigger.xpm"
#include "images/triggers.xpm"

pgTriggerFactory::pgTriggerFactory()
	: pgTableObjFactory(__("Trigger"), __("New Trigger..."), __("Create a new Trigger."), trigger_xpm)
{
	metaType = PGM_TRIGGER;
}


pgTriggerFactory triggerFactory;
static pgaCollectionFactory cf(&triggerFactory, __("Triggers"), triggers_xpm);

enabledisableTriggerFactory::enabledisableTriggerFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar) : contextActionFactory(list)
{
	mnu->Append(id, _("Trigger enabled?"), _("Enable or disable selected trigger."), wxITEM_CHECK);
}


wxWindow *enabledisableTriggerFactory::StartDialog(frmMain *form, pgObject *obj)
{
	((pgTrigger *)obj)->SetEnabled(!((pgTrigger *)obj)->GetEnabled());

	wxTreeItemId item = form->GetBrowser()->GetSelection();
	if (obj == form->GetBrowser()->GetObject(item))
		obj->ShowTreeDetail(form->GetBrowser(), 0, form->GetProperties());
	form->GetMenuFactories()->CheckMenu(obj, form->GetMenuBar(), (ctlMenuToolbar *)form->GetToolBar());

	return 0;
}


bool enabledisableTriggerFactory::CheckEnable(pgObject *obj)
{
	return obj && obj->IsCreatedBy(triggerFactory) && obj->CanEdit()
	       && ((pgTrigger *)obj)->GetConnection()->BackendMinimumVersion(8, 1);
}

bool enabledisableTriggerFactory::CheckChecked(pgObject *obj)
{
	return obj && obj->IsCreatedBy(triggerFactory) && ((pgTrigger *)obj)->GetEnabled();
}
