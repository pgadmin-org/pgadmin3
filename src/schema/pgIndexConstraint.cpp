//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2003, The pgAdmin Development Team
// This software is released under the Artistic Licence
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
            + wxT(" DROP CONSTRAINT ") + GetQuotedIdentifier() + wxT(";"));
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

    sql = GetQuotedIdentifier() + wxT(" ") 
        + GetTypeName().Upper() + GetDefinition();

    return sql;
};


wxString pgIndexConstraint::GetSql(wxTreeCtrl *browser)
{
    if (sql.IsNull())
    {
        sql = wxT("-- Constraint: ") + GetQuotedFullIdentifier() 
            + wxT("\n\n-- ALTER TABLE ") + qtIdent(GetIdxSchema()) + wxT(".") + qtIdent(GetIdxTable())
            + wxT(" DROP CONSTRAINT ") + GetQuotedIdentifier() 
            + wxT("\n\nALTER TABLE ") + qtIdent(GetIdxSchema()) + wxT(".") + qtIdent(GetIdxTable())
            + wxT("\n  ADD CONSTRAINT ")
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

        InsertListItem(properties, pos++, _("Name"), GetName());
        InsertListItem(properties, pos++, _("OID"), GetOid());
        if (GetProcName().IsNull())
            InsertListItem(properties, pos++, _("Columns"), GetColumns());
        else
        {
            InsertListItem(properties, pos++, _("Procedure "), GetProcNamespace() + wxT(".")+GetProcName()+wxT("(")+GetTypedColumns()+wxT(")"));
            InsertListItem(properties, pos++, _("Operator classes"), GetOperatorClasses());
        }
        InsertListItem(properties, pos++, _("Unique?"), GetIsUnique());
        InsertListItem(properties, pos++, _("Primary?"), GetIsPrimary());
        InsertListItem(properties, pos++, _("Clustered?"), GetIsClustered());
        InsertListItem(properties, pos++, _("Access method"), GetIndexType());
        InsertListItem(properties, pos++, _("Constraint"), GetConstraint());
        InsertListItem(properties, pos++, _("System index?"), GetSystemObject());
        InsertListItem(properties, pos++, _("Comment"), GetComment());
    }
}



pgObject *pgPrimaryKey::ReadObjects(pgCollection *collection, wxTreeCtrl *browser, const wxString &where)
{
    return pgIndex::ReadObjects(collection, browser, wxT("   AND contype='p'\n") + where);
}


pgObject *pgUnique::ReadObjects(pgCollection *collection, wxTreeCtrl *browser, const wxString &where)
{
    return pgIndex::ReadObjects(collection, browser, wxT("   AND contype='u'\n") + where);
}

