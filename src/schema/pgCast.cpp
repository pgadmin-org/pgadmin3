//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2003, The pgAdmin Development Team
// This software is released under the Artistic Licence
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
        sql = wxT("-- Cast: ") + GetQuotedFullIdentifier() + wxT("\n\n")
            + wxT("-- DROP CAST (") + GetSourceType() + wxT(" AS ") + GetTargetType() + wxT(")")
            + wxT("\n\nCREATE CAST (") + GetSourceType()
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

void pgCast::ShowTreeDetail(wxTreeCtrl *browser, frmMain *form, ctlListView *properties, ctlListView *statistics, ctlSQLBox *sqlPane)
{
    if (properties)
    {
        CreateListColumns(properties);

        properties->AppendItem(_("Name"), GetName());
        properties->AppendItem(_("OID"), GetOid());
        properties->AppendItem(_("Source type"), GetSourceType());
        properties->AppendItem(_("Target type"), GetTargetType());
        if (GetCastFunction().IsNull())
            properties->AppendItem(_("Function"), _("(binary compatible)"));
        else
        properties->AppendItem(_("Function"), GetCastFunction() + wxT("(") + GetSourceType() + wxT(")"));
        properties->AppendItem(_("Context"), GetCastContext());
        properties->AppendItem(_("System cast?"), GetSystemObject());
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
    wxString systemRestriction;
    if (!settings->GetShowSystemObjects() && restriction.IsEmpty())
        systemRestriction = wxT(" WHERE ca.oid > ") + NumToStr(collection->GetConnection()->GetLastSystemOID()) + wxT("\n");

    pgSet *casts= collection->GetDatabase()->ExecuteSet(
        wxT("SELECT ca.oid, ca.*, st.typname AS srctyp, tt.typname AS trgtyp, proname, nspname\n")
        wxT("  FROM pg_cast ca\n")
        wxT("  JOIN pg_type st ON st.oid=castsource\n")
        wxT("  JOIN pg_type tt ON tt.oid=casttarget\n")
        wxT("  LEFT JOIN pg_proc pr ON pr.oid=castfunc\n")
        wxT("  LEFT JOIN pg_namespace na ON na.oid=pr.pronamespace\n")
        + restriction + systemRestriction +
        wxT(" ORDER BY st.typname, tt.typname"));

    if (casts)
    {
        while (!casts->Eof())
        {
            wxString name=casts->GetVal(wxT("srctyp"))+wxT("->")+casts->GetVal(wxT("trgtyp"));
            cast = new pgCast(name);

            cast->iSetOid(casts->GetOid(wxT("oid")));
            cast->iSetDatabase(collection->GetDatabase());
            cast->iSetSourceType(casts->GetVal(wxT("srctyp")));
            cast->iSetSourceTypeOid(casts->GetOid(wxT("castsource")));
            cast->iSetTargetType(casts->GetVal(wxT("trgtyp")));
            cast->iSetTargetTypeOid(casts->GetOid(wxT("casttarget")));
            cast->iSetCastFunction(casts->GetVal(wxT("proname")));
            cast->iSetCastNamespace(casts->GetVal(wxT("nspname")));
            wxString ct=casts->GetVal(wxT("castcontext"));
            cast->iSetCastContext(
                ct == wxT("i") ? wxT("IMPLICIT") :
                ct == wxT("a") ? wxT("ASSIGNMENT") : wxT("EXPLICIT"));

            if (settings->GetShowSystemObjects() || 
                (cast->GetOid() > collection->GetServer()->GetLastSystemOID()))
            {
            if (browser)
            {
                collection->AppendBrowserItem(browser, cast);
			    casts->MoveNext();
            }
            else
                break;
        }
            else
                break;
        }
		delete casts;
    }
    return cast;
}
