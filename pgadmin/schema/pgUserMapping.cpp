//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// pgUserMapping.cpp - User Mapping class
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "utils/misc.h"
#include "schema/pgForeignServer.h"
#include "schema/pgUserMapping.h"
#include "schema/pgDatatype.h"


pgUserMapping::pgUserMapping(pgForeignServer *newForeignServer, const wxString &newName)
	: pgForeignServerObject(newForeignServer, userMappingFactory, newName)
{
}


wxString pgUserMapping::GetTranslatedMessage(int kindOfMessage) const
{
	wxString message = wxEmptyString;

	switch (kindOfMessage)
	{
		case RETRIEVINGDETAILS:
			message = _("Retrieving details on user mapping");
			message += wxT(" ") + GetName();
			break;
		case REFRESHINGDETAILS:
			message = _("Refreshing user mapping");
			message += wxT(" ") + GetName();
			break;
		case DROPINCLUDINGDEPS:
			message = wxString::Format(_("Are you sure you wish to drop user mapping \"%s\" including all objects that depend on it?"),
			                           GetFullIdentifier().c_str());
			break;
		case DROPEXCLUDINGDEPS:
			message = wxString::Format(_("Are you sure you wish to drop user mapping \"%s\"?"),
			                           GetFullIdentifier().c_str());
			break;
		case DROPCASCADETITLE:
			message = _("Drop user mapping cascaded?");
			break;
		case DROPTITLE:
			message = _("Drop user mapping?");
			break;
		case PROPERTIESREPORT:
			message = _("User mapping properties report");
			message += wxT(" - ") + GetName();
			break;
		case PROPERTIES:
			message = _("User mapping properties");
			break;
		case DDLREPORT:
			message = _("User mapping DDL report");
			message += wxT(" - ") + GetName();
			break;
		case DDL:
			message = _("User mapping DDL");
			break;
		case STATISTICSREPORT:
			message = _("User mapping statistics report");
			message += wxT(" - ") + GetName();
			break;
		case OBJSTATISTICS:
			message = _("User mapping statistics");
			break;
		case DEPENDENCIESREPORT:
			message = _("User mapping dependencies report");
			message += wxT(" - ") + GetName();
			break;
		case DEPENDENCIES:
			message = _("User mapping dependencies");
			break;
		case DEPENDENTSREPORT:
			message = _("User mapping dependents report");
			message += wxT(" - ") + GetName();
			break;
		case DEPENDENTS:
			message = _("User mapping dependents");
			break;
	}

	return message;
}


bool pgUserMapping::DropObject(wxFrame *frame, ctlTree *browser, bool cascaded)
{
	wxString sql = wxT("DROP USER MAPPING FOR ") + GetUsr() + wxT(" SERVER ") + qtIdent(GetForeignServer()->GetName());
	return GetDatabase()->ExecuteVoid(sql);
}


wxString pgUserMapping::GetSql(ctlTree *browser)
{
	if (sql.IsNull())
	{
		sql = wxT("-- Server: ") + GetQuotedFullIdentifier() + wxT("\n\n")
		      + wxT("-- DROP USER MAPPING FOR ") + GetUsr() + wxT(" SERVER ") + qtIdent(GetForeignServer()->GetName()) + wxT(";")
		      + wxT("\n\nCREATE USER MAPPING ")
		      + wxT("\n   FOR ") + qtIdent(GetUsr())
		      + wxT("\n   SERVER ") + qtIdent(GetForeignServer()->GetName());

		if (!GetOptions().IsEmpty())
			sql += wxT("\n  OPTIONS (") + GetCreateOptions() + wxT(")");

		sql += wxT(";\n");
	}
	return sql;
}


void pgUserMapping::ShowTreeDetail(ctlTree *browser, frmMain *form, ctlListView *properties, ctlSQLBox *sqlPane)
{
	if (properties)
	{
		CreateListColumns(properties);

		properties->AppendItem(_("Name"), GetName());
		properties->AppendItem(_("OID"), GetOid());
		properties->AppendItem(_("User"), GetUsr());
		properties->AppendItem(_("Options"), GetOptions());
	}
}



