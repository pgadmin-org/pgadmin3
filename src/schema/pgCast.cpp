//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
//
// pgCast.cpp - Cast class
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "misc.h"
#include "pgObject.h"
#include "pgCast.h"
#include "pgCollection.h"
#include "frmMain.h"

pgCast::pgCast(const wxString& newName)
: pgObject(PG_CAST, newName)
{
}

pgCast::~pgCast()
{
}




wxString pgCast::GetSql(wxTreeCtrl *browser)
{
    if (sql.IsNull())
    {
        sql = wxT("CREATE CAST (") + GetSourceType()
            + wxT(" AS ") + GetTargetType();
        if (GetCastFunction().IsNull())
            sql += wxT(")\n    WITHOUT ");
        else
            sql += wxT(")\n    WITH ") + qtIdent(GetCastNamespace()) + wxT(".") + qtIdent(GetCastFunction());
        sql += wxT(" AS ") + GetCastContext()
            + wxT(";\n");
    }

    return sql;
}

void pgCast::ShowTreeDetail(wxTreeCtrl *browser, frmMain *form, wxListCtrl *properties, wxListCtrl *statistics, ctlSQLBox *sqlPane)
{
    if (form)
        form->SetButtons(true, true, true, true, false, false, false);

    if (properties)
    {
        properties->ClearAll();
        properties->InsertColumn(0, wxT("Property"), wxLIST_FORMAT_LEFT, 150);
        properties->InsertColumn(1, wxT("Value"), wxLIST_FORMAT_LEFT, 200);

        int pos=0;

        InsertListItem(properties, pos++, wxT("Name"), GetName());
        InsertListItem(properties, pos++, wxT("OID"), GetOid());
        InsertListItem(properties, pos++, wxT("Source Type"), GetSourceType());
        InsertListItem(properties, pos++, wxT("Target Type"), GetTargetType());
        InsertListItem(properties, pos++, wxT("Function"), GetCastFunction() + wxT("(") + GetSourceType() + wxT(")"));
        InsertListItem(properties, pos++, wxT("Context"), GetCastContext());
        InsertListItem(properties, pos++, wxT("Comment"), GetComment());
    }
}



void pgCast::ShowTreeCollection(pgCollection *collection, frmMain *form, wxTreeCtrl *browser, wxListCtrl *properties, wxListCtrl *statistics, ctlSQLBox *sqlPane)
{
    if (browser->GetChildrenCount(collection->GetId(), FALSE) == 0)
    {
        // Log
        wxLogInfo(wxT("Adding Casts to database ")+ collection->GetDatabase()->GetIdentifier());

        // Get the Casts
        pgSet *casts= collection->GetDatabase()->ExecuteSet(wxT(
            "SELECT ca.oid, ca.*, st.typname AS srctyp, tt.typname AS trgtyp, proname, nspname\n"
            "  FROM pg_cast ca\n"
            "  JOIN pg_type st ON st.oid=castsource\n"
            "  JOIN pg_type tt ON tt.oid=casttarget\n"
            "  JOIN pg_proc pr ON pr.oid=castfunc\n"
            "  JOIN pg_namespace na ON na.oid=pr.pronamespace\n"
            " ORDER BY st.typname, tt.typname"));

        if (casts)
        {
            while (!casts->Eof())
            {
                wxString name=casts->GetVal(wxT("srctyp"))+wxT("->")+casts->GetVal(wxT("trgtyp"));
                pgCast *cast = new pgCast(name);

                cast->iSetOid(casts->GetOid(wxT("oid")));
                cast->iSetSourceType(casts->GetVal(wxT("srctyp")));
                cast->iSetTargetType(casts->GetVal(wxT("trgtyp")));
                cast->iSetCastFunction(casts->GetVal(wxT("proname")));
                cast->iSetCastNamespace(casts->GetVal(wxT("nspname")));
                wxString ct=casts->GetVal(wxT("castcontext"));
                cast->iSetCastContext(
                    ct == wxT("i") ? wxT("IMMEDIATE") :
                    ct == wxT("a") ? wxT("ASSIGNMENT") : wxT("unknown"));

                browser->AppendItem(collection->GetId(), cast->GetIdentifier(), PGICON_CAST, -1, cast);
	    
			    casts->MoveNext();
            }

		    delete casts;
        }
    }
}

