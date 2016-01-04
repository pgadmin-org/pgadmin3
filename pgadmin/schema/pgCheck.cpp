//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// pgCheck.cpp - Check class
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "frm/frmMain.h"
#include "utils/misc.h"
#include "schema/pgCheck.h"


pgCheck::pgCheck(pgSchema *newSchema, const wxString &newName)
	: pgSchemaObject(newSchema, checkFactory, newName)
{
}

pgCheck::~pgCheck()
{
}


wxString pgCheck::GetTranslatedMessage(int kindOfMessage) const
{
	wxString message = wxEmptyString;

	switch (kindOfMessage)
	{
		case RETRIEVINGDETAILS:
			message = _("Retrieving details on check constraint");
			message += wxT(" ") + GetName();
			break;
		case REFRESHINGDETAILS:
			message = _("Refreshing check constraint");
			message += wxT(" ") + GetName();
			break;
		case GRANTWIZARDTITLE:
			message = _("Privileges for check constraint");
			message += wxT(" ") + GetName();
			break;
		case DROPINCLUDINGDEPS:
			message = wxString::Format(_("Are you sure you wish to drop check constraint \"%s\" including all objects that depend on it?"),
			                           GetFullIdentifier().c_str());
			break;
		case DROPEXCLUDINGDEPS:
			message = wxString::Format(_("Are you sure you wish to drop check constraint \"%s\"?"),
			                           GetFullIdentifier().c_str());
			break;
		case DROPCASCADETITLE:
			message = _("Drop check constraint cascaded?");
			break;
		case DROPTITLE:
			message = _("Drop check constraint?");
			break;
		case PROPERTIESREPORT:
			message = _("Check constraint properties report");
			message += wxT(" - ") + GetName();
			break;
		case PROPERTIES:
			message = _("Check constraint properties");
			break;
		case DDLREPORT:
			message = _("Check constraint DDL report");
			message += wxT(" - ") + GetName();
			break;
		case DDL:
			message = _("Check constraint DDL");
			break;
		case DEPENDENCIESREPORT:
			message = _("Check constraint dependencies report");
			message += wxT(" - ") + GetName();
			break;
		case DEPENDENCIES:
			message = _("Check constraint dependencies");
			break;
		case DEPENDENTSREPORT:
			message = _("Check constraint dependents report");
			message += wxT(" - ") + GetName();
			break;
		case DEPENDENTS:
			message = _("Check constraint dependents");
			break;
	}

	return message;
}


int pgCheck::GetIconId()
{
	if (!GetDatabase()->BackendMinimumVersion(9, 2) || GetValid())
		return checkFactory.GetIconId();
	else
		return checkFactory.GetClosedIconId();
}


bool pgCheck::DropObject(wxFrame *frame, ctlTree *browser, bool cascaded)
{
	wxString sql = wxT("ALTER ") + objectKind + wxT(" ") + qtIdent(objectSchema) + wxT(".") + qtIdent(objectName)
	               + wxT(" DROP CONSTRAINT ") + GetQuotedIdentifier();
	if (cascaded)
		sql += wxT(" CASCADE");
	return GetDatabase()->ExecuteVoid(sql);
}


wxString pgCheck::GetConstraint()
{
	sql = GetQuotedIdentifier() +  wxT(" CHECK ");

	sql += wxT("(") + GetDefinition() + wxT(")");

	if (GetDatabase()->BackendMinimumVersion(9, 2) && GetNoInherit())
		sql += wxT(" NO INHERIT");

	if (GetDatabase()->BackendMinimumVersion(9, 2) && !GetValid())
		sql += wxT(" NOT VALID");

	return sql;
}


