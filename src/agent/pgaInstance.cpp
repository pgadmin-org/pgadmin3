//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002 - 2003, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// pgaInstance.h - PostgreSQL Server Instance
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "misc.h"
#include "pgObject.h"
#include "pgDatabase.h"
#include "pgCollection.h"
#include "pgaInstance.h"


pgaInstance::pgaInstance(const wxString& newName)
: pgDatabaseObject(PGA_INSTANCE, newName)
{
    wxLogInfo(wxT("Creating a pgaInstance object"));
}

pgaInstance::~pgaInstance()
{
    wxLogInfo(wxT("Destroying a pgaInstance object"));
}



void pgaInstance::ShowTreeDetail(wxTreeCtrl *browser, frmMain *form, wxListCtrl *properties, wxListCtrl *statistics, ctlSQLBox *sqlPane)
{
    if (!expandedKids)
    {
        expandedKids=true;

        RemoveDummyChild(browser);
        // Log
        wxLogInfo(wxT("Adding child object to Instance."));

    }
    if (properties)
    {
        // Setup listview
        CreateListColumns(properties);
        int pos=0;
        InsertListItem(properties, pos++, _("Instance"), GetName());
        InsertListItem(properties, pos++, _("Login time"), GetLogintime());
    }
}



pgObject *pgaInstance::Refresh(wxTreeCtrl *browser, const wxTreeItemId item)
{
    pgObject *instance=0;
    wxTreeItemId parentItem=browser->GetItemParent(item);
    if (parentItem)
    {
        pgObject *obj=(pgObject*)browser->GetItemData(parentItem);
        if (obj->GetType() == PGA_INSTANCES)
            instance = ReadObjects((pgCollection*)obj, 0);
    }
    return instance;
}



pgObject *pgaInstance::ReadObjects(pgCollection *collection, wxTreeCtrl *browser, const wxString &restriction)
{
    pgaInstance *instance=0;

    pgSet *instances= collection->GetDatabase()->ExecuteSet(
       wxT("SELECT * FROM pg_jobagent\n")
       + restriction +
       wxT(" ORDER BY logintime"));

    if (instances)
    {
        while (!instances->Eof())
        {

            instance = new pgaInstance(instances->GetVal(wxT("station")) + wxT(" (") + instances->GetVal(wxT("jagpid"))+wxT(")"));
            instance->iSetDatabase(collection->GetDatabase());
            instance->iSetPid(instances->GetLong(wxT("jagpid")));
            instance->iSetLogintime(instances->GetDateTime(wxT("logintime")));

            if (browser)
            {
                collection->AppendBrowserItem(browser, instance);
				instances->MoveNext();
            }
            else
                break;
        }

		delete instances;
    }
    return instance;
}
