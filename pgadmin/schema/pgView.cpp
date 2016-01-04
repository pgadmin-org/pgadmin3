//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// pgView.cpp - View class
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "utils/misc.h"
#include "schema/pgColumn.h"
#include "schema/pgIndexConstraint.h"
#include "schema/pgView.h"
#include "frm/frmMain.h"
#include "frm/frmHint.h"
#include "schema/pgTrigger.h"


pgView::pgView(pgSchema *newSchema, const wxString &newName)
	: pgRuleObject(newSchema, viewFactory, newName)
{
	hasInsertRule = false;
	hasUpdateRule = false;
	hasDeleteRule = false;
}

pgView::~pgView()
{
}

wxString pgView::GetTranslatedMessage(int kindOfMessage) const
{
	wxString message = wxEmptyString;

	switch (kindOfMessage)
	{
		case RETRIEVINGDETAILS:
			message = _("Retrieving details on view");
			message += wxT(" ") + GetName();
			break;
		case REFRESHINGDETAILS:
			message = _("Refreshing view");
			message += wxT(" ") + GetName();
			break;
		case DROPINCLUDINGDEPS:
			message = wxString::Format(_("Are you sure you wish to drop view \"%s\" including all objects that depend on it?"),
			                           GetFullIdentifier().c_str());
			break;
		case DROPEXCLUDINGDEPS:
			message = wxString::Format(_("Are you sure you wish to drop view \"%s\"?"),
			                           GetFullIdentifier().c_str());
			break;
		case DROPCASCADETITLE:
			message = _("Drop view cascaded?");
			break;
		case DROPTITLE:
			message = _("Drop view?");
			break;
		case PROPERTIESREPORT:
			message = _("View properties report");
			message += wxT(" - ") + GetName();
			break;
		case PROPERTIES:
			message = _("View properties");
			break;
		case DDLREPORT:
			message = _("View DDL report");
			message += wxT(" - ") + GetName();
			break;
		case DDL:
			message = _("View DDL");
			break;
		case DEPENDENCIESREPORT:
			message = _("View dependencies report");
			message += wxT(" - ") + GetName();
			break;
		case DEPENDENCIES:
			message = _("View dependencies");
			break;
		case DEPENDENTSREPORT:
			message = _("View dependents report");
			message += wxT(" - ") + GetName();
			break;
		case DEPENDENTS:
			message = _("View dependents");
			break;
	}

	return message;
}


bool pgView::IsUpToDate()
{
	wxString sql = wxT("SELECT xmin FROM pg_class WHERE oid = ") + this->GetOidStr();
	if (!this->GetDatabase()->GetConnection() || this->GetDatabase()->ExecuteScalar(sql) != NumToStr(GetXid()))
		return false;
	else
		return true;
}

wxMenu *pgView::GetNewMenu()
{
	wxMenu *menu = pgObject::GetNewMenu();
	if (schema->GetCreatePrivilege())
	{
		ruleFactory.AppendMenu(menu);
		triggerFactory.AppendMenu(menu);
		if (GetMaterializedView())
		{
			indexFactory.AppendMenu(menu);
		}
	}
	return menu;
}


bool pgView::DropObject(wxFrame *frame, ctlTree *browser, bool cascaded)
{
	if (GetMaterializedView())
		sql = wxT("DROP MATERIALIZED VIEW ") + this->GetSchema()->GetQuotedIdentifier() + wxT(".") + this->GetQuotedIdentifier();
	else
		sql = wxT("DROP VIEW ") + this->GetSchema()->GetQuotedIdentifier() + wxT(".") + this->GetQuotedIdentifier();

	if (cascaded)
		sql += wxT(" CASCADE");
	return GetDatabase()->ExecuteVoid(sql);
}


