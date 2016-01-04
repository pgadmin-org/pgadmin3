//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// pgRule.cpp - Rule class
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "frm/frmMain.h"
#include "utils/misc.h"
#include "schema/pgRule.h"


pgRule::pgRule(pgSchema *newSchema, const wxString &newName)
	: pgRuleObject(newSchema, ruleFactory, newName)
{
}

pgRule::~pgRule()
{
}

wxString pgRule::GetTranslatedMessage(int kindOfMessage) const
{
	wxString message = wxEmptyString;

	switch (kindOfMessage)
	{
		case RETRIEVINGDETAILS:
			message = _("Retrieving details on rule");
			message += wxT(" ") + GetName();
			break;
		case REFRESHINGDETAILS:
			message = _("Refreshing rule");
			message += wxT(" ") + GetName();
			break;
		case GRANTWIZARDTITLE:
			message = _("Privileges for rule");
			message += wxT(" ") + GetName();
			break;
		case DROPINCLUDINGDEPS:
			message = wxString::Format(_("Are you sure you wish to drop rule \"%s\" including all objects that depend on it?"),
			                           GetFullIdentifier().c_str());
			break;
		case DROPEXCLUDINGDEPS:
			message = wxString::Format(_("Are you sure you wish to drop rule \"%s\"?"),
			                           GetFullIdentifier().c_str());
			break;
		case DROPCASCADETITLE:
			message = _("Drop rule cascaded?");
			break;
		case DROPTITLE:
			message = _("Drop rule?");
			break;
		case PROPERTIESREPORT:
			message = _("Rule properties report");
			message += wxT(" - ") + GetName();
			break;
		case PROPERTIES:
			message = _("Rule properties");
			break;
		case DDLREPORT:
			message = _("Rule DDL report");
			message += wxT(" - ") + GetName();
			break;
		case DDL:
			message = _("Rule DDL");
			break;
		case DEPENDENCIESREPORT:
			message = _("Rule dependencies report");
			message += wxT(" - ") + GetName();
			break;
		case DEPENDENCIES:
			message = _("Rule dependencies");
			break;
		case DEPENDENTSREPORT:
			message = _("Rule dependents report");
			message += wxT(" - ") + GetName();
			break;
		case DEPENDENTS:
			message = _("Rule dependents");
			break;
	}

	return message;
}


int pgRule::GetIconId()
{
	if (GetEnabled())
		return ruleFactory.GetIconId();
	else
		return ruleFactory.GetClosedIconId();
}


bool pgRule::DropObject(wxFrame *frame, ctlTree *browser, bool cascaded)
{
	wxString sql = wxT("DROP RULE ") + GetQuotedIdentifier() + wxT(" ON ") + GetQuotedFullTable();
	if (cascaded)
		sql += wxT(" CASCADE");
	return GetDatabase()->ExecuteVoid(sql);
}


void pgRule::SetEnabled(ctlTree *browser, const bool b)
{
	if (GetQuotedFullTable().Len() > 0 && ((enabled && !b) || (!enabled && b)))
	{
		wxString sql = wxT("ALTER TABLE ") + GetQuotedFullTable() + wxT(" ");
		if (enabled && !b)
			sql += wxT("DISABLE");
		else if (!enabled && b)
			sql += wxT("ENABLE");
		sql += wxT(" RULE ") + GetQuotedIdentifier();
		GetDatabase()->ExecuteVoid(sql);
	}

	enabled = b;
	UpdateIcon(browser);
}


wxString pgRule::GetSql(ctlTree *browser)
{
	if (sql.IsNull())
	{
		sql = wxT("-- Rule: ") + GetQuotedIdentifier() + wxT(" ON ") + GetQuotedFullTable() + wxT("\n\n")
		      + wxT("-- DROP RULE ") + GetQuotedIdentifier() + wxT(" ON ") + GetQuotedFullTable() + wxT(";\n\n")
		      + wxT("CREATE OR REPLACE") + GetFormattedDefinition().Mid(6) // the backend pg_get_ruledef gives CREATE only
		      + wxT("\n");

		if (!GetEnabled())
		{
			sql += wxT("ALTER TABLE ") + GetQuotedFullTable() + wxT(" ")
			       +  wxT("DISABLE RULE ") + GetQuotedIdentifier() + wxT(";\n");
		}

		if (!GetComment().IsEmpty())
			sql += wxT("COMMENT ON RULE ") + GetQuotedIdentifier() + wxT(" ON ") + GetQuotedFullTable()
			       +  wxT(" IS ") + qtDbString(GetComment()) + wxT(";\n");
	}
	return sql;
}


