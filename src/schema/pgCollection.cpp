//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002 - 2003, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// pgCollection.cpp - Simple object for use with 'collection' nodes
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "pgCollection.h"
#include "pgServer.h"
#include "pgGroup.h"
#include "pgUser.h"
#include "pgLanguage.h"
#include "pgAggregate.h"
#include "pgCast.h"
#include "pgConversion.h"
#include "pgDomain.h"
#include "pgFunction.h"
#include "pgOperator.h"
#include "pgOperatorClass.h"
#include "pgSequence.h"
#include "pgTable.h"
#include "pgType.h"
#include "pgView.h"
#include "pgColumn.h"
#include "pgIndex.h"
#include "pgRule.h"
#include "pgTrigger.h"


pgCollection::pgCollection(int newType, pgServer *sv)
: pgObject(newType, typeNameList[newType])
{ 
    wxLogInfo(wxT("Creating a pgCollection object")); 
    schema=0;
    database=0;
    server= sv;
}


pgCollection::pgCollection(int newType, pgDatabase *db)
: pgObject(newType, typeNameList[newType])
{ 
    wxLogInfo(wxT("Creating a pgCollection object")); 
    schema=0;
    database=db;
    server= database->GetServer();
}


pgCollection::pgCollection(int newType, pgSchema *sch)
: pgObject(newType, typeNameList[newType])
{ 
    wxLogInfo(wxT("Creating a pgCollection object")); 
    schema = sch;
    database = sch->GetDatabase();
    server= database->GetServer();
}


pgCollection::~pgCollection()
{
    wxLogInfo(wxT("Destroying a pgCollection object"));
}


void pgCollection::ShowList(const wxString& name, wxTreeCtrl *browser, wxListCtrl *properties)
{
    if (properties)
    {
        // Display the properties.
        long cookie;
        pgObject *data;

        // Setup listview
        CreateListColumns(properties, name, _("Comment"));

        wxTreeItemId item = browser->GetFirstChild(GetId(), cookie);
        long pos=0;
        while (item)
        {
            data = (pgObject *)browser->GetItemData(item);
            if (IsCollectionForType(data->GetType()))
            {
                properties->InsertItem(pos, data->GetFullName(), data->GetIcon());
                properties->SetItem(pos, 1, data->GetComment());
            }
            // Get the next item
            item = browser->GetNextChild(GetId(), cookie);
            pos++;
        }
    }
}



void pgCollection::UpdateChildCount(wxTreeCtrl *browser, int substract)
{
    wxString label;
    label.Printf(wxString(wxGetTranslation(GetName())) + wxT(" (%d)"), browser->GetChildrenCount(GetId(), FALSE) -substract);
    browser->SetItemText(GetId(), label);
}


bool pgCollection::CanCreate()
{
    switch (GetType())
    {
        case PG_USERS:
        case PG_GROUPS:
            return GetServer()->GetSuperUser();
        case PG_DATABASES:
            return GetServer()->GetCreatePrivilege();
        case PG_CASTS:
        case PG_LANGUAGES:
        case PG_SCHEMAS:
            return GetDatabase()->GetCreatePrivilege();
        case PG_AGGREGATES:
        case PG_CONVERSIONS:
        case PG_DOMAINS:
        case PG_FUNCTIONS:
        case PG_TRIGGERFUNCTIONS:
        case PG_OPERATORS:
        case PG_SEQUENCES:
        case PG_TABLES:
        case PG_TYPES:
        case PG_VIEWS:
        case PG_COLUMNS:
        case PG_INDEXES:
        case PG_RULES:
        case PG_TRIGGERS:
            return GetSchema()->GetCreatePrivilege();
        case PG_OPERATORCLASSES:
        default:
            return false;
    }
}


