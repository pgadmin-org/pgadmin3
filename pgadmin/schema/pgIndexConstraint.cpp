//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// pgIndexConstraint.cpp - IndexConstraint class: Primary Key, Unique
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "utils/misc.h"
#include "schema/pgConstraints.h"
#include "schema/pgIndexConstraint.h"



wxString pgIndexConstraint::GetTranslatedMessage(int kindOfMessage) const
{
	wxString message = wxEmptyString;

	switch (kindOfMessage)
	{
		case RETRIEVINGDETAILS:
			message = _("Retrieving details on index constraint");
			message += wxT(" ") + GetName();
			break;
		case REFRESHINGDETAILS:
			message = _("Refreshing index constraint");
			message += wxT(" ") + GetName();
			break;
		case GRANTWIZARDTITLE:
			message = _("Privileges for index constraint");
			message += wxT(" ") + GetName();
			break;
		case DROPINCLUDINGDEPS:
			message = wxString::Format(_("Are you sure you wish to drop index constraint \"%s\" including all objects that depend on it?"),
			                           GetFullIdentifier().c_str());
			break;
		case DROPEXCLUDINGDEPS:
			message = wxString::Format(_("Are you sure you wish to drop index constraint \"%s\"?"),
			                           GetFullIdentifier().c_str());
			break;
		case DROPCASCADETITLE:
			message = _("Drop index constraint cascaded?");
			break;
		case DROPTITLE:
			message = _("Drop index constraint?");
			break;
		case PROPERTIESREPORT:
			message = _("Index constraint properties report");
			message += wxT(" - ") + GetName();
			break;
		case PROPERTIES:
			message = _("Index constraint properties");
			break;
		case DDLREPORT:
			message = _("Index constraint DDL report");
			message += wxT(" - ") + GetName();
			break;
		case DDL:
			message = _("Index constraint DDL");
			break;
		case STATISTICSREPORT:
			message = _("Index constraint statistics report");
			message += wxT(" - ") + GetName();
			break;
		case OBJSTATISTICS:
			message = _("Index constraint statistics");
			break;
		case DEPENDENCIESREPORT:
			message = _("Index constraint dependencies report");
			message += wxT(" - ") + GetName();
			break;
		case DEPENDENCIES:
			message = _("Index constraint dependencies");
			break;
		case DEPENDENTSREPORT:
			message = _("Index constraint dependents report");
			message += wxT(" - ") + GetName();
			break;
		case DEPENDENTS:
			message = _("Index constraint dependents");
			break;
	}

	return message;
}


bool pgIndexConstraint::DropObject(wxFrame *frame, ctlTree *browser, bool cascaded)
{
	wxString sql = wxT("ALTER TABLE ") + qtIdent(GetIdxSchema()) + wxT(".") + qtIdent(GetIdxTable())
	               + wxT(" DROP CONSTRAINT ") + GetQuotedIdentifier();
	if (cascaded)
		sql += wxT(" CASCADE");
	return GetDatabase()->ExecuteVoid(sql);
}


wxString pgIndexConstraint::GetDefinition()
{
	wxString sql = wxEmptyString;

	if (wxString(GetTypeName()).Upper() == wxT("EXCLUDE"))
		sql += wxT("\n  USING ") + GetIndexType() + wxT(" ");

	sql += wxT("(") + GetQuotedColumns() + wxT(")");

	if (GetConnection()->BackendMinimumVersion(8, 2) && GetFillFactor().Length() > 0)
		sql += wxT("\n  WITH (FILLFACTOR=") + GetFillFactor() + wxT(")");

	if (GetConnection()->BackendMinimumVersion(8, 0) && GetTablespace() != GetDatabase()->GetDefaultTablespace())
		sql += wxT("\n  USING INDEX TABLESPACE ") + qtIdent(GetTablespace());

	if (GetConstraint().Length() > 0)
		sql += wxT(" WHERE (") + GetConstraint() + wxT(")");

	if (GetDeferrable())
	{
		sql += wxT("\n  DEFERRABLE INITIALLY ");
		if (GetDeferred())
			sql += wxT("DEFERRED");
		else
			sql += wxT("IMMEDIATE");
	}
	return sql;
}


