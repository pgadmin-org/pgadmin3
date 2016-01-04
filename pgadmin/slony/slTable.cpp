//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// slTable.cpp PostgreSQL Slony-I table
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "utils/misc.h"
#include "slony/slTable.h"
#include "frm/frmMain.h"


slTable::slTable(slSet *s, const wxString &newName)
	: slSetObject(s, slTableFactory, newName)
{
}

bool slTable::DropObject(wxFrame *frame, ctlTree *browser, bool cascaded)
{
	return GetDatabase()->ExecuteVoid(
	           wxT("SELECT ") + GetCluster()->GetSchemaPrefix()
	           + wxT("setdroptable(") + NumToStr(GetSlId()) + wxT(");\n"));
}


wxString slTable::GetTranslatedMessage(int kindOfMessage) const
{
	wxString message = wxEmptyString;

	switch (kindOfMessage)
	{
		case RETRIEVINGDETAILS:
			message = _("Retrieving details on Slony table");
			message += wxT(" ") + GetName();
			break;
		case REFRESHINGDETAILS:
			message = _("Refreshing Slony table");
			message += wxT(" ") + GetName();
			break;
		case DROPINCLUDINGDEPS:
			message = wxString::Format(_("Are you sure you wish to drop Slony table \"%s\" including all objects that depend on it?"),
			                           GetFullIdentifier().c_str());
			break;
		case DROPEXCLUDINGDEPS:
			message = wxString::Format(_("Are you sure you wish to drop Slony table \"%s\"?"),
			                           GetFullIdentifier().c_str());
			break;
		case DROPCASCADETITLE:
			message = _("Drop Slony table cascaded?");
			break;
		case DROPTITLE:
			message = _("Drop Slony table?");
			break;
		case PROPERTIESREPORT:
			message = _("Slony table properties report");
			message += wxT(" - ") + GetName();
			break;
		case PROPERTIES:
			message = _("Slony table properties");
			break;
		case DDLREPORT:
			message = _("Slony table DDL report");
			message += wxT(" - ") + GetName();
			break;
		case DDL:
			message = _("Slony table DDL");
			break;
		case DEPENDENCIESREPORT:
			message = _("Slony table dependencies report");
			message += wxT(" - ") + GetName();
			break;
		case DEPENDENCIES:
			message = _("Slony table dependencies");
			break;
		case DEPENDENTSREPORT:
			message = _("Slony table dependents report");
			message += wxT(" - ") + GetName();
			break;
		case DEPENDENTS:
			message = _("Slony table dependents");
			break;
	}

	return message;
}

wxString slTable::GetSql(ctlTree *browser)
{
	if (sql.IsNull())
	{
		sql = wxT("-- Register table ") + GetName() + wxT(" for replication.\n\n")
		      wxT("SELECT ") + GetCluster()->GetSchemaPrefix() + wxT("setaddtable(")
		      + NumToStr(GetSet()->GetSlId()) + wxT(", ")
		      + NumToStr(GetSlId()) + wxT(", ")
		      + qtDbString(GetName()) + wxT(", ")
		      + qtDbString(GetIndexName()) + wxT(", ")
		      + qtDbString(GetComment()) + wxT(");\n");


		size_t i;
		for (i = 0 ; i < triggers.GetCount() ; i++)
		{
			sql += wxT("SELECT ") + GetCluster()->GetSchemaPrefix() + wxT("storetrigger(")
			       + NumToStr(GetSlId()) + wxT(", ")
			       + qtDbString(triggers[i]) + wxT(");\n");
		}
	}
	return sql;
}


