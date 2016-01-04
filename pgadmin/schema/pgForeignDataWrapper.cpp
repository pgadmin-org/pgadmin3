//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// pgForeignDataWrapper.cpp - Foreign Data Wrapper class
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "utils/misc.h"
#include "schema/pgForeignDataWrapper.h"
#include "schema/pgForeignServer.h"


pgForeignDataWrapper::pgForeignDataWrapper(const wxString &newName)
	: pgDatabaseObject(foreignDataWrapperFactory, newName)
{
}


wxString pgForeignDataWrapper::GetTranslatedMessage(int kindOfMessage) const
{
	wxString message = wxEmptyString;

	switch (kindOfMessage)
	{
		case RETRIEVINGDETAILS:
			message = _("Retrieving details on foreign data wrapper");
			message += wxT(" ") + GetName();
			break;
		case REFRESHINGDETAILS:
			message = _("Refreshing foreign data wrapper");
			message += wxT(" ") + GetName();
			break;
		case DROPINCLUDINGDEPS:
			message = wxString::Format(_("Are you sure you wish to drop foreign data wrapper \"%s\" including all objects that depend on it?"),
			                           GetFullIdentifier().c_str());
			break;
		case DROPEXCLUDINGDEPS:
			message = wxString::Format(_("Are you sure you wish to drop foreign data wrapper \"%s\"?"),
			                           GetFullIdentifier().c_str());
			break;
		case DROPCASCADETITLE:
			message = _("Drop foreign data wrapper cascaded?");
			break;
		case DROPTITLE:
			message = _("Drop foreign data wrapper?");
			break;
		case PROPERTIESREPORT:
			message = _("Foreign data wrapper properties report");
			message += wxT(" - ") + GetName();
			break;
		case PROPERTIES:
			message = _("Foreign data wrapper properties");
			break;
		case DDLREPORT:
			message = _("Foreign data wrapper DDL report");
			message += wxT(" - ") + GetName();
			break;
		case DDL:
			message = _("Foreign data wrapper DDL");
			break;
		case STATISTICSREPORT:
			message = _("Foreign data wrapper statistics report");
			message += wxT(" - ") + GetName();
			break;
		case OBJSTATISTICS:
			message = _("Foreign data wrapper statistics");
			break;
		case DEPENDENCIESREPORT:
			message = _("Foreign data wrapper dependencies report");
			message += wxT(" - ") + GetName();
			break;
		case DEPENDENCIES:
			message = _("Foreign data wrapper dependencies");
			break;
		case DEPENDENTSREPORT:
			message = _("Foreign data wrapper dependents report");
			message += wxT(" - ") + GetName();
			break;
		case DEPENDENTS:
			message = _("Foreign data wrapper dependents");
			break;
	}

	return message;
}


bool pgForeignDataWrapper::DropObject(wxFrame *frame, ctlTree *browser, bool cascaded)
{
	wxString sql = wxT("DROP FOREIGN DATA WRAPPER ") + GetQuotedFullIdentifier();
	if (cascaded)
		sql += wxT(" CASCADE");
	return GetDatabase()->ExecuteVoid(sql);
}


wxString pgForeignDataWrapper::GetSql(ctlTree *browser)
{
	if (sql.IsNull())
	{
		sql = wxT("-- Foreign Data Wrapper: ") + GetQuotedFullIdentifier() + wxT("\n\n")
		      + wxT("-- DROP FOREIGN DATA WRAPPER ") + GetQuotedFullIdentifier() + wxT(";")
		      + wxT("\n\nCREATE ");
		sql += wxT("FOREIGN DATA WRAPPER ") + GetName();

		if (!GetHandlerProc().IsEmpty())
			sql += wxT("\n  HANDLER ") + GetHandlerProc();

		if (!GetValidatorProc().IsEmpty())
			sql += wxT("\n  VALIDATOR ") + GetValidatorProc();

		if (!GetOptions().IsEmpty())
			sql += wxT("\n  OPTIONS (") + GetCreateOptions() + wxT(")");

		sql += wxT(";\n")
		       +  GetOwnerSql(8, 4, wxT("FOREIGN DATA WRAPPER ") + GetName())
		       +  GetGrant(wxT("U"), wxT("FOREIGN DATA WRAPPER ") + GetQuotedFullIdentifier());
	}
	return sql;
}