wxString pgCheck::GetSql(ctlTree *browser)
{
	if (sql.IsNull())
	{
		sql = wxT("-- Check: ") + GetQuotedFullIdentifier() + wxT("\n\n")
		      + wxT("-- ALTER ") + objectKind + wxT(" ") + GetQuotedSchemaPrefix(objectSchema) + qtIdent(objectName)
		      + wxT(" DROP CONSTRAINT ") + GetQuotedIdentifier()
		      + wxT(";\n\nALTER ") + objectKind + wxT(" ") + GetQuotedSchemaPrefix(objectSchema) + qtIdent(objectName)
		      + wxT("\n  ADD CONSTRAINT ") + GetConstraint()
		      + wxT(";\n");

		if (!GetComment().IsNull())
		{
			sql += wxT("COMMENT ON CONSTRAINT ") + GetQuotedIdentifier()
			       + wxT(" ON ") + GetQuotedSchemaPrefix(objectSchema) + qtIdent(objectName)
			       + wxT(" IS ") + qtDbString(GetComment()) + wxT(";\n");
		}
	}

	return sql;
}


void pgCheck::ShowTreeDetail(ctlTree *browser, frmMain *form, ctlListView *properties, ctlSQLBox *sqlPane)
{
	if (properties)
	{
		CreateListColumns(properties);

		properties->AppendItem(_("Name"), GetName());
		properties->AppendItem(_("OID"), GetOid());
		properties->AppendItem(_("Definition"), GetDefinition());
		if (GetDatabase()->BackendMinimumVersion(9, 2))
		{
			properties->AppendItem(_("No Inherit?"), BoolToYesNo(GetNoInherit()));
			properties->AppendItem(_("Valid?"), BoolToYesNo(GetValid()));
		}
		// Check constraints on a domain don't have comments
		if (objectKind.Upper() == wxT("TABLE"))
			properties->AppendItem(_("Comment"), firstLineOnly(GetComment()));
	}
}


pgObject *pgCheck::Refresh(ctlTree *browser, const wxTreeItemId item)
{
	pgObject *check = 0;

	pgCollection *coll = browser->GetParentCollection(item);
	if (coll)
		check = checkFactory.CreateObjects(coll, 0, wxT("\n   AND c.oid=") + GetOidStr());

	return check;
}


void pgCheck::Validate(frmMain *form)
{
	wxString sql = wxT("ALTER ") + objectKind + wxT(" ")
	               + GetQuotedSchemaPrefix(objectSchema) + qtIdent(objectName)
	               + wxT("\n  VALIDATE CONSTRAINT ") + GetQuotedIdentifier();
	GetDatabase()->ExecuteVoid(sql);

	iSetValid(true);
	UpdateIcon(form->GetBrowser());
}


