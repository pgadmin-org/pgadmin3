//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002 - 2003, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// pgCheck.cpp - Check class
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "misc.h"
#include "pgObject.h"
#include "pgCheck.h"
#include "pgCollection.h"


pgCheck::pgCheck(pgSchema *newSchema, const wxString& newName)
: pgSchemaObject(newSchema, PG_CHECK, newName)
{
}

pgCheck::~pgCheck()
{
}


bool pgCheck::DropObject(wxFrame *frame, wxTreeCtrl *browser)
{
    return GetDatabase()->ExecuteVoid(
        wxT("ALTER TABLE ") + qtIdent(fkSchema) + wxT(".") + qtIdent(fkTable)
        + wxT(" DROP CONSTRAINT ") + GetQuotedIdentifier());
    
}


wxString pgCheck::GetConstraint()
{
    return GetQuotedIdentifier() +  wxT(" CHECK (") + GetDefinition() + wxT(")");
}


wxString pgCheck::GetSql(wxTreeCtrl *browser)
{
    if (sql.IsNull())
    {
        sql = wxT("-- Check: ") + GetQuotedFullIdentifier() + wxT("\n\n")
            + wxT("-- ALTER TABLE ") + qtIdent(fkSchema) + wxT(".") + qtIdent(fkTable)
            + wxT(" DROP CONSTRAINT ") + GetQuotedIdentifier() 
            + wxT(";\n\nALTER TABLE ") + qtIdent(fkSchema) + wxT(".") + qtIdent(fkTable)
            + wxT("\n  ADD CONSTRAINT ") + GetConstraint() 
            + wxT(";\n");
        sql += GetCommentSql();
    }

    return sql;
}


void pgCheck::ShowTreeDetail(wxTreeCtrl *browser, frmMain *form, wxListCtrl *properties, wxListCtrl *statistics, ctlSQLBox *sqlPane)
{
    if (properties)
    {
        CreateListColumns(properties);
        int pos=0;

        InsertListItem(properties, pos++, _("Name"), GetName());
        InsertListItem(properties, pos++, _("OID"), GetOid());
        InsertListItem(properties, pos++, _("Definition"), GetDefinition());
        InsertListItem(properties, pos++, _("Deferrable?"), BoolToYesNo(GetDeferrable()));
        InsertListItem(properties, pos++, _("Initially?"), 
            GetDeferred() ? wxT("DEFERRED") : wxT("IMMEDIATE"));
        InsertListItem(properties, pos++, _("Comment"), GetComment());
    }
}


pgObject *pgCheck::Refresh(wxTreeCtrl *browser, const wxTreeItemId item)
{
    pgObject *check=0;
    wxTreeItemId parentItem=browser->GetItemParent(item);
    if (parentItem)
    {
        pgObject *obj=(pgObject*)browser->GetItemData(parentItem);
        if (obj->GetType() == PG_CONSTRAINTS)
            check = ReadObjects((pgCollection*)obj, 0, wxT("\n   AND c.oid=") + GetOidStr());
    }
    return check;
}



pgObject *pgCheck::ReadObjects(pgCollection *collection, wxTreeCtrl *browser, const wxString &restriction)
{
    pgCheck *check=0;
    pgSet *checks= collection->GetDatabase()->ExecuteSet(
        wxT("SELECT c.oid, conname, condeferrable, condeferred, relname, nspname, description,\n")
        wxT("       pg_get_expr(conbin, conrelid") + collection->GetDatabase()->GetPrettyOption() + wxT(") as consrc\n")
        wxT("  FROM pg_constraint c\n")
        wxT("  JOIN pg_class cl ON cl.oid=conrelid\n")
        wxT("  JOIN pg_namespace nl ON nl.oid=relnamespace\n")
        wxT("  LEFT OUTER JOIN pg_description des ON des.objoid=c.oid\n")
        wxT(" WHERE contype = 'c' AND conrelid =  ") + NumToStr(collection->GetOid())
        + restriction + wxT("::oid\n")
        wxT(" ORDER BY conname"));

    if (checks)
    {
        while (!checks->Eof())
        {
            check = new pgCheck(collection->GetSchema(), checks->GetVal(wxT("conname")));

            check->iSetOid(checks->GetOid(wxT("oid")));
            check->iSetTableOid(collection->GetOid());
            check->iSetDefinition(checks->GetVal(wxT("consrc")));
            check->iSetFkTable(checks->GetVal(wxT("relname")));
            check->iSetFkSchema(checks->GetVal(wxT("nspname")));
            check->iSetDeferrable(checks->GetBool(wxT("condeferrable")));
            check->iSetDeferred(checks->GetBool(wxT("condeferred")));
            check->iSetComment(checks->GetVal(wxT("description")));

            if (browser)
            {
                collection->AppendBrowserItem(browser, check);
    			checks->MoveNext();
            }
            else
                break;
        }

		delete checks;
    }
    return check;
}
