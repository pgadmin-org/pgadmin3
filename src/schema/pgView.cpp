//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2005, The pgAdmin Development Team
// This software is released under the Artistic Licence
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
#include "pgSchema.h"


pgView::pgView(pgSchema *newSchema, const wxString& newName)
: pgRuleObject(newSchema, viewFactory, newName)
{
}

pgView::~pgView()
{
}


wxMenu *pgView::GetNewMenu()
{
    wxMenu *menu=pgObject::GetNewMenu();
    if (schema->GetCreatePrivilege())
        AppendMenu(menu, PG_RULE);
    return menu;
}


bool pgView::DropObject(wxFrame *frame, wxTreeCtrl *browser, bool cascaded)
{
    wxString sql = wxT("DROP VIEW ") + GetQuotedFullIdentifier();
    if (cascaded)
        sql += wxT(" CASCADE");
    return GetDatabase()->ExecuteVoid(sql);
}

wxString pgView::GetSql(wxTreeCtrl *browser)
{
    if (sql.IsNull())
    {
        sql = wxT("-- View: \"") + GetQuotedFullIdentifier() + wxT("\"\n\n")
            + wxT("-- DROP VIEW ") + GetQuotedFullIdentifier() + wxT(";")
            + wxT("\n\nCREATE OR REPLACE VIEW ") + GetQuotedFullIdentifier() + wxT(" AS \n")
            + GetFormattedDefinition()
            + wxT("\n\n") 
            + GetOwnerSql(7, 3, wxT("TABLE ") + GetQuotedFullIdentifier())
            + GetGrant(wxT("arwdRxt"), wxT("TABLE ") + GetQuotedFullIdentifier())
            + GetCommentSql();
    }
    return sql;
}


void pgView::ShowTreeDetail(wxTreeCtrl *browser, frmMain *form, ctlListView *properties, ctlSQLBox *sqlPane)
{
    if (!expandedKids)
    {
        expandedKids = true;

        RemoveDummyChild(browser);

        pgCollection *collection;
        collection = new pgCollection(PG_RULES, GetSchema());
        collection->iSetOid(GetOid());
        AppendBrowserItem(browser, collection);
    }
    if (properties)
    {
        CreateListColumns(properties);
        wxString def=GetDefinition().Left(250);
        def.Replace(wxT("\n"), wxT(" "));

        properties->AppendItem(_("Name"), GetName());
        properties->AppendItem(_("OID"), GetOid());
        properties->AppendItem(_("Owner"), GetOwner());
        properties->AppendItem(_("ACL"), GetAcl());
        properties->AppendItem(_("Definition"), def);
        properties->AppendItem(_("System view?"), GetSystemObject());
        properties->AppendItem(_("Comment"), GetComment());
    }
}



pgObject *pgView::Refresh(wxTreeCtrl *browser, const wxTreeItemId item)
{
    pgObject *view=0;
    wxTreeItemId parentItem=browser->GetItemParent(item);
    if (parentItem)
    {
        pgObject *obj=(pgObject*)browser->GetItemData(parentItem);
        if (obj->IsCollection())
            view = viewFactory.CreateObjects((pgCollection*)obj, 0, wxT("\n   AND c.oid=") + GetOidStr());
    }
    return view;
}


///////////////////////////////////////////////////////


pgObject *pgaViewFactory::CreateObjects(pgCollection *collection, wxTreeCtrl *browser, const wxString &restriction)
{
    pgView *view=0;


    pgSet *views= collection->GetDatabase()->ExecuteSet(
        wxT("SELECT c.oid, c.relname, pg_get_userbyid(c.relowner) AS viewowner, c.relacl, description, ")
               wxT("pg_get_viewdef(c.oid") + collection->GetDatabase()->GetPrettyOption() + wxT(") AS definition\n")
        wxT("  FROM pg_class c\n")
        wxT("  LEFT OUTER JOIN pg_description des ON (des.objoid=c.oid and des.objsubid=0)\n")
        wxT(" WHERE ((c.relhasrules AND (EXISTS (\n")
        wxT("           SELECT r.rulename FROM pg_rewrite r\n")
        wxT("            WHERE ((r.ev_class = c.oid)\n")
        wxT("              AND (bpchar(r.ev_type) = '1'::bpchar)) ))) OR (c.relkind = 'v'::char))\n")
        wxT("   AND relnamespace = ") + collection->GetSchema()->GetOidStr() + wxT("\n")
        + restriction
        + wxT(" ORDER BY relname"));

    if (views)
    {
        while (!views->Eof())
        {
            view = new pgView(collection->GetSchema(), views->GetVal(wxT("relname")));

            view->iSetOid(views->GetOid(wxT("oid")));
            view->iSetOwner(views->GetVal(wxT("viewowner")));
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


#include "images/view.xpm"
#include "images/views.xpm"

pgaViewFactory::pgaViewFactory() 
: pgaFactory(__("View"), __("New View"), __("Create a new View."), view_xpm)
{
    metaType = PGM_VIEW;
}


pgaViewFactory viewFactory;
static pgaCollectionFactory cf(&viewFactory, __("Views"), views_xpm);
