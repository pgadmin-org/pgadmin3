//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
//
// pgOperatorClass.cpp - OperatorClass class
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "misc.h"
#include "pgObject.h"
#include "pgOperatorClass.h"
#include "pgCollection.h"


pgOperatorClass::pgOperatorClass(pgSchema *newSchema, const wxString& newName)
: pgSchemaObject(newSchema, PG_OPERATORCLASS, newName)
{
}

pgOperatorClass::~pgOperatorClass()
{
}



wxString pgOperatorClass::GetSql(wxTreeCtrl *browser)
{
    if (sql.IsNull())
    {
        sql=wxT("Not yet implemented.\n");
    }

    return sql;
}

void pgOperatorClass::ShowTreeDetail(wxTreeCtrl *browser, frmMain *form, wxListCtrl *properties, wxListCtrl *statistics, ctlSQLBox *sqlPane)
{
    SetButtons(form);

    if (properties)
    {
        properties->ClearAll();
        properties->InsertColumn(0, wxT("Property"), wxLIST_FORMAT_LEFT, 150);
        properties->InsertColumn(1, wxT("Value"), wxLIST_FORMAT_LEFT, 200);

        int pos=0;

        InsertListItem(properties, pos++, wxT("Name"), GetName());
        InsertListItem(properties, pos++, wxT("OID"), GetOid());
    }
}



void pgOperatorClass::ShowTreeCollection(pgCollection *collection, frmMain *form, wxTreeCtrl *browser, wxListCtrl *properties, wxListCtrl *statistics, ctlSQLBox *sqlPane)
{

    if (browser->GetChildrenCount(collection->GetId(), FALSE) == 0)
    {
        // Log
        wxLogInfo(wxT("Adding OperatorClasss to schema ") + collection->GetSchema()->GetIdentifier());

        // Get the OperatorClasss
        pgSet *operatorClasses= collection->GetDatabase()->ExecuteSet(wxT(
            "SELECT op.*, it.typname as intypename, dt.typname as keytypename\n"
            "  FROM pg_opclass op\n"
            "  JOIN pg_type it ON it.oid=opcintype\n"
            "  JOIN pg_type dt ON dt.oid=opckeytype\n"
            " WHERE opcnamespace = ") + collection->GetSchema()->GetOidStr() + wxT("\n"
            " ORDER BY opcname"));

        if (operatorClasses)
        {
            while (!operatorClasses->Eof())
            {
                pgOperatorClass *operatorClass = new pgOperatorClass(
                            collection->GetSchema(), operatorClasses->GetVal(wxT("opcname")));

                operatorClass->iSetOid(operatorClasses->GetOid(wxT("oid")));
                operatorClass->iSetOwner(operatorClasses->GetVal(wxT("opcowner")));
                operatorClass->iSetInType(operatorClasses->GetVal(wxT("intypename")));
                operatorClass->iSetKeyType(operatorClasses->GetVal(wxT("keytypename")));
                operatorClass->iSetOpcDefault(operatorClasses->GetBool(wxT("opcdefault")));

                browser->AppendItem(collection->GetId(), operatorClass->GetIdentifier(), PGICON_OPERATORCLASS, -1, operatorClass);
	    
			    operatorClasses->MoveNext();
            }

		    delete operatorClasses;
        }
    }
}

