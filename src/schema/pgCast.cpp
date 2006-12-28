//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2006, The pgAdmin Development Team
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
#include "pgCast.h"

pgCast::pgCast(const wxString& newName)
: pgDatabaseObject(castFactory, newName)
{
}

pgCast::~pgCast()
{
}

bool pgCast::DropObject(wxFrame *frame, ctlTree *browser, bool cascaded)
{
    wxString sql=wxT("DROP CAST (") + GetSourceType() + wxT(" AS ") + GetTargetType() + wxT(")");
    if (cascaded)
        sql += wxT(" CASCADE");
    return GetDatabase()->ExecuteVoid(sql);
}

wxString pgCast::GetSql(ctlTree *browser)
{
    if (sql.IsNull())
    {
        sql = wxT("-- Cast: ") + GetQuotedFullIdentifier() + wxT("\n\n")
              wxT("-- DROP CAST (") + GetQuotedSchemaPrefix(GetSourceNamespace()) + GetSourceType() +
                              wxT(" AS ") + GetQuotedSchemaPrefix(GetTargetNamespace()) + GetTargetType() + wxT(")") 
              wxT("\n\nCREATE CAST (") + GetQuotedSchemaPrefix(GetSourceNamespace()) + GetSourceType() +
              wxT(" AS ") + GetQuotedSchemaPrefix(GetTargetNamespace()) + GetTargetType();
        if (GetCastFunction().IsNull())
            sql += wxT(")\n  WITHOUT FUNCTION");
        else
            sql += wxT(")\n  WITH FUNCTION ") + GetQuotedSchemaPrefix(GetCastNamespace()) + qtIdent(GetCastFunction()) + wxT("(") + GetSourceType() + wxT(")");
        if (GetCastContext() != wxT("EXPLICIT"))
          sql += wxT("\n  AS ") + GetCastContext();
        sql += wxT(";\n");
    }

    return sql;
}

void pgCast::ShowTreeDetail(ctlTree *browser, frmMain *form, ctlListView *properties, ctlSQLBox *sqlPane)
{
    if (properties)
    {
        CreateListColumns(properties);

        properties->AppendItem(_("Name"), GetName());
        properties->AppendItem(_("OID"), GetOid());
        properties->AppendItem(_("Source type"), GetSchemaPrefix(GetSourceNamespace()) + GetSourceType());
        properties->AppendItem(_("Target type"), GetSchemaPrefix(GetTargetNamespace()) + GetTargetType());
        if (GetCastFunction().IsNull())
            properties->AppendItem(_("Function"), _("(binary compatible)"));
        else
        properties->AppendItem(_("Function"), GetCastFunction() + wxT("(") + GetSourceType() + wxT(")"));
        properties->AppendItem(_("Context"), GetCastContext());
        properties->AppendItem(_("System cast?"), GetSystemObject());
        if (GetConnection()->BackendMinimumVersion(7, 5))
            properties->AppendItem(_("Comment"), firstLineOnly(GetComment()));
    }
}



pgObject *pgCast::Refresh(ctlTree *browser, const wxTreeItemId item)
{
    pgObject *cast=0;

    pgCollection *coll=browser->GetParentCollection(item);
    if (coll)
        cast = castFactory.CreateObjects(coll, 0, wxT(" WHERE ca.oid=") + GetOidStr());

    return cast;
}



pgObject *pgCastFactory::CreateObjects(pgCollection *collection, ctlTree *browser, const wxString &restriction)
{
    pgCast *cast=0;
    wxString systemRestriction;
    if (!settings->GetShowSystemObjects() && restriction.IsEmpty())
        systemRestriction = wxT(" WHERE ca.oid > ") + NumToStr(collection->GetConnection()->GetLastSystemOID()) + wxT("\n");

    pgSet *casts= collection->GetDatabase()->ExecuteSet(
        wxT("SELECT ca.oid, ca.*, format_type(st.oid,NULL) AS srctyp, format_type(tt.oid,tt.typtypmod) AS trgtyp,")
        wxT(      " ns.nspname AS srcnspname, nt.nspname AS trgnspname,\n")
        wxT(      " proname, np.nspname AS pronspname, description\n")
        wxT("  FROM pg_cast ca\n")
        wxT("  JOIN pg_type st ON st.oid=castsource\n")
        wxT("  JOIN pg_namespace ns ON ns.oid=st.typnamespace\n")
        wxT("  JOIN pg_type tt ON tt.oid=casttarget\n")
        wxT("  JOIN pg_namespace nt ON nt.oid=tt.typnamespace\n")
        wxT("  LEFT JOIN pg_proc pr ON pr.oid=castfunc\n")
        wxT("  LEFT JOIN pg_namespace np ON np.oid=pr.pronamespace\n")
        wxT("  LEFT OUTER JOIN pg_description des ON des.objoid=ca.oid AND des.objsubid=0\n")
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
            cast->iSetSourceNamespace(casts->GetVal(wxT("srcnspname")));
            cast->iSetSourceTypeOid(casts->GetOid(wxT("castsource")));
            cast->iSetTargetType(casts->GetVal(wxT("trgtyp")));
            cast->iSetTargetNamespace(casts->GetVal(wxT("trgnspname")));
            cast->iSetTargetTypeOid(casts->GetOid(wxT("casttarget")));
            cast->iSetCastFunction(casts->GetVal(wxT("proname")));
            cast->iSetCastNamespace(casts->GetVal(wxT("pronspname")));
            cast->iSetComment(casts->GetVal(wxT("description")));
            wxString ct=casts->GetVal(wxT("castcontext"));
            cast->iSetCastContext(
                ct == wxT("i") ? wxT("IMPLICIT") :
                ct == wxT("a") ? wxT("ASSIGNMENT") : wxT("EXPLICIT"));

            if (settings->GetShowSystemObjects() || 
                (cast->GetOid() > collection->GetServer()->GetLastSystemOID()))
            {
            if (browser)
            {
                browser->AppendObject(collection, cast);
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


/////////////////////////////

#include "images/cast.xpm"
#include "images/cast-sm.xpm"
#include "images/casts.xpm"

pgCastFactory::pgCastFactory() 
: pgDatabaseObjFactory(__("Cast"), __("New Cast..."), __("Create a new Cast."), cast_xpm, cast_sm_xpm)
{
}


pgCastFactory castFactory;
static pgaCollectionFactory cf(&castFactory, __("Casts"), casts_xpm);
