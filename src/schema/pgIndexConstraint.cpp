//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
//
// pgIndexConstraint.cpp - IndexConstraint class: Primary Key, Unique
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "misc.h"
#include "pgObject.h"
#include "pgConstraints.h"
#include "pgIndexConstraint.h"



bool pgIndexConstraint::DropObject(wxFrame *frame, wxTreeCtrl *browser)
{
    return GetDatabase()->ExecuteVoid(wxT(
        "ALTER TABLE ") + qtIdent(GetIdxSchema()) + wxT(".") + qtIdent(GetIdxTable())
            + wxT(" DROP CONSTRAINT ") + GetQuotedFullIdentifier());
}


wxString pgIndexConstraint::GetDefinition()
{
    wxString sql;

    sql = wxT("(") + GetQuotedColumns() + wxT(")");

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

    sql = GetQuotedFullIdentifier() + wxT(" ") 
        + GetTypeName().Upper() + GetDefinition();

    return sql;
};


wxString pgIndexConstraint::GetSql(wxTreeCtrl *browser)
{
    if (sql.IsNull())
    {
        sql = wxT("-- Constraint: ") + GetQuotedFullIdentifier() 
            + wxT("\n-- ALTER TABLE ") + qtIdent(GetIdxSchema()) + wxT(".") + qtIdent(GetIdxTable())
            + wxT(" DROP CONSTRAINT ") + GetQuotedFullIdentifier() 
            + wxT("\nALTER TABLE ") + qtIdent(GetIdxSchema()) + wxT(".") + qtIdent(GetIdxTable())
            + wxT(" ADD CONSTRAINT ")
            + GetCreate()
            + wxT(";\n")
            + GetCommentSql();
    }
    return sql;
}



void pgIndexConstraint::ShowTreeDetail(wxTreeCtrl *browser, frmMain *form, wxListCtrl *properties, wxListCtrl *statistics, ctlSQLBox *sqlPane)
{
    ReadColumnDetails();
    if (properties)
    {
        CreateListColumns(properties);
        int pos=0;

        InsertListItem(properties, pos++, wxT("Name"), GetName());
        InsertListItem(properties, pos++, wxT("OID"), GetOid());
        if (GetProcName().IsNull())
            InsertListItem(properties, pos++, wxT("Columns"), GetColumns());
        else
        {
            InsertListItem(properties, pos++, wxT("Procedure "), GetProcNamespace() + wxT(".")+GetProcName()+wxT("(")+GetTypedColumns()+wxT(")"));
            InsertListItem(properties, pos++, wxT("Operator Classes"), GetOperatorClasses());
        }
        InsertListItem(properties, pos++, wxT("Unique?"), GetIsUnique());
        InsertListItem(properties, pos++, wxT("Primary?"), GetIsPrimary());
        InsertListItem(properties, pos++, wxT("Clustered?"), GetIsClustered());
        InsertListItem(properties, pos++, wxT("Index Type"), GetIndexType());
        InsertListItem(properties, pos++, wxT("Constraint"), GetConstraint());
        InsertListItem(properties, pos++, wxT("System index?"), GetSystemObject());
        InsertListItem(properties, pos++, wxT("Comment"), GetComment());
    }
}



pgObject *pgPrimaryKey::ReadObjects(pgCollection *collection, wxTreeCtrl *browser)
{
    return pgIndex::ReadObjects(collection, browser, wxT("   AND contype='p'\n"));
}


pgObject *pgUnique::ReadObjects(pgCollection *collection, wxTreeCtrl *browser)
{
    return pgIndex::ReadObjects(collection, browser, wxT("   AND contype='u'\n"));
}
