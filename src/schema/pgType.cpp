//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002 - 2003, The pgAdmin Development Team
// This software is released under the Artistic Licence
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
#include "pgDatatype.h"


pgType::pgType(pgSchema *newSchema, const wxString& newName)
: pgSchemaObject(newSchema, PG_TYPE, newName)
{
}

pgType::~pgType()
{
}

bool pgType::DropObject(wxFrame *frame, wxTreeCtrl *browser)
{
    return GetDatabase()->ExecuteVoid(wxT("DROP TYPE ") + GetQuotedFullIdentifier() + wxT(";"));
}

wxString pgType::GetSql(wxTreeCtrl *browser)
{
    if (sql.IsNull())
    {
        sql = wxT("-- Type: ") + GetQuotedFullIdentifier() + wxT("\n\n")
            + wxT("-- DROP TYPE ") + GetQuotedFullIdentifier() + wxT(";")
            + wxT("\n\nCREATE TYPE ") + GetQuotedFullIdentifier();
        if (GetIsComposite())
        {
            sql += wxT(" AS\n   (");
            sql += GetQuotedTypesList();
        }
        else
        {
            sql += wxT("\n   (INPUT=") + qtIdent(GetInputFunction()) 
                + wxT(", OUTPUT=") + qtIdent(GetOutputFunction());
            AppendIfFilled(sql, wxT(", DEFAULT="), qtString(GetDefault()));
            if (!GetElement().IsNull())
            {
                sql += wxT(",\n       ELEMENT=") + GetElement()
                    + wxT(", DELIMITER='") + GetDelimiter() + wxT("'");
            }
            sql +=wxT(",\n       INTERNALLENGTH=") + NumToStr(GetInternalLength())
                + wxT(", ALIGNMENT=" + GetAlignment()
                + wxT(", STORAGE=") + GetStorage());
        }
        sql += wxT(");\n")
            + GetCommentSql();
    }

    return sql;
}



