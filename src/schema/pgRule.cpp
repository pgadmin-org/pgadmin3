//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
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



wxString pgRule::GetSql(wxTreeCtrl *browser)
{
    if (sql.IsNull())
    {
        sql = wxT("CREATE OR REPLACE RULE ") + GetQuotedIdentifier()
            + wxT(" AS ON ") + GetEvent()
            + wxT(" TO ") + GetQuotedFullTable()
            + wxT(" DO ");
        if (GetDoInstead())
            sql += wxT("INSTEAD ");
        if (GetDefinition().IsEmpty())
            sql += wxT("NOTHING");
        else
            sql += wxT("\n(\n")
                + GetDefinition()
                + wxT(")\n");

        sql += wxT(";\n");
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

        InsertListItem(properties, pos++, wxT("Name"), GetName());
        InsertListItem(properties, pos++, wxT("OID"), GetOid());
        InsertListItem(properties, pos++, wxT("Event"), GetEvent());
        InsertListItem(properties, pos++, wxT("Condition"), GetCondition());
        InsertListItem(properties, pos++, wxT("Do Instead?"), GetDoInstead());
        InsertListItem(properties, pos++, wxT("Action"), GetAction());
        if (GetDefinition().IsEmpty())
            InsertListItem(properties, pos++, wxT("Definition"), wxT("NOTHING"));
        else
            InsertListItem(properties, pos++, wxT("Definition"), GetDefinition());
        InsertListItem(properties, pos++, wxT("System Rule?"), GetSystemObject());
        InsertListItem(properties, pos++, wxT("Comment"), GetComment());
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
            rule = ReadObjects((pgCollection*)obj, 0, wxT("\n   AND rw.ev_class=") + GetOidStr());
    }
    return rule;
}



pgObject *pgRule::ReadObjects(pgCollection *collection, wxTreeCtrl *browser, const wxString &restriction)
{
    pgRule *rule=0;

    pgSet *rules= collection->GetDatabase()->ExecuteSet(wxT(
        "SELECT rw.ev_class, rulename, pg_get_ruledef(oid) as definition, is_instead, ev_type, ev_action, ev_qual, description\n"
        "  FROM pg_rewrite rw\n"
        "  JOIN pg_class cl ON cl.oid=rw.ev_class\n"
        "  JOIN pg_namespace nsp ON nsp.oid=cl.relnamespace\n"
        "  LEFT OUTER JOIN pg_description des ON des.objoid=rw.oid\n"
        " WHERE ev_class = ") + NumToStr(collection->GetOid()) 
        + restriction + wxT("::oid\n"
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
            rule->iSetDefinition(rules->GetVal(wxT("definition")));
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
                browser->AppendItem(collection->GetId(), rule->GetFullName(), PGICON_RULE, -1, rule);
				rules->MoveNext();
            }
            else
                break;
        }

		delete rules;
    }
    return rule;
}



void pgRule::ShowTreeCollection(pgCollection *collection, frmMain *form, wxTreeCtrl *browser, wxListCtrl *properties, wxListCtrl *statistics, ctlSQLBox *sqlPane)
{
    if (browser->GetChildrenCount(collection->GetId(), FALSE) == 0)
    {
        // Log
        wxLogInfo(wxT("Adding Rules to schema ") + collection->GetSchema()->GetIdentifier());

        // Get the Rules
        ReadObjects(collection, browser);
    }
}

