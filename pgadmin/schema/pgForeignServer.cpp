//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// pgForeignServer.cpp - Foreign Server class
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


pgForeignServer::pgForeignServer(pgForeignDataWrapper *newForeignDataWrapper, const wxString &newName)
	: pgForeignDataWrapperObject(newForeignDataWrapper, foreignServerFactory, newName)
{
}


wxString pgForeignServer::GetTranslatedMessage(int kindOfMessage) const
{
	wxString message = wxEmptyString;

	switch (kindOfMessage)
	{
		case RETRIEVINGDETAILS:
			message = _("Retrieving details on foreign server");
			message += wxT(" ") + GetName();
			break;
		case REFRESHINGDETAILS:
			message = _("Refreshing foreign server");
			message += wxT(" ") + GetName();
			break;
		case DROPINCLUDINGDEPS:
			message = wxString::Format(_("Are you sure you wish to drop foreign server \"%s\" including all objects that depend on it?"),
			                           GetFullIdentifier().c_str());
			break;
		case DROPEXCLUDINGDEPS:
			message = wxString::Format(_("Are you sure you wish to drop foreign server \"%s\"?"),
			                           GetFullIdentifier().c_str());
			break;
		case DROPCASCADETITLE:
			message = _("Drop foreign server cascaded?");
			break;
		case DROPTITLE:
			message = _("Drop foreign server?");
			break;
		case PROPERTIESREPORT:
			message = _("Foreign server properties report");
			message += wxT(" - ") + GetName();
			break;
		case PROPERTIES:
			message = _("Foreign server properties");
			break;
		case DDLREPORT:
			message = _("Foreign server DDL report");
			message += wxT(" - ") + GetName();
			break;
		case DDL:
			message = _("Foreign server DDL");
			break;
		case STATISTICSREPORT:
			message = _("Foreign server statistics report");
			message += wxT(" - ") + GetName();
			break;
		case OBJSTATISTICS:
			message = _("Foreign server statistics");
			break;
		case DEPENDENCIESREPORT:
			message = _("Foreign server dependencies report");
			message += wxT(" - ") + GetName();
			break;
		case DEPENDENCIES:
			message = _("Foreign server dependencies");
			break;
		case DEPENDENTSREPORT:
			message = _("Foreign server dependents report");
			message += wxT(" - ") + GetName();
			break;
		case DEPENDENTS:
			message = _("Foreign server dependents");
			break;
	}

	return message;
}


bool pgForeignServer::DropObject(wxFrame *frame, ctlTree *browser, bool cascaded)
{
	wxString sql = wxT("DROP SERVER ") + GetQuotedFullIdentifier();
	if (cascaded)
		sql += wxT(" CASCADE");
	return GetDatabase()->ExecuteVoid(sql);
}


wxString pgForeignServer::GetSql(ctlTree *browser)
{
	if (sql.IsNull())
	{
		sql = wxT("-- Server: ") + GetQuotedFullIdentifier() + wxT("\n\n")
		      + wxT("-- DROP SERVER ") + GetQuotedFullIdentifier() + wxT(";")
		      + wxT("\n\nCREATE SERVER ") + GetQuotedFullIdentifier()
		      + wxT("\n   FOREIGN DATA WRAPPER ") + qtIdent(GetForeignDataWrapper()->GetName());

		if (!GetType().IsEmpty())
			sql += wxT("\n  TYPE ") + qtDbString(GetType());

		if (!GetVersion().IsEmpty())
			sql += wxT("\n  VERSION ") + qtDbString(GetVersion());

		if (!GetOptions().IsEmpty())
			sql += wxT("\n  OPTIONS (") + GetCreateOptions() + wxT(")");

		sql += wxT(";\n")
		       +  GetOwnerSql(8, 4, wxT("SERVER ") + GetQuotedFullIdentifier())
		       +  GetGrant(wxT("U"), wxT("FOREIGN SERVER ") + GetQuotedFullIdentifier());
	}
	return sql;
}


void pgForeignServer::ShowTreeDetail(ctlTree *browser, frmMain *form, ctlListView *properties, ctlSQLBox *sqlPane)
{
	if (!expandedKids)
	{
		expandedKids = true;

		browser->RemoveDummyChild(this);

		// Log
		wxLogInfo(wxT("Adding child object to foreign server %s"), GetIdentifier().c_str());

		if (settings->GetDisplayOption(_("User Mappings")))
			browser->AppendCollection(this, userMappingFactory);
	}

	if (properties)
	{
		CreateListColumns(properties);

		properties->AppendItem(_("Name"), GetName());
		properties->AppendItem(_("OID"), GetOid());
		properties->AppendItem(_("Owner"), GetOwner());
		properties->AppendItem(_("ACL"), GetAcl());
		properties->AppendItem(_("Type"), GetType());
		properties->AppendItem(_("Version"), GetVersion());
		properties->AppendItem(_("Options"), GetOptions());
	}
}



pgObject *pgForeignServer::Refresh(ctlTree *browser, const wxTreeItemId item)
{
	pgObject *fs = 0;
	pgCollection *coll = browser->GetParentCollection(item);
	if (coll)
		fs = foreignServerFactory.CreateObjects(coll, 0, wxT(" AND srv.oid=") + GetOidStr());

	return fs;
}


