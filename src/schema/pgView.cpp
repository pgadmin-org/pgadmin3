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



typedef struct __tokenaction
{
    char *keyword, *replaceKeyword;
    int actionBefore, actionAfter;
    bool doBreak;
} tokenAction;


tokenAction sqlTokens[] =
{
    { "WHERE",  "  WHERE"   -8, 8, true},
    { "SELECT", " SELECT",   0, 8, true},
    { "FROM",   "   FROM",  -8, 8, true},
    { "ORDER",  "  ORDER",  -8, 8, true},
    { "GROUP",  "  GROUP",  -8, 8, true},
    { "HAVING", " HAVING",  -8, 8, true},
    { "LIMIT",  "  LIMIT",  -8, 8, true},
    { "UNION",  "UNION  ",  -88888, 8, true},
    { "CASE",   "CASE",      0, 4, true},
    { "WHEN",   "WHEN",      0, 0, true},
    { "ELSE",   "ELSE",      0, 0, true},
    { "END",    "END ",     -4, 0, true},
    {0, 0, 0, 0}
};



wxString pgView::GetSql(wxTreeCtrl *browser)
{
    if (sql.IsNull())
    {
        // ok, this code looks weird. It's necessary, because somebody (NOT the running code)
        // will screw up that entry. It's broken in pgAdmin3::OnInit() already.
        // maybe your compiler does better (VC6SP5, but an older c2xx to avoid other bugs)
        sqlTokens[0].replaceKeyword="  WHERE";
        sqlTokens[0].actionBefore = -8;
        sqlTokens[0].actionAfter = 8;
        sqlTokens[0].doBreak = true;

        wxString fc, token;
        queryTokenizer tokenizer(GetDefinition());
        int indent=0;
        int position=0;  // col position. updated, but not used at the moment.

        while (tokenizer.HasMoreTokens())
        {
            token=tokenizer.GetNextToken();

            // token may contain brackets
            int bracketPos=token.Find('(', true);
            if (bracketPos >= 0)
            {
                fc += token.Left(bracketPos+1);
                token = token.Mid(bracketPos+1);
            }

            bracketPos=token.Find(')', true);
            if (bracketPos >= 0)
            {
                fc += token.Left(bracketPos+1);
                token = token.Mid(bracketPos+1);
            }
            // identify token
            tokenAction *tp=sqlTokens;
            while (tp->keyword)
            {
                if (!token.CmpNoCase(tp->keyword))
                    break;
                tp++;
            }
            
            if (tp->keyword)
            {
                // we found a keyword.
                indent += tp->actionBefore;
                if (indent<0)   indent=0;
                if (tp->doBreak)
                {
                    fc += "\n" + wxString(' ', indent);
                    position = indent;
                }
                else
                {
                    fc += wxT(" ");
                    position += 1;
                }
                fc += tp->replaceKeyword;
                position += strlen(tp->replaceKeyword);

                indent += tp->actionAfter;
                if (indent<0)   indent=0;
            }
            else
            {
                fc += wxT(" ") + token;
                position += token.Length()+1;
            }
        }

        sql = wxT("CREATE VIEW ") + GetQuotedFullIdentifier() + wxT(" AS \n")
            + fc
            + wxT(";\n\n") 
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