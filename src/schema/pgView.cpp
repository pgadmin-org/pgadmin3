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
: pgSchemaObject(newSchema, PG_VIEW, newName)
{
}

pgView::~pgView()
{
}


wxString pgView::GetSql(wxTreeCtrl *browser)
{
    if (sql.IsNull())
    {
        wxString str=GetDefinition();
        sql = wxT("CREATE VIEW ") + GetQuotedFullIdentifier() + wxT(" AS \n");

        sql += str; // Some line splitting here: SELECT FROM WHERE ORDER GROUP HAVING LIMIT CASE ELSE END
        sql += wxT(";\n\n") 
            + GetGrant()
            + GetCommentSql();
    }
    return sql;
}

void pgView::ShowTreeDetail(wxTreeCtrl *browser, frmMain *form, wxListCtrl *properties, wxListCtrl *statistics, ctlSQLBox *sqlPane)
{
    SetButtons(form);

    if (!expandedKids)
    {
        expandedKids = true;
        // append type here
    }
    properties->ClearAll();
    properties->InsertColumn(0, wxT("Property"), wxLIST_FORMAT_LEFT, 150);
    properties->InsertColumn(1, wxT("Value"), wxLIST_FORMAT_LEFT, 200);
  

    int pos=0;

    InsertListItem(properties, pos++, wxT("Name"), GetName());
    InsertListItem(properties, pos++, wxT("OID"), NumToStr(GetOid()));
    InsertListItem(properties, pos++, wxT("Owner"), GetOwner());
    InsertListItem(properties, pos++, wxT("ACL"), GetAcl());
    InsertListItem(properties, pos++, wxT("Definition"), GetDefinition());
    InsertListItem(properties, pos++, wxT("Comment"), GetComment());
}



void pgView::ShowTreeCollection(pgCollection *collection, frmMain *form, wxTreeCtrl *browser, wxListCtrl *properties, wxListCtrl *statistics, ctlSQLBox *sqlPane)
{
    wxString msg;
    pgView *view;

    if (browser->GetChildrenCount(collection->GetId(), FALSE) == 0)
    {
        // Log
        msg.Printf(wxT("Adding Views to schema %s"), collection->GetSchema()->GetIdentifier().c_str());
        wxLogInfo(msg);

        // Add View node
//        pgObject *addViewObj = new pgObject(PG_ADD_VIEW, wxString("Add View"));
//        browser->AppendItem(collection->GetId(), wxT("Add View..."), 4, -1, addViewObj);

        // Get the Views
        pgSet *views= collection->GetDatabase()->ExecuteSet(wxT(
            "SELECT c.oid, c.relname, pg_get_userbyid(c.relowner) AS viewowner, c.relacl, pg_get_viewdef(c.oid) AS definition\n"
            "  FROM pg_class c\n"
            " WHERE ((c.relhasrules AND (EXISTS (\n"
            "           SELECT r.rulename FROM pg_rewrite r\n"
            "            WHERE ((r.ev_class = c.oid)\n"
            "              AND (bpchar(r.ev_type) = '1'::bpchar)) ))) OR (c.relkind = 'v'::char))\n"
            "   AND relnamespace = ") + collection->GetSchema()->GetOidStr() + wxT("\n"
            " ORDER BY relname"));

        if (views)
        {
            while (!views->Eof())
            {
                view = new pgView(collection->GetSchema(), views->GetVal(wxT("relname")));

                view->iSetOid(StrToDouble(views->GetVal(wxT("oid"))));
                view->iSetOwner(views->GetVal(wxT("Viewowner")));
                view->iSetAcl(views->GetVal(wxT("relacl")));
                view->iSetDefinition(views->GetVal(wxT("definition")));

                browser->AppendItem(collection->GetId(), view->GetIdentifier(), PGICON_VIEW, -1, view);
	    
			    views->MoveNext();
            }

		    delete views;
        }
    }
}