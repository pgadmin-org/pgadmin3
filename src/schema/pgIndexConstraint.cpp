//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2004, The pgAdmin Development Team
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


wxString pgIndexConstraint::GetSql(wxTreeCtrl *browser)
{
    if (sql.IsNull())
    {
        sql = wxT("-- Constraint: ") + GetQuotedFullIdentifier() 
            + wxT("\n\n-- ALTER TABLE ") + GetQuotedSchemaPrefix(GetIdxSchema()) + qtIdent(GetIdxTable())
            + wxT(" DROP CONSTRAINT ") + GetQuotedIdentifier() 
            + wxT("\n\nALTER TABLE ") + GetQuotedSchemaPrefix(GetIdxSchema()) + qtIdent(GetIdxTable())
            + wxT("\n  ADD CONSTRAINT ")
            + GetCreate()
            + wxT(";\n");

		if (!GetComment().IsNull())
		{
		    sql += wxT("COMMENT ON CONSTRAINT ") + GetQuotedIdentifier() + wxT(" ON ") + GetQuotedSchemaPrefix(GetIdxSchema()) + qtIdent(GetIdxTable())
			    + wxT(" IS ") + qtString(GetComment()) + wxT(";\n");
		}
    }
    return sql;
}



void pgIndexConstraint::ShowTreeDetail(wxTreeCtrl *browser, frmMain *form, ctlListView *properties, ctlSQLBox *sqlPane)
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



pgObject *pgPrimaryKey::ReadObjects(pgCollection *collection, wxTreeCtrl *browser, const wxString &where)
{
    return pgIndex::ReadObjects(collection, browser, wxT("   AND contype='p'\n") + where);
}


pgObject *pgUnique::ReadObjects(pgCollection *collection, wxTreeCtrl *browser, const wxString &where)
{
    return pgIndex::ReadObjects(collection, browser, wxT("   AND contype='u'\n") + where);
}

