//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// pgEventTrigger.cpp - EventTrigger class
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "frm/frmMain.h"
#include "utils/misc.h"
#include "schema/pgEventTrigger.h"
#include "schema/pgFunction.h"
#include "schema/pgSchema.h"

pgEventTrigger::pgEventTrigger(const wxString &newName)
	: pgDatabaseObject(eventTriggerFactory, newName)
{
	eventTriggerFunction = 0;
	eventTriggerFunctionSchema = 0;
}

pgEventTrigger::~pgEventTrigger()
{
	if (!expandedKids && eventTriggerFunction)
	{
		// eventTriggerFunction wasn't appended to tree, so we need to delete it manually.
		delete eventTriggerFunction;
	}

	delete eventTriggerFunctionSchema;
}

pgCollection *pgEventTriggerFactory::CreateCollection(pgObject *obj)
{
	return new pgEventTriggerCollection(GetCollectionFactory(), (pgDatabase *)obj);
}

pgObject *pgEventTriggerFactory::CreateObjects(pgCollection *collection, ctlTree *browser, const wxString &restriction)
{
	wxString sql;
	pgEventTrigger *eventTrigger = 0;

	sql = wxT("SELECT e.oid, e.xmin, e.evtname AS name, REPLACE(e.evtevent, '_', ' ') AS eventname, pg_catalog.pg_get_userbyid(e.evtowner) AS eventowner, ")
	      wxT(" CASE e.evtenabled WHEN 'O' THEN 'enabled' WHEN 'R' THEN 'replica' WHEN 'A' THEN 'always' WHEN 'D' THEN 'disabled' END AS enabled, ")
	      wxT(" e.evtfoid AS eventfuncoid, e.evtfoid::regproc AS eventfunname, array_to_string(array(select quote_literal(x) from unnest(evttags) as t(x)), ', ') AS when, ")
	      wxT(" pg_catalog.obj_description(e.oid, 'pg_event_trigger') AS comment, ")
	      wxT(" p.prosrc AS source, p.pronamespace AS schemaoid, l.lanname AS language")
	      wxT(" FROM pg_event_trigger e\n")
	      wxT(" LEFT OUTER JOIN pg_proc p ON p.oid=e.evtfoid\n")
	      wxT(" LEFT OUTER JOIN pg_language l ON l.oid=p.prolang\n")
	      wxT(" WHERE 1=1 ");

	if (!restriction.IsEmpty())
		sql += restriction + wxT("\n");
	sql += wxT(" ORDER BY e.evtname ");

	pgSet *eventTriggers = collection->GetDatabase()->ExecuteSet(sql);

	if (eventTriggers)
	{
		while (!eventTriggers->Eof())
		{
			eventTrigger = new pgEventTrigger(eventTriggers->GetVal(wxT("name")));
			eventTrigger->iSetDatabase(collection->GetDatabase());
			eventTrigger->iSetOid(eventTriggers->GetOid(wxT("oid")));
			eventTrigger->iSetXid(eventTriggers->GetOid(wxT("xmin")));
			eventTrigger->iSetOwner(eventTriggers->GetVal(wxT("eventowner")));
			eventTrigger->iSetFunctionOid(eventTriggers->GetOid(wxT("eventfuncoid")));
			eventTrigger->iSetLanguage(eventTriggers->GetVal(wxT("language")));
			eventTrigger->iSetSource(eventTriggers->GetVal(wxT("source")));
			eventTrigger->iSetEventName(eventTriggers->GetVal(wxT("eventname")));
			eventTrigger->iSetFunction(eventTriggers->GetVal(wxT("eventfunname")));
			(eventTriggers->GetVal(wxT("enabled")) == wxT("disabled")) ? eventTrigger->iSetEnabled(false), eventTrigger->iSetEnableStatus(wxT("disabled"))
			: eventTrigger->iSetEnabled(true), eventTrigger->iSetEnableStatus(eventTriggers->GetVal(wxT("enabled")));
			eventTrigger->iSetComment(eventTriggers->GetVal(wxT("comment")));
			eventTrigger->iSetWhen(eventTriggers->GetVal(wxT("when")));
			eventTrigger->iSetSchemaOid(eventTriggers->GetOid(wxT("schemaoid")));

			if (browser)
			{
				browser->AppendObject(collection, eventTrigger);
				eventTriggers->MoveNext();
			}
			else
				break;
		}

		delete eventTriggers;
	}
	return eventTrigger;
}

pgObject *pgEventTrigger::Refresh(ctlTree *browser, const wxTreeItemId item)
{
	pgObject *eventTrigger = 0;
	pgCollection *coll = browser->GetParentCollection(item);

	if (coll)
	{
		wxString restr = wxT(" \n AND e.oid = ") + GetOidStr();
		eventTrigger = eventTriggerFactory.CreateObjects(coll, 0, restr);
	}
	return eventTrigger;
}