wxString pgView::GetSql(ctlTree *browser)
{
	wxString withoptions;

	if (sql.IsNull())
	{
		bool IsMatViewFlag = false;
		if (!GetMaterializedView())
		{
			sql = wxT("-- View: ") + GetQuotedFullIdentifier() + wxT("\n\n")
			      + wxT("-- DROP VIEW ") + GetQuotedFullIdentifier() + wxT(";")
			      + wxT("\n\nCREATE OR REPLACE VIEW ") + GetQuotedFullIdentifier();

			if (GetConnection()->BackendMinimumVersion(9, 2) && GetSecurityBarrier().Length() > 0)
				withoptions = wxT("security_barrier=") + GetSecurityBarrier();
			if (GetConnection()->BackendMinimumVersion(9, 4) && GetCheckOption().Length() > 0)
			{
				if (withoptions.Length() > 0)
					withoptions += wxT(", ");
				withoptions = wxT("check_option=") + GetCheckOption();
			}
			if (withoptions.Length() > 0)
				sql += wxT(" WITH (") + withoptions + wxT(")");
		}
		else
		{
			sql = wxT("-- Materialized View: ") + GetQuotedFullIdentifier() + wxT("\n\n")
			      + wxT("-- DROP MATERIALIZED VIEW ") + GetQuotedFullIdentifier() + wxT(";")
			      + wxT("\n\nCREATE MATERIALIZED VIEW ") + GetQuotedFullIdentifier();

			IsMatViewFlag = true;

			if (GetConnection()->BackendMinimumVersion(9, 3))
			{
				if (GetFillFactor().Length() > 0 || GetAutoVacuumEnabled() == 1 || GetToastAutoVacuumEnabled() == 1)
				{
					bool tmpFlagTable = false;
					bool tmpFlagToastTable = false;

					sql += wxT("\nWITH (");
					if (GetFillFactor().Length() > 0)
						sql += wxT("\n  FILLFACTOR=") + GetFillFactor();
					else
						tmpFlagTable = true;

					if (GetCustomAutoVacuumEnabled())
					{
						if (GetAutoVacuumEnabled() == 1)
						{
							if (tmpFlagTable)
								sql += wxT("\n  autovacuum_enabled=true");
							else
								sql += wxT(",\n  autovacuum_enabled=true");
							tmpFlagToastTable = true;
						}
						else if (GetCustomAutoVacuumEnabled() == 0)
						{
							sql += wxT(",\n  autovacuum_enabled=false");
						}
						if (!GetAutoVacuumVacuumThreshold().IsEmpty())
						{
							sql += wxT(",\n  autovacuum_vacuum_threshold=") + GetAutoVacuumVacuumThreshold();
						}
						if (!GetAutoVacuumVacuumScaleFactor().IsEmpty())
						{
							sql += wxT(",\n  autovacuum_vacuum_scale_factor=") + GetAutoVacuumVacuumScaleFactor();
						}
						if (!GetAutoVacuumAnalyzeThreshold().IsEmpty())
						{
							sql += wxT(",\n  autovacuum_analyze_threshold=") + GetAutoVacuumAnalyzeThreshold();
						}
						if (!GetAutoVacuumAnalyzeScaleFactor().IsEmpty())
						{
							sql += wxT(",\n  autovacuum_analyze_scale_factor=") + GetAutoVacuumAnalyzeScaleFactor();
						}
						if (!GetAutoVacuumVacuumCostDelay().IsEmpty())
						{
							sql += wxT(",\n  autovacuum_vacuum_cost_delay=") + GetAutoVacuumVacuumCostDelay();
						}
						if (!GetAutoVacuumVacuumCostLimit().IsEmpty())
						{
							sql += wxT(",\n  autovacuum_vacuum_cost_limit=") + GetAutoVacuumVacuumCostLimit();
						}
						if (!GetAutoVacuumFreezeMinAge().IsEmpty())
						{
							sql += wxT(",\n  autovacuum_freeze_min_age=") + GetAutoVacuumFreezeMinAge();
						}
						if (!GetAutoVacuumFreezeMaxAge().IsEmpty())
						{
							sql += wxT(",\n  autovacuum_freeze_max_age=") + GetAutoVacuumFreezeMaxAge();
						}
						if (!GetAutoVacuumFreezeTableAge().IsEmpty())
						{
							sql += wxT(",\n  autovacuum_freeze_table_age=") + GetAutoVacuumFreezeTableAge();
						}
					}
					if (GetHasToastTable() && GetToastCustomAutoVacuumEnabled())
					{
						if (GetToastAutoVacuumEnabled() == 1)
						{
							if (tmpFlagTable && !tmpFlagToastTable)
								sql += wxT("\n  toast.autovacuum_enabled=true");
							else
								sql += wxT(",\n  toast.autovacuum_enabled=true");
						}
						else if (GetToastAutoVacuumEnabled() == 0)
							sql += wxT(",\n  toast.autovacuum_enabled=false");
						if (!GetToastAutoVacuumVacuumThreshold().IsEmpty())
						{
							sql += wxT(",\n  toast.autovacuum_vacuum_threshold=") + GetToastAutoVacuumVacuumThreshold();
						}
						if (!GetToastAutoVacuumVacuumScaleFactor().IsEmpty())
						{
							sql += wxT(",\n  toast.autovacuum_vacuum_scale_factor=") + GetToastAutoVacuumVacuumScaleFactor();
						}
						if (!GetToastAutoVacuumVacuumCostDelay().IsEmpty())
						{
							sql += wxT(",\n  toast.autovacuum_vacuum_cost_delay=") + GetToastAutoVacuumVacuumCostDelay();
						}
						if (!GetToastAutoVacuumVacuumCostLimit().IsEmpty())
						{
							sql += wxT(",\n  toast.autovacuum_vacuum_cost_limit=") + GetToastAutoVacuumVacuumCostLimit();
						}
						if (!GetToastAutoVacuumFreezeMinAge().IsEmpty())
						{
							sql += wxT(",\n  toast.autovacuum_freeze_min_age=") + GetToastAutoVacuumFreezeMinAge();
						}
						if (!GetToastAutoVacuumFreezeMaxAge().IsEmpty())
						{
							sql += wxT(",\n  toast.autovacuum_freeze_max_age=") + GetToastAutoVacuumFreezeMaxAge();
						}
						if (!GetToastAutoVacuumFreezeTableAge().IsEmpty())
						{
							sql += wxT(",\n  toast.autovacuum_freeze_table_age=") + GetToastAutoVacuumFreezeTableAge();
						}
					}
					sql += wxT("\n)");
				}

				if (tablespace != GetDatabase()->GetDefaultTablespace())
					sql += wxT("\nTABLESPACE ") + qtIdent(tablespace);

				wxString isPopulated;
				if (GetIsPopulated().Cmp(wxT("t")) == 0)
					isPopulated = wxT("WITH DATA;");
				else
					isPopulated = wxT("WITH NO DATA;");

				wxString sqlDefinition;
				bool tmpLoopFlag = true;
				sqlDefinition = GetFormattedDefinition();

				// Remove semicolon from the end of the string
				while(tmpLoopFlag)
				{
					int length = sqlDefinition.Len();
					int position = sqlDefinition.Find(';', true);
					if ((position != wxNOT_FOUND) && (position = (length - 1)))
						sqlDefinition.Remove(position, 1);
					else
						tmpLoopFlag = false;
				}

				sql += wxT(" AS \n")
				       + sqlDefinition
				       + wxT("\n")
				       + isPopulated
				       + wxT("\n\n")
				       + GetOwnerSql(7, 3, wxT("TABLE ") + GetQuotedFullIdentifier());
			}
		}

		if (!IsMatViewFlag)
		{
			sql += wxT(" AS \n")
			       + GetFormattedDefinition()
			       + wxT("\n\n")
			       + GetOwnerSql(7, 3, wxT("TABLE ") + GetQuotedFullIdentifier());
		}

		if (GetConnection()->BackendMinimumVersion(8, 2))
			sql += GetGrant(wxT("arwdxt"), wxT("TABLE ") + GetQuotedFullIdentifier());
		else
			sql += GetGrant(wxT("arwdRxt"), wxT("TABLE ") + GetQuotedFullIdentifier());

		// "MATERIALIZED" isn't part of the object type name, it's a property, so
		// we need to generate the comment SQL manually here, instead of using
		// wxString pgObject::GetCommentSql()

		if (!GetComment().IsNull())
		{
			if (IsMatViewFlag)
			{
				sql += wxT("COMMENT ON MATERIALIZED VIEW ") + GetQuotedFullIdentifier()
				       + wxT("\n  IS ") + qtDbString(GetComment()) + wxT(";\n");
			}
			else
			{
				sql += wxT("COMMENT ON VIEW ") + GetQuotedFullIdentifier()
				       + wxT("\n  IS ") + qtDbString(GetComment()) + wxT(";\n");
			}
		}

		pgCollection *columns = browser->FindCollection(columnFactory, GetId());
		if (columns)
		{
			wxString defaults, comments;
			columns->ShowTreeDetail(browser);
			treeObjectIterator colIt(browser, columns);

			pgColumn *column;
			while ((column = (pgColumn *)colIt.GetNextObject()) != 0)
			{
				column->ShowTreeDetail(browser);
				if (column->GetColNumber() > 0)
				{
					if (!column->GetDefault().IsEmpty())
					{
						defaults += wxT("ALTER TABLE ") + GetQuotedFullIdentifier()
						            +  wxT(" ALTER COLUMN ") + column->GetQuotedIdentifier()
						            +  wxT(" SET DEFAULT ") + column->GetDefault()
						            + wxT(";\n");
					}
					comments += column->GetCommentSql();
				}
			}
			if (!defaults.IsEmpty())
				sql += defaults + wxT("\n");

			if (!comments.IsEmpty())
				sql += comments + wxT("\n");

			if (GetConnection()->BackendMinimumVersion(9, 1))
				sql += GetSeqLabelsSql();
		}

		if (IsMatViewFlag)
		{
			AppendStuff(sql, browser, indexFactory);
		}
		AppendStuff(sql, browser, ruleFactory);
		AppendStuff(sql, browser, triggerFactory);
	}
	return sql;
}


