//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
//
// pgTrigger.cpp - Trigger class
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "misc.h"
#include "pgObject.h"
#include "pgTrigger.h"
#include "pgCollection.h"



// These constants come from pgsql/src/include/catalog/pg_trigger.h
#define TRIGGER_TYPE_ROW				(1 << 0)
#define TRIGGER_TYPE_BEFORE				(1 << 1)
#define TRIGGER_TYPE_INSERT				(1 << 2)
#define TRIGGER_TYPE_DELETE				(1 << 3)
#define TRIGGER_TYPE_UPDATE				(1 << 4)


pgTrigger::pgTrigger(pgSchema *newSchema, const wxString& newName)
: pgSchemaObject(newSchema, PG_TRIGGER, newName)
{
}

pgTrigger::~pgTrigger()
{
}


wxString pgTrigger::GetSql(wxTreeCtrl *browser)
{
    if (sql.IsNull())
    {
    }

    return sql;
}


wxString pgTrigger::GetFireWhen() const 
{
    return (triggerType & TRIGGER_TYPE_BEFORE) ? wxT("BEFORE") : wxT("AFTER");
}

wxString pgTrigger::GetEvent() const
{
    wxString event;
    if (triggerType & TRIGGER_TYPE_INSERT)
        event = wxT("INSERT");
    if (triggerType & TRIGGER_TYPE_UPDATE)
    {
        if (!event.IsNull())
            event += wxT(" OR ");
        event += wxT("UPDATE");
    }
    if (triggerType & TRIGGER_TYPE_DELETE)
    {
        if (!event.IsNull())
            event += wxT(" OR ");
        event += wxT("DELETE");
    }
    return event;
}

wxString pgTrigger::GetForEach() const
{
    return (triggerType & TRIGGER_TYPE_ROW) ? wxT("ROW") : wxT("STATEMENT");
}



void pgTrigger::ShowTreeDetail(wxTreeCtrl *browser, frmMain *form, wxListCtrl *properties, wxListCtrl *statistics, ctlSQLBox *sqlPane)
{
    SetButtons(form, true);

    if (!expandedKids)
    {
        expandedKids = true;
        // append function here
    }

    if (properties)
    {
        properties->ClearAll();
        properties->InsertColumn(0, wxT("Property"), wxLIST_FORMAT_LEFT, 150);
        properties->InsertColumn(1, wxT("Value"), wxLIST_FORMAT_LEFT, 200);
  

        int pos=0;

        InsertListItem(properties, pos++, wxT("Name"), GetName());
        InsertListItem(properties, pos++, wxT("OID"), NumToStr(GetOid()));
        InsertListItem(properties, pos++, wxT("Fires"), GetFireWhen());
        InsertListItem(properties, pos++, wxT("Event"), GetEvent());
        InsertListItem(properties, pos++, wxT("For Each"), GetForEach());
        InsertListItem(properties, pos++, wxT("Function"), GetFunction());
        InsertListItem(properties, pos++, wxT("Enabled?"), BoolToYesNo(GetEnabled()));
        InsertListItem(properties, pos++, wxT("System Trigger?"), BoolToYesNo(GetSystemObject()));
        InsertListItem(properties, pos++, wxT("Comment"), GetComment());
    }
}


void pgTrigger::ShowTreeCollection(pgCollection *collection, frmMain *form, wxTreeCtrl *browser, wxListCtrl *properties, wxListCtrl *statistics, ctlSQLBox *sqlPane)
{
    wxString msg;
    pgTrigger *trigger;

    if (browser->GetChildrenCount(collection->GetId(), FALSE) == 0)
    {
        // Log
        msg.Printf(wxT("Adding Triggers to schema %s"), collection->GetSchema()->GetIdentifier().c_str());
        wxLogInfo(msg);

        // Get the Triggers
        pgSet *triggers= collection->GetDatabase()->ExecuteSet(wxT(
            "SELECT t.oid, tgname, proname, tgtype, tgenabled\n"
            "  FROM pg_trigger t, pg_proc p\n"
            " WHERE t.tgfoid = p.oid AND tgisconstraint = FALSE AND tgrelid = ") + collection->GetOidStr() + wxT("\n"
            " ORDER BY tgname"));

        if (triggers)
        {
            while (!triggers->Eof())
            {
                trigger = new pgTrigger(collection->GetSchema(), triggers->GetVal(wxT("tgname")));

                trigger->iSetOid(StrToDouble(triggers->GetVal(wxT("oid"))));
                trigger->iSetTableOid(collection->GetOid());
                trigger->iSetFunction(triggers->GetVal(wxT("proname")));
                trigger->iSetEnabled(StrToBool(triggers->GetVal(wxT("tgenabled"))));
                trigger->iSetTriggerType(StrToLong(triggers->GetVal(wxT("tgtype"))));

                browser->AppendItem(collection->GetId(), trigger->GetIdentifier(), PGICON_TRIGGER, -1, trigger);
	    
			    triggers->MoveNext();
            }

		    delete triggers;
        }
    }
}