pgObject *pgCheckFactory::CreateObjects(pgCollection *coll, ctlTree *browser, const wxString &restriction)
{
	pgSchemaObjCollection *collection = (pgSchemaObjCollection *)coll;
	pgCheck *check = 0;

	wxString connoinherit = collection->GetDatabase()->BackendMinimumVersion(9, 2) ? wxT(", connoinherit") : wxEmptyString;
	wxString convalidated = collection->GetDatabase()->BackendMinimumVersion(9, 2) ? wxT(", convalidated") : wxEmptyString;

	wxString sql =
	    wxT("SELECT 'TABLE' AS objectkind, c.oid, conname, relname, nspname, description,\n")
	    wxT("       pg_get_expr(conbin, conrelid") + collection->GetDatabase()->GetPrettyOption() + wxT(") as consrc\n")
	    + connoinherit + convalidated +
	    wxT("  FROM pg_constraint c\n")
	    wxT("  JOIN pg_class cl ON cl.oid=conrelid\n")
	    wxT("  JOIN pg_namespace nl ON nl.oid=relnamespace\n")
	    wxT("  LEFT OUTER JOIN pg_description des ON (des.objoid=c.oid AND des.classoid='pg_constraint'::regclass)\n")
	    wxT(" WHERE contype = 'c' AND conrelid =  ") + NumToStr(collection->GetOid())
	    + restriction + wxT("::oid\n")
	    wxT("UNION\n")
	    wxT("SELECT 'DOMAIN' AS objectkind, c.oid, conname, typname as relname, nspname, description,\n")
	    wxT("       regexp_replace(pg_get_constraintdef(c.oid, true), E'CHECK \\\\((.*)\\\\).*', E'\\\\1') as consrc\n")
	    + connoinherit + convalidated +
	    wxT("  FROM pg_constraint c\n")
	    wxT("  JOIN pg_type t ON t.oid=contypid\n")
	    wxT("  JOIN pg_namespace nl ON nl.oid=typnamespace\n")
	    wxT("  LEFT OUTER JOIN pg_description des ON (des.objoid=t.oid AND des.classoid='pg_constraint'::regclass)\n")
	    wxT(" WHERE contype = 'c' AND contypid =  ") + NumToStr(collection->GetOid())
	    + restriction + wxT("::oid\n")
	    wxT(" ORDER BY conname");

	pgSet *checks = collection->GetDatabase()->ExecuteSet(sql);

	if (checks)
	{
		while (!checks->Eof())
		{
			check = new pgCheck(collection->GetSchema()->GetSchema(), checks->GetVal(wxT("conname")));

			check->iSetOid(checks->GetOid(wxT("oid")));
			check->iSetDefinition(checks->GetVal(wxT("consrc")));
			check->iSetObjectKind(checks->GetVal(wxT("objectkind")));
			check->iSetObjectName(checks->GetVal(wxT("relname")));
			check->iSetObjectSchema(checks->GetVal(wxT("nspname")));
			if (collection->GetDatabase()->BackendMinimumVersion(9, 2))
			{
				check->iSetNoInherit(checks->GetBool(wxT("connoinherit")));
				check->iSetValid(checks->GetBool(wxT("convalidated")));
			}
			check->iSetComment(checks->GetVal(wxT("description")));

			if (browser)
			{
				browser->AppendObject(collection, check);
				checks->MoveNext();
			}
			else
				break;
		}

		delete checks;
	}
	return check;
}

/////////////////////////////

wxString pgCheckCollection::GetTranslatedMessage(int kindOfMessage) const
{
	wxString message = wxEmptyString;

	switch (kindOfMessage)
	{
		case RETRIEVINGDETAILS:
			message = _("Retrieving details on check constraints");
			break;
		case REFRESHINGDETAILS:
			message = _("Refreshing check constraints");
			break;
		case OBJECTSLISTREPORT:
			message = _("Check constraints list report");
			break;
	}

	return message;
}

/////////////////////////////

#include "images/check.pngc"
#include "images/checkbad.pngc"

pgCheckFactory::pgCheckFactory()
	: pgSchemaObjFactory(__("Check"), __("New Check..."), __("Create a new Check constraint."), check_png_img)
{
	metaType = PGM_CHECK;
	collectionFactory = &constraintCollectionFactory;
	closedId = addIcon(checkbad_png_img);
}


pgCheckFactory checkFactory;

validateCheckFactory::validateCheckFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar) : contextActionFactory(list)
{
	mnu->Append(id, _("Validate check constraint"), _("Validate the selected check constraint."));
}


wxWindow *validateCheckFactory::StartDialog(frmMain *form, pgObject *obj)
{
	((pgCheck *)obj)->Validate(form);
	((pgCheck *)obj)->SetDirty();

	wxTreeItemId item = form->GetBrowser()->GetSelection();
	if (obj == form->GetBrowser()->GetObject(item))
	{
		obj->ShowTreeDetail(form->GetBrowser(), 0, form->GetProperties());
		form->GetSqlPane()->SetReadOnly(false);
		form->GetSqlPane()->SetText(((pgCheck *)obj)->GetSql(form->GetBrowser()));
		form->GetSqlPane()->SetReadOnly(true);
	}
	form->GetMenuFactories()->CheckMenu(obj, form->GetMenuBar(), (ctlMenuToolbar *)form->GetToolBar());

	return 0;
}


bool validateCheckFactory::CheckEnable(pgObject *obj)
{
	return obj && obj->IsCreatedBy(checkFactory) && obj->CanEdit()
	       && ((pgCheck *)obj)->GetConnection()->BackendMinimumVersion(9, 2)
	       && !((pgCheck *)obj)->GetValid();
}
