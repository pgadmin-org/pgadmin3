//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2005, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// pgCollection.cpp - Simple object for use with 'collection' nodes
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "misc.h"
#include "pgCollection.h"
#include "pgServer.h"
#include "pgGroup.h"
#include "pgUser.h"
#include "pgLanguage.h"
#include "pgTablespace.h"
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
#include "pgaStep.h"
#include "pgaSchedule.h"
#include "pgaJob.h"
#include "slCluster.h"

#include "pgSchema.h"

pgCollection::pgCollection(pgaFactory &factory)
: pgObject(factory)
{ 
    wxLogInfo(wxT("Creating a pgCollection object")); 
	job=0;
    schema=0;
    database=0;
    server= 0;
}


pgCollection::pgCollection(int newType, pgServer *sv)
: pgObject(newType)
{ 
    wxLogInfo(wxT("Creating a pgCollection object")); 
	job=0;
    schema=0;
    database=0;
    server= sv;
}


pgCollection::pgCollection(int newType, pgDatabase *db)
: pgObject(newType)
{ 
    wxLogInfo(wxT("Creating a pgCollection object")); 
	job=0;
    schema=0;
    database=db;
    server= database->GetServer();
}


pgCollection::pgCollection(int newType, pgSchema *sch)
: pgObject(newType)
{ 
    wxLogInfo(wxT("Creating a pgCollection object")); 
	job=0;
    schema = sch;
    database = sch->GetDatabase();
    server= database->GetServer();
}


pgCollection::pgCollection(int newType, pgaJob *jb)
: pgObject(newType)
{ 
    wxLogInfo(wxT("Creating a pgCollection object")); 
	job = jb;
    schema=0;
    server= job->GetServer();
    database = server->GetDatabase();
}

pgCollection::~pgCollection()
{
    wxLogInfo(wxT("Destroying a pgCollection object"));
}


bool pgCollection::IsCollectionForType(int objType)
{
    if (GetFactory())
    {
        pgaFactory *f=pgaFactory::GetFactory(objType);
        return (f && f->GetCollectionFactory() == GetFactory());
    }
    return (GetType() == objType-1);
}


void pgCollection::ShowList(wxTreeCtrl *browser, ctlListView *properties)
{
    if (GetFactory())
        ShowList(((pgaCollectionFactory*)GetFactory())->GetItemTypeName(), browser, properties);
    else
        ShowList(typesList[GetType()+1].typName, browser, properties);
}


void pgCollection::ShowList(const wxString& name, wxTreeCtrl *browser, ctlListView *properties)
{
    if (properties)
    {
        // Display the properties.
        wxCookieType cookie;
        pgObject *data;

        // Setup listview
        CreateListColumns(properties, wxGetTranslation(name), _("Comment"));

        wxTreeItemId item = browser->GetFirstChild(GetId(), cookie);
        long pos=0;
        while (item)
        {
            data = (pgObject *)browser->GetItemData(item);
            if (IsCollectionForType(data->GetType()))
            {
                properties->InsertItem(pos, data->GetFullName(), data->GetIconId());
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
    label.Printf(wxString(wxGetTranslation(GetName())) + wxT(" (%d)"), browser->GetChildrenCount(GetId(), false) -substract);
    browser->SetItemText(GetId(), label);
}


bool pgCollection::CanCreate()
{
    switch (GetType())
    {
		case PGA_JOBS:
		case PGA_STEPS:
		case PGA_SCHEDULES:
            return GetServer()->GetSuperUser();
        case PG_COLUMNS:
        case PG_INDEXES:
        case PG_RULES:
        case PG_TRIGGERS:
            return GetSchema()->GetCreatePrivilege();
        default:
            return false;
    }
}


int pgCollection::GetIconId()
{
    switch (GetType())
    {
        case PG_COLUMNS:            return PGICON_COLUMN;
        case PG_INDEXES:            return PGICON_INDEX;
        case PG_RULES:              return PGICON_RULE;
        case PG_TRIGGERS:           return PGICON_TRIGGER;
		case PGA_JOBS:				return PGAICON_JOB;
		case PGA_STEPS:				return PGAICON_STEP;
		case PGA_SCHEDULES:			return PGAICON_SCHEDULE;
        default:
        {
            pgaFactory *objFactory=pgaFactory::GetFactory(GetType());
            if (objFactory)
                return objFactory->GetIconId();

            return 0;
        }
    }
}


pgObject *pgCollection::FindChild(wxTreeCtrl *browser, int index)
{
    wxCookieType cookie;
    pgObject *data;

    wxTreeItemId item = browser->GetFirstChild(GetId(), cookie);
    long pos=0;
    while (item && index >= 0)
    {
        data = (pgObject *)browser->GetItemData(item);
        if (data && IsCollectionForType(data->GetType()))
        {
            if (index == pos)
                return data;

            pos++;
        }
        item = browser->GetNextChild(GetId(), cookie);
    }
    return 0;
}



void pgCollection::ShowTreeDetail(wxTreeCtrl *browser, frmMain *form, ctlListView *properties, ctlSQLBox *sqlPane)
{
    if (browser->GetChildrenCount(GetId(), false) == 0)
    {
        switch (GetType())
        {
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
			case PGA_JOBS:
				pgaJob::ReadObjects(this, browser);
				break;
			case PGA_STEPS:
				pgaStep::ReadObjects(this, browser);
				break;
			case PGA_SCHEDULES:
				pgaSchedule::ReadObjects(this, browser);
				break;
            default:
            {
                if (GetFactory())
                    GetFactory()->CreateObjects(this, browser);
                else
                    return;
            }
        }
    }

    UpdateChildCount(browser);
    if (properties)
    {
        ShowList(browser, properties);
    }
}
