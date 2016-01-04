//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// pgCast.cpp - Cast class
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "utils/misc.h"
#include "schema/pgCast.h"

pgCast::pgCast(const wxString &newName)
	: pgDatabaseObject(castFactory, newName)
{
}

pgCast::~pgCast()
{
}


wxString pgCast::GetTranslatedMessage(int kindOfMessage) const
{
	wxString message = wxEmptyString;

	switch (kindOfMessage)
	{
		case RETRIEVINGDETAILS:
			message = _("Retrieving details on cast");
			message += wxT(" ") + GetName();
			break;
		case REFRESHINGDETAILS:
			message = _("Refreshing cast");
			message += wxT(" ") + GetName();
			break;
		case DROPINCLUDINGDEPS:
			message = wxString::Format(_("Are you sure you wish to drop cast \"%s\" including all objects that depend on it?"),
			                           GetFullIdentifier().c_str());
			break;
		case DROPEXCLUDINGDEPS:
			message = wxString::Format(_("Are you sure you wish to drop cast \"%s\"?"),
			                           GetFullIdentifier().c_str());
			break;
		case DROPCASCADETITLE:
			message = _("Drop cast cascaded?");
			break;
		case DROPTITLE:
			message = _("Drop cast?");
			break;
		case PROPERTIESREPORT:
			message = _("Cast properties report");
			message += wxT(" - ") + GetName();
			break;
		case PROPERTIES:
			message = _("Cast properties");
			break;
		case DDLREPORT:
			message = _("Cast DDL report");
			message += wxT(" - ") + GetName();
			break;
		case DDL:
			message = _("Cast DDL");
			break;
		case DEPENDENCIESREPORT:
			message = _("Cast dependencies report");
			message += wxT(" - ") + GetName();
			break;
		case DEPENDENCIES:
			message = _("Cast dependencies");
			break;
		case DEPENDENTSREPORT:
			message = _("Cast dependents report");
			message += wxT(" - ") + GetName();
			break;
		case DEPENDENTS:
			message = _("Cast dependents");
			break;
	}

	return message;
}


bool pgCast::DropObject(wxFrame *frame, ctlTree *browser, bool cascaded)
{
	wxString sql = wxT("DROP CAST (") + GetSourceType() + wxT(" AS ") + GetTargetType() + wxT(")");
	if (cascaded)
		sql += wxT(" CASCADE");
	return GetDatabase()->ExecuteVoid(sql);
}

wxString pgCast::GetSql(ctlTree *browser)
{
	if (sql.IsNull())
	{
		sql = wxT("-- Cast: ") + GetQuotedFullIdentifier() + wxT("\n\n")
		      wxT("-- DROP CAST (") + GetSourceType() +
		      wxT(" AS ") + GetTargetType() + wxT(");")
		      wxT("\n\nCREATE CAST (") + GetSourceType() +
		      wxT(" AS ") + GetTargetType();
		if (GetCastFunction().IsNull())
			sql += wxT(")\n  WITHOUT FUNCTION");
		else
			sql += wxT(")\n  WITH FUNCTION ") + GetQuotedSchemaPrefix(GetCastNamespace()) + qtIdent(GetCastFunction()) + wxT("(") + GetSourceType() + wxT(")");
		if (GetCastContext() != wxT("EXPLICIT"))
			sql += wxT("\n  AS ") + GetCastContext();
		sql += wxT(";\n");
	}

	return sql;
}

void pgCast::ShowTreeDetail(ctlTree *browser, frmMain *form, ctlListView *properties, ctlSQLBox *sqlPane)
{
	if (properties)
	{
		CreateListColumns(properties);

		properties->AppendItem(_("Name"), GetName());
		properties->AppendItem(_("OID"), GetOid());
		properties->AppendItem(_("Source type"), GetSourceType());
		properties->AppendItem(_("Target type"), GetTargetType());
		if (GetCastFunction().IsNull())
			properties->AppendItem(_("Function"), _("(binary compatible)"));
		else
			properties->AppendItem(_("Function"), GetCastFunction() + wxT("(") + GetSourceType() + wxT(")"));
		properties->AppendItem(_("Context"), GetCastContext());
		properties->AppendYesNoItem(_("System cast?"), GetSystemObject());
		if (GetConnection()->BackendMinimumVersion(7, 5))
			properties->AppendItem(_("Comment"), firstLineOnly(GetComment()));
	}
}



pgObject *pgCast::Refresh(ctlTree *browser, const wxTreeItemId item)
{
	pgObject *cast = 0;

	pgCollection *coll = browser->GetParentCollection(item);
	if (coll)
		cast = castFactory.CreateObjects(coll, 0, wxT(" WHERE ca.oid=") + GetOidStr());

	return cast;
}



