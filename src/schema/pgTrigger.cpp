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
#include "pgFunction.h"



// These constants come from pgsql/src/include/catalog/pg_trigger.h
#define TRIGGER_TYPE_ROW				(1 << 0)
#define TRIGGER_TYPE_BEFORE				(1 << 1)
#define TRIGGER_TYPE_INSERT				(1 << 2)
#define TRIGGER_TYPE_DELETE				(1 << 3)
#define TRIGGER_TYPE_UPDATE				(1 << 4)


pgTrigger::pgTrigger(pgSchema *newSchema, const wxString& newName)
: pgSchemaObject(newSchema, PG_TRIGGER, newName)
{
    triggerFunction=0;
}


pgTrigger::~pgTrigger()
{
    if (!expandedKids && triggerFunction)
    {
        // triggerFunction wasn't appended to tree, so we 
        // need to delete it manually
        delete triggerFunction;
    }
}

bool pgTrigger::DropObject(wxFrame *frame, wxTreeCtrl *browser)
{
    return GetDatabase()->ExecuteVoid(wxT("DROP TRIGGER ") + GetQuotedFullIdentifier() + wxT(" ON ") + GetQuotedFullTable());
}


void pgTrigger::SetDirty()
{
    if (expandedKids)
        triggerFunction=0;
    pgObject::SetDirty();
}


wxString pgTrigger::GetSql(wxTreeCtrl *browser)
{
    if (sql.IsNull() && this->triggerFunction)
    {
        sql = wxT("-- DROP TRIGGER ") + GetName()
            + wxT(" ON ") + GetQuotedFullTable() +wxT(";\n"
                  "CREATE TRIGGER ") + GetName()
            + wxT(" ") + GetFireWhen() 
            + wxT(" ") + GetEvent()
            + wxT("\n    ON ") + GetQuotedFullTable()
            + wxT(" FOR EACH ") + GetForEach()
            + wxT("\n    EXECUTE PROCEDURE ") + triggerFunction->GetFullName()
            + wxT(";\n");

        if (!GetComment().IsEmpty())
            sql += wxT("COMMENT ON TRIGGER ") + GetQuotedIdentifier() + wxT(" ON ") + GetQuotedFullTable()
                +  wxT(" IS ") + qtString(GetComment()) + wxT(";\n");
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
    if (!expandedKids)
    {
        if (browser)
        {
            // if no browser present, function will not be appended to tree
            expandedKids = true;
        }
        if (triggerFunction)
            delete triggerFunction;

        // append function here
        triggerFunction=pgFunction::AppendFunctions(this, GetSchema(), browser, wxT(
            "WHERE pr.oid=") + NumToStr(functionOid) + wxT("::oid\n"));
        if (triggerFunction)
        {
            iSetFunction(triggerFunction->GetName());
        }
    }

    if (properties)
    {
        CreateListColumns(properties);
        int pos=0;

        InsertListItem(properties, pos++, wxT("Name"), GetName());
        InsertListItem(properties, pos++, wxT("OID"), GetOid());
        InsertListItem(properties, pos++, wxT("Fires"), GetFireWhen());
        InsertListItem(properties, pos++, wxT("Event"), GetEvent());
        InsertListItem(properties, pos++, wxT("For Each"), GetForEach());
        InsertListItem(properties, pos++, wxT("Function"), GetFunction());
        InsertListItem(properties, pos++, wxT("Enabled?"), GetEnabled());
        InsertListItem(properties, pos++, wxT("System Trigger?"), GetSystemObject());
        InsertListItem(properties, pos++, wxT("Comment"), GetComment());
    }
}



pgObject *pgTrigger::Refresh(wxTreeCtrl *browser, const wxTreeItemId item)
{
    pgObject *trigger=0;
    wxTreeItemId parentItem=browser->GetItemParent(item);
    if (parentItem)
    {
        pgObject *obj=(pgObject*)browser->GetItemData(parentItem);
        if (obj->GetType() == PG_TRIGGERS)
            trigger = ReadObjects((pgCollection*)obj, 0, wxT("\n   AND t.oid=") + GetOidStr());
    }
    return trigger;
}



pgObject *pgTrigger::ReadObjects(pgCollection *collection, wxTreeCtrl *browser, const wxString &restriction)
{
    pgTrigger *trigger=0;

        pgSet *triggers= collection->GetDatabase()->ExecuteSet(wxT(
        "SELECT t.oid, t.*, relname, nspname\n"
        "  FROM pg_trigger t\n"
        "  JOIN pg_class cl ON cl.oid=tgrelid\n"
        "  JOIN pg_namespace na ON na.oid=relnamespace\n"
        "  LEFT OUTER JOIN pg_description des ON des.objoid=t.oid\n"
        " WHERE NOT tgisconstraint AND tgrelid = ") + collection->GetOidStr() + wxT("\n"
        " ORDER BY tgname"));

    if (triggers)
    {
        while (!triggers->Eof())
        {
            trigger = new pgTrigger(collection->GetSchema(), triggers->GetVal(wxT("tgname")));

            trigger->iSetOid(triggers->GetOid(wxT("oid")));
            trigger->iSetTableOid(collection->GetOid());
            trigger->iSetComment(triggers->GetVal(wxT("description")));
            trigger->iSetFunctionOid(triggers->GetOid(wxT("tgfoid")));
            trigger->iSetEnabled(triggers->GetBool(wxT("tgenabled")));
            trigger->iSetTriggerType(triggers->GetLong(wxT("tgtype")));
            trigger->iSetQuotedFullTable(qtIdent(triggers->GetVal(wxT("nspname")))+wxT(".")+qtIdent(triggers->GetVal(wxT("relname"))));

            if (browser)
            {
                wxTreeItemId item=browser->AppendItem(collection->GetId(), trigger->GetIdentifier(), PGICON_TRIGGER, -1, trigger);
    			triggers->MoveNext();
            }
            else
                break;
        }

		delete triggers;
    }
    return trigger;
}



void pgTrigger::ShowTreeCollection(pgCollection *collection, frmMain *form, wxTreeCtrl *browser, wxListCtrl *properties, wxListCtrl *statistics, ctlSQLBox *sqlPane)
{
    if (browser->GetChildrenCount(collection->GetId(), FALSE) == 0)
    {
        // Log
        wxLogInfo(wxT("Adding Triggers to schema %s"), collection->GetSchema()->GetIdentifier().c_str());

        // Get the Triggers
        ReadObjects(collection, browser);
    }
}

