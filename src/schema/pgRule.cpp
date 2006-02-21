//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id: pgRule.cpp 4936 2006-01-19 14:13:54Z dpage $
// Copyright (C) 2002 - 2006, The pgAdmin Development Team
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
#include "pgRule.h"


pgRule::pgRule(pgSchema *newSchema, const wxString& newName)
: pgRuleObject(newSchema, ruleFactory, newName)
{
}

pgRule::~pgRule()
{
}

bool pgRule::DropObject(wxFrame *frame, ctlTree *browser, bool cascaded)
{
    wxString sql = wxT("DROP RULE ") + GetQuotedIdentifier() + wxT(" ON ") + GetQuotedFullTable();
    if (cascaded)
        sql += wxT(" CASCADE");
    return GetDatabase()->ExecuteVoid(sql);
}


wxString pgRule::GetSql(ctlTree *browser)
{
    if (sql.IsNull())
    {
        sql = wxT("-- Rule: \"") + GetQuotedIdentifier() + wxT(" ON ") + GetQuotedFullTable() + wxT("\"\n\n")
            + wxT("-- DROP RULE ") + GetQuotedIdentifier() + wxT(" ON ") + GetQuotedFullTable() + wxT(";\n\n")
            + wxT("CREATE OR REPLACE") + GetFormattedDefinition().Mid(6) // the backend pg_get_ruledef gives CREATE only
            + wxT("\n");
        if (!GetComment().IsEmpty())
            sql += wxT("COMMENT ON RULE ") + GetQuotedIdentifier() + wxT(" ON ") + GetQuotedFullTable()
                +  wxT(" IS ") + qtString(GetComment()) + wxT(";\n");
    }
    return sql;
}


void pgRule::ShowTreeDetail(ctlTree *browser, frmMain *form, ctlListView *properties, ctlSQLBox *sqlPane)
{
    if (properties)
    {
        CreateListColumns(properties);
        wxString def=GetDefinition().Left(250);

        properties->AppendItem(_("Name"), GetName());
        properties->AppendItem(_("OID"), GetOid());
        properties->AppendItem(_("Event"), GetEvent());
        properties->AppendItem(_("Condition"), GetCondition());
        properties->AppendItem(_("Do instead?"), GetDoInstead());
        properties->AppendItem(_("Action"), GetAction().Left(250));
        if (def.IsEmpty())
            properties->AppendItem(_("Definition"), wxT("NOTHING"));
        else
        {
            def.Replace(wxT("\n"), wxT(" "));
            properties->AppendItem(_("Definition"), def);
        }
        properties->AppendItem(_("System rule?"), GetSystemObject());
        properties->AppendItem(_("Comment"), GetComment());
    }
}



pgObject *pgRule::Refresh(ctlTree *browser, const wxTreeItemId item)
{
    pgObject *rule=0;
    pgCollection *coll=browser->GetParentCollection(item);
    if (coll)
        rule = ruleFactory.CreateObjects(coll, 0, wxT("\n   AND rw.oid=") + GetOidStr());

    return rule;
}


pgObject *pgRuleFactory::CreateObjects(pgCollection *collection, ctlTree *browser, const wxString &restriction)
{
    pgRule *rule=0;

    pgSet *rules= collection->GetDatabase()->ExecuteSet(
        wxT("SELECT rw.oid, rw.ev_class, rulename, relname, nspname, description, is_instead, ev_type, ev_action, ev_qual,\n")
        wxT("       pg_get_ruledef(rw.oid") + collection->GetDatabase()->GetPrettyOption() + wxT(") AS definition\n")
        wxT("  FROM pg_rewrite rw\n")
        wxT("  JOIN pg_class cl ON cl.oid=rw.ev_class\n")
        wxT("  JOIN pg_namespace nsp ON nsp.oid=cl.relnamespace\n")
        wxT("  LEFT OUTER JOIN pg_description des ON des.objoid=rw.oid\n")
        wxT(" WHERE ev_class = ") + NumToStr(collection->GetOid())
        + restriction + wxT("\n")
        wxT(" ORDER BY rulename"));

    if (rules)
    {
        while (!rules->Eof())
        {
            rule = new pgRule(collection->GetSchema(), rules->GetVal(wxT("rulename")));

            rule->iSetOid(rules->GetOid(wxT("oid")));
            rule->iSetComment(rules->GetVal(wxT("description")));
            rule->iSetDoInstead(rules->GetBool(wxT("is_instead")));
            rule->iSetAction(rules->GetVal(wxT("ev_action")));
            wxString definition=rules->GetVal(wxT("definition"));
            int doPos = definition.Find(wxT(" DO "));
            int wherePos = definition.Find(wxT(" WHERE "));
            if (wherePos > 0 && wherePos < doPos)
                rule->iSetCondition(definition.Mid(wherePos+7, doPos-wherePos-7));

            rule->iSetDefinition(definition);
            rule->iSetQuotedFullTable(collection->GetDatabase()->GetSchemaPrefix(rules->GetVal(wxT("nspname"))) 
                + qtIdent(rules->GetVal(wxT("relname"))));
            wxChar *evts[] = {0, wxT("SELECT"), wxT("UPDATE"), wxT("INSERT"), wxT("DELETE")};
            int evno=StrToLong(rules->GetVal(wxT("ev_type")));
            if (evno > 0 && evno < 5)
                rule->iSetEvent(evts[evno]);
            else
                rule->iSetEvent(wxT("Unknown"));

            if (browser)
            {
                browser->AppendObject(collection, rule);
				rules->MoveNext();
            }
            else
                break;
        }

		delete rules;
    }
    return rule;
}


/////////////////////////////

#include "images/rule.xpm"
#include "images/rules.xpm"

pgRuleFactory::pgRuleFactory() 
: pgSchemaObjFactory(__("Rule"), __("New Rule..."), __("Create a new Rule."), rule_xpm)
{
}


pgRuleFactory ruleFactory;
static pgaCollectionFactory cf(&ruleFactory, __("Rules"), rules_xpm);