wxString pgView::GetCols(ctlTree *browser, size_t indent, wxString &QMs, bool withQM)
{
	wxString sql;
	wxString line;

	int colcount = 0;
	pgSetIterator set(GetConnection(),
	                  wxT("SELECT attname\n")
	                  wxT("  FROM pg_attribute\n")
	                  wxT(" WHERE attrelid=") + GetOidStr() + wxT(" AND attnum>0\n")
	                  wxT(" ORDER BY attnum"));


	while (set.RowsLeft())
	{
		if (colcount++)
		{
			line += wxT(", ");
			QMs += wxT(", ");
		}
		if (line.Length() > 60)
		{
			if (!sql.IsEmpty())
			{
				sql += wxT("\n") + wxString(' ', indent);
			}
			sql += line;
			line = wxEmptyString;
			QMs += wxT("\n") + wxString(' ', indent);
		}

		line += qtIdent(set.GetVal(0));
		if (withQM)
			line += wxT("=?");
		QMs += wxT("?");
	}

	if (!line.IsEmpty())
	{
		if (!sql.IsEmpty())
			sql += wxT("\n") + wxString(' ', indent);
		sql += line;
	}
	return sql;
}


wxString pgView::GetSelectSql(ctlTree *browser)
{
	wxString qms;
	wxString sql =
	    wxT("SELECT ") + GetCols(browser, 7, qms, false) + wxT("\n")
	    wxT("  FROM ") + GetQuotedFullIdentifier() + wxT(";\n");
	return sql;
}


wxString pgView::GetInsertSql(ctlTree *browser)
{
	wxString qms;
	wxString sql =
	    wxT("INSERT INTO ") + GetQuotedFullIdentifier() + wxT("(\n")
	    wxT("            ") + GetCols(browser, 12, qms, false) + wxT(")\n")
	    wxT("    VALUES (") + qms + wxT(");\n");
	return sql;
}


