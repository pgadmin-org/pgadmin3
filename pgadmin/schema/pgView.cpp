//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2012, The pgAdmin Development Team
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
#include "schema/pgView.h"
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
	}
	return menu;
}


bool pgView::DropObject(wxFrame *frame, ctlTree *browser, bool cascaded)
{
	wxString sql = wxT("DROP VIEW ") + this->GetSchema()->GetQuotedIdentifier() + wxT(".") + this->GetQuotedIdentifier();
	if (cascaded)
		sql += wxT(" CASCADE");
	return GetDatabase()->ExecuteVoid(sql);
}


wxString pgView::GetSql(ctlTree *browser)
{
	if (sql.IsNull())
	{
		sql = wxT("-- View: ") + GetQuotedFullIdentifier() + wxT("\n\n")
		      + wxT("-- DROP VIEW ") + GetQuotedFullIdentifier() + wxT(";")
		      + wxT("\n\nCREATE OR REPLACE VIEW ") + GetQuotedFullIdentifier();
		if (GetConnection()->BackendMinimumVersion(9, 2) && GetSecurityBarrier().Length() > 0)
			sql += wxT(" WITH (security_barrier=") + GetSecurityBarrier() + wxT(")");
		sql += wxT(" AS \n")
		       + GetFormattedDefinition()
		       + wxT("\n\n")
		       + GetOwnerSql(7, 3, wxT("TABLE ") + GetQuotedFullIdentifier());

		if (GetConnection()->BackendMinimumVersion(8, 2))
			sql += GetGrant(wxT("arwdxt"), wxT("TABLE ") + GetQuotedFullIdentifier());
		else
			sql += GetGrant(wxT("arwdRxt"), wxT("TABLE ") + GetQuotedFullIdentifier());

		sql += GetCommentSql()
		       + wxT("\n");

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

void pgView::ShowTreeDetail(ctlTree *browser, frmMain *form, ctlListView *properties, ctlSQLBox *sqlPane)
{
	if (!expandedKids)
	{
		expandedKids = true;
		browser->RemoveDummyChild(this);

		browser->AppendCollection(this, columnFactory);

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
		properties->AppendItem(_("Comment"), firstLineOnly(GetComment()));
		if (GetConnection()->BackendMinimumVersion(9, 2) && GetSecurityBarrier().Length() > 0)
			properties->AppendItem(_("Security barrier?"), GetSecurityBarrier());

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
	wxString sql = wxT("SELECT c.oid, c.xmin, c.relname, pg_get_userbyid(c.relowner) AS viewowner, c.relacl, description, ")
	               wxT("pg_get_viewdef(c.oid") + collection->GetDatabase()->GetPrettyOption() + wxT(") AS definition");
	if (collection->GetDatabase()->BackendMinimumVersion(9, 1))
	{
		sql += wxT(",\n(SELECT array_agg(label) FROM pg_seclabels sl1 WHERE sl1.objoid=c.oid AND sl1.objsubid=0) AS labels");
		sql += wxT(",\n(SELECT array_agg(provider) FROM pg_seclabels sl2 WHERE sl2.objoid=c.oid AND sl2.objsubid=0) AS providers");
	}
	if (collection->GetConnection()->BackendMinimumVersion(9, 2))
	{
		sql += wxT(",\nsubstring(array_to_string(c.reloptions, ',') FROM 'security_barrier=([a-z|0-9]*)') AS security_barrier");
	}
	sql += wxT("\n  FROM pg_class c\n")
	       wxT("  LEFT OUTER JOIN pg_description des ON (des.objoid=c.oid and des.objsubid=0 AND des.classoid='pg_class'::regclass)\n")
	       wxT(" WHERE ((c.relhasrules AND (EXISTS (\n")
	       wxT("           SELECT r.rulename FROM pg_rewrite r\n")
	       wxT("            WHERE ((r.ev_class = c.oid)\n")
	       wxT("              AND (bpchar(r.ev_type) = '1'::bpchar)) ))) OR (c.relkind = 'v'::char))\n")
	       wxT("   AND relnamespace = ") + collection->GetSchema()->GetOidStr() + wxT("\n")
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

			if (collection->GetDatabase()->BackendMinimumVersion(9, 1))
			{
				view->iSetProviders(views->GetVal(wxT("providers")));
				view->iSetLabels(views->GetVal(wxT("labels")));
			}
			if (collection->GetConnection()->BackendMinimumVersion(9, 2))
			{
				view->iSetSecurityBarrier(views->GetVal(wxT("security_barrier")));
			}
			if (browser)
			{
				collection->AppendBrowserItem(browser, view);
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

pgViewFactory::pgViewFactory()
	: pgSchemaObjFactory(__("View"), __("New View..."), __("Create a new View."), view_png_img, view_sm_png_img)
{
	metaType = PGM_VIEW;
}


pgCollection *pgViewFactory::CreateCollection(pgObject *obj)
{
	return new pgViewCollection(GetCollectionFactory(), (pgSchema *)obj);
}

pgViewFactory viewFactory;
static pgaCollectionFactory cf(&viewFactory, __("Views"), views_png_img);