void pgForeignDataWrapper::ShowTreeDetail(ctlTree *browser, frmMain *form, ctlListView *properties, ctlSQLBox *sqlPane)
{
	if (!expandedKids)
	{
		expandedKids = true;

		browser->RemoveDummyChild(this);

		// Log
		wxLogInfo(wxT("Adding child object to foreign data wrapper %s"), GetIdentifier().c_str());

		if (settings->GetDisplayOption(_("Foreign Servers")))
			browser->AppendCollection(this, foreignServerFactory);
	}

	if (properties)
	{
		CreateListColumns(properties);

		properties->AppendItem(_("Name"), GetName());
		properties->AppendItem(_("OID"), GetOid());
		properties->AppendItem(_("Owner"), GetOwner());
		properties->AppendItem(_("ACL"), GetAcl());
		properties->AppendItem(_("Handler"), GetHandlerProc());
		properties->AppendItem(_("Validator"), GetValidatorProc());
		properties->AppendItem(_("Options"), GetOptions());
	}
}



pgObject *pgForeignDataWrapper::Refresh(ctlTree *browser, const wxTreeItemId item)
{
	pgObject *fdw = 0;
	pgCollection *coll = browser->GetParentCollection(item);
	if (coll)
		fdw = foreignDataWrapperFactory.CreateObjects(coll, 0, wxT("\n   WHERE fdw.oid=") + GetOidStr());

	return fdw;
}



wxMenu *pgForeignDataWrapper::GetNewMenu()
{
	wxMenu *menu = pgObject::GetNewMenu();
	if (database->GetCreatePrivilege())
	{
		foreignServerFactory.AppendMenu(menu);
	}
	return menu;
}


pgObject *pgForeignDataWrapperFactory::CreateObjects(pgCollection *collection, ctlTree *browser, const wxString &restriction)
{
	wxString sql;
	pgForeignDataWrapper *fdw = 0;

	bool fdwHandlerSupport = (collection->GetDatabase()->BackendMinimumVersion(9, 1));

	if(fdwHandlerSupport)
	{

		sql = wxT("SELECT fdw.oid, fdwname, fdwhandler, fdwvalidator, fdwacl, ")
		      wxT("vh.proname as fdwhan, vp.proname as fdwval, description, ")
		      wxT("array_to_string(fdwoptions, ',') AS fdwoptions, ")
		      wxT("pg_get_userbyid(fdwowner) as fdwowner\n");
		sql += wxT("  FROM pg_foreign_data_wrapper fdw\n")
		       wxT("  LEFT OUTER JOIN pg_proc vh on vh.oid=fdwhandler\n")
		       wxT("  LEFT OUTER JOIN pg_proc vp on vp.oid=fdwvalidator\n")
		       wxT("  LEFT OUTER JOIN pg_description des ON (des.objoid=fdw.oid AND des.objsubid=0 AND des.classoid='pg_foreign_data_wrapper'::regclass)\n")
		       + restriction + wxT("\n")
		       wxT(" ORDER BY fdwname");
	}
	else
	{

		sql = wxT("SELECT fdw.oid, fdwname, fdwvalidator, fdwacl, ")
		      wxT("vp.proname as fdwval, description, ")
		      wxT("array_to_string(fdwoptions, ',') AS fdwoptions, ")
		      wxT("pg_get_userbyid(fdwowner) as fdwowner\n");
		sql += wxT("  FROM pg_foreign_data_wrapper fdw\n")
		       wxT("  LEFT OUTER JOIN pg_proc vp on vp.oid=fdwvalidator\n")
		       wxT("  LEFT OUTER JOIN pg_description des ON (des.objoid=fdw.oid AND des.objsubid=0 AND des.classoid='pg_foreign_data_wrapper'::regclass)\n")
		       + restriction + wxT("\n")
		       wxT(" ORDER BY fdwname");
	}
	pgSet *fdws = collection->GetDatabase()->ExecuteSet(sql);

	if (fdws)
	{
		while (!fdws->Eof())
		{

			fdw = new pgForeignDataWrapper(fdws->GetVal(wxT("fdwname")));
			fdw->iSetDatabase(collection->GetDatabase());
			fdw->iSetOid(fdws->GetOid(wxT("oid")));
			fdw->iSetOwner(fdws->GetVal(wxT("fdwowner")));
			fdw->iSetAcl(fdws->GetVal(wxT("fdwacl")));
			if(fdwHandlerSupport)
				fdw->iSetHandlerProc(fdws->GetVal(wxT("fdwhan")));
			fdw->iSetValidatorProc(fdws->GetVal(wxT("fdwval")));
			fdw->iSetOptions(fdws->GetVal(wxT("fdwoptions")));

			if (browser)
			{
				browser->AppendObject(collection, fdw);

				fdws->MoveNext();
			}
			else
				break;
		}

		delete fdws;
	}
	return fdw;
}