pgObject *pgUserMapping::Refresh(ctlTree *browser, const wxTreeItemId item)
{
	pgObject *um = 0;
	pgCollection *coll = browser->GetParentCollection(item);
	if (coll)
		um = userMappingFactory.CreateObjects(coll, 0, wxT(" AND u.oid=") + GetOidStr());

	return um;
}


////////////////////////////////////////////////////


pgObject *pgUserMappingFactory::CreateObjects(pgCollection *collection, ctlTree *browser, const wxString &restriction)
{
	wxString sql;
	pgUserMapping *um = 0;

	sql = wxT("SELECT u.oid AS um_oid,\n")
	      wxT("CASE WHEN u.umuser = 0::oid THEN 'public'::name ELSE a.rolname END AS usr_name,\n")
	      wxT("array_to_string(u.umoptions, ',') AS um_options\n")
	      wxT("FROM pg_user_mapping u\n")
	      wxT("  LEFT JOIN pg_authid a ON a.oid = u.umuser\n")
	      wxT(" WHERE u.umserver = ") + collection->GetOidStr()
	      + restriction + wxT("\n")
	      wxT("ORDER BY 2");
	pgSet *usermappings = collection->GetDatabase()->ExecuteSet(sql);
	if (usermappings)
	{
		while (!usermappings->Eof())
		{
			um = new pgUserMapping(collection->GetForeignServer(), usermappings->GetVal(wxT("usr_name")));
			um->iSetOid(usermappings->GetOid(wxT("um_oid")));
			um->iSetUsr(usermappings->GetVal(wxT("usr_name")));
			um->iSetOptions(usermappings->GetVal(wxT("um_options")));

			if (browser)
			{
				browser->AppendObject(collection, um);
				usermappings->MoveNext();
			}
			else
				break;
		}

		delete usermappings;
	}

	return um;
}


wxString pgUserMapping::GetCreateOptions()
{
	wxString options_create = wxEmptyString;
	wxString opt;
	wxString val;

	wxStringTokenizer tkz_options(options, wxT(","));
	while (tkz_options.HasMoreTokens())
	{
		wxStringTokenizer tkz_option(tkz_options.GetNextToken(), wxT("="));
		opt = tkz_option.GetNextToken();
		val = tkz_option.GetNextToken();

		if (!options_create.IsEmpty())
			options_create += wxT(",");

		options_create += opt + wxT(" '") + val + wxT("'");
	}

	return options_create;
}


pgCollection *pgUserMappingObjFactory::CreateCollection(pgObject *obj)
{
	return new pgUserMappingCollection(GetCollectionFactory(), (pgUserMapping *)obj);
}


/////////////////////////////

pgUserMappingCollection::pgUserMappingCollection(pgaFactory *factory, pgUserMapping *newum)
	: pgCollection(factory)
{
	um = newum;
	fsrv = um->GetForeignServer();
	fdw = fsrv->GetForeignDataWrapper();
	database = fdw->GetDatabase();
	server = database->GetServer();
	iSetOid(fsrv->GetOid());
}


wxString pgUserMappingCollection::GetTranslatedMessage(int kindOfMessage) const
{
	wxString message = wxEmptyString;

	switch (kindOfMessage)
	{
		case RETRIEVINGDETAILS:
			message = _("Retrieving details on user mappings");
			break;
		case REFRESHINGDETAILS:
			message = _("Refreshing user mappings");
			break;
		case OBJECTSLISTREPORT:
			message = _("User mappings list report");
			break;
	}

	return message;
}

///////////////////////////////////////////////////

#include "images/usermapping.pngc"
#include "images/usermapping-sm.pngc"
#include "images/usermappings.pngc"

pgUserMappingFactory::pgUserMappingFactory()
	: pgForeignServerObjFactory(__("User Mapping"), __("New User Mapping..."),
	                            __("Create a new User Mapping."), usermapping_png_img, usermapping_sm_png_img)
{
}


pgUserMappingFactory userMappingFactory;
static pgaCollectionFactory cf(&userMappingFactory, __("User Mappings"), usermappings_png_img);