wxString pgView::GetUpdateSql(ctlTree *browser)
{
	wxString qms;
	wxString sql =
	    wxT("UPDATE ") + GetQuotedFullIdentifier() + wxT("\n")
	    wxT("   SET ") + GetCols(browser, 7, qms, true) + wxT("\n")
	    wxT(" WHERE <condition>;\n");
	return sql;
}


void pgView::RefreshMatView(bool concurrently)
{
	wxString sql = wxT("REFRESH MATERIALIZED VIEW ");
	if (concurrently)
		sql += wxT("CONCURRENTLY ");
	sql += GetQuotedFullIdentifier();
	GetDatabase()->ExecuteVoid(sql);
}


void pgView::ShowTreeDetail(ctlTree *browser, frmMain *form, ctlListView *properties, ctlSQLBox *sqlPane)
{
	if (!expandedKids)
	{
		expandedKids = true;
		browser->RemoveDummyChild(this);

		browser->AppendCollection(this, columnFactory);

		if (GetMaterializedView())
			browser->AppendCollection(this, indexFactory);

		pgCollection *collection = browser->AppendCollection(this, ruleFactory);
		collection->iSetOid(GetOid());
		collection->ShowTreeDetail(browser);
		treeObjectIterator colIt(browser, collection);

		pgRule *rule;
		while (!hasInsertRule && !hasUpdateRule && !hasDeleteRule && (rule = (pgRule *)colIt.GetNextObject()) != 0)
		{
			if (rule->GetEvent().Find(wxT("INSERT")) >= 0)
				hasInsertRule = true;
			if (rule->GetEvent().Find(wxT("UPDATE")) >= 0)
				hasUpdateRule = true;
			if (rule->GetEvent().Find(wxT("DELETE")) >= 0)
				hasDeleteRule = true;
		}

		if (GetConnection()->BackendMinimumVersion(9, 1))
			browser->AppendCollection(this, triggerFactory);
	}
	if (properties)
	{
		CreateListColumns(properties);
		wxString def = GetDefinition().Left(250);
		def.Replace(wxT("\n"), wxT(" "));

		properties->AppendItem(_("Name"), GetName());
		properties->AppendItem(_("OID"), GetOid());
		properties->AppendItem(_("Owner"), GetOwner());
		properties->AppendItem(_("ACL"), GetAcl());
		properties->AppendItem(_("Definition"), def);
		properties->AppendYesNoItem(_("System view?"), GetSystemObject());
		if (GetConnection()->BackendMinimumVersion(9, 2) && GetSecurityBarrier().Length() > 0)
			properties->AppendItem(_("Security barrier?"), GetSecurityBarrier());

		if (GetConnection()->BackendMinimumVersion(9, 3))
			properties->AppendYesNoItem(_("Materialized view?"), GetMaterializedView());

		/* Custom AutoVacuum Settings */
		if (GetConnection()->BackendMinimumVersion(9, 3) && GetMaterializedView())
		{
			if (!GetFillFactor().IsEmpty())
				properties->AppendItem(_("Fill factor"), GetFillFactor());

			if (GetCustomAutoVacuumEnabled())
			{
				if (GetAutoVacuumEnabled() != 2)
				{
					properties->AppendItem(_("Table auto-vacuum enabled?"), GetAutoVacuumEnabled() == 1 ? _("Yes") : _("No"));
				}
				if (!GetAutoVacuumVacuumThreshold().IsEmpty())
					properties->AppendItem(_("Table auto-vacuum VACUUM base threshold"), GetAutoVacuumVacuumThreshold());
				if (!GetAutoVacuumVacuumScaleFactor().IsEmpty())
					properties->AppendItem(_("Table auto-vacuum VACUUM scale factor"), GetAutoVacuumVacuumScaleFactor());
				if (!GetAutoVacuumAnalyzeThreshold().IsEmpty())
					properties->AppendItem(_("Table auto-vacuum ANALYZE base threshold"), GetAutoVacuumAnalyzeThreshold());
				if (!GetAutoVacuumAnalyzeScaleFactor().IsEmpty())
					properties->AppendItem(_("Table auto-vacuum ANALYZE scale factor"), GetAutoVacuumAnalyzeScaleFactor());
				if (!GetAutoVacuumVacuumCostDelay().IsEmpty())
					properties->AppendItem(_("Table auto-vacuum VACUUM cost delay"), GetAutoVacuumVacuumCostDelay());
				if (!GetAutoVacuumVacuumCostLimit().IsEmpty())
					properties->AppendItem(_("Table auto-vacuum VACUUM cost limit"), GetAutoVacuumVacuumCostLimit());
				if (!GetAutoVacuumFreezeMinAge().IsEmpty())
					properties->AppendItem(_("Table auto-vacuum FREEZE minimum age"), GetAutoVacuumFreezeMinAge());
				if (!GetAutoVacuumFreezeMaxAge().IsEmpty())
					properties->AppendItem(_("Table auto-vacuum FREEZE maximum age"), GetAutoVacuumFreezeMaxAge());
				if (!GetAutoVacuumFreezeTableAge().IsEmpty())
					properties->AppendItem(_("Table auto-vacuum FREEZE table age"), GetAutoVacuumFreezeTableAge());
			}

			if (GetHasToastTable() && GetToastCustomAutoVacuumEnabled())
			{
				if (GetToastAutoVacuumEnabled() != 2)
				{
					properties->AppendItem(_("Toast auto-vacuum enabled?"), GetToastAutoVacuumEnabled() == 1 ? _("Yes") : _("No"));
				}
				if (!GetToastAutoVacuumVacuumThreshold().IsEmpty())
					properties->AppendItem(_("Toast auto-vacuum VACUUM base threshold"), GetToastAutoVacuumVacuumThreshold());
				if (!GetToastAutoVacuumVacuumScaleFactor().IsEmpty())
					properties->AppendItem(_("Toast auto-vacuum VACUUM scale factor"), GetToastAutoVacuumVacuumScaleFactor());
				if (!GetToastAutoVacuumVacuumCostDelay().IsEmpty())
					properties->AppendItem(_("Toast auto-vacuum VACUUM cost delay"), GetToastAutoVacuumVacuumCostDelay());
				if (!GetToastAutoVacuumVacuumCostLimit().IsEmpty())
					properties->AppendItem(_("Toast auto-vacuum VACUUM cost limit"), GetToastAutoVacuumVacuumCostLimit());
				if (!GetToastAutoVacuumFreezeMinAge().IsEmpty())
					properties->AppendItem(_("Toast auto-vacuum FREEZE minimum age"), GetToastAutoVacuumFreezeMinAge());
				if (!GetToastAutoVacuumFreezeMaxAge().IsEmpty())
					properties->AppendItem(_("Toast auto-vacuum FREEZE maximum age"), GetToastAutoVacuumFreezeMaxAge());
				if (!GetToastAutoVacuumFreezeTableAge().IsEmpty())
					properties->AppendItem(_("Toast auto-vacuum FREEZE table age"), GetToastAutoVacuumFreezeTableAge());
			}

			properties->AppendItem(_("Tablespace"), tablespace);

			if (GetIsPopulated().Cmp(wxT("t")) == 0)
				properties->AppendItem(_("With data?"), _("Yes"));
			else
				properties->AppendItem(_("With data?"), _("No"));
		}

		if (GetConnection()->BackendMinimumVersion(9, 4))
			properties->AppendItem(_("Check Option"), GetCheckOption());

		if (!GetLabels().IsEmpty())
		{
			wxArrayString seclabels = GetProviderLabelArray();
			if (seclabels.GetCount() > 0)
			{
				for (unsigned int index = 0 ; index < seclabels.GetCount() - 1 ; index += 2)
				{
					properties->AppendItem(seclabels.Item(index), seclabels.Item(index + 1));
				}
			}
		}

		properties->AppendItem(_("Comment"), firstLineOnly(GetComment()));
	}
}



