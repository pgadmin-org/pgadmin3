//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2006, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// pgIndexConstraint.cpp - IndexConstraint class: Primary Key, Unique
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "utils/misc.h"
#include "pgConstraints.h"
#include "pgIndexConstraint.h"



bool pgIndexConstraint::DropObject(wxFrame *frame, ctlTree *browser, bool cascaded)
{
    return GetDatabase()->ExecuteVoid(wxT(
        "ALTER TABLE ") + GetQuotedSchemaPrefix(GetIdxSchema()) + qtIdent(GetIdxTable())
            + wxT(" DROP CONSTRAINT ") + GetQuotedIdentifier() + wxT(";"));
}


wxString pgIndexConstraint::GetDefinition()
{
    wxString sql;

    sql = wxT("(") + GetQuotedColumns() + wxT(")");

    if (!GetTablespace().IsEmpty())
        sql += wxT(" USING INDEX TABLESPACE ") + qtIdent(GetTablespace());

    if (GetDeferrable())
    {
        sql += wxT(" DEFERRABLE INITIALLY ");
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
        properties->AppendItem(_("OID"), GetOid());
        if (GetProcName().IsNull())
            properties->AppendItem(_("Columns"), GetColumns());
        else
        {
            properties->AppendItem(_("Procedure "), GetSchemaPrefix(GetProcNamespace()) + GetProcName()+wxT("(")+GetTypedColumns()+wxT(")"));
            properties->AppendItem(_("Operator classes"), GetOperatorClasses());
        }
        properties->AppendItem(_("Unique?"), GetIsUnique());
        properties->AppendItem(_("Primary?"), GetIsPrimary());
        properties->AppendItem(_("Clustered?"), GetIsClustered());
        properties->AppendItem(_("Access method"), GetIndexType());
        properties->AppendItem(_("Constraint"), GetConstraint());
        properties->AppendItem(_("System index?"), GetSystemObject());
        properties->AppendItem(_("Comment"), GetComment());
    }
}



pgObject *pgPrimaryKeyFactory::CreateObjects(pgCollection *collection, ctlTree *browser, const wxString &where)
{
    return pgIndexBaseFactory::CreateObjects(collection, browser, wxT("   AND contype='p'\n") + where);
}


pgObject *pgUniqueFactory::CreateObjects(pgCollection *collection, ctlTree *browser, const wxString &where)
{
    return pgIndexBaseFactory::CreateObjects(collection, browser, wxT("   AND contype='u'\n") + where);
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