wxString pgIndexConstraint::GetCreate()
{
	wxString sql;

	sql = GetQuotedIdentifier() + wxT(" ")
	      + GetTypeName().Upper() + GetDefinition();

	return sql;
};


wxString pgIndexConstraint::GetSql(ctlTree *browser)
{
	if (sql.IsNull())
	{
		sql = wxT("-- Constraint: ") + GetQuotedFullIdentifier()
		      + wxT("\n\n-- ALTER TABLE ") + GetQuotedSchemaPrefix(GetIdxSchema()) + qtIdent(GetIdxTable())
		      + wxT(" DROP CONSTRAINT ") + GetQuotedIdentifier() + wxT(";")
		      + wxT("\n\nALTER TABLE ") + GetQuotedSchemaPrefix(GetIdxSchema()) + qtIdent(GetIdxTable())
		      + wxT("\n  ADD CONSTRAINT ")
		      + GetCreate()
		      + wxT(";\n");

		if (!GetComment().IsNull())
		{
			sql += wxT("COMMENT ON CONSTRAINT ") + GetQuotedIdentifier() + wxT(" ON ") + GetQuotedSchemaPrefix(GetIdxSchema()) + qtIdent(GetIdxTable())
			       + wxT(" IS ") + qtDbString(GetComment()) + wxT(";\n");
		}
	}
	return sql;
}



void pgIndexConstraint::ShowTreeDetail(ctlTree *browser, frmMain *form, ctlListView *properties, ctlSQLBox *sqlPane)
{
	ReadColumnDetails();
	if (properties)
	{
		CreateListColumns(properties);

		properties->AppendItem(_("Name"), GetName());
		properties->AppendItem(_("OID"), GetConstraintOid());
		properties->AppendItem(_("Index OID"), GetOid());
		if (GetConnection()->BackendMinimumVersion(8, 0))
			properties->AppendItem(_("Tablespace"), GetTablespace());
		if (GetProcName().IsNull())
			properties->AppendItem(_("Columns"), GetColumns());
		else
		{
			properties->AppendItem(_("Procedure "), GetSchemaPrefix(GetProcNamespace()) + GetProcName() + wxT("(") + GetTypedColumns() + wxT(")"));
			properties->AppendItem(_("Operator classes"), GetOperatorClasses());
		}
		properties->AppendYesNoItem(_("Unique?"), GetIsUnique());
		properties->AppendYesNoItem(_("Primary?"), GetIsPrimary());
		properties->AppendYesNoItem(_("Clustered?"), GetIsClustered());
		properties->AppendYesNoItem(_("Valid?"), GetIsValid());
		properties->AppendItem(_("Access method"), GetIndexType());
		properties->AppendItem(_("Constraint"), GetConstraint());
		properties->AppendYesNoItem(_("System index?"), GetSystemObject());
		if (GetConnection()->BackendMinimumVersion(8, 2))
			properties->AppendItem(_("Fill factor"), GetFillFactor());
		properties->AppendItem(_("Comment"), firstLineOnly(GetComment()));
	}
}

wxString pgPrimaryKey::GetTranslatedMessage(int kindOfMessage) const
{
	wxString message = wxEmptyString;

	switch (kindOfMessage)
	{
		case RETRIEVINGDETAILS:
			message = _("Retrieving details on primary key");
			message += wxT(" ") + GetName();
			break;
		case REFRESHINGDETAILS:
			message = _("Refreshing primary key");
			message += wxT(" ") + GetName();
			break;
		case GRANTWIZARDTITLE:
			message = _("Privileges for primary key");
			message += wxT(" ") + GetName();
			break;
		case DROPINCLUDINGDEPS:
			message = wxString::Format(_("Are you sure you wish to drop primary key \"%s\" including all objects that depend on it?"),
			                           GetFullIdentifier().c_str());
			break;
		case DROPEXCLUDINGDEPS:
			message = wxString::Format(_("Are you sure you wish to drop primary key \"%s\"?"),
			                           GetFullIdentifier().c_str());
			break;
		case DROPCASCADETITLE:
			message = _("Drop primary key cascaded?");
			break;
		case DROPTITLE:
			message = _("Drop primary key?");
			break;
		case PROPERTIESREPORT:
			message = _("Primary key properties report");
			message += wxT(" - ") + GetName();
			break;
		case PROPERTIES:
			message = _("Primary key properties");
			break;
		case DDLREPORT:
			message = _("Primary key DDL report");
			message += wxT(" - ") + GetName();
			break;
		case DDL:
			message = _("Primary key DDL");
			break;
		case STATISTICSREPORT:
			message = _("Primary key statistics report");
			message += wxT(" - ") + GetName();
			break;
		case OBJSTATISTICS:
			message = _("Primary key statistics");
			break;
		case DEPENDENCIESREPORT:
			message = _("Primary key dependencies report");
			message += wxT(" - ") + GetName();
			break;
		case DEPENDENCIES:
			message = _("Primary key dependencies");
			break;
		case DEPENDENTSREPORT:
			message = _("Primary key dependents report");
			message += wxT(" - ") + GetName();
			break;
		case DEPENDENTS:
			message = _("Primary key dependents");
			break;
	}

	return message;
}


