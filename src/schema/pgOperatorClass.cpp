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
    if (properties)
    {
        CreateListColumns(properties);
        int pos=0;

        InsertListItem(properties, pos++, wxT("Name"), GetName());
        InsertListItem(properties, pos++, wxT("OID"), GetOid());
    }
}



pgObject *pgOperatorClass::Refresh(wxTreeCtrl *browser, const wxTreeItemId item)
{
    pgObject *operatorClass=0;
    wxTreeItemId parentItem=browser->GetItemParent(item);
    if (parentItem)
    {
        pgObject *obj=(pgObject*)browser->GetItemData(parentItem);
        if (obj->GetType() == PG_OPERATORCLASSES)
            operatorClass = ReadObjects((pgCollection*)obj, 0, wxT("\n   AND op.oid=") + GetOidStr());
    }
    return operatorClass;
}



pgObject *pgOperatorClass::ReadObjects(pgCollection *collection, wxTreeCtrl *browser, const wxString &restriction)
{
    pgOperatorClass *operatorClass=0;

    pgSet *operatorClasses= collection->GetDatabase()->ExecuteSet(wxT(
        "SELECT op.*, it.typname as intypename, dt.typname as keytypename\n"
        "  FROM pg_opclass op\n"
        "  JOIN pg_type it ON it.oid=opcintype\n"
        "  JOIN pg_type dt ON dt.oid=opckeytype\n"
        " WHERE opcnamespace = ") + collection->GetSchema()->GetOidStr() 
        + restriction + wxT("\n"
        " ORDER BY opcname"));

    if (operatorClasses)
    {
        while (!operatorClasses->Eof())
        {
            operatorClass = new pgOperatorClass(
                        collection->GetSchema(), operatorClasses->GetVal(wxT("opcname")));

            operatorClass->iSetOid(operatorClasses->GetOid(wxT("oid")));
            operatorClass->iSetOwner(operatorClasses->GetVal(wxT("opcowner")));
            operatorClass->iSetInType(operatorClasses->GetVal(wxT("intypename")));
            operatorClass->iSetKeyType(operatorClasses->GetVal(wxT("keytypename")));
            operatorClass->iSetOpcDefault(operatorClasses->GetBool(wxT("opcdefault")));

            if (browser)
            {
                browser->AppendItem(collection->GetId(), operatorClass->GetIdentifier(), PGICON_OPERATORCLASS, -1, operatorClass);
				operatorClasses->MoveNext();
            }
            else
                break;
        }

		delete operatorClasses;
    }
    return operatorClass;
}



void pgOperatorClass::ShowTreeCollection(pgCollection *collection, frmMain *form, wxTreeCtrl *browser, wxListCtrl *properties, wxListCtrl *statistics, ctlSQLBox *sqlPane)
{
    if (browser->GetChildrenCount(collection->GetId(), FALSE) == 0)
    {
        // Log
        wxLogInfo(wxT("Adding OperatorClasss to schema ") + collection->GetSchema()->GetIdentifier());

        // Get the OperatorClasss
        ReadObjects(collection, browser);
    }
}

