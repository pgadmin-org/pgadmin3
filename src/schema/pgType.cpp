//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
//
// pgType.cpp - Type class
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "misc.h"
#include "pgObject.h"
#include "pgType.h"
#include "pgCollection.h"


pgType::pgType(pgSchema *newSchema, const wxString& newName)
: pgSchemaObject(newSchema, PG_TYPE, newName)
{
}

pgType::~pgType()
{
}

bool pgType::DropObject(wxFrame *frame, wxTreeCtrl *browser)
{
    return GetDatabase()->ExecuteVoid(wxT("DROP TYPE ") + GetQuotedFullIdentifier());
}

wxString pgType::GetSql(wxTreeCtrl *browser)
{
    if (sql.IsNull())
    {
        sql = wxT("CREATE TYPE ") + GetQuotedFullIdentifier() 
            + wxT("(INPUT=") + qtIdent(GetInputFunction()) 
            + wxT(", OUTPUT=") + qtIdent(GetOutputFunction());
        AppendIfFilled(sql, wxT(", DEFAULT="), qtString(GetDefault()));
        if (!GetElement().IsNull())
        {
            sql += wxT(",\n       ELEMENT=") + GetElement()
                + wxT(", DELIMITER='") + GetDelimiter() + wxT("'");
        }
        sql +=wxT(",\n       INTERNALLENGTH=") + NumToStr(GetInternalLength())
            + wxT(", ALIGNMENT=" + GetAlignment()
            + wxT(", STORAGE=") + GetStorage())
            + wxT(");\n")
            + GetCommentSql();
    }

    return sql;
}



void pgType::ShowTreeDetail(wxTreeCtrl *browser, frmMain *form, wxListCtrl *properties, wxListCtrl *statistics, ctlSQLBox *sqlPane)
{

    if (properties)
    {
        wxLogInfo(wxT("Displaying properties for type %s"), GetIdentifier().c_str());

        CreateListColumns(properties);
        int pos=0;

        InsertListItem(properties, pos++, wxT("Name"), GetName());
        InsertListItem(properties, pos++, wxT("OID"), GetOid());
        InsertListItem(properties, pos++, wxT("Owner"), GetOwner());
        InsertListItem(properties, pos++, wxT("Alignment"), GetAlignment());
        InsertListItem(properties, pos++, wxT("Internal Length"), GetInternalLength());
        InsertListItem(properties, pos++, wxT("Default"), GetDefault());
        InsertListItem(properties, pos++, wxT("Passed by Value?"), BoolToYesNo(GetPassedByValue()));
        InsertListItem(properties, pos++, wxT("Delimiter"), GetDelimiter());
    //    InsertListItem(properties, pos++, wxT("Element"), GetElement());
        InsertListItem(properties, pos++, wxT("Input Function"), GetInputFunction());
        InsertListItem(properties, pos++, wxT("Output Function"), GetOutputFunction());
        InsertListItem(properties, pos++, wxT("Storage"), GetStorage());
        InsertListItem(properties, pos++, wxT("System Type?"), GetSystemObject());
        InsertListItem(properties, pos++, wxT("Comment"), GetComment());
    }
}



pgObject *pgType::Refresh(wxTreeCtrl *browser, const wxTreeItemId item)
{
    pgObject *type=0;
    wxTreeItemId parentItem=browser->GetItemParent(item);
    if (parentItem)
    {
        pgObject *obj=(pgObject*)browser->GetItemData(parentItem);
        if (obj->GetType() == PG_TYPES)
            type = ReadObjects((pgCollection*)obj, 0, wxT("\n   AND t.oid=") + GetOidStr());
    }
    return type;
}



pgObject *pgType::ReadObjects(pgCollection *collection, wxTreeCtrl *browser, const wxString &restriction)
{
    pgType *type=0;
    pgSet *types= collection->GetDatabase()->ExecuteSet(wxT(
        "SELECT t.oid, t.*, pg_get_userbyid(t.typowner) as typeowner, e.typname as element, description\n"
        "  FROM pg_type t\n"
        "  LEFT OUTER JOIN pg_type e ON e.oid=t.typelem\n"
        "  LEFT OUTER JOIN pg_description des ON des.objoid=t.oid\n"
        " WHERE t.typtype != 'd' AND t.typtype != 'c' AND t.typname NOT LIKE '\\\\_%%' AND t.typnamespace = ") + collection->GetSchema()->GetOidStr() + wxT("\n"
        " ORDER BY t.typname"));
    if (types)
    {
        while (!types->Eof())
        {
            type = new pgType(collection->GetSchema(), types->GetVal(wxT("typname")));

            type->iSetOid(types->GetOid(wxT("oid")));
            type->iSetOwner(types->GetVal(wxT("typeowner")));
            type->iSetComment(types->GetVal(wxT("description")));
            type->iSetPassedByValue(types->GetBool(wxT("typbyval")));
            type->iSetInternalLength(types->GetLong(wxT("typlen")));
            type->iSetDelimiter(types->GetVal(wxT("typdelim")));
            type->iSetElement(types->GetVal(wxT("element")));
            type->iSetInputFunction(types->GetVal(wxT("typinput")));
            type->iSetOutputFunction(types->GetVal(wxT("typoutput")));
            wxString align=types->GetVal(wxT("typalign"));
            type->iSetAlignment( 
                align == wxT("c") ? wxT("char") :
                align == wxT("s") ? wxT("int2") :
                align == wxT("i") ? wxT("int4") :
                align == wxT("d") ? wxT("double") : wxT("unknown"));
            type->iSetDefault(types->GetVal(wxT("typdefault")));
            wxString storage=types->GetVal(wxT("typstorage"));
            type->iSetStorage(
                storage == wxT("p") ? wxT("PLAIN") :
                storage == wxT("e") ? wxT("EXTERNAL") :
                storage == wxT("m") ? wxT("MAIN") :
                storage == wxT("s") ? wxT("EXTENDED") : wxT("unknown"));

            if (browser)
            {
                browser->AppendItem(collection->GetId(), type->GetIdentifier(), PGICON_TYPE, -1, type);
    			types->MoveNext();
            }
            else
                break;
        }

		delete types;
    }
    return type;
}



void pgType::ShowTreeCollection(pgCollection *collection, frmMain *form, wxTreeCtrl *browser, wxListCtrl *properties, wxListCtrl *statistics, ctlSQLBox *sqlPane)
{
    if (browser->GetChildrenCount(collection->GetId(), FALSE) == 0)
    {
        // Log
        wxLogInfo(wxT("Adding Types to schema %s"), collection->GetSchema()->GetIdentifier().c_str());

        // Get the Types
        ReadObjects(collection, browser);
    }
}