pgObject *pgView::Refresh(ctlTree *browser, const wxTreeItemId item)
{
	pgObject *view = 0;
	pgCollection *coll = browser->GetParentCollection(item);
	if (coll)
	{
		// OIDs may change in EDB which allows the returned column set to be changed.
		if (GetConnection()->EdbMinimumVersion(8, 0))
			view = viewFactory.CreateObjects(coll, 0, wxT("\n   AND c.relname=") + GetConnection()->qtDbString(GetName()));
		else
			view = viewFactory.CreateObjects(coll, 0, wxT("\n   AND c.oid=") + GetOidStr());
	}

	return view;
}

void pgView::ShowHint(frmMain *form, bool force)
{
	wxArrayString hints;
	hints.Add(HINT_OBJECT_EDITING);
	frmHint::ShowHint((wxWindow *)form, hints, GetFullIdentifier(), force);
}

void pgView::AppendStuff(wxString &sql, ctlTree *browser, pgaFactory &factory)
{
	wxString tmp;

	pgCollection *collection = browser->FindCollection(factory, GetId());
	if (collection)
	{
		tmp += wxT("\n");
		collection->ShowTreeDetail(browser);

		treeObjectIterator idxIt(browser, collection);
		pgObject *obj;
		while ((obj = idxIt.GetNextObject()) != 0)
		{
			if (obj->GetName() != wxT("_RETURN"))
			{
				obj->ShowTreeDetail(browser);

				tmp += obj->GetSql(browser) + wxT("\n");
			}
		}
	}

	if (!tmp.IsEmpty() && tmp != wxT("\n"))
		sql += tmp;
}

int pgView::GetIconId()
{
	if (GetMaterializedView())
		return viewFactory.GetMaterializedIconId();
	else
		return viewFactory.GetIconId();
}

///////////////////////////////////////////////////


pgViewCollection::pgViewCollection(pgaFactory *factory, pgSchema *sch)
	: pgSchemaObjCollection(factory, sch)
{
}


wxString pgViewCollection::GetTranslatedMessage(int kindOfMessage) const
{
	wxString message = wxEmptyString;

	switch (kindOfMessage)
	{
		case RETRIEVINGDETAILS:
			message = _("Retrieving details on views");
			break;
		case REFRESHINGDETAILS:
			message = _("Refreshing views");
			break;
		case GRANTWIZARDTITLE:
			message = _("Privileges for views");
			break;
		case OBJECTSLISTREPORT:
			message = _("Views list report");
			break;
	}

	return message;
}


///////////////////////////////////////////////////////


