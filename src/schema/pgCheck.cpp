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
            + wxT(" ADD CONSTRAINT ") + GetConstraint() 
            + wxT(";\n");
    }

    return sql;
}


void pgCheck::ShowTreeDetail(wxTreeCtrl *browser, frmMain *form, wxListCtrl *properties, wxListCtrl *statistics, ctlSQLBox *sqlPane)
{
    SetButtons(form);

    if (properties)
    {
        properties->ClearAll();
        properties->InsertColumn(0, wxT("Property"), wxLIST_FORMAT_LEFT, 150);
        properties->InsertColumn(1, wxT("Value"), wxLIST_FORMAT_LEFT, 200);
  

        int pos=0;

        InsertListItem(properties, pos++, wxT("Name"), GetName());
        InsertListItem(properties, pos++, wxT("OID"), NumToStr(GetOid()));
        InsertListItem(properties, pos++, wxT("Definition"), GetDefinition());
        InsertListItem(properties, pos++, wxT("Comment"), GetComment());
    }
}



void pgCheck::ShowTreeCollection(pgCollection *collection, frmMain *form, wxTreeCtrl *browser, wxListCtrl *properties, wxListCtrl *statistics, ctlSQLBox *sqlPane)
{
    pgCheck *check;

    if (browser->GetChildrenCount(collection->GetId(), FALSE) == 0)
    {
        // Log
        wxLogInfo(wxT("Adding Checks to schema ") + collection->GetSchema()->GetIdentifier());

        // Get the Checks
        pgSet *checks= collection->GetDatabase()->ExecuteSet(wxT(
            "SELECT c.oid, conname, consrc, relname, nspname\n"
            "  FROM pg_constraint c\n"
            "  JOIN pg_class cl ON cl.oid=conrelid\n"
            "  JOIN pg_namespace nl ON nl.oid=relnamespace\n"
            " WHERE contype = 'c' AND conrelid =  ") + NumToStr(collection->GetOid()) + wxT("::oid\n"
            " ORDER BY conname"));

        if (checks)
        {
            while (!checks->Eof())
            {
                check = new pgCheck(collection->GetSchema(), checks->GetVal(wxT("conname")));

                check->iSetOid(StrToDouble(checks->GetVal(wxT("oid"))));
                check->iSetDefinition(checks->GetVal(wxT("consrc")));
                check->iSetFkTable(checks->GetVal(wxT("relname")));
                check->iSetFkSchema(checks->GetVal(wxT("nspname")));

                browser->AppendItem(collection->GetId(), check->GetFullName(), PGICON_CHECK, -1, check);
	    
			    checks->MoveNext();
            }

		    delete checks;
        }
    }
}

