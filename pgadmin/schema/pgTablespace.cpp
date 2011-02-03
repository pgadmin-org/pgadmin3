//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2010, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// pgTablespace.cpp - PostgreSQL Tablespace
//
//////////////////////////////////////////////////////////////////////////


// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "utils/misc.h"
#include "utils/pgfeatures.h"
#include "schema/pgTablespace.h"
#include "schema/pgDatabase.h"
#include "frm/frmMain.h"


pgTablespace::pgTablespace(const wxString &newName)
	: pgServerObject(tablespaceFactory, newName)
{
}

void pgTablespace::ShowDependents(frmMain *form, ctlListView *referencedBy, const wxString &where)
{
	form->StartMsg(_(" Retrieving tablespace usage"));

	referencedBy->ClearAll();
	referencedBy->AddColumn(_("Type"), 60);
	referencedBy->AddColumn(_("Database"), 80);
	referencedBy->AddColumn(_("Name"), 300);

	wxArrayString dblist;

	pgSet *set = GetConnection()->ExecuteSet(
	                 wxT("SELECT datname, datallowconn, dattablespace\n")
	                 wxT("  FROM pg_database db\n")
	                 wxT(" ORDER BY datname"));

	if (set)
	{
		while (!set->Eof())
		{
			wxString datname = set->GetVal(wxT("datname"));
			if (set->GetBool(wxT("datallowconn")))
				dblist.Add(datname);
			OID oid = set->GetOid(wxT("dattablespace"));
			if (oid == GetOid())
				referencedBy->AppendItem(databaseFactory.GetIconId(), _("Database"), datname);

			set->MoveNext();
		}
		delete set;
	}

	FillOwned(form->GetBrowser(), referencedBy, dblist,
	          wxT("SELECT cl.relkind, COALESCE(cin.nspname, cln.nspname) as nspname, COALESCE(ci.relname, cl.relname) as relname, cl.relname as indname\n")
	          wxT("  FROM pg_class cl\n")
	          wxT("  JOIN pg_namespace cln ON cl.relnamespace=cln.oid\n")
	          wxT("  LEFT OUTER JOIN pg_index ind ON ind.indexrelid=cl.oid\n")
	          wxT("  LEFT OUTER JOIN pg_class ci ON ind.indrelid=ci.oid\n")
	          wxT("  LEFT OUTER JOIN pg_namespace cin ON ci.relnamespace=cin.oid,\n")
	          wxT("       pg_database\n")
	          wxT(" WHERE datname = current_database()\n")
	          wxT("   AND (cl.reltablespace = ") + GetOidStr() + wxT("\n")
	          wxT("        OR (cl.reltablespace=0 AND dattablespace = ") + GetOidStr() + wxT("))\n")
	          wxT(" ORDER BY 1,2,3"));

	form->EndMsg(set != 0);
}


bool pgTablespace::DropObject(wxFrame *frame, ctlTree *browser, bool cascaded)
{
	return GetConnection()->ExecuteVoid(wxT("DROP TABLESPACE ") + GetQuotedFullIdentifier() + wxT(";"));
}


wxString pgTablespace::GetSql(ctlTree *browser)
{
	if (sql.IsNull())
	{
		sql = wxT("-- Tablespace: ") + GetName() + wxT("\n\n");
		if (location.IsEmpty())
			sql += wxT("-- System Tablespace\n");
		else
			sql += wxT("-- DROP TABLESPACE ") + GetQuotedIdentifier()
			       +  wxT("\n\nCREATE TABLESPACE ") + GetQuotedIdentifier()
			       +  wxT("\n  OWNER ") + qtIdent(GetOwner())
			       +  wxT("\n  LOCATION ") + qtDbString(location)
			       +  wxT(";\n");
		sql += GetCommentSql();

		size_t i;
		for (i = 0 ; i < variables.GetCount() ; i++)
			sql += wxT("ALTER TABLESPACE ") + GetQuotedFullIdentifier()
			       +  wxT(" SET (") + variables.Item(i) + wxT(");\n");

	}
	return sql;
}


void pgTablespace::ShowTreeDetail(ctlTree *browser, frmMain *form, ctlListView *properties, ctlSQLBox *sqlPane)
{
	if (!expandedKids)
	{
		expandedKids = true;

	}
	if (properties)
	{
		CreateListColumns(properties);

		properties->AppendItem(_("Name"), GetName());
		properties->AppendItem(_("OID"), GetOid());
		properties->AppendItem(_("Owner"), GetOwner());
		properties->AppendItem(_("Location"), GetLocation());
		size_t i;
		for (i = 0 ; i < variables.GetCount() ; i++)
		{
			wxString item = variables.Item(i);
			properties->AppendItem(item.BeforeFirst('='), item.AfterFirst('='));
		}
		properties->AppendItem(_("ACL"), GetAcl());
		properties->AppendItem(_("Comment"), firstLineOnly(GetComment()));
	}
}


