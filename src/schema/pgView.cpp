//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
//
// pgView.cpp - View class
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "misc.h"
#include "pgObject.h"
#include "pgView.h"
#include "pgCollection.h"


pgView::pgView(pgSchema *newSchema, const wxString& newName)
: pgRuleObject(newSchema, PG_VIEW, newName)
{
}

pgView::~pgView()
{
}

bool pgView::DropObject(wxFrame *frame, wxTreeCtrl *browser)
{
    return GetDatabase()->ExecuteVoid(wxT("DROP VIEW ") + GetQuotedFullIdentifier());
}

wxString pgView::GetSql(wxTreeCtrl *browser)
{
    if (sql.IsNull())
    {
        sql = wxT("CREATE OR REPLACE VIEW ") + GetQuotedFullIdentifier() + wxT(" AS \n")
            + GetFormattedDefinition()
            + wxT("\n\n") 
            + GetGrant(wxT("arwdRxt"), wxT("Table"))
            + GetCommentSql();
    }
    return sql;
}


void pgView::ShowTreeDetail(wxTreeCtrl *browser, frmMain *form, wxListCtrl *properties, wxListCtrl *statistics, ctlSQLBox *sqlPane)
{
    if (!expandedKids)
    {
        expandedKids = true;

        pgCollection *collection;
        collection = new pgCollection(PG_RULES);
        collection->SetInfo(GetSchema());
        collection->iSetOid(GetOid());
        AppendBrowserItem(browser, collection);
    }
    if (properties)
    {
        CreateListColumns(properties);
        int pos=0;

        InsertListItem(properties, pos++, wxT("Name"), GetName());
        InsertListItem(properties, pos++, wxT("OID"), GetOid());
        InsertListItem(properties, pos++, wxT("Owner"), GetOwner());
        InsertListItem(properties, pos++, wxT("ACL"), GetAcl());
        InsertListItem(properties, pos++, wxT("Definition"), GetDefinition().Left(250));
        InsertListItem(properties, pos++, wxT("Comment"), GetComment());
    }
}



pgObject *pgView::Refresh(wxTreeCtrl *browser, const wxTreeItemId item)
{
    pgObject *view=0;
    wxTreeItemId parentItem=browser->GetItemParent(item);
    if (parentItem)
    {
        pgObject *obj=(pgObject*)browser->GetItemData(parentItem);
        if (obj->GetType() == PG_VIEWS)
            view = ReadObjects((pgCollection*)obj, 0, wxT("\n   AND c.oid=") + GetOidStr());
    }
    return view;
}



pgObject *pgView::ReadObjects(pgCollection *collection, wxTreeCtrl *browser, const wxString &restriction)
{
    pgView *view=0;

    pgSet *views= collection->GetDatabase()->ExecuteSet(wxT(
        "SELECT c.oid, c.relname, pg_get_userbyid(c.relowner) AS viewowner, c.relacl, description, ")
                + collection->GetDatabase()->GetViewdefFunction() + wxT("(c.oid) AS definition\n"
        "  FROM pg_class c\n"
        "  LEFT OUTER JOIN pg_description des ON des.objoid=c.oid\n"
        " WHERE ((c.relhasrules AND (EXISTS (\n"
        "           SELECT r.rulename FROM pg_rewrite r\n"
        "            WHERE ((r.ev_class = c.oid)\n"
        "              AND (bpchar(r.ev_type) = '1'::bpchar)) ))) OR (c.relkind = 'v'::char))\n"
        "   AND relnamespace = ") + collection->GetSchema()->GetOidStr() + wxT("\n")
        + restriction
        + wxT(" ORDER BY relname"));

    if (views)
    {
        while (!views->Eof())
        {
            view = new pgView(collection->GetSchema(), views->GetVal(wxT("relname")));

            view->iSetOid(views->GetOid(wxT("oid")));
            view->iSetOwner(views->GetVal(wxT("Viewowner")));
            view->iSetComment(views->GetVal(wxT("description")));
            view->iSetAcl(views->GetVal(wxT("relacl")));
            view->iSetDefinition(views->GetVal(wxT("definition")));

            if (browser)
            {
                collection->AppendBrowserItem(browser, view);
    			views->MoveNext();
            }
            else
                break;
        }

		delete views;
    }
    return view;
}