wxString pgForeignDataWrapper::GetCreateOptions()
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


///////////////////////////////////////////////////////////////

void pgForeignDataWrapperObject::SetForeignDataWrapper(pgForeignDataWrapper *newForeignDataWrapper)
{
	fdw = newForeignDataWrapper;
	database = fdw->GetDatabase();
}

bool pgForeignDataWrapperObject::CanDrop()
{
	return true; //fdw->GetCreatePrivilege();
}


bool pgForeignDataWrapperObject::CanCreate()
{
	return true; //fdw->GetCreatePrivilege();
}


void pgForeignDataWrapperObject::SetContextInfo(frmMain *form)
{
}


pgSet *pgForeignDataWrapperObject::ExecuteSet(const wxString &sql)
{
	return fdw->GetDatabase()->ExecuteSet(sql);
}

wxString pgForeignDataWrapperObject::ExecuteScalar(const wxString &sql)
{
	return fdw->GetDatabase()->ExecuteScalar(sql);
}


bool pgForeignDataWrapperObject::ExecuteVoid(const wxString &sql)
{
	return fdw->GetDatabase()->ExecuteVoid(sql);
}


/////////////////////////////////////////////////////

pgForeignDataWrapperObjCollection::pgForeignDataWrapperObjCollection(pgaFactory *factory, pgForeignDataWrapper *newfdw)
	: pgCollection(factory)
{
	fdw = newfdw;
	database = fdw->GetDatabase();
	server = database->GetServer();
	iSetOid(fdw->GetOid());
}


wxString pgForeignDataWrapperObjCollection::GetTranslatedMessage(int kindOfMessage) const
{
	wxString message = wxEmptyString;

	switch (kindOfMessage)
	{
		case RETRIEVINGDETAILS:
			message = _("Retrieving details on foreign data wrappers");
			break;
		case REFRESHINGDETAILS:
			message = _("Refreshing foreign data wrappers");
			break;
		case OBJECTSLISTREPORT:
			message = _("Foreign data wrappers list report");
			break;
	}

	return message;
}

bool pgForeignDataWrapperObjCollection::CanCreate()
{
	return GetDatabase()->GetCreatePrivilege();
}


pgCollection *pgForeignDataWrapperObjFactory::CreateCollection(pgObject *obj)
{
	return new pgForeignDataWrapperObjCollection(GetCollectionFactory(), (pgForeignDataWrapper *)obj);
}


/////////////////////////////

pgForeignDataWrapperCollection::pgForeignDataWrapperCollection(pgaFactory *factory, pgDatabase *db)
	: pgDatabaseObjCollection(factory, db)
{
}


wxString pgForeignDataWrapperCollection::GetTranslatedMessage(int kindOfMessage) const
{
	wxString message = wxEmptyString;

	switch (kindOfMessage)
	{
		case RETRIEVINGDETAILS:
			message = _("Retrieving details on foreign data wrappers");
			break;
		case REFRESHINGDETAILS:
			message = _("Refreshing foreign data wrappers");
			break;
		case OBJECTSLISTREPORT:
			message = _("Foreign data wrappers list report");
			break;
	}

	return message;
}

///////////////////////////////////////////////////

#include "images/foreigndatawrapper.pngc"
#include "images/foreigndatawrapper-sm.pngc"
#include "images/foreigndatawrappers.pngc"

pgForeignDataWrapperFactory::pgForeignDataWrapperFactory()
	: pgDatabaseObjFactory(__("Foreign Data Wrapper"), __("New Foreign Data Wrapper..."), __("Create a new Foreign Data Wrapper."), foreigndatawrapper_png_img, foreigndatawrapper_sm_png_img)
{
}


pgCollection *pgForeignDataWrapperFactory::CreateCollection(pgObject *obj)
{
	return new pgForeignDataWrapperCollection(GetCollectionFactory(), (pgDatabase *)obj);
}

pgForeignDataWrapperFactory foreignDataWrapperFactory;
static pgaCollectionFactory cf(&foreignDataWrapperFactory, __("Foreign Data Wrappers"), foreigndatawrappers_png_img);