void pgRule::ShowTreeDetail(ctlTree *browser, frmMain *form, ctlListView *properties, ctlSQLBox *sqlPane)
{
	if (properties)
	{
		CreateListColumns(properties);
		wxString def = GetFormattedDefinition();
		if (!def.IsEmpty())
		{
			int doPos = def.Find(wxT(" DO INSTEAD "));
			if (doPos > 0)
				def = def.Mid(doPos + 12).Strip(wxString::both);
			else
			{
				doPos = def.Find(wxT(" DO "));
				if (doPos > 0)
					def = def.Mid(doPos + 4).Strip(wxString::both);
			}
		}

		properties->AppendItem(_("Name"), GetName());
		properties->AppendItem(_("OID"), GetOid());
		properties->AppendItem(_("Event"), GetEvent());
		properties->AppendItem(_("Condition"), GetCondition());
		properties->AppendYesNoItem(_("Do instead?"), GetDoInstead());
		properties->AppendItem(_("Definition"), firstLineOnly(def));
		if (this->GetDatabase()->connection()->BackendMinimumVersion(8, 3))
			properties->AppendYesNoItem(_("Enabled?"), GetEnabled());
		properties->AppendYesNoItem(_("System rule?"), GetSystemObject());
		properties->AppendItem(_("Comment"), firstLineOnly(GetComment()));
	}
}



pgObject *pgRule::Refresh(ctlTree *browser, const wxTreeItemId item)
{
	pgObject *rule = 0;
	pgCollection *coll = browser->GetParentCollection(item);
	if (coll)
		rule = ruleFactory.CreateObjects(coll, 0, wxT("\n   AND rw.oid=") + GetOidStr());

	return rule;
}


pgObject *pgRuleFactory::CreateObjects(pgCollection *collection, ctlTree *browser, const wxString &restriction)
{
	pgRule *rule = 0;

	pgSet *rules = collection->GetDatabase()->ExecuteSet(
	                   wxT("SELECT rw.oid, rw.*, relname, CASE WHEN relkind = 'r' THEN TRUE ELSE FALSE END AS parentistable, nspname, description,\n")
	                   wxT("       pg_get_ruledef(rw.oid") + collection->GetDatabase()->GetPrettyOption() + wxT(") AS definition\n")
	                   wxT("  FROM pg_rewrite rw\n")
	                   wxT("  JOIN pg_class cl ON cl.oid=rw.ev_class\n")
	                   wxT("  JOIN pg_namespace nsp ON nsp.oid=cl.relnamespace\n")
	                   wxT("  LEFT OUTER JOIN pg_description des ON (des.objoid=rw.oid AND des.classoid='pg_rewrite'::regclass)\n")
	                   wxT(" WHERE ev_class = ") + NumToStr(collection->GetOid())
	                   + restriction + wxT("\n")
	                   wxT(" ORDER BY rw.rulename"));

	if (rules)
	{
		while (!rules->Eof())
		{
			// Be careful that the schema of a rule (and a trigger) is the schema of the schema
			rule = new pgRule(collection->GetSchema()->GetSchema(), rules->GetVal(wxT("rulename")));

			rule->iSetOid(rules->GetOid(wxT("oid")));
			rule->iSetComment(rules->GetVal(wxT("description")));

			if (collection->GetDatabase()->connection()->BackendMinimumVersion(8, 3))
			{
				if (rules->GetVal(wxT("ev_enabled")) != wxT("D"))
					rule->iSetEnabled(true);
				else
					rule->iSetEnabled(false);
			}

			rule->iSetParentIsTable(rules->GetBool(wxT("parentistable")));
			rule->iSetDoInstead(rules->GetBool(wxT("is_instead")));
			rule->iSetAction(rules->GetVal(wxT("ev_action")));
			wxString definition = rules->GetVal(wxT("definition"));
			int doPos = definition.Find(wxT(" DO "));
			int wherePos = definition.Find(wxT(" WHERE "));
			if (wherePos > 0 && wherePos < doPos)
				rule->iSetCondition(definition.Mid(wherePos + 7, doPos - wherePos - 7));

			rule->iSetDefinition(definition);
			rule->iSetQuotedFullTable(collection->GetDatabase()->GetQuotedSchemaPrefix(rules->GetVal(wxT("nspname")))
			                          + qtIdent(rules->GetVal(wxT("relname"))));
			const wxChar *evts[] = {0, wxT("SELECT"), wxT("UPDATE"), wxT("INSERT"), wxT("DELETE")};
			int evno = StrToLong(rules->GetVal(wxT("ev_type")));
			if (evno > 0 && evno < 5)
				rule->iSetEvent(evts[evno]);
			else
				rule->iSetEvent(wxT("Unknown"));

			if (browser)
			{
				browser->AppendObject(collection, rule);
				rules->MoveNext();
			}
			else
				break;
		}

		delete rules;
	}
	return rule;
}