pgObject *pgPrimaryKey::Refresh(ctlTree *browser, const wxTreeItemId item)
{
	pgObject *index = 0;
	pgCollection *coll = browser->GetParentCollection(item);
	if (coll)
		index = primaryKeyFactory.CreateObjects(coll, 0, wxT("\n   AND cls.oid=") + GetOidStr());

	return index;
}

wxString pgUnique::GetTranslatedMessage(int kindOfMessage) const
{
	wxString message = wxEmptyString;

	switch (kindOfMessage)
	{
		case RETRIEVINGDETAILS:
			message = _("Retrieving details on unique constraint");
			message += wxT(" ") + GetName();
			break;
		case REFRESHINGDETAILS:
			message = _("Refreshing unique constraint");
			message += wxT(" ") + GetName();
			break;
		case GRANTWIZARDTITLE:
			message = _("Privileges for unique constraint");
			message += wxT(" ") + GetName();
			break;
		case DROPINCLUDINGDEPS:
			message = wxString::Format(_("Are you sure you wish to drop unique constraint \"%s\" including all objects that depend on it?"),
			                           GetFullIdentifier().c_str());
			break;
		case DROPEXCLUDINGDEPS:
			message = wxString::Format(_("Are you sure you wish to drop unique constraint \"%s\"?"),
			                           GetFullIdentifier().c_str());
			break;
		case DROPCASCADETITLE:
			message = _("Drop unique constraint cascaded?");
			break;
		case DROPTITLE:
			message = _("Drop unique constraint?");
			break;
		case PROPERTIESREPORT:
			message = _("Unique constraint properties report");
			message += wxT(" - ") + GetName();
			break;
		case PROPERTIES:
			message = _("Unique constraint properties");
			break;
		case DDLREPORT:
			message = _("Unique constraint DDL report");
			message += wxT(" - ") + GetName();
			break;
		case DDL:
			message = _("Unique constraint DDL");
			break;
		case STATISTICSREPORT:
			message = _("Unique constraint statistics report");
			message += wxT(" - ") + GetName();
			break;
		case OBJSTATISTICS:
			message = _("Unique constraint statistics");
			break;
		case DEPENDENCIESREPORT:
			message = _("Unique constraint dependencies report");
			message += wxT(" - ") + GetName();
			break;
		case DEPENDENCIES:
			message = _("Unique constraint dependencies");
			break;
		case DEPENDENTSREPORT:
			message = _("Unique constraint dependents report");
			message += wxT(" - ") + GetName();
			break;
		case DEPENDENTS:
			message = _("Unique constraint dependents");
			break;
	}

	return message;
}


pgObject *pgUnique::Refresh(ctlTree *browser, const wxTreeItemId item)
{
	pgObject *index = 0;
	pgCollection *coll = browser->GetParentCollection(item);
	if (coll)
		index = uniqueFactory.CreateObjects(coll, 0, wxT("\n   AND cls.oid=") + GetOidStr());

	return index;
}

