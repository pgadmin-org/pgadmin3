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



enum tokentype
{
    SQLTK_NORMAL=0,
    SQLTK_JOINMOD,
    SQLTK_JOIN,
    SQLTK_ON,
    SQLTK_UNION

};

typedef struct __tokenaction
{
    char *keyword, *replaceKeyword;
    int actionBefore, actionAfter;
    tokentype special;
    bool doBreak;
} tokenAction;


tokenAction sqlTokens[] =
{
    { "WHERE"},     // initializing fails, so we're doing it in the code
    { "SELECT", " SELECT",   0, 8,      SQLTK_NORMAL,   true},
    { "FROM",   "   FROM",  -8, 8,      SQLTK_NORMAL,   true},
    { "LEFT",   "   LEFT",  -8, 13,     SQLTK_JOINMOD,  true},
    { "RIGHT",  "   RIGHT", -8, 13,     SQLTK_JOINMOD,  true},
    { "NATURAL","   NATURAL", -8, 13,   SQLTK_JOINMOD,  true},
    { "FULL",   "   FULL",  -8, 13,     SQLTK_JOINMOD,  true},
    { "CROSS",  "   CROSS", -8, 13,     SQLTK_JOINMOD,  true},
    { "UNION",  "   UNION", -8, 13,     SQLTK_UNION,    true},
    { "JOIN",   "   JOIN",  -8, 13,     SQLTK_JOIN,     true},
    { "ON",     "ON",        0, -5,     SQLTK_ON,       false},
    { "ORDER",  "  ORDER",  -8, 8,      SQLTK_NORMAL,   true},
    { "GROUP",  "  GROUP",  -8, 8,      SQLTK_NORMAL,   true},
    { "HAVING", " HAVING",  -8, 8,      SQLTK_NORMAL,   true},
    { "LIMIT",  "  LIMIT",  -8, 8,      SQLTK_NORMAL,   true},
    { "CASE",   "CASE",      0, 4,      SQLTK_NORMAL,   true},
    { "WHEN",   "WHEN",      0, 0,      SQLTK_NORMAL,   true},
    { "ELSE",   "ELSE",      0, 0,      SQLTK_NORMAL,   true},
    { "END",    "END ",     -4, 0,      SQLTK_NORMAL,   true},
    {0, 0, 0, 0, SQLTK_NORMAL, false}
};

tokenAction secondOnToken= 
    { "ON",     "ON",       -5, 0,      SQLTK_ON,       true};


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
        sqlTokens[0].special = SQLTK_NORMAL;
        sqlTokens[0].doBreak = true;

        wxString fc, token;
        queryTokenizer tokenizer(GetDefinition());
        int indent=0;
        int position=0;  // col position. updated, but not used at the moment.
        bool wasOn=false;

        while (tokenizer.HasMoreTokens())
        {
            token=tokenizer.GetNextToken();

gotToken:
            wxString trailingChars;

            // token may contain brackets
            int bracketPos;
            bracketPos=token.Find('(', true);
            while (bracketPos >= 0)
            {
                fc += token.Left(bracketPos+1);
                token = token.Mid(bracketPos+1);
                bracketPos=token.Find('(', true);
            }

            bracketPos=token.Find(')', true);
            while (bracketPos >= 0)
            {
                trailingChars = token.Mid(bracketPos) + trailingChars;
                token = token.Left(bracketPos);
                bracketPos=token.Find(')', true);
            }
            // identify token
            tokenAction *tp=sqlTokens;
            while (tp->keyword)
            {
                if (!token.CmpNoCase(tp->keyword))
                {
                    if (tp->special == SQLTK_ON && wasOn)
                        tp=&secondOnToken;
                    else
                        wasOn = (tp->special == SQLTK_ON);
                    break;
                }
                tp++;
            }
            
            if (tp && tp->keyword)
            {
                // we found a keyword.
                if (tp->special == SQLTK_UNION || tp->special == SQLTK_JOINMOD)
                {
                    token=tokenizer.GetNextToken();
                    if (tp->special == SQLTK_UNION && token.CmpNoCase(wxT("JOIN")))
                    {
                        fc += wxT("\nUNION\n");
                        indent=0;
                        goto gotToken;
                    }
                    else
                    {
                        trailingChars = token + wxT(" ") + trailingChars;
                        indent += tp->actionBefore;
                        if (indent<0)   indent=0;
                    }
                }
                else
                {
                    indent += tp->actionBefore;
                    if (indent<0)   indent=0;
                }
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
                fc += token;
                position += token.Length();
            }
            fc += wxT(" ");
            position++;
            if (!trailingChars.IsNull())
            {
                fc += trailingChars + wxT(" ");;
                position += trailingChars.Length() + 1;
            }
        }

        sql = wxT("CREATE VIEW ") + GetQuotedFullIdentifier() + wxT(" AS \n")
            + fc
            + wxT("\n\n") 
            + GetGrant(wxT("Table"))
            + GetCommentSql();
    }
    return sql;
}

void pgView::ShowTreeDetail(wxTreeCtrl *browser, frmMain *form, wxListCtrl *properties, wxListCtrl *statistics, ctlSQLBox *sqlPane)
{
    if (!expandedKids)
    {
        expandedKids = true;
        // append type here
    }
    if (properties)
    {
        CreateListColumns(properties);
        int pos=0;

        InsertListItem(properties, pos++, wxT("Name"), GetName());
        InsertListItem(properties, pos++, wxT("OID"), GetOid());
        InsertListItem(properties, pos++, wxT("Owner"), GetOwner());
        InsertListItem(properties, pos++, wxT("ACL"), GetAcl());
        InsertListItem(properties, pos++, wxT("Definition"), GetDefinition());
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

            view->iSetOid(views->GetOid(wxT("oid")));
            view->iSetOwner(views->GetVal(wxT("Viewowner")));
            view->iSetAcl(views->GetVal(wxT("relacl")));
            view->iSetDefinition(views->GetVal(wxT("definition")));

            if (browser)
            {
                browser->AppendItem(collection->GetId(), view->GetIdentifier(), PGICON_VIEW, -1, view);
    			views->MoveNext();
            }
            else
                break;
        }

		delete views;
    }
    return view;
}



void pgView::ShowTreeCollection(pgCollection *collection, frmMain *form, wxTreeCtrl *browser, wxListCtrl *properties, wxListCtrl *statistics, ctlSQLBox *sqlPane)
{

    if (browser->GetChildrenCount(collection->GetId(), FALSE) == 0)
    {
        // Log
        wxLogInfo(wxT("Adding Views to schema %s"), collection->GetSchema()->GetIdentifier().c_str());


        // Get the Views
        ReadObjects(collection, browser);
    }
}