void pgEventTrigger::ShowTreeDetail(ctlTree *browser, frmMain *form, ctlListView *properties, ctlSQLBox *sqlPane)
{
	if (!expandedKids && GetLanguage() != wxT("edbspl") && GetLanguage() != wxT("sql"))
	{
		if (browser)
		{
			// if no browser present, function will not be appended to tree
			expandedKids = true;
		}

		if (eventTriggerFunction)
			delete eventTriggerFunction;

		wxString restr = wxT(" WHERE nsp.oid= ") + NumToStr(GetSchemaOid()) + wxT("::oid\n");
		eventTriggerFunctionSchema = (pgSchema *)schemaFactory.CreateObjects((pgCollection *)browser->GetObject(browser->GetSelection()), 0, restr);

		// append function here
		eventTriggerFunction = functionFactory.AppendFunctions(this, eventTriggerFunctionSchema, browser, wxT(" WHERE pr.oid = ") + NumToStr(functionOid) + wxT("::oid\n"));
		if (eventTriggerFunction)
		{
			iSetFunction(eventTriggerFunction->GetQuotedFullIdentifier());
		}

	}

	if (properties)
	{
		CreateListColumns(properties);

		properties->AppendItem(_("Name"), GetName());
		properties->AppendItem(_("OID"), GetOid());
		properties->AppendItem(_("Event"), GetEventName());
		properties->AppendItem(_("Function"), GetFunction());
		properties->AppendItem(_("When?"), GetWhen());
		properties->AppendItem(_("Enabled?"), GetEnabled() ? wxT("Yes") : wxT("No"));
		properties->AppendItem(_("Comment"), firstLineOnly(GetComment()));
	}
}

void pgEventTrigger::SetDirty()
{
	if (expandedKids)
		eventTriggerFunction = 0;
	pgObject::SetDirty();
}

wxString pgEventTrigger::GetSql(ctlTree *browser)
{
	if (sql.IsNull() && (this->eventTriggerFunction))
	{
		sql = wxT("-- Event Trigger: ") + qtIdent(GetName()) + wxT(" on database ")
		      + qtIdent(((pgCollection *)browser->GetObject(browser->GetSelection()))->GetDatabase()->GetName()) + wxT("\n\n")
		      + wxT("-- DROP EVENT TRIGGER ") + qtIdent(GetName()) + wxT(";\n\n");

		sql += wxT("CREATE EVENT TRIGGER ") + qtIdent(GetName()) + wxT(" ON ")
		       + GetEventName() + (GetWhen().IsEmpty() ? wxT("") : (wxT(" WHEN TAG IN (") + GetWhen() + wxT(")\n\n")));

		// Event trigger backend function don't accept any arguments.
		// Hence, it's not required to get/set empty arguments.

		sql += wxT(" EXECUTE PROCEDURE ") + eventTriggerFunction->GetQuotedFullIdentifier() + wxT("();\n\n");

		if (!GetEnabled())
		{
			sql += wxT("ALTER EVENT TRIGGER ") + qtIdent(GetName()) + wxT(" DISABLE;\n");
		}

		if (!GetComment().IsEmpty())
			sql += wxT("COMMENT ON EVENT TRIGGER ") + qtIdent(GetName()) + wxT(" IS ") + qtDbString(GetComment()) + wxT(";\n");
	}
	return sql;
}

wxString pgEventTrigger::GetTranslatedMessage(int kindOfMessage) const
{
	wxString message = wxEmptyString;

	switch (kindOfMessage)
	{
		case RETRIEVINGDETAILS:
			message = _("Retrieving details on event trigger");
			message += wxT(" ") + GetName();
			break;
		case REFRESHINGDETAILS:
			message = _("Refreshing event trigger");
			message += wxT(" ") + GetName();
			break;
		case DROPCASCADETITLE:
			message = _("Drop event trigger cascaded?");
			break;
		case DROPTITLE:
			message = _("Drop event trigger?");
			break;
		case DROPINCLUDINGDEPS:
			message = wxString::Format(_("Are you sure you wish to drop event trigger \"%s\" including all objects that depend on it?"),
			                           GetFullIdentifier().c_str());
			break;
		case DROPEXCLUDINGDEPS:
			message = wxString::Format(_("Are you sure you wish to drop event trigger \"%s\"?"),
			                           GetFullIdentifier().c_str());
			break;
		case PROPERTIESREPORT:
			message = _("Event trigger properties report");
			message += wxT(" - ") + GetName();
			break;
		case DDLREPORT:
			message = _("Event trigger DDL report");
			message += wxT(" - ") + GetName();
			break;
		case DDL:
			message = _("Event trigger DDL");
			break;
	}
	return message;
}

