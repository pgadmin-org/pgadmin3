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
: pgSchemaObject(newSchema, PG_RULE, newName)
{
}

pgRule::~pgRule()
{
}



wxString pgRule::GetSql(wxTreeCtrl *browser)
{
    if (sql.IsNull())
    {
    }
    return sql;
}



void pgRule::ShowTreeDetail(wxTreeCtrl *browser, frmMain *form, wxListCtrl *properties, wxListCtrl *statistics, ctlSQLBox *sqlPane)
{
    SetButtons(form);

    properties->ClearAll();
    properties->InsertColumn(0, wxT("Property"), wxLIST_FORMAT_LEFT, 150);
    properties->InsertColumn(1, wxT("Value"), wxLIST_FORMAT_LEFT, 200);
  

    int pos=0;

    InsertListItem(properties, pos++, wxT("Name"), GetName());
    InsertListItem(properties, pos++, wxT("OID"), NumToStr(GetOid()));
    InsertListItem(properties, pos++, wxT("Event"), GetEvent());
    InsertListItem(properties, pos++, wxT("Condition"), GetCondition());
    InsertListItem(properties, pos++, wxT("Do Instead?"), BoolToYesNo(GetDoInstead()));
    InsertListItem(properties, pos++, wxT("Action"), GetAction());
    InsertListItem(properties, pos++, wxT("Definition"), GetDefinition());
    InsertListItem(properties, pos++, wxT("System Rule?"), BoolToYesNo(GetSystemObject()));
    InsertListItem(properties, pos++, wxT("Comment"), GetComment());
}



void pgRule::ShowTreeCollection(pgCollection *collection, frmMain *form, wxTreeCtrl *browser, wxListCtrl *properties, wxListCtrl *statistics, ctlSQLBox *sqlPane)
{
    pgRule *rule;

    if (browser->GetChildrenCount(collection->GetId(), FALSE) == 0)
    {
        // Log
        wxLogInfo(wxT("Adding Rules to schema ") + collection->GetSchema()->GetIdentifier());

        // Add Rule node
//        pgObject *addRuleObj = new pgObject(PG_ADD_Rule, wxString("Add Rule"));
//        browser->AppendItem(collection->GetId(), wxT("Add Rule..."), 4, -1, addRuleObj);

        // Get the Rules
        pgSet *rules= collection->GetDatabase()->ExecuteSet(wxT(
            "SELECT oid, rulename, pg_get_ruledef(oid) as definition, is_instead, ev_type, ev_action, ev_qual\n"
            "  FROM pg_rewrite WHERE ev_class = ") + NumToStr(collection->GetOid()) + wxT("::oid\n"
            " ORDER BY rulename"));

        if (rules)
        {
            while (!rules->Eof())
            {
                rule = new pgRule(collection->GetSchema(), rules->GetVal(wxT("rulename")));

                rule->iSetOid(StrToDouble(rules->GetVal(wxT("oid"))));
                rule->iSetTableOid(collection->GetOid());
                rule->iSetDefinition(rules->GetVal(wxT("definition")));
                rule->iSetDoInstead(StrToBool(rules->GetVal(wxT("is_instead"))));
                rule->iSetAction(rules->GetVal(wxT("ev_action")));
                rule->iSetComment(rules->GetVal(wxT("ev_qual")));
                char *evts[] = {0, "SELECT", "UPDATE", "INSERT", "DELETE"};
                int evno=StrToLong(rules->GetVal(wxT("ev_type")));
                if (evno > 0 && evno < 5)
                    rule->iSetEvent(evts[evno]);
                else
                    rule->iSetEvent(wxT("Unknown"));

                browser->AppendItem(collection->GetId(), rule->GetFullName(), PGICON_RULE, -1, rule);
	    
			    rules->MoveNext();
            }

		    delete rules;
        }
    }
}

