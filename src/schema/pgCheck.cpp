//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
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
    return GetQuotedIdentifier() +  wxT(" CHECK ") + GetDefinition();
}


wxString pgCheck::GetSql(wxTreeCtrl *browser)
{
    if (sql.IsNull())
    {
        sql = wxT("-- ALTER TABLE ") + qtIdent(fkSchema) + wxT(".") + qtIdent(fkTable)
            + wxT(" DROP CONSTRAINT ") + GetQuotedIdentifier() 
            + wxT(";\nALTER TABLE ") + qtIdent(fkSchema) + wxT(".") + qtIdent(fkTable)
            + wxT("\n    ADD CONSTRAINT ") + GetConstraint() 
            + wxT(";\n");
    }

    return sql;
}


void pgCheck::ShowTreeDetail(wxTreeCtrl *browser, frmMain *form, wxListCtrl *properties, wxListCtrl *statistics, ctlSQLBox *sqlPane)
{
    if (properties)
    {
        CreateListColumns(properties);
        int pos=0;

        InsertListItem(properties, pos++, wxT("Name"), GetName());
        InsertListItem(properties, pos++, wxT("OID"), GetOid());
        InsertListItem(properties, pos++, wxT("Definition"), GetDefinition());
        InsertListItem(properties, pos++, wxT("Comment"), GetComment());
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
    pgSet *checks= collection->GetDatabase()->ExecuteSet(wxT(
        "SELECT c.oid, conname, consrc, relname, nspname\n"
        "  FROM pg_constraint c\n"
        "  JOIN pg_class cl ON cl.oid=conrelid\n"
        "  JOIN pg_namespace nl ON nl.oid=relnamespace\n"
        " WHERE contype = 'c' AND conrelid =  ") + NumToStr(collection->GetOid()) 
        + restriction + wxT("::oid\n"
        " ORDER BY conname"));

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
