//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
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
#include "dlg/dlgMoveTablespace.h"


pgTablespace::pgTablespace(const wxString &newName)
	: pgServerObject(tablespaceFactory, newName)
{
}

wxString pgTablespace::GetTranslatedMessage(int kindOfMessage) const
{
	wxString message = wxEmptyString;

	switch (kindOfMessage)
	{
		case RETRIEVINGDETAILS:
			message = _("Retrieving details on tablespace");
			message += wxT(" ") + GetName();
			break;
		case REFRESHINGDETAILS:
			message = _("Refreshing tablespace");
			message += wxT(" ") + GetName();
			break;
		case DROPINCLUDINGDEPS:
			message = wxString::Format(_("Are you sure you wish to drop tablespace \"%s\" including all objects that depend on it?"),
			                           GetFullIdentifier().c_str());
			break;
		case DROPEXCLUDINGDEPS:
			message = wxString::Format(_("Are you sure you wish to drop tablespace \"%s\"?"),
			                           GetFullIdentifier().c_str());
			break;
		case DROPCASCADETITLE:
			message = _("Drop tablespace cascaded?");
			break;
		case DROPTITLE:
			message = _("Drop tablespace?");
			break;
		case PROPERTIESREPORT:
			message = _("Tablespace properties report");
			message += wxT(" - ") + GetName();
			break;
		case PROPERTIES:
			message = _("Tablespace properties");
			break;
		case DDLREPORT:
			message = _("Tablespace DDL report");
			message += wxT(" - ") + GetName();
			break;
		case DDL:
			message = _("Tablespace DDL");
			break;
		case STATISTICSREPORT:
			message = _("Tablespace statistics report");
			message += wxT(" - ") + GetName();
			break;
		case OBJSTATISTICS:
			message = _("Tablespace statistics");
			break;
		case DEPENDENCIESREPORT:
			message = _("Tablespace dependencies report");
			message += wxT(" - ") + GetName();
			break;
		case DEPENDENCIES:
			message = _("Tablespace dependencies");
			break;
		case DEPENDENTSREPORT:
			message = _("Tablespace dependents report");
			message += wxT(" - ") + GetName();
			break;
		case DEPENDENTS:
			message = _("Tablespace dependents");
			break;
	}

	return message;
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

		if (GetConnection()->BackendMinimumVersion(9, 2))
			sql += GetSeqLabelsSql();
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


void pgTablespace::MoveTablespace(frmMain *form)
{
	wxString query;

	dlgMoveTablespace rdo(form, GetConnection(), this);
	if (rdo.ShowModal() != wxID_CANCEL)
	{
		if (wxMessageBox(wxString::Format(
		                     _("Are you sure you wish to move objects from %s to %s?"),
		                     GetQuotedFullIdentifier().c_str(), rdo.GetTablespace().c_str()),
		                 _("Move tablespace?"),
		                 wxYES_NO) != wxYES)
			return;

		wxArrayString kind = rdo.GetKind();
		wxString      ownerInfo,
		              moveTo = qtIdent(rdo.GetTablespace()),
		              currTblSpace = GetQuotedFullIdentifier();

		if (rdo.GetOwner().Length() > 0)
		{
			ownerInfo = wxString::Format(
			                wxT(" OWNED BY %s"),
			                qtIdent(rdo.GetOwner()).c_str());
		}

		for(size_t index = 0; index < kind.GetCount(); ++index)
		{
			query += wxString::Format(
			             wxT("ALTER %s ALL IN TABLESPACE %s%s SET TABLESPACE %s;\n"),
			             kind.Item(index).c_str(),
			             currTblSpace.c_str(),
			             ownerInfo.c_str(),
			             moveTo.c_str());
		}
		GetConnection()->ExecuteVoid(query);
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

	pgSet *tablespaces;
	if (collection->GetConnection()->BackendMinimumVersion(9, 2))
		tablespaces = collection->GetServer()->ExecuteSet(
		                  wxT("SELECT ts.oid, spcname, pg_catalog.pg_tablespace_location(ts.oid) AS spclocation, spcoptions, ")
		                  wxT("pg_get_userbyid(spcowner) as spcuser, spcacl, ")
		                  wxT("pg_catalog.shobj_description(oid, 'pg_tablespace') AS description, ")
		                  wxT("(SELECT array_agg(label) FROM pg_shseclabel sl1 WHERE sl1.objoid=ts.oid) AS labels, ")
		                  wxT("(SELECT array_agg(provider) FROM pg_shseclabel sl2 WHERE sl2.objoid=ts.oid) AS providers ")
		                  wxT("FROM pg_tablespace ts\n")
		                  + restriction + wxT(" ORDER BY spcname"));
	else if (collection->GetConnection()->BackendMinimumVersion(8, 5))
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
			if (collection->GetConnection()->BackendMinimumVersion(9, 2))
			{
				tablespace->iSetProviders(tablespaces->GetVal(wxT("providers")));
				tablespace->iSetLabels(tablespaces->GetVal(wxT("labels")));
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


wxString pgTablespaceCollection::GetTranslatedMessage(int kindOfMessage) const
{
	wxString message = wxEmptyString;

	switch (kindOfMessage)
	{
		case RETRIEVINGDETAILS:
			message = _("Retrieving details on tablespaces");
			break;
		case REFRESHINGDETAILS:
			message = _("Refreshing tablespaces");
			break;
		case STATISTICSREPORT:
			message = _("Tablespaces statistics report");
			break;
		case OBJSTATISTICS:
			message = _("Tablespaces statistics");
			break;
		case OBJECTSLISTREPORT:
			message = _("Tablespaces list report");
			break;
	}

	return message;
}


#include "images/tablespace.pngc"
#include "images/tablespaces.pngc"


pgTablespaceFactory::pgTablespaceFactory()
	: pgServerObjFactory(__("Tablespace"), __("New Tablespace..."), __("Create a new Tablespace."), tablespace_png_img)
{
	metaType = PGM_TABLESPACE;
}


pgCollection *pgTablespaceFactory::CreateCollection(pgObject *obj)
{
	return new pgTablespaceCollection(GetCollectionFactory(), (pgServer *)obj);
}

pgTablespaceFactory tablespaceFactory;
static pgaCollectionFactory cf(&tablespaceFactory, __("Tablespaces"), tablespaces_png_img);

moveTablespaceFactory::moveTablespaceFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar) : contextActionFactory(list)
{
	mnu->Append(id, _("Move objects to..."), _("Move objects of the selected tablespace to another one."));
}


wxWindow *moveTablespaceFactory::StartDialog(frmMain *form, pgObject *obj)
{
	((pgTablespace *)obj)->MoveTablespace(form);

	return 0;
}

bool moveTablespaceFactory::CheckEnable(pgObject *obj)
{
	return obj && obj->IsCreatedBy(tablespaceFactory) && ((pgTablespace *)obj)->GetConnection()->BackendMinimumVersion(9, 4);
}