/////////////////////////////


pgRuleCollection::pgRuleCollection(pgaFactory *factory, pgSchema *sch)
	: pgSchemaObjCollection(factory, sch)
{
}


wxString pgRuleCollection::GetTranslatedMessage(int kindOfMessage) const
{
	wxString message = wxEmptyString;

	switch (kindOfMessage)
	{
		case RETRIEVINGDETAILS:
			message = _("Retrieving details on rules");
			break;
		case REFRESHINGDETAILS:
			message = _("Refreshing rules");
			break;
		case OBJECTSLISTREPORT:
			message = _("Rules list report");
			break;
	}

	return message;
}


/////////////////////////////

#include "images/rule.pngc"
#include "images/rulebad.pngc"
#include "images/rules.pngc"

pgRuleFactory::pgRuleFactory()
	: pgSchemaObjFactory(__("Rule"), __("New Rule..."), __("Create a new Rule."), rule_png_img)
{
	metaType = PGM_RULE;
	closedId = addIcon(rulebad_png_img);
}


pgCollection *pgRuleFactory::CreateCollection(pgObject *obj)
{
	return new pgRuleCollection(GetCollectionFactory(), (pgSchema *)obj);
}

pgRuleFactory ruleFactory;
static pgaCollectionFactory cf(&ruleFactory, __("Rules"), rules_png_img);


enabledisableRuleFactory::enabledisableRuleFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar) : contextActionFactory(list)
{
	mnu->Append(id, _("Rule enabled?"), _("Enable or disable selected rule."), wxITEM_CHECK);
}


wxWindow *enabledisableRuleFactory::StartDialog(frmMain *form, pgObject *obj)
{
	((pgRule *)obj)->SetEnabled(form->GetBrowser(), !((pgRule *)obj)->GetEnabled());
	((pgRule *)obj)->SetDirty();

	wxTreeItemId item = form->GetBrowser()->GetSelection();
	if (obj == form->GetBrowser()->GetObject(item))
	{
		obj->ShowTreeDetail(form->GetBrowser(), 0, form->GetProperties());
		form->GetSqlPane()->SetReadOnly(false);
		form->GetSqlPane()->SetText(((pgRule *)obj)->GetSql(form->GetBrowser()));
		form->GetSqlPane()->SetReadOnly(true);
	}
	form->GetMenuFactories()->CheckMenu(obj, form->GetMenuBar(), (ctlMenuToolbar *)form->GetToolBar());

	return 0;
}


bool enabledisableRuleFactory::CheckEnable(pgObject *obj)
{
	return obj && obj->IsCreatedBy(ruleFactory)
	       && ((pgRule *)obj)->GetConnection()->BackendMinimumVersion(8, 3)
	       && ((pgRule *)obj)->GetParentIsTable();
}

bool enabledisableRuleFactory::CheckChecked(pgObject *obj)
{
	return obj && obj->IsCreatedBy(ruleFactory) && ((pgRule *)obj)->GetEnabled();
}