pgObject *pgCastFactory::CreateObjects(pgCollection *collection, ctlTree *browser, const wxString &restriction)
{
	pgCast *cast = 0;
	wxString systemRestriction;
	if (!settings->GetShowSystemObjects() && restriction.IsEmpty())
		systemRestriction = wxT(" WHERE ca.oid > ") + NumToStr(collection->GetConnection()->GetLastSystemOID()) + wxT("\n");

	pgSet *casts = collection->GetDatabase()->ExecuteSet(
	                   wxT("SELECT ca.oid, ca.*, format_type(st.oid,NULL) AS srctyp, format_type(tt.oid,tt.typtypmod) AS trgtyp,")
	                   wxT(      " ns.nspname AS srcnspname, nt.nspname AS trgnspname,\n")
	                   wxT(      " proname, np.nspname AS pronspname, description\n")
	                   wxT("  FROM pg_cast ca\n")
	                   wxT("  JOIN pg_type st ON st.oid=castsource\n")
	                   wxT("  JOIN pg_namespace ns ON ns.oid=st.typnamespace\n")
	                   wxT("  JOIN pg_type tt ON tt.oid=casttarget\n")
	                   wxT("  JOIN pg_namespace nt ON nt.oid=tt.typnamespace\n")
	                   wxT("  LEFT JOIN pg_proc pr ON pr.oid=castfunc\n")
	                   wxT("  LEFT JOIN pg_namespace np ON np.oid=pr.pronamespace\n")
	                   wxT("  LEFT OUTER JOIN pg_description des ON (des.objoid=ca.oid AND des.objsubid=0 AND des.classoid='pg_cast'::regclass)\n")
	                   + restriction + systemRestriction +
	                   wxT(" ORDER BY st.typname, tt.typname"));

	if (casts)
	{
		while (!casts->Eof())
		{
			wxString name = casts->GetVal(wxT("srctyp")) + wxT("->") + casts->GetVal(wxT("trgtyp"));
			cast = new pgCast(name);

			cast->iSetOid(casts->GetOid(wxT("oid")));
			cast->iSetDatabase(collection->GetDatabase());
			cast->iSetSourceType(casts->GetVal(wxT("srctyp")));
			cast->iSetSourceNamespace(casts->GetVal(wxT("srcnspname")));
			cast->iSetSourceTypeOid(casts->GetOid(wxT("castsource")));
			cast->iSetTargetType(casts->GetVal(wxT("trgtyp")));
			cast->iSetTargetNamespace(casts->GetVal(wxT("trgnspname")));
			cast->iSetTargetTypeOid(casts->GetOid(wxT("casttarget")));
			cast->iSetCastFunction(casts->GetVal(wxT("proname")));
			cast->iSetCastNamespace(casts->GetVal(wxT("pronspname")));
			cast->iSetComment(casts->GetVal(wxT("description")));
			wxString ct = casts->GetVal(wxT("castcontext"));
			cast->iSetCastContext(
			    ct == wxT("i") ? wxT("IMPLICIT") :
			    ct == wxT("a") ? wxT("ASSIGNMENT") : wxT("EXPLICIT"));

			if (settings->GetShowSystemObjects() ||
			        (cast->GetOid() > collection->GetServer()->GetLastSystemOID()))
			{
				if (browser)
				{
					browser->AppendObject(collection, cast);
					casts->MoveNext();
				}
				else
					break;
			}
			else
				break;
		}
		delete casts;
	}
	return cast;
}


/////////////////////////////

pgCastCollection::pgCastCollection(pgaFactory *factory, pgDatabase *db)
	: pgDatabaseObjCollection(factory, db)
{
}


wxString pgCastCollection::GetTranslatedMessage(int kindOfMessage) const
{
	wxString message = wxEmptyString;

	switch (kindOfMessage)
	{
		case RETRIEVINGDETAILS:
			message = _("Retrieving details on casts");
			break;
		case REFRESHINGDETAILS:
			message = _("Refreshing casts");
			break;
		case OBJECTSLISTREPORT:
			message = _("Casts list report");
			break;
	}

	return message;
}

/////////////////////////////

#include "images/cast.pngc"
#include "images/cast-sm.pngc"
#include "images/casts.pngc"

pgCastFactory::pgCastFactory()
	: pgDatabaseObjFactory(__("Cast"), __("New Cast..."), __("Create a new Cast."), cast_png_img, cast_sm_png_img)
{
}


pgCollection *pgCastFactory::CreateCollection(pgObject *obj)
{
	return new pgCastCollection(GetCollectionFactory(), (pgDatabase *)obj);
}

pgCastFactory castFactory;
static pgaCollectionFactory cf(&castFactory, __("Casts"), casts_png_img);