void pgType::ShowTreeDetail(wxTreeCtrl *browser, frmMain *form, wxListCtrl *properties, wxListCtrl *statistics, ctlSQLBox *sqlPane)
{
    if (!expandedKids)
    {
        expandedKids=true;
        if (isComposite)
        {
            pgSet *set=ExecuteSet(
                wxT("SELECT attname, t.typname, attndims, atttypmod, nspname\n")
                wxT("  FROM pg_attribute att\n")
                wxT("  JOIN pg_type t ON t.oid=atttypid\n")
                wxT("  JOIN pg_namespace nsp ON t.typnamespace=nsp.oid\n")
                wxT("  LEFT OUTER JOIN pg_type b ON t.typelem=b.oid\n")
                wxT(" WHERE att.attrelid=") + NumToStr(relOid) + wxT("\n")
                wxT(" ORDER by attnum"));
            if (set)
            {
                int anzvar=0;
                while (!set->Eof())
                {
                    if (anzvar++)
                    {
                        typesList += wxT(", ");
                        quotedTypesList += wxT(",\n    ");
                    }
                    typesList += set->GetVal(wxT("attname")) + wxT(" ");
                    quotedTypesList += qtIdent(set->GetVal(wxT("attname"))) + wxT(" ");

                    pgDatatype dt(set->GetVal(wxT("typname")), set->GetLong(wxT("attndims")) > 0, set->GetLong(wxT("atttypmod")));
                    wxString nspname=set->GetVal(wxT("nspname"));
                    if (!nspname.IsSameAs(wxT("pg_catalog")))
                    {
                        typesList += nspname + wxT(".");
                        quotedTypesList += qtIdent(nspname) + wxT(".");
                    }

                    typesList +=  dt.FullName();
                    quotedTypesList += dt.QuotedFullName();

                    set->MoveNext();
                }
                delete set;
            }
        }
    }
    if (properties)
    {
        wxLogInfo(wxT("Displaying properties for type %s"), GetIdentifier().c_str());

        CreateListColumns(properties);
        int pos=0;

        InsertListItem(properties, pos++, _("Name"), GetName());
        InsertListItem(properties, pos++, _("OID"), GetOid());
        InsertListItem(properties, pos++, _("Owner"), GetOwner());
        if (isComposite)
        {
            InsertListItem(properties, pos++, _("Members"), GetTypesList());
        }
        else
        {
            InsertListItem(properties, pos++, _("Alignment"), GetAlignment());
            InsertListItem(properties, pos++, _("Internal length"), GetInternalLength());
            InsertListItem(properties, pos++, _("Default"), GetDefault());
            InsertListItem(properties, pos++, _("Passed by Value?"), BoolToYesNo(GetPassedByValue()));
            if (!GetElement().IsEmpty())
            {
                InsertListItem(properties, pos++, _("Element"), GetElement());
                InsertListItem(properties, pos++, _("Delimiter"), GetDelimiter());
            }
            InsertListItem(properties, pos++, _("Input function"), GetInputFunction());
            InsertListItem(properties, pos++, _("Output function"), GetOutputFunction());
            if (GetConnection()->BackendMinimumVersion(7, 4))
            {
            InsertListItem(properties, pos++, _("Receive function"), GetReceiveFunction());
            InsertListItem(properties, pos++, _("Send function"), GetSendFunction());
            }
            InsertListItem(properties, pos++, _("Storage"), GetStorage());
        }
        InsertListItem(properties, pos++, _("System type?"), GetSystemObject());
        InsertListItem(properties, pos++, _("Comment"), GetComment());
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
    wxString systemRestriction;
    if (!settings->GetShowSystemObjects())
        systemRestriction = wxT("   AND ct.oid IS NULL\n");

    pgSet *types= collection->GetDatabase()->ExecuteSet(
        wxT("SELECT t.oid, t.*, pg_get_userbyid(t.typowner) as typeowner, e.typname as element, description, ct.oid AS taboid\n")
        wxT("  FROM pg_type t\n")
        wxT("  LEFT OUTER JOIN pg_type e ON e.oid=t.typelem\n")
        wxT("  LEFT OUTER JOIN pg_class ct ON ct.oid=t.typrelid AND ct.relkind <> 'c'\n")
        wxT("  LEFT OUTER JOIN pg_description des ON des.objoid=t.oid\n")
        wxT(" WHERE t.typtype != 'd' AND t.typname NOT LIKE '\\\\_%%' AND t.typnamespace = ") + collection->GetSchema()->GetOidStr() + wxT("\n")
        + systemRestriction +
        wxT(" ORDER BY t.typname"));
    if (types)
    {
        while (!types->Eof())
        {
            type = new pgType(collection->GetSchema(), types->GetVal(wxT("typname")));

            type->iSetOid(types->GetOid(wxT("oid")));
            type->iSetOwner(types->GetVal(wxT("typeowner")));
            type->iSetComment(types->GetVal(wxT("description")));
            type->iSetPassedByValue(types->GetBool(wxT("typbyval")));
            type->iSetIsComposite(types->GetVal(wxT("typtype")) == wxT("c"));
            type->iSetRelOid(types->GetOid(wxT("typrelid")));
            type->iSetIsRecordType(types->GetOid(wxT("taboid")) != 0);
            type->iSetInternalLength(types->GetLong(wxT("typlen")));
            type->iSetDelimiter(types->GetVal(wxT("typdelim")));
            type->iSetElement(types->GetVal(wxT("element")));
            type->iSetInputFunction(types->GetVal(wxT("typinput")));
            type->iSetOutputFunction(types->GetVal(wxT("typoutput")));
            if (collection->GetConnection()->BackendMinimumVersion(7, 4))
            {
                type->iSetReceiveFunction(types->GetVal(wxT("typreceive")));
                type->iSetSendFunction(types->GetVal(wxT("typsend")));
            }
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
                storage == wxT("x") ? wxT("EXTENDED") : wxT("unknown"));

            if (browser)
            {
                collection->AppendBrowserItem(browser, type);
    			types->MoveNext();
            }
            else
                break;
        }

		delete types;
    }
    return type;
}