wxString pgExclude::GetTranslatedMessage(int kindOfMessage) const
{
	wxString message = wxEmptyString;

	switch (kindOfMessage)
	{
		case RETRIEVINGDETAILS:
			message = _("Retrieving details on exclusion constraint");
			message += wxT(" ") + GetName();
			break;
		case REFRESHINGDETAILS:
			message = _("Refreshing exclusion constraint");
			message += wxT(" ") + GetName();
			break;
		case GRANTWIZARDTITLE:
			message = _("Privileges for exclusion constraint");
			message += wxT(" ") + GetName();
			break;
		case DROPINCLUDINGDEPS:
			message = wxString::Format(_("Are you sure you wish to drop exclusion constraint \"%s\" including all objects that depend on it?"),
			                           GetFullIdentifier().c_str());
			break;
		case DROPEXCLUDINGDEPS:
			message = wxString::Format(_("Are you sure you wish to drop exclusion constraint \"%s\"?"),
			                           GetFullIdentifier().c_str());
			break;
		case DROPCASCADETITLE:
			message = _("Drop exclusion constraint cascaded?");
			break;
		case DROPTITLE:
			message = _("Drop exclusion constraint?");
			break;
		case PROPERTIESREPORT:
			message = _("Exclusion constraint properties report");
			message += wxT(" - ") + GetName();
			break;
		case PROPERTIES:
			message = _("Exclusion constraint properties");
			break;
		case DDLREPORT:
			message = _("Exclusion constraint DDL report");
			message += wxT(" - ") + GetName();
			break;
		case DDL:
			message = _("Exclusion constraint DDL");
			break;
		case STATISTICSREPORT:
			message = _("Exclusion constraint statistics report");
			message += wxT(" - ") + GetName();
			break;
		case OBJSTATISTICS:
			message = _("Exclusion constraint statistics");
			break;
		case DEPENDENCIESREPORT:
			message = _("Exclusion constraint dependencies report");
			message += wxT(" - ") + GetName();
			break;
		case DEPENDENCIES:
			message = _("Exclusion constraint dependencies");
			break;
		case DEPENDENTSREPORT:
			message = _("Exclusion constraint dependents report");
			message += wxT(" - ") + GetName();
			break;
		case DEPENDENTS:
			message = _("Exclusion constraint dependents");
			break;
	}

	return message;
}


pgObject *pgExclude::Refresh(ctlTree *browser, const wxTreeItemId item)
{
	pgObject *index = 0;
	pgCollection *coll = browser->GetParentCollection(item);
	if (coll)
		index = excludeFactory.CreateObjects(coll, 0, wxT("\n   AND cls.oid=") + GetOidStr());

	return index;
}

pgObject *pgPrimaryKeyFactory::CreateObjects(pgCollection *collection, ctlTree *browser, const wxString &where)
{
	return pgIndexBaseFactory::CreateObjects(collection, browser, wxT("   AND contype='p'\n") + where);
}


pgObject *pgUniqueFactory::CreateObjects(pgCollection *collection, ctlTree *browser, const wxString &where)
{
	return pgIndexBaseFactory::CreateObjects(collection, browser, wxT("   AND contype='u'\n") + where);
}


pgObject *pgExcludeFactory::CreateObjects(pgCollection *collection, ctlTree *browser, const wxString &where)
{
	return pgIndexBaseFactory::CreateObjects(collection, browser, wxT("   AND contype='x'\n") + where);
}


#include "images/primarykey.pngc"

pgPrimaryKeyFactory::pgPrimaryKeyFactory()
	: pgIndexBaseFactory(__("Primary Key"), __("New Primary Key..."), __("Create a new Primary Key constraint."), primarykey_png_img)
{
	metaType = PGM_PRIMARYKEY;
	collectionFactory = &constraintCollectionFactory;
}


pgPrimaryKeyFactory primaryKeyFactory;

#include "images/unique.pngc"

pgUniqueFactory::pgUniqueFactory()
	: pgIndexBaseFactory(__("Unique"), __("New Unique Constraint..."), __("Create a new Unique constraint."), unique_png_img)
{
	metaType = PGM_UNIQUE;
	collectionFactory = &constraintCollectionFactory;
}


pgUniqueFactory uniqueFactory;


#include "images/exclude.pngc"

pgExcludeFactory::pgExcludeFactory()
	: pgIndexBaseFactory(__("Exclude"), __("New Exclusion Constraint..."), __("Create a new Exclusion constraint."), exclude_png_img)
{
	metaType = PGM_EXCLUDE;
	collectionFactory = &constraintCollectionFactory;
}


pgExcludeFactory excludeFactory;