int pgEventTrigger::GetIconId()
{
	if (GetEnabled())
		return eventTriggerFactory.GetIconId();
	else
		return eventTriggerFactory.GetClosedIconId();
}


wxString pgEventTriggerCollection::GetTranslatedMessage(int kindOfMessage) const
{
	wxString message = wxEmptyString;

	switch (kindOfMessage)
	{
		case RETRIEVINGDETAILS:
			message = _("Retrieving details on event triggers");
			break;
		case REFRESHINGDETAILS:
			message = _("Refreshing event triggers");
			break;
		case OBJECTSLISTREPORT:
			message = _("Event triggers list report");
			break;
	}
	return message;
}

bool pgEventTrigger::DropObject(wxFrame *frame, ctlTree *browser, bool cascaded)
{
	wxString sql = wxT("DROP EVENT TRIGGER ") + qtIdent(GetName());
	if (cascaded)
		sql += wxT(" CASCADE");
	sql += wxT(" ;");

	return GetDatabase()->ExecuteVoid(sql);
}

void pgEventTrigger::SetEnabled(ctlTree *browser, const bool b)
{
	if (qtIdent(GetName()).Len() > 0 && ((enabled && !b) || (!enabled && b)))
	{
		wxString sql = wxT("ALTER EVENT TRIGGER ") + qtIdent(GetName()) ;
		if (enabled && !b)
			sql += wxT(" DISABLE");
		else if (!enabled && b)
			sql += wxT(" ENABLE");
		GetDatabase()->ExecuteVoid(sql);
	}
	enabled = b;
	UpdateIcon(browser);
}

bool pgEventTrigger::IsUpToDate()
{
	wxString sql = wxT("SELECT xmin FROM pg_event_trigger WHERE oid = ") + this->GetOidStr();
	if (!this->GetDatabase()->GetConnection() || this->GetDatabase()->ExecuteScalar(sql) != NumToStr(GetXid()))
		return false;
	else
		return true;
}

/////////////////////////////

pgEventTriggerCollection::pgEventTriggerCollection(pgaFactory *factory, pgDatabase *db)
	: pgDatabaseObjCollection(factory, db)
{

}


///////////////////////////////////////////////////

#include "images/trigger.pngc"
#include "images/triggerbad.pngc"
#include "images/triggers.pngc"

pgEventTriggerFactory::pgEventTriggerFactory()
	: pgDatabaseObjFactory(__("Event Trigger"), __("New Event Trigger..."), __("Create a new Event Trigger."), trigger_png_img)
{
	metaType = PGM_EVENTTRIGGER;
	closedId = addIcon(triggerbad_png_img);
}

pgEventTriggerFactory eventTriggerFactory;
static pgaCollectionFactory cf(&eventTriggerFactory, __("Event Triggers"), triggers_png_img);

enabledisableEventTriggerFactory::enabledisableEventTriggerFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar) : contextActionFactory(list)
{
	mnu->Append(id, _("Event trigger enabled?"), _("Enable or disable selected event trigger."), wxITEM_CHECK);
}

wxWindow *enabledisableEventTriggerFactory::StartDialog(frmMain *form, pgObject *obj)
{
	((pgEventTrigger *)obj)->SetEnabled(form->GetBrowser(), !((pgEventTrigger *)obj)->GetEnabled());
	((pgEventTrigger *)obj)->SetDirty();

	wxTreeItemId item = form->GetBrowser()->GetSelection();
	if (obj == form->GetBrowser()->GetObject(item))
	{
		form->GetBrowser()->DeleteChildren(item);
		obj->ShowTreeDetail(form->GetBrowser(), 0, form->GetProperties());
		form->GetSqlPane()->SetReadOnly(false);
		form->GetSqlPane()->SetText(((pgEventTrigger *)obj)->GetSql(form->GetBrowser()));
		form->GetSqlPane()->SetReadOnly(true);
	}
	form->GetMenuFactories()->CheckMenu(obj, form->GetMenuBar(), (ctlMenuToolbar *)form->GetToolBar());

	return 0;
}

bool enabledisableEventTriggerFactory::CheckEnable(pgObject *obj)
{
	return obj && obj->IsCreatedBy(eventTriggerFactory) && obj->CanEdit()
	       && ((pgEventTrigger *)obj)->GetConnection()->BackendMinimumVersion(9, 3);
}

bool enabledisableEventTriggerFactory::CheckChecked(pgObject *obj)
{
	return obj && obj->IsCreatedBy(eventTriggerFactory) && ((pgEventTrigger *)obj)->GetEnabled();
}