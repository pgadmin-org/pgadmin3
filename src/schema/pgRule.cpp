//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002 - 2003, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// pgRule.cpp - Rule class
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "misc.h"
#include "pgObject.h"
#include "pgRule.h"
#include "pgCollection.h"


pgRule::pgRule(pgSchema *newSchema, const wxString& newName)
: pgRuleObject(newSchema, PG_RULE, newName)
{
}

pgRule::~pgRule()
{
}

bool pgRule::DropObject(wxFrame *frame, wxTreeCtrl *browser)
{
    return GetDatabase()->ExecuteVoid(wxT("DROP RULE ") + GetQuotedFullIdentifier() + wxT(" ON ") + GetQuotedFullTable() + wxT(";"));
}

wxString pgRule::GetSql(wxTreeCtrl *browser)
{
    if (sql.IsNull())
    {
#if 0
        sql = wxT("-- Rule: \"") + GetQuotedFullIdentifier() + wxT("\"\n\n")
            + wxT("-- DROP RULE ") + GetQuotedFullIdentifier() + wxT(" ON ") + GetQuotedFullTable() + wxT(";")
            + wxT("\n\nCREATE OR REPLACE RULE ") + GetQuotedIdentifier()
            + wxT("\n  AS ON ") + GetEvent()
            + wxT("\n  TO ") + GetQuotedFullTable()
            + wxT("\n  DO ");
        if (GetDoInstead())
            sql += wxT("INSTEAD ");
        if (GetDefinition().IsEmpty())
            sql += wxT("NOTHING;");
        else
#endif
            sql += GetFormattedDefinition();

        sql += wxT("\n");
        if (!GetComment().IsEmpty())
            sql += wxT("COMMENT ON RULE ") + GetQuotedIdentifier() + wxT(" ON ") + GetQuotedFullTable()
                +  wxT(" IS ") + qtString(GetComment()) + wxT(";\n");
    }
    return sql;
}



void pgRule::ShowTreeDetail(wxTreeCtrl *browser, frmMain *form, wxListCtrl *properties, wxListCtrl *statistics, ctlSQLBox *sqlPane)
{
    if (properties)
    {
        CreateListColumns(properties);
        int pos=0;

        InsertListItem(properties, pos++, _("Name"), GetName());
        InsertListItem(properties, pos++, _("OID"), GetOid());
        InsertListItem(properties, pos++, _("Event"), GetEvent());
        InsertListItem(properties, pos++, _("Condition"), GetCondition());
        InsertListItem(properties, pos++, _("Do Instead?"), GetDoInstead());
        InsertListItem(properties, pos++, _("Action"), GetAction());
        if (GetDefinition().IsEmpty())
            InsertListItem(properties, pos++, _("Definition"), wxT("NOTHING"));
        else
            InsertListItem(properties, pos++, _("Definition"), GetDefinition().Left(250));
        InsertListItem(properties, pos++, _("System Rule?"), GetSystemObject());
        InsertListItem(properties, pos++, _("Comment"), GetComment());
    }
}



pgObject *pgRule::Refresh(wxTreeCtrl *browser, const wxTreeItemId item)
{
    pgObject *rule=0;
    wxTreeItemId parentItem=browser->GetItemParent(item);
    if (parentItem)
    {
        pgObject *obj=(pgObject*)browser->GetItemData(parentItem);
        if (obj->GetType() == PG_RULES)
            rule = ReadObjects((pgCollection*)obj, 0, wxT("\n   AND rw.oid=") + GetOidStr());
    }
    return rule;
}



pgObject *pgRule::ReadObjects(pgCollection *collection, wxTreeCtrl *browser, const wxString &restriction)
{
    pgRule *rule=0;

    pgSet *rules= collection->GetDatabase()->ExecuteSet(wxT(
        "SELECT rw.oid, rw.ev_class, rulename, relname, nspname, description, is_instead, ev_type, ev_action, ev_qual,\n"
        "       ") + collection->GetDatabase()->GetRuledefFunction() + wxT("(rw.oid) as definition\n"
        "  FROM pg_rewrite rw\n"
        "  JOIN pg_class cl ON cl.oid=rw.ev_class\n"
        "  JOIN pg_namespace nsp ON nsp.oid=cl.relnamespace\n"
        "  LEFT OUTER JOIN pg_description des ON des.objoid=rw.oid\n"
        " WHERE ev_class = ") + NumToStr(collection->GetOid()) 
        + restriction + wxT("\n"
        " ORDER BY rulename"));

    if (rules)
    {
        while (!rules->Eof())
        {
            rule = new pgRule(collection->GetSchema(), rules->GetVal(wxT("rulename")));

            rule->iSetOid(rules->GetOid(wxT("oid")));
            rule->iSetTableOid(collection->GetOid());
            rule->iSetComment(rules->GetVal(wxT("description")));
            rule->iSetDoInstead(rules->GetBool(wxT("is_instead")));
            rule->iSetAction(rules->GetVal(wxT("ev_action")));
            wxString definition=rules->GetVal(wxT("definition"));
            if (!definition.IsEmpty())
            {
#if 0
                int doPos=definition.Find(wxT(" DO INSTEAD "));
                if (doPos > 0)
                    rule->iSetDefinition(definition.Mid(doPos + 12));
                else
                {
                    doPos = definition.Find(wxT(" DO "));
                    if (doPos > 0)
                        rule->iSetDefinition(definition.Mid(doPos+4));
                    else
                        rule->iSetDefinition(definition);
                }
#else
            rule->iSetDefinition(definition);
#endif
            }
            rule->iSetQuotedFullTable(qtIdent(rules->GetVal(wxT("nspname"))) + wxT(".")
                + qtIdent(rules->GetVal(wxT("relname"))));
            char *evts[] = {0, "SELECT", "UPDATE", "INSERT", "DELETE"};
            int evno=StrToLong(rules->GetVal(wxT("ev_type")));
            if (evno > 0 && evno < 5)
                rule->iSetEvent(evts[evno]);
            else
                rule->iSetEvent(wxT("Unknown"));

            if (browser)
            {
                collection->AppendBrowserItem(browser, rule);
				rules->MoveNext();
            }
            else
                break;
        }

		delete rules;
    }
    return rule;
}
