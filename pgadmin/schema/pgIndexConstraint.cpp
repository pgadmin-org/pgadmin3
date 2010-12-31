//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2010, The pgAdmin Development Team
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
	return GetDatabase()->ExecuteVoid(wxT(
	                                      "ALTER TABLE ") + qtIdent(GetIdxSchema()) + wxT(".") + qtIdent(GetIdxTable())
	                                  + wxT(" DROP CONSTRAINT ") + GetQuotedIdentifier() + wxT(";"));
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
		properties->AppendItem(_("Unique?"), GetIsUnique());
		properties->AppendItem(_("Primary?"), GetIsPrimary());
		properties->AppendItem(_("Clustered?"), GetIsClustered());
		properties->AppendItem(_("Access method"), GetIndexType());
		properties->AppendItem(_("Constraint"), GetConstraint());
		properties->AppendItem(_("System index?"), GetSystemObject());
		if (GetConnection()->BackendMinimumVersion(8, 2))
			properties->AppendItem(_("Fill factor"), GetFillFactor());
		properties->AppendItem(_("Comment"), firstLineOnly(GetComment()));
	}
}

pgObject *pgPrimaryKey::Refresh(ctlTree *browser, const wxTreeItemId item)
{
	pgObject *index = 0;
	pgCollection *coll = browser->GetParentCollection(item);
	if (coll)
		index = primaryKeyFactory.CreateObjects(coll, 0, wxT("\n   AND cls.oid=") + GetOidStr());

	return index;
}

pgObject *pgUnique::Refresh(ctlTree *browser, const wxTreeItemId item)
{
	pgObject *index = 0;
	pgCollection *coll = browser->GetParentCollection(item);
	if (coll)
		index = uniqueFactory.CreateObjects(coll, 0, wxT("\n   AND cls.oid=") + GetOidStr());

	return index;
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


#include "images/primarykey.xpm"

pgPrimaryKeyFactory::pgPrimaryKeyFactory()
	: pgIndexBaseFactory(__("Primary Key"), __("New Primary Key..."), __("Create a new Primary Key constraint."), primarykey_xpm)
{
	metaType = PGM_PRIMARYKEY;
	collectionFactory = &constraintCollectionFactory;
}


pgPrimaryKeyFactory primaryKeyFactory;

#include "images/unique.xpm"

pgUniqueFactory::pgUniqueFactory()
	: pgIndexBaseFactory(__("Unique"), __("New Unique Constraint..."), __("Create a new Unique constraint."), unique_xpm)
{
	metaType = PGM_UNIQUE;
	collectionFactory = &constraintCollectionFactory;
}


pgUniqueFactory uniqueFactory;


#include "images/exclude.xpm"

pgExcludeFactory::pgExcludeFactory()
	: pgIndexBaseFactory(__("Exclude"), __("New Exclusion Constraint..."), __("Create a new Exclusion constraint."), exclude_xpm)
{
	metaType = PGM_EXCLUDE;
	collectionFactory = &constraintCollectionFactory;
}


pgExcludeFactory excludeFactory;
