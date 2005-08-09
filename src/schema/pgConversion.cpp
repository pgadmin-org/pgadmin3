//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2005, The pgAdmin Development Team
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
#include "pgSchema.h"


pgConversion::pgConversion(pgSchema *newSchema, const wxString& newName)
: pgSchemaObject(newSchema, PG_CONVERSION, newName)
{
}

pgConversion::~pgConversion()
{
}

bool pgConversion::DropObject(wxFrame *frame, wxTreeCtrl *browser, bool cascaded)
{
    wxString sql=wxT("DROP CONVERSION ") + GetQuotedFullIdentifier();
    if (cascaded)
        sql += wxT(" CASCADE");
    return GetDatabase()->ExecuteVoid(sql);
}

wxString pgConversion::GetSql(wxTreeCtrl *browser)
{
    if (sql.IsNull())
    {
        sql = wxT("-- Conversion: ") + GetQuotedFullIdentifier() + wxT("\n\n")
            + wxT("-- DROP CONVERSION ") + GetQuotedFullIdentifier() + wxT(";")
            + wxT("\n\nCREATE ");
        if (GetDefaultConversion())
            sql += wxT("DEFAULT ");
        sql += wxT("CONVERSION ") + qtIdent(GetName())
            + wxT("\n  FOR '") + GetForEncoding() + wxT("'")
            + wxT("\n  TO '") + GetToEncoding() + wxT("'")
            + wxT("\n  FROM ") + GetDatabase()->GetQuotedSchemaPrefix(GetProcNamespace()) 
                + qtIdent(GetProc()) + wxT(";\n")
            + GetOwnerSql(8, 0);
    }

    return sql;
}

void pgConversion::ShowTreeDetail(wxTreeCtrl *browser, frmMain *form, ctlListView *properties, ctlSQLBox *sqlPane)
{
    if (properties)
    {
        CreateListColumns(properties);

        properties->AppendItem(_("Name"), GetName());
        properties->AppendItem(_("OID"), GetOid());
        properties->AppendItem(_("Owner"), GetOwner());
        properties->AppendItem(_("From"), GetForEncoding());
        properties->AppendItem(_("To"), GetToEncoding());
        properties->AppendItem(_("Function"), GetSchemaPrefix(GetProcNamespace()) + GetProc());
        properties->AppendItem(_("Default?"), GetDefaultConversion());
        properties->AppendItem(_("System conversion?"), GetSystemObject());
        if (GetConnection()->BackendMinimumVersion(7, 5))
            properties->AppendItem(_("Comment"), GetComment());
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

        pgSet *conversions= collection->GetDatabase()->ExecuteSet(
            wxT("SELECT co.oid, co.*, pg_encoding_to_char(conforencoding) as forencoding, pg_get_userbyid(conowner) as owner,")
            wxT("pg_encoding_to_char(contoencoding) as toencoding, proname, nspname, description\n")
            wxT("  FROM pg_conversion co\n")
            wxT("  JOIN pg_proc pr ON pr.oid=conproc\n")
            wxT("  JOIN pg_namespace na ON na.oid=pr.pronamespace\n")
            wxT("  LEFT OUTER JOIN pg_description des ON des.objoid=co.oid AND des.objsubid=0\n")
            wxT(" WHERE connamespace = ") + collection->GetSchema()->GetOidStr()
            + restriction + wxT("\n")
            wxT(" ORDER BY conname"));

    if (conversions)
    {
        while (!conversions->Eof())
        {
            conversion = new pgConversion(collection->GetSchema(), 
                        conversions->GetVal(wxT("conname")));

            conversion->iSetOid(conversions->GetOid(wxT("oid")));
            conversion->iSetOwner(conversions->GetVal(wxT("owner")));
            conversion->iSetComment(conversions->GetVal(wxT("description")));
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
