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

pgCast::pgCast(const wxString& newName)
: pgDatabaseObject(PG_CAST, newName)
{
}

pgCast::~pgCast()
{
}

bool pgCast::DropObject(wxFrame *frame, wxTreeCtrl *browser)
{
    return GetDatabase()->ExecuteVoid(wxT("DROP CAST (") + GetSourceType() + wxT(" AS ") + GetTargetType() + wxT(")"));
}

wxString pgCast::GetSql(wxTreeCtrl *browser)
{
    if (sql.IsNull())
    {
        sql = wxT("-- Cast: ") + GetQuotedFullIdentifier() + wxT("\n")
            + wxT("CREATE CAST (") + GetSourceType()
            + wxT(" AS ") + GetTargetType();
        if (GetCastFunction().IsNull())
            sql += wxT(")\n  WITHOUT FUNCTION");
        else
            sql += wxT(")\n  WITH FUNCTION ") + qtIdent(GetCastNamespace()) + wxT(".") + qtIdent(GetCastFunction()) + wxT("(") + GetSourceType() + wxT(")");
        if (GetCastContext() != wxT("Explicit"))
          sql += wxT("\n  AS ") + GetCastContext();
        sql += wxT(";\n");
    }

    return sql;
}

void pgCast::ShowTreeDetail(wxTreeCtrl *browser, frmMain *form, wxListCtrl *properties, wxListCtrl *statistics, ctlSQLBox *sqlPane)
{
    if (properties)
    {
        CreateListColumns(properties);
        int pos=0;

        InsertListItem(properties, pos++, wxT("Name"), GetName());
        InsertListItem(properties, pos++, wxT("OID"), GetOid());
        InsertListItem(properties, pos++, wxT("Source Type"), GetSourceType());
        InsertListItem(properties, pos++, wxT("Target Type"), GetTargetType());
        InsertListItem(properties, pos++, wxT("Function"), GetCastFunction() + wxT("(") + GetSourceType() + wxT(")"));
        InsertListItem(properties, pos++, wxT("Context"), GetCastContext());
        InsertListItem(properties, pos++, wxT("System Cast?"), GetSystemObject());
    }
}



pgObject *pgCast::Refresh(wxTreeCtrl *browser, const wxTreeItemId item)
{
    pgObject *cast=0;
    wxTreeItemId parentItem=browser->GetItemParent(item);
    if (parentItem)
    {
        pgObject *obj=(pgObject*)browser->GetItemData(parentItem);
        if (obj->GetType() == PG_CASTS)
            cast = ReadObjects((pgCollection*)obj, 0, wxT(" WHERE ca.oid=") + GetOidStr());
    }
    return cast;
}



pgObject *pgCast::ReadObjects(pgCollection *collection, wxTreeCtrl *browser, const wxString &restriction)
{
    pgCast *cast=0;
    pgSet *casts= collection->GetDatabase()->ExecuteSet(wxT(
        "SELECT ca.oid, ca.*, st.typname AS srctyp, tt.typname AS trgtyp, proname, nspname\n"
        "  FROM pg_cast ca\n"
        "  JOIN pg_type st ON st.oid=castsource\n"
        "  JOIN pg_type tt ON tt.oid=casttarget\n"
        "  JOIN pg_proc pr ON pr.oid=castfunc\n"
        "  JOIN pg_namespace na ON na.oid=pr.pronamespace\n"
        + restriction +
        " ORDER BY st.typname, tt.typname"));

    if (casts)
    {
        while (!casts->Eof())
        {
            wxString name=casts->GetVal(wxT("srctyp"))+wxT("->")+casts->GetVal(wxT("trgtyp"));
            cast = new pgCast(name);

            cast->iSetOid(casts->GetOid(wxT("oid")));
            cast->iSetDatabase(collection->GetDatabase());
            cast->iSetSourceType(casts->GetVal(wxT("srctyp")));
            cast->iSetTargetType(casts->GetVal(wxT("trgtyp")));
            cast->iSetCastFunction(casts->GetVal(wxT("proname")));
            cast->iSetCastNamespace(casts->GetVal(wxT("nspname")));
            wxString ct=casts->GetVal(wxT("castcontext"));
            cast->iSetCastContext(
                ct == wxT("i") ? wxT("IMPLICIT") :
                ct == wxT("a") ? wxT("ASSIGNMENT") : wxT("EXPLICIT"));

            if (browser)
            {
                collection->AppendBrowserItem(browser, cast);
			    casts->MoveNext();
            }
            else
                break;
        }

		delete casts;
    }
    return cast;
}
