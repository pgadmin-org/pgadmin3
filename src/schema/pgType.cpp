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


wxString pgType::GetSql(wxTreeCtrl *browser)
{
    if (sql.IsNull())
    {
        sql = wxT("CREATE TYPE ") + GetQuotedFullIdentifier() 
            + wxT("(INPUT=") + qtIdent(GetInputFunction()) 
            + wxT(", OUTPUT=") + qtIdent(GetOutputFunction());
        if (!GetDefault().IsNull())
            sql += wxT(", DEFAULT=") + GetDefault();
        if (!GetElement().IsNull())
        {
            sql += wxT(",\n       ELEMENT=") + GetElement()
                + wxT(", DELIMITER=") + GetDelimiter();
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
    wxString msg;
    msg.Printf(wxT("Displaying properties for type %s"), GetIdentifier().c_str());
    wxLogInfo(msg);

    properties->ClearAll();
    properties->InsertColumn(0, wxT("Property"), wxLIST_FORMAT_LEFT, 150);
    properties->InsertColumn(1, wxT("Value"), wxLIST_FORMAT_LEFT, 200);
  

    int pos=0;

    InsertListItem(properties, pos++, wxT("Name"), GetName());
    InsertListItem(properties, pos++, wxT("OID"), NumToStr(GetOid()));
    InsertListItem(properties, pos++, wxT("Owner"), GetOwner());
    InsertListItem(properties, pos++, wxT("Alignment"), GetAlignment());
    InsertListItem(properties, pos++, wxT("Internal Length"), NumToStr(GetInternalLength()));
    InsertListItem(properties, pos++, wxT("Default"), GetDefault());
    InsertListItem(properties, pos++, wxT("Passed by Value?"), BoolToYesNo(GetPassedByValue()));
    InsertListItem(properties, pos++, wxT("Delimiter"), GetDelimiter());
//    InsertListItem(properties, pos++, wxT("Element"), GetElement());
    InsertListItem(properties, pos++, wxT("Input Function"), GetInputFunction());
    InsertListItem(properties, pos++, wxT("Output Function"), GetOutputFunction());
    InsertListItem(properties, pos++, wxT("Storage"), GetStorage());
    InsertListItem(properties, pos++, wxT("System Type?"), BoolToYesNo(GetSystemObject()));
    InsertListItem(properties, pos++, wxT("Comment"), GetComment());
}



void pgType::ShowTreeCollection(pgCollection *collection, frmMain *form, wxTreeCtrl *browser, wxListCtrl *properties, wxListCtrl *statistics, ctlSQLBox *sqlPane)
{
    wxString msg;
    pgType *type;

    if (browser->GetChildrenCount(collection->GetId(), FALSE) == 0)
    {
        // Log
        msg.Printf(wxT("Adding Types to schema %s"), collection->GetSchema()->GetIdentifier().c_str());
        wxLogInfo(msg);

        // Add Type node
//        pgObject *addTypeObj = new pgObject(PG_ADD_TYPE, wxString("Add Type"));
//        browser->AppendItem(collection->GetId(), wxT("Add Type..."), 4, -1, addTypeObj);

        // Get the Types
        pgSet *types= collection->GetDatabase()->ExecuteSet(wxT(
            "SELECT t.oid, t.*, pg_get_userbyid(t.typowner) as typeowner, e.typname as element\n"
            "  FROM pg_type t\n"
            "  LEFT OUTER JOIN pg_type e ON e.oid=t.typelem\n"
            " WHERE t.typtype != 'd' AND t.typtype != 'c' AND t.typname NOT LIKE '\\\\_%%' AND t.typnamespace = ") + collection->GetSchema()->GetOidStr() + wxT("\n"
            " ORDER BY t.typname"));
        if (types)
        {
            while (!types->Eof())
            {
                type = new pgType(collection->GetSchema(), types->GetVal(wxT("typname")));

                type->iSetOid(StrToDouble(types->GetVal(wxT("oid"))));
                type->iSetOwner(types->GetVal(wxT("typeowner")));
                type->iSetPassedByValue(StrToBool(types->GetVal(wxT("typbyval"))));
                type->iSetInternalLength(StrToLong(types->GetVal(wxT("typlen"))));
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

                browser->AppendItem(collection->GetId(), type->GetIdentifier(), PGICON_TYPE, -1, type);
	    
			    types->MoveNext();
            }

		    delete types;
        }
        wxString label;
        label.Printf(wxT("Types (%d)"), browser->GetChildrenCount(collection->GetId(), FALSE));
        browser->SetItemText(collection->GetId(), label);
    }
}