int pgCollection::GetIcon()
{
    switch (GetType())
    {
        case PG_SERVERS:            return PGICON_SERVER;
        case PG_USERS:              return PGICON_USER;
        case PG_GROUPS:             return PGICON_GROUP;
        case PG_DATABASES:          return PGICON_DATABASE;
        case PG_CASTS:              return PGICON_CAST;
        case PG_LANGUAGES:          return PGICON_LANGUAGE;
        case PG_SCHEMAS:            return PGICON_SCHEMA;
        case PG_AGGREGATES:         return PGICON_AGGREGATE;
        case PG_CONVERSIONS:        return PGICON_CONVERSION;
        case PG_DOMAINS:            return PGICON_DOMAIN;
        case PG_FUNCTIONS:          return PGICON_FUNCTION;
        case PG_TRIGGERFUNCTIONS:   return PGICON_TRIGGERFUNCTION;
        case PG_OPERATORS:          return PGICON_OPERATOR;
        case PG_OPERATORCLASSES:    return PGICON_OPERATORCLASS;
        case PG_SEQUENCES:          return PGICON_SEQUENCE;
        case PG_TABLES:             return PGICON_TABLE;
        case PG_TYPES:              return PGICON_TYPE;
        case PG_VIEWS:              return PGICON_VIEW;
        case PG_COLUMNS:            return PGICON_COLUMN;
        case PG_INDEXES:            return PGICON_INDEX;
        case PG_RULES:              return PGICON_RULE;
        case PG_TRIGGERS:           return PGICON_TRIGGER;
        default:    return 0;
    }
}


void pgCollection::SetSql(wxTreeCtrl *browser, ctlSQLBox *sqlPane, int index)
{
    sqlPane->Clear();

    long cookie;
    pgObject *data;

    wxTreeItemId item = browser->GetFirstChild(GetId(), cookie);
    long pos=0;
    while (item)
    {
        data = (pgObject *)browser->GetItemData(item);
        if (IsCollectionForType(data->GetType() == GetType()))
        {
            if (index == pos)
            {
                sqlPane->SetText(data->GetSql(browser));
                return;
            }
            pos++;
        }
        item = browser->GetNextChild(GetId(), cookie);
    }
}



void pgCollection::ShowTreeDetail(wxTreeCtrl *browser, frmMain *form, wxListCtrl *properties, wxListCtrl *statistics, ctlSQLBox *sqlPane)
{
    if (browser->GetChildrenCount(GetId(), FALSE) == 0)
    {
        switch (GetType())
        {
            case PG_DATABASES:
                pgDatabase::ReadObjects(this, browser);
                break;
            case PG_GROUPS:
                pgGroup::ReadObjects(this, browser);
                break;
            case PG_USERS:
                pgUser::ReadObjects(this, browser);
                break;
            case PG_LANGUAGES:
                pgLanguage::ReadObjects(this, browser);
                break;
            case PG_SCHEMAS:
                pgSchema::ReadObjects(this, browser);
                break;
            case PG_AGGREGATES:
                pgAggregate::ReadObjects(this, browser);
                break;
            case PG_CASTS:
                pgCast::ReadObjects(this, browser);
                break;
            case PG_CONVERSIONS:
                pgConversion::ReadObjects(this, browser);
                break;
            case PG_DOMAINS:
                pgDomain::ReadObjects(this, browser);
                break;
            case PG_FUNCTIONS:
                pgFunction::ReadObjects(this, browser);
                break;
            case PG_TRIGGERFUNCTIONS:
                pgTriggerFunction::ReadObjects(this, browser);
                break;
            case PG_OPERATORS:
                pgOperator::ReadObjects(this, browser);
                break;
            case PG_OPERATORCLASSES:
                pgOperatorClass::ReadObjects(this, browser);
                break;
            case PG_SEQUENCES:
                pgSequence::ReadObjects(this, browser);
                break;
            case PG_TABLES:
                pgTable::ReadObjects(this, browser);
                break;
            case PG_TYPES:
                pgType::ReadObjects(this, browser);
                break;
            case PG_VIEWS:
                pgView::ReadObjects(this, browser);
                break;
            case PG_COLUMNS:
                pgColumn::ReadObjects(this, browser);
                break;
            case PG_INDEXES:
                pgIndex::ReadObjects(this, browser);
                break;
            case PG_RULES:
                pgRule::ReadObjects(this, browser);
                break;
            case PG_TRIGGERS:
                pgTrigger::ReadObjects(this, browser);
                break;
            default:
                return;
        }
    }


    if (statistics)
    {
        switch (GetType())
        {
            case PG_DATABASES:
                pgDatabase::ShowStatistics(this, statistics);
                break;
            case PG_TABLES:
                pgTable::ShowStatistics(this, statistics);
                break;
            default:
                break;
        }
    }

    UpdateChildCount(browser);
    if (properties)
        ShowList(typeNameList[GetType()+1], browser, properties);
}