wxMenu *pgForeignServer::GetNewMenu()
{
	wxMenu *menu = pgObject::GetNewMenu();
	if (database->GetCreatePrivilege())
	{
		userMappingFactory.AppendMenu(menu);
	}
	return menu;
}


////////////////////////////////////////////////////


pgObject *pgForeignServerFactory::CreateObjects(pgCollection *collection, ctlTree *browser, const wxString &restriction)
{
	wxString sql;
	pgForeignServer *fs = 0;

	sql = wxT("SELECT srv.oid, srvname, srvtype, srvversion, srvacl, fdw.fdwname as fdwname, description, ")
	      wxT("array_to_string(srvoptions, ',') AS srvoptions, ")
	      wxT("pg_get_userbyid(srvowner) as srvowner\n")
	      wxT("  FROM pg_foreign_server srv\n")
	      wxT("  LEFT OUTER JOIN pg_foreign_data_wrapper fdw on fdw.oid=srvfdw\n")
	      wxT("  LEFT OUTER JOIN pg_description des ON (des.objoid=srv.oid AND des.objsubid=0 AND des.classoid='pg_foreign_server'::regclass)\n")
	      wxT(" WHERE srvfdw = ") + collection->GetOidStr()
	      + restriction + wxT("\n")
	      wxT(" ORDER BY srvname");
	pgSet *foreignservers = collection->GetDatabase()->ExecuteSet(sql);
	if (foreignservers)
	{
		while (!foreignservers->Eof())
		{
			fs = new pgForeignServer(collection->GetForeignDataWrapper(), foreignservers->GetVal(wxT("srvname")));
			fs->iSetOid(foreignservers->GetOid(wxT("oid")));
			fs->iSetOwner(foreignservers->GetVal(wxT("srvowner")));
			fs->iSetAcl(foreignservers->GetVal(wxT("srvacl")));
			fs->iSetComment(foreignservers->GetVal(wxT("description")));
			fs->iSetType(foreignservers->GetVal(wxT("srvtype")));
			fs->iSetVersion(foreignservers->GetVal(wxT("srvversion")));
			fs->iSetOptions(foreignservers->GetVal(wxT("srvoptions")));

			if (browser)
			{
				browser->AppendObject(collection, fs);
				foreignservers->MoveNext();
			}
			else
				break;
		}

		delete foreignservers;
	}

	return fs;
}


wxString pgForeignServer::GetCreateOptions()
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


/////////////////////////////

pgForeignServerObjCollection::pgForeignServerObjCollection(pgaFactory *factory, pgForeignServer *newsrv)
	: pgCollection(factory)
{
	fsrv = newsrv;
	fdw = fsrv->GetForeignDataWrapper();
	database = fdw->GetDatabase();
	server = database->GetServer();
	iSetOid(fsrv->GetOid());
}


wxString pgForeignServerObjCollection::GetTranslatedMessage(int kindOfMessage) const
{
	wxString message = wxEmptyString;

	switch (kindOfMessage)
	{
		case RETRIEVINGDETAILS:
			message = _("Retrieving details on foreign servers");
			break;
		case REFRESHINGDETAILS:
			message = _("Refreshing foreign servers");
			break;
		case OBJECTSLISTREPORT:
			message = _("Foreign servers list report");
			break;
	}

	return message;
}

pgCollection *pgForeignServerObjFactory::CreateCollection(pgObject *obj)
{
	return new pgForeignServerObjCollection(GetCollectionFactory(), (pgForeignServer *)obj);
}


///////////////////////////////////////////////////////////////

void pgForeignServerObject::SetForeignServer(pgForeignServer *newForeignServer)
{
	srv = newForeignServer;
	database = fdw->GetDatabase();
}

bool pgForeignServerObject::CanDrop()
{
	return true; //fdw->GetCreatePrivilege();
}


bool pgForeignServerObject::CanCreate()
{
	return true; //fdw->GetCreatePrivilege();
}


void pgForeignServerObject::SetContextInfo(frmMain *form)
{
}


pgSet *pgForeignServerObject::ExecuteSet(const wxString &sql)
{
	return srv->GetDatabase()->ExecuteSet(sql);
}

wxString pgForeignServerObject::ExecuteScalar(const wxString &sql)
{
	return srv->GetDatabase()->ExecuteScalar(sql);
}


bool pgForeignServerObject::ExecuteVoid(const wxString &sql)
{
	return srv->GetDatabase()->ExecuteVoid(sql);
}


///////////////////////////////////////////////////

#include "images/foreignserver.pngc"
#include "images/foreignserver-sm.pngc"
#include "images/foreignservers.pngc"

pgForeignServerFactory::pgForeignServerFactory()
	: pgForeignDataWrapperObjFactory(__("Foreign Server"), __("New Foreign Server..."),
	                                 __("Create a new Foreign Server."), foreignserver_png_img, foreignserver_sm_png_img)
{
}


pgForeignServerFactory foreignServerFactory;
static pgaCollectionFactory cf(&foreignServerFactory, __("Foreign Servers"), foreignservers_png_img);
