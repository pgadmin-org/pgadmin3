//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002 - 2003, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// pgConversion.cpp - Conversion class
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "misc.h"
#include "pgObject.h"
#include "pgConversion.h"
#include "pgCollection.h"


pgConversion::pgConversion(pgSchema *newSchema, const wxString& newName)
: pgSchemaObject(newSchema, PG_CONVERSION, newName)
{
}

pgConversion::~pgConversion()
{
}

bool pgConversion::DropObject(wxFrame *frame, wxTreeCtrl *browser)
{
    return GetDatabase()->ExecuteVoid(wxT("DROP CONVERSION ") + GetQuotedFullIdentifier());
}

wxString pgConversion::GetSql(wxTreeCtrl *browser)
{
    if (sql.IsNull())
    {
        sql = wxT("-- Conversion: ") + GetQuotedFullIdentifier() + wxT("\n")
            + wxT("CREATE ");
        if (GetDefaultConversion())
            sql += wxT("DEFAULT ");
        sql += wxT("CONVERSION ") + qtIdent(GetName())
            + wxT("\n  FOR '") + GetForEncoding() + wxT("'")
            + wxT("\n  TO '") + GetToEncoding() + wxT("'")
            + wxT("\n  FROM ") + qtIdent(GetProcNamespace()) + wxT(".") + qtIdent(GetProc()) + wxT(";\n");
    }

    return sql;
}

void pgConversion::ShowTreeDetail(wxTreeCtrl *browser, frmMain *form, wxListCtrl *properties, wxListCtrl *statistics, ctlSQLBox *sqlPane)
{
    if (properties)
    {
        CreateListColumns(properties);
        int pos=0;

        InsertListItem(properties, pos++, wxT("Name"), GetName());
        InsertListItem(properties, pos++, wxT("OID"), GetOid());
        InsertListItem(properties, pos++, wxT("Owner"), GetOwner());
        InsertListItem(properties, pos++, wxT("From"), GetForEncoding());
        InsertListItem(properties, pos++, wxT("To"), GetToEncoding());
        InsertListItem(properties, pos++, wxT("Function"), GetProcNamespace() + wxT(".") + GetProc());
        InsertListItem(properties, pos++, wxT("Default?"), GetDefaultConversion());
        InsertListItem(properties, pos++, wxT("System Conversion?"), GetSystemObject());

    }
}




pgObject *pgConversion::Refresh(wxTreeCtrl *browser, const wxTreeItemId item)
{
    pgObject *conversion=0;
    wxTreeItemId parentItem=browser->GetItemParent(item);
    if (parentItem)
    {
        pgObject *obj=(pgObject*)browser->GetItemData(parentItem);
        if (obj->GetType() == PG_CONVERSIONS)
            conversion = ReadObjects((pgCollection*)obj, 0, wxT("\n   AND co.oid=") + GetOidStr());
    }
    return conversion;
}



pgObject *pgConversion::ReadObjects(pgCollection *collection, wxTreeCtrl *browser, const wxString &restriction)
{
    pgConversion *conversion=0;

        pgSet *conversions= collection->GetDatabase()->ExecuteSet(wxT(
        "SELECT co.oid, co.*, pg_encoding_to_char(conforencoding) as forencoding, pg_encoding_to_char(contoencoding) as toencoding, proname, nspname\n"
        "  FROM pg_conversion co\n"
        "  JOIN pg_proc pr ON pr.oid=conproc\n"
        "  JOIN pg_namespace na ON na.oid=pr.pronamespace\n"
        " WHERE connamespace = ") + collection->GetSchema()->GetOidStr() 
        + restriction + wxT("\n"
        " ORDER BY conname"));

    if (conversions)
    {
        while (!conversions->Eof())
        {
            conversion = new pgConversion(collection->GetSchema(), 
                        conversions->GetVal(wxT("conname")));

            conversion->iSetOid(conversions->GetOid(wxT("oid")));
            conversion->iSetOwner(conversions->GetVal(wxT("conowner")));
            conversion->iSetForEncoding(conversions->GetVal(wxT("forencoding")));
            conversion->iSetToEncoding(conversions->GetVal(wxT("toencoding")));
            conversion->iSetProc(conversions->GetVal(wxT("proname")));
            conversion->iSetProcNamespace(conversions->GetVal(wxT("nspname")));
            conversion->iSetDefaultConversion(conversions->GetBool(wxT("condefault")));

            if (browser)
            {
                collection->AppendBrowserItem(browser, conversion);
			    conversions->MoveNext();
            }
            else
                break;
        }

		delete conversions;
    }
    return conversion;
}