pgObject *pgViewFactory::CreateObjects(pgCollection *collection, ctlTree *browser, const wxString &restriction)
{
	pgView *view = 0;
	wxString sql;

	if (collection->GetDatabase()->BackendMinimumVersion(9, 3))
	{
		sql = wxT("SELECT c.oid, c.xmin, c.relname,c.reltablespace AS spcoid, c.relkind, c.relispopulated AS ispopulated,spc.spcname, pg_get_userbyid(c.relowner) AS viewowner, c.relacl, description, ")
		      wxT("pg_get_viewdef(c.oid") + collection->GetDatabase()->GetPrettyOption() + wxT(") AS definition");
	}
	else
	{
		sql = wxT("SELECT c.oid, c.xmin, c.relname,c.reltablespace AS spcoid,spc.spcname, pg_get_userbyid(c.relowner) AS viewowner, c.relacl, description, ")
		      wxT("pg_get_viewdef(c.oid") + collection->GetDatabase()->GetPrettyOption() + wxT(") AS definition");
	}

	if (collection->GetDatabase()->BackendMinimumVersion(9, 1))
	{
		sql += wxT(",\n(SELECT array_agg(label) FROM pg_seclabels sl1 WHERE sl1.objoid=c.oid AND sl1.objsubid=0) AS labels");
		sql += wxT(",\n(SELECT array_agg(provider) FROM pg_seclabels sl2 WHERE sl2.objoid=c.oid AND sl2.objsubid=0) AS providers");
	}
	if (collection->GetConnection()->BackendMinimumVersion(9, 2))
	{
		sql += wxT(",\nsubstring(array_to_string(c.reloptions, ',') FROM 'security_barrier=([a-z|0-9]*)') AS security_barrier");
	}

	if (collection->GetConnection()->BackendMinimumVersion(9, 3))
	{
		sql += wxT(", substring(array_to_string(c.reloptions, ',') FROM 'fillfactor=([0-9]*)') AS fillfactor \n");

		sql += wxT(", substring(array_to_string(c.reloptions, ',') FROM 'autovacuum_enabled=([a-z|0-9]*)') AS autovacuum_enabled \n")
		       wxT(", substring(array_to_string(c.reloptions, ',') FROM 'autovacuum_vacuum_threshold=([0-9]*)') AS autovacuum_vacuum_threshold \n")
		       wxT(", substring(array_to_string(c.reloptions, ',') FROM 'autovacuum_vacuum_scale_factor=([0-9]*[.][0-9]*)') AS autovacuum_vacuum_scale_factor \n")
		       wxT(", substring(array_to_string(c.reloptions, ',') FROM 'autovacuum_analyze_threshold=([0-9]*)') AS autovacuum_analyze_threshold \n")
		       wxT(", substring(array_to_string(c.reloptions, ',') FROM 'autovacuum_analyze_scale_factor=([0-9]*[.][0-9]*)') AS autovacuum_analyze_scale_factor \n")
		       wxT(", substring(array_to_string(c.reloptions, ',') FROM 'autovacuum_vacuum_cost_delay=([0-9]*)') AS autovacuum_vacuum_cost_delay \n")
		       wxT(", substring(array_to_string(c.reloptions, ',') FROM 'autovacuum_vacuum_cost_limit=([0-9]*)') AS autovacuum_vacuum_cost_limit \n")
		       wxT(", substring(array_to_string(c.reloptions, ',') FROM 'autovacuum_freeze_min_age=([0-9]*)') AS autovacuum_freeze_min_age \n")
		       wxT(", substring(array_to_string(c.reloptions, ',') FROM 'autovacuum_freeze_max_age=([0-9]*)') AS autovacuum_freeze_max_age \n")
		       wxT(", substring(array_to_string(c.reloptions, ',') FROM 'autovacuum_freeze_table_age=([0-9]*)') AS autovacuum_freeze_table_age \n")
		       wxT(", substring(array_to_string(tst.reloptions, ',') FROM 'autovacuum_enabled=([a-z|0-9]*)') AS toast_autovacuum_enabled \n")
		       wxT(", substring(array_to_string(tst.reloptions, ',') FROM 'autovacuum_vacuum_threshold=([0-9]*)') AS toast_autovacuum_vacuum_threshold \n")
		       wxT(", substring(array_to_string(tst.reloptions, ',') FROM 'autovacuum_vacuum_scale_factor=([0-9]*[.][0-9]*)') AS toast_autovacuum_vacuum_scale_factor \n")
		       wxT(", substring(array_to_string(tst.reloptions, ',') FROM 'autovacuum_analyze_threshold=([0-9]*)') AS toast_autovacuum_analyze_threshold \n")
		       wxT(", substring(array_to_string(tst.reloptions, ',') FROM 'autovacuum_analyze_scale_factor=([0-9]*[.][0-9]*)') AS toast_autovacuum_analyze_scale_factor \n")
		       wxT(", substring(array_to_string(tst.reloptions, ',') FROM 'autovacuum_vacuum_cost_delay=([0-9]*)') AS toast_autovacuum_vacuum_cost_delay \n")
		       wxT(", substring(array_to_string(tst.reloptions, ',') FROM 'autovacuum_vacuum_cost_limit=([0-9]*)') AS toast_autovacuum_vacuum_cost_limit \n")
		       wxT(", substring(array_to_string(tst.reloptions, ',') FROM 'autovacuum_freeze_min_age=([0-9]*)') AS toast_autovacuum_freeze_min_age \n")
		       wxT(", substring(array_to_string(tst.reloptions, ',') FROM 'autovacuum_freeze_max_age=([0-9]*)') AS toast_autovacuum_freeze_max_age \n")
		       wxT(", substring(array_to_string(tst.reloptions, ',') FROM 'autovacuum_freeze_table_age=([0-9]*)') AS toast_autovacuum_freeze_table_age \n")
		       wxT(", c.reloptions AS reloptions, tst.reloptions AS toast_reloptions \n")
		       wxT(", (CASE WHEN c.reltoastrelid = 0 THEN false ELSE true END) AS hastoasttable\n");
	}
	if (collection->GetConnection()->BackendMinimumVersion(9, 4))
	{
		sql += wxT(",\nsubstring(array_to_string(c.reloptions, ',') FROM 'check_option=([a-z]*)') AS check_option");
	}



	sql += wxT("\n  FROM pg_class c\n")
	       wxT("  LEFT OUTER JOIN pg_tablespace spc on spc.oid=c.reltablespace\n")
	       wxT("  LEFT OUTER JOIN pg_description des ON (des.objoid=c.oid and des.objsubid=0 AND des.classoid='pg_class'::regclass)\n");

	// Add the toast table for vacuum parameters.
	if (collection->GetConnection()->BackendMinimumVersion(9, 3))
		sql += wxT("  LEFT OUTER JOIN pg_class tst ON tst.oid = c.reltoastrelid\n");

	sql += wxT(" WHERE ((c.relhasrules AND (EXISTS (\n")
	       wxT("           SELECT r.rulename FROM pg_rewrite r\n")
	       wxT("            WHERE ((r.ev_class = c.oid)\n")
	       wxT("              AND (bpchar(r.ev_type) = '1'::bpchar)) ))) OR (c.relkind = 'v'::char))\n")
	       wxT("   AND c.relnamespace = ") + collection->GetSchema()->GetOidStr() + wxT("\n")
	       + restriction
	       + wxT(" ORDER BY relname");

	pgSet *views = collection->GetDatabase()->ExecuteSet(sql);

	if (views)
	{
		while (!views->Eof())
		{
			view = new pgView(collection->GetSchema(), views->GetVal(wxT("relname")));

			view->iSetOid(views->GetOid(wxT("oid")));
			view->iSetXid(views->GetOid(wxT("xmin")));
			view->iSetOwner(views->GetVal(wxT("viewowner")));
			view->iSetComment(views->GetVal(wxT("description")));
			view->iSetAcl(views->GetVal(wxT("relacl")));
			view->iSetDefinition(views->GetVal(wxT("definition")));
			view->iSetMaterializedView(false);
			if (collection->GetDatabase()->BackendMinimumVersion(9, 4))
			{
				view->iSetCheckOption(views->GetVal(wxT("check_option")));
			}

			if (collection->GetDatabase()->BackendMinimumVersion(9, 1))
			{
				view->iSetProviders(views->GetVal(wxT("providers")));
				view->iSetLabels(views->GetVal(wxT("labels")));
			}
			if (collection->GetConnection()->BackendMinimumVersion(9, 2))
			{
				view->iSetSecurityBarrier(views->GetVal(wxT("security_barrier")));
			}

			if (collection->GetConnection()->BackendMinimumVersion(9, 3))
			{
				view->iSetFillFactor(views->GetVal(wxT("fillfactor")));

				if (views->GetOid(wxT("spcoid")) == 0)
					view->iSetTablespaceOid(collection->GetDatabase()->GetTablespaceOid());
				else
					view->iSetTablespaceOid(views->GetOid(wxT("spcoid")));

				view->iSetRelOptions(views->GetVal(wxT("reloptions")));

				view->iSetIsPopulated(views->GetVal(wxT("ispopulated")));

				if (view->GetCustomAutoVacuumEnabled())
				{
					if (views->GetVal(wxT("autovacuum_enabled")).IsEmpty())
						view->iSetAutoVacuumEnabled(2);
					else if (views->GetBool(wxT("autovacuum_enabled")))
						view->iSetAutoVacuumEnabled(1);
					else
						view->iSetAutoVacuumEnabled(0);
					view->iSetAutoVacuumVacuumThreshold(views->GetVal(wxT("autovacuum_vacuum_threshold")));
					view->iSetAutoVacuumVacuumScaleFactor(views->GetVal(wxT("autovacuum_vacuum_scale_factor")));
					view->iSetAutoVacuumAnalyzeThreshold(views->GetVal(wxT("autovacuum_analyze_threshold")));
					view->iSetAutoVacuumAnalyzeScaleFactor(views->GetVal(wxT("autovacuum_analyze_scale_factor")));
					view->iSetAutoVacuumVacuumCostDelay(views->GetVal(wxT("autovacuum_vacuum_cost_delay")));
					view->iSetAutoVacuumVacuumCostLimit(views->GetVal(wxT("autovacuum_vacuum_cost_limit")));
					view->iSetAutoVacuumFreezeMinAge(views->GetVal(wxT("autovacuum_freeze_min_age")));
					view->iSetAutoVacuumFreezeMaxAge(views->GetVal(wxT("autovacuum_freeze_max_age")));
					view->iSetAutoVacuumFreezeTableAge(views->GetVal(wxT("autovacuum_freeze_table_age")));
				}

				view->iSetHasToastTable(views->GetBool(wxT("hastoasttable")));

				if (view->GetHasToastTable())
				{
					view->iSetToastRelOptions(views->GetVal(wxT("toast_reloptions")));

					if (view->GetToastCustomAutoVacuumEnabled())
					{
						if (views->GetVal(wxT("toast_autovacuum_enabled")).IsEmpty())
							view->iSetToastAutoVacuumEnabled(2);
						else if (views->GetBool(wxT("toast_autovacuum_enabled")))
							view->iSetToastAutoVacuumEnabled(1);
						else
							view->iSetToastAutoVacuumEnabled(0);

						view->iSetToastAutoVacuumVacuumThreshold(views->GetVal(wxT("toast_autovacuum_vacuum_threshold")));
						view->iSetToastAutoVacuumVacuumScaleFactor(views->GetVal(wxT("toast_autovacuum_vacuum_scale_factor")));
						view->iSetToastAutoVacuumVacuumCostDelay(views->GetVal(wxT("toast_autovacuum_vacuum_cost_delay")));
						view->iSetToastAutoVacuumVacuumCostLimit(views->GetVal(wxT("toast_autovacuum_vacuum_cost_limit")));
						view->iSetToastAutoVacuumFreezeMinAge(views->GetVal(wxT("toast_autovacuum_freeze_min_age")));
						view->iSetToastAutoVacuumFreezeMaxAge(views->GetVal(wxT("toast_autovacuum_freeze_max_age")));
						view->iSetToastAutoVacuumFreezeTableAge(views->GetVal(wxT("toast_autovacuum_freeze_table_age")));
					}
				}

				if (views->GetVal(wxT("spcname")) == wxEmptyString)
					view->iSetTablespace(collection->GetDatabase()->GetTablespace());
				else
					view->iSetTablespace(views->GetVal(wxT("spcname")));

				if (views->GetVal(wxT("relkind")).Cmp(wxT("m")) == 0)
					view->iSetMaterializedView(true);
			}

			if (browser)
			{
				collection->AppendBrowserItem(browser, view);
				// If it is materialized view then display the materialized view icon
				if (collection->GetConnection()->BackendMinimumVersion(9, 3) && views->GetVal(wxT("relkind")).Cmp(wxT("m")) == 0)
					browser->SetItemImage(view->GetId(), viewFactory.GetMaterializedIconId());

				views->MoveNext();
			}
			else
				break;
		}

		delete views;
	}
	return view;
}


