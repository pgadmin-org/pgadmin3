//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2010, The pgAdmin Development Team
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


pgForeignDataWrapper::pgForeignDataWrapper(const wxString &newName)
	: pgDatabaseObject(foreignDataWrapperFactory, newName)
{
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
	if (properties)
	{
		CreateListColumns(properties);

		properties->AppendItem(_("Name"), GetName());
		properties->AppendItem(_("OID"), GetOid());
		properties->AppendItem(_("Owner"), GetOwner());
		properties->AppendItem(_("ACL"), GetAcl());
		properties->AppendItem(_("Validator"), GetValidatorProc());
		properties->AppendItem(_("Options"), GetOptions());
		properties->AppendItem(_("Comment"), firstLineOnly(GetComment()));
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



pgObject *pgForeignDataWrapperFactory::CreateObjects(pgCollection *collection, ctlTree *browser, const wxString &restriction)
{
	wxString sql;
	pgForeignDataWrapper *fdw = 0;

	sql = wxT("SELECT fdw.oid, fdwname, fdwvalidator, fdwacl, vp.proname as fdwval, description, ")
	      wxT("array_to_string(fdwoptions, ',') AS fdwoptions, ")
	      wxT("pg_get_userbyid(fdwowner) as fdwowner\n");
	sql += wxT("  FROM pg_foreign_data_wrapper fdw\n")
	       wxT("  LEFT OUTER JOIN pg_proc vp on vp.oid=fdwvalidator\n")
	       wxT("  LEFT OUTER JOIN pg_description des ON des.objoid=fdw.oid AND des.objsubid=0\n")
	       + restriction + wxT("\n")
	       wxT(" ORDER BY fdwname");
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
			fdw->iSetComment(fdws->GetVal(wxT("description")));
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


///////////////////////////////////////////////////

#include "images/foreigndatawrapper.pngc"
#include "images/foreigndatawrapper-sm.pngc"
#include "images/foreigndatawrappers.pngc"

pgForeignDataWrapperFactory::pgForeignDataWrapperFactory()
	: pgDatabaseObjFactory(__("Foreign Data Wrapper"), __("New Foreign Data Wrapper..."), __("Create a new Foreign Data Wrapper."), foreigndatawrapper_png_img, foreigndatawrapper_sm_png_img)
{
}


pgForeignDataWrapperFactory foreignDataWrapperFactory;
static pgaCollectionFactory cf(&foreignDataWrapperFactory, __("Foreign Data Wrappers"), foreigndatawrappers_png_img);