void pgTablespace::ShowStatistics(frmMain *form, ctlListView *statistics)
{
	if (statistics)
	{
		if (GetConnection()->HasFeature(FEATURE_SIZE))
		{
			wxLogInfo(wxT("Displaying statistics for %s"), GetTypeName().c_str());

			// Add the statistics view columns
			CreateListColumns(statistics, _("Statistic"), _("Value"));

			pgSet *stats = GetConnection()->ExecuteSet(
			                   wxT("SELECT pg_size_pretty(pg_tablespace_size(") + GetOidStr() + wxT(")) AS ") + qtIdent(_("Tablespace Size")));

			if (stats)
			{
				int col;
				for (col = 0 ; col < stats->NumCols() ; col++)
				{
					if (!stats->ColName(col).IsEmpty())
						statistics->AppendItem(stats->ColName(col), stats->GetVal(col));
				}
				delete stats;
			}
		}
	}
}


pgObject *pgTablespace::Refresh(ctlTree *browser, const wxTreeItemId item)
{
	pgObject *tablespace = 0;
	pgCollection *coll = browser->GetParentCollection(item);
	if (coll)
		tablespace = tablespaceFactory.CreateObjects(coll, 0, wxT("\n WHERE ts.oid=") + GetOidStr());

	return tablespace;
}



pgObject *pgTablespaceFactory::CreateObjects(pgCollection *collection, ctlTree *browser, const wxString &restriction)
{
	pgTablespace *tablespace = 0;

	wxString tabname;

	pgSet *tablespaces;
	if (collection->GetConnection()->BackendMinimumVersion(8, 5))
		tablespaces = collection->GetServer()->ExecuteSet(
		                  wxT("SELECT ts.oid, spcname, spclocation, spcoptions, pg_get_userbyid(spcowner) as spcuser, spcacl, pg_catalog.shobj_description(oid, 'pg_tablespace') AS description FROM pg_tablespace ts\n")
		                  + restriction + wxT(" ORDER BY spcname"));
	else if (collection->GetConnection()->BackendMinimumVersion(8, 2))
		tablespaces = collection->GetServer()->ExecuteSet(
		                  wxT("SELECT ts.oid, spcname, spclocation, pg_get_userbyid(spcowner) as spcuser, spcacl, pg_catalog.shobj_description(oid, 'pg_tablespace') AS description FROM pg_tablespace ts\n")
		                  + restriction + wxT(" ORDER BY spcname"));
	else
		tablespaces = collection->GetServer()->ExecuteSet(
		                  wxT("SELECT ts.oid, spcname, spclocation, '' AS description, pg_get_userbyid(spcowner) as spcuser, spcacl FROM pg_tablespace ts\n")
		                  + restriction + wxT(" ORDER BY spcname"));

	if (tablespaces)
	{
		while (!tablespaces->Eof())
		{

			tablespace = new pgTablespace(tablespaces->GetVal(wxT("spcname")));
			tablespace->iSetServer(collection->GetServer());
			tablespace->iSetOid(tablespaces->GetOid(wxT("oid")));
			tablespace->iSetOwner(tablespaces->GetVal(wxT("spcuser")));
			tablespace->iSetLocation(tablespaces->GetVal(wxT("spclocation")));
			tablespace->iSetAcl(tablespaces->GetVal(wxT("spcacl")));
			if (collection->GetConnection()->BackendMinimumVersion(8, 2))
				tablespace->iSetComment(tablespaces->GetVal(wxT("description")));
			if (collection->GetConnection()->BackendMinimumVersion(8, 5))
			{
				wxString str = tablespaces->GetVal(wxT("spcoptions"));
				if (!str.IsEmpty())
					FillArray(tablespace->GetVariables(), str.Mid(1, str.Length() - 2));
			}

			if (browser)
			{
				browser->AppendObject(collection, tablespace);
				tablespaces->MoveNext();
			}
			else
				break;
		}

		delete tablespaces;
	}
	return tablespace;
}


pgTablespaceCollection::pgTablespaceCollection(pgaFactory *factory, pgServer *sv)
	: pgServerObjCollection(factory, sv)
{
}


void pgTablespaceCollection::ShowStatistics(frmMain *form, ctlListView *statistics)
{
	if (GetConnection()->HasFeature(FEATURE_SIZE))
	{
		wxLogInfo(wxT("Displaying statistics for tablespaces"));

		// Add the statistics view columns
		statistics->ClearAll();
		statistics->AddColumn(_("Tablespace"), 100);
		statistics->AddColumn(_("Size"), 60);

		pgSet *stats = GetConnection()->ExecuteSet(
		                   wxT("SELECT spcname, pg_size_pretty(pg_tablespace_size(oid)) AS size FROM pg_tablespace ORDER BY spcname"));

		if (stats)
		{
			long pos = 0;
			while (!stats->Eof())
			{
				statistics->InsertItem(pos, stats->GetVal(wxT("spcname")), PGICON_STATISTICS);
				statistics->SetItem(pos, 1, stats->GetVal(wxT("size")));
				stats->MoveNext();
				pos++;
			}

			delete stats;
		}
	}
}



#include "images/tablespace.xpm"
#include "images/tablespaces.xpm"


pgTablespaceFactory::pgTablespaceFactory()
	: pgServerObjFactory(__("Tablespace"), __("New Tablespace..."), __("Create a new Tablespace."), tablespace_xpm)
{
	metaType = PGM_TABLESPACE;
}


pgCollection *pgTablespaceFactory::CreateCollection(pgObject *obj)
{
	return new pgTablespaceCollection(GetCollectionFactory(), (pgServer *)obj);
}

pgTablespaceFactory tablespaceFactory;
static pgaCollectionFactory cf(&tablespaceFactory, __("Tablespaces"), tablespaces_xpm);