#include "images/view.pngc"
#include "images/view-sm.pngc"
#include "images/views.pngc"
#include "images/mview.pngc"
#include "images/mview-sm.pngc"

pgViewFactory::pgViewFactory()
	: pgSchemaObjFactory(__("View"), __("New View..."), __("Create a new View."), view_png_img, view_sm_png_img)
{
	metaType = PGM_VIEW;
	materializedId = addIcon(mview_png_img);
	smallMaterializedId = addIcon(mview_sm_png_img);
}


pgCollection *pgViewFactory::CreateCollection(pgObject *obj)
{
	return new pgViewCollection(GetCollectionFactory(), (pgSchema *)obj);
}

pgViewFactory viewFactory;
static pgaCollectionFactory cf(&viewFactory, __("Views"), views_png_img);

refreshMatViewFactory::refreshMatViewFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar) : contextActionFactory(list)
{
	mnu->Append(id, _("&Refresh data"), _("Refresh data for the selected object."));
}


wxWindow *refreshMatViewFactory::StartDialog(frmMain *form, pgObject *obj)
{
	form->StartMsg(_("Refreshing data"));

	((pgView *)obj)->RefreshMatView(false);
	wxTreeItemId item = form->GetBrowser()->GetSelection();
	if (obj == form->GetBrowser()->GetObject(item))
		obj->ShowTreeDetail(form->GetBrowser(), 0, form->GetProperties());

	form->EndMsg();

	return 0;
}


bool refreshMatViewFactory::CheckEnable(pgObject *obj)
{
	return obj && obj->IsCreatedBy(viewFactory) && ((pgView *)obj)->GetMaterializedView();
}


refreshConcurrentlyMatViewFactory::refreshConcurrentlyMatViewFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar) : contextActionFactory(list)
{
	mnu->Append(id, _("&Refresh data concurrently"), _("Refresh data concurrently for the selected object."));
}


wxWindow *refreshConcurrentlyMatViewFactory::StartDialog(frmMain *form, pgObject *obj)
{
	form->StartMsg(_("Refreshing data concurrently"));

	((pgView *)obj)->RefreshMatView(true);
	wxTreeItemId item = form->GetBrowser()->GetSelection();
	if (obj == form->GetBrowser()->GetObject(item))
		obj->ShowTreeDetail(form->GetBrowser(), 0, form->GetProperties());

	form->EndMsg();

	return 0;
}


bool refreshConcurrentlyMatViewFactory::CheckEnable(pgObject *obj)
{
	return obj && obj->IsCreatedBy(viewFactory)
	       && ((pgView *)obj)->GetMaterializedView()
	       && ((pgView *)obj)->GetConnection()->BackendMinimumVersion(9, 4);;
}
