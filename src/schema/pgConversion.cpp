//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
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




wxString pgConversion::GetSql(wxTreeCtrl *browser)
{
    if (sql.IsNull())
    {
        sql = wxT("CREATE ");
        if (GetDefaultConversion())
            sql += wxT("DEFAULT ");
        sql += wxT("CONVERSION ") + qtIdent(GetName())
            + wxT(" FOR ") + NumToStr(GetForEncoding())
            + wxT(" TO ") + NumToStr(GetToEncoding())
            + wxT(" FROM ") + qtIdent(GetProcNamespace()) + wxT(".") + qtIdent(GetProc());
    }

    return sql;
}

void pgConversion::ShowTreeDetail(wxTreeCtrl *browser, frmMain *form, wxListCtrl *properties, wxListCtrl *statistics, ctlSQLBox *sqlPane)
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
        InsertListItem(properties, pos++, wxT("From"), GetForEncoding());
        InsertListItem(properties, pos++, wxT("From"), GetToEncoding());
        InsertListItem(properties, pos++, wxT("Function"), GetProc());
        InsertListItem(properties, pos, wxT("Default?"), GetDefaultConversion());
    }
}



void pgConversion::ShowTreeCollection(pgCollection *collection, frmMain *form, wxTreeCtrl *browser, wxListCtrl *properties, wxListCtrl *statistics, ctlSQLBox *sqlPane)
{

    if (browser->GetChildrenCount(collection->GetId(), FALSE) == 0)
    {
        // Log
        wxLogInfo(wxT("Adding Conversions to schema %s"), collection->GetSchema()->GetIdentifier());

        // Get the Conversions
        pgSet *conversions= collection->GetDatabase()->ExecuteSet(wxT(
            "SELECT co.oid, co.*, proname, nspname\n"
            "  FROM pg_conversion co\n"
            "  JOIN pg_proc pr ON pr.oid=conproc\n"
            "  JOIN pg_namespace na ON na.oid=pr.pronamespace\n"
            " WHERE connamespace = ") + collection->GetSchema()->GetOidStr() + wxT("\n"
            " ORDER BY conname"));

        if (conversions)
        {
            while (!conversions->Eof())
            {
                pgConversion *conversion = new pgConversion(collection->GetSchema(), 
                            conversions->GetVal(wxT("conname")));

                conversion->iSetOid(conversions->GetOid(wxT("oid")));
                conversion->iSetOwner(conversions->GetVal(wxT("conowner")));
                conversion->iSetForEncoding(conversions->GetLong(wxT("conforencoding")));
                conversion->iSetToEncoding(conversions->GetLong(wxT("contoencoding")));
                conversion->iSetProc(conversions->GetVal(wxT("proname")));
                conversion->iSetProcNamespace(conversions->GetVal(wxT("nspname")));
                conversion->iSetDefaultConversion(conversions->GetBool(wxT("condefault")));

                browser->AppendItem(collection->GetId(), conversion->GetIdentifier(), PGICON_CONVERSION, -1, conversion);
	    
			    conversions->MoveNext();
            }

		    delete conversions;
        }
    }
}