void slTable::ShowTreeDetail(ctlTree *browser, frmMain *form, ctlListView *properties, ctlSQLBox *sqlPane)
{
	if (!expandedKids)
	{
		expandedKids = true;
		pgSet *set;

		if (GetConnection()->BackendMinimumVersion(9, 0))
		{
			set = GetConnection()->ExecuteSet(
			          wxT("SELECT tgname AS trig_tgname FROM pg_trigger t, pg_proc p, pg_namespace n ")
			          wxT("WHERE t.tgfoid = p.oid AND p.pronamespace = n.oid ")
			          wxT("AND n.nspname = ") + qtDbString(wxT("_") + GetCluster()->GetName()));
		}
		else
		{
			set = GetConnection()->ExecuteSet(
			          wxT("SELECT trig_tgname\n")
			          wxT("  FROM ") + GetCluster()->GetSchemaPrefix() + wxT("sl_trigger\n")
			          wxT(" WHERE trig_tabid = ") + NumToStr(GetSlId()));
		}

		if (set)
		{
			while (!set->Eof())
			{
				triggers.Add(set->GetVal(wxT("trig_tgname")));
				set->MoveNext();
			}
			delete set;
		}
	}

	if (properties)
	{
		CreateListColumns(properties);

		properties->AppendItem(_("Name"), GetName());
		properties->AppendItem(_("ID"), GetSlId());
		properties->AppendItem(_("Index Name"), GetIndexName());
		properties->AppendYesNoItem(_("Altered"), GetAltered());

		if (triggers.GetCount() > 0)
		{
			size_t i;
			wxString triglist;
			for (i = 0; i < triggers.GetCount() ; i++)
				triglist += triggers[i] + wxT(", ");

			properties->AppendItem(_("Triggers"), triglist.BeforeLast(','));
		}

		properties->AppendItem(_("Comment"), firstLineOnly(GetComment()));
	}
}



pgObject *slTable::Refresh(ctlTree *browser, const wxTreeItemId item)
{
	pgObject *table = 0;
	pgCollection *coll = browser->GetParentCollection(item);
	if (coll)
		table = slTableFactory.CreateObjects(coll, 0, wxT(" WHERE tab_id=") + NumToStr(GetSlId()) + wxT("\n"));

	return table;
}



pgObject *slSlTableFactory::CreateObjects(pgCollection *coll, ctlTree *browser, const wxString &restr)
{
	slSetObjCollection *collection = (slSetObjCollection *)coll;
	slTable *table = 0;
	wxString restriction;
	if (restr.IsEmpty())
		restriction = wxT(" WHERE tab_set = ") + NumToStr(collection->GetSlId());
	else
		restriction = restr;

	pgSet *tables = collection->GetDatabase()->ExecuteSet(
	                    wxT("SELECT tab_id, tab_reloid, tab_set, nspname, relname, tab_idxname, tab_altered, tab_comment")
	                    wxT("  FROM ") + collection->GetCluster()->GetSchemaPrefix() + wxT("sl_table\n")
	                    wxT("  JOIN ") + collection->GetCluster()->GetSchemaPrefix() + wxT("sl_set ON set_id=tab_set\n")
	                    wxT("  JOIN pg_class cl ON cl.oid=tab_reloid\n")
	                    wxT("  JOIN pg_namespace nsp ON nsp.oid=relnamespace\n")
	                    + restriction +
	                    wxT(" ORDER BY tab_id"));

	if (tables)
	{
		while (!tables->Eof())
		{
			table = new slTable(collection->GetSet(), tables->GetVal(wxT("nspname")) + wxT(".") + tables->GetVal(wxT("relname")));
			table->iSetSlId(tables->GetLong(wxT("tab_id")));
			table->iSetIndexName(tables->GetVal(wxT("tab_idxname")));
			table->iSetComment(tables->GetVal(wxT("tab_comment")));
			table->iSetAltered(tables->GetBool(wxT("tab_altered")));
			table->iSetOid(tables->GetOid(wxT("tab_reloid")));

			if (browser)
			{
				browser->AppendObject(collection, table);
				tables->MoveNext();
			}
			else
				break;
		}

		delete tables;
	}
	return table;
}


wxString slSlTableCollection::GetTranslatedMessage(int kindOfMessage) const
{
	wxString message = wxEmptyString;

	switch (kindOfMessage)
	{
		case RETRIEVINGDETAILS:
			message = _("Retrieving details on Slony tables");
			break;
		case REFRESHINGDETAILS:
			message = _("Refreshing Slony tables");
			break;
		case OBJECTSLISTREPORT:
			message = _("Slony tables list report");
			break;
	}

	return message;
}


///////////////////////////////////////////////////

#include "images/table-repl.pngc"
#include "images/table-repl-sm.pngc"
#include "images/tables.pngc"

slSlTableFactory::slSlTableFactory()
	: slSetObjFactory(__("Table"), __("New Table"), __("Create a new Table."), table_repl_png_img, table_repl_sm_png_img)
{
	metaType = SLM_TABLE;
}


pgCollection *slSlTableFactory::CreateCollection(pgObject *obj)
{
	return new slSlTableCollection(GetCollectionFactory(), (slSet *)obj);
}


slSlTableFactory slTableFactory;
static pgaCollectionFactory cf(&slTableFactory, __("Tables"), tables_png_img);
