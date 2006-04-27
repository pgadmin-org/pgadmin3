//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id: pgCollection.cpp 4875 2006-01-06 21:06:46Z dpage $
// Copyright (C) 2002 - 2006, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// pgCollection.cpp - Simple object for use with 'collection' nodes
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "menu.h"
#include "misc.h"
#include "pgaJob.h"
#include "pgSchema.h"
#include "pgTable.h"
#include "frmMain.h"
#include "frmReport.h"

pgCollection::pgCollection(pgaFactory *factory)
: pgObject(*factory)
{ 
    wxLogInfo(wxT("Creating a pgCollection object")); 
	job=0;
    schema=0;
    database=0;
    server= 0;
}


pgCollection::~pgCollection()
{
    wxLogInfo(wxT("Destroying a pgCollection object"));
}


bool pgCollection::IsCollectionFor(pgObject *obj)
{
    if (!obj)
        return false;
    pgaFactory *f=obj->GetFactory();
    if (!f)
        return false;
    return GetFactory() == f->GetCollectionFactory();
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


void pgCollection::ShowList(ctlTree *browser, ctlListView *properties)
{
    ShowList(((pgaCollectionFactory*)GetFactory())->GetItemTypeName(), browser, properties);
}

wxMenu *pgCollection::GetObjectReportMenu(wxMenu *menu)
{
    wxString title;
    title.Printf(_("%s list report..."), GetIdentifier().c_str());
    menu->Append(MNU_REPORTS_OBJECT_LIST, title);

    pgaFactory *f = this->GetItemFactory();

    if (f)
    {
        if (f->GetMetaType() == PGM_TABLE ||
            f->GetMetaType() == PGM_TABLESPACE || 
            f->GetMetaType() == PGM_DATABASE)
            menu->Append(MNU_REPORTS_STATISTICS, _("Statistics report..."));
    }
    return menu;
}

bool pgCollection::CreateObjectReport(frmMain *parent, int type, frmReport *rep)
{
    wxString title;

    switch (type)
    {
        case MNU_REPORTS_OBJECT_LIST:
            {
                wxCookieType cookie;
                pgObject *data;
                ctlTree *browser = parent->GetBrowser();
                wxTreeItemId item;
                long pos=0;

                title.Printf(_("%s list report"), GetIdentifier().c_str());
                rep->SetReportTitle(title);

                rep->AddReportDetailHeader(this->GetFullIdentifier());

                rep->StartReportTable();
                rep->AddReportDataTableHeaderRow(2, _("Name"), _("Comment"));

                item = browser->GetFirstChild(GetId(), cookie);
                while (item)
                {
                    data = browser->GetObject(item);
                    if (IsCollectionFor(data))
                        rep->AddReportDataTableDataRow(2, data->GetFullName().c_str(), data->GetComment().c_str());

                    item = browser->GetNextChild(GetId(), cookie);
                    pos++;
                }

                rep->EndReportTable();
            }
            break;

        default:
            return false;
    }

    return true;
}

void pgCollection::ShowList(const wxString& name, ctlTree *browser, ctlListView *properties)
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
            data = browser->GetObject(item);
            if (IsCollectionFor(data))
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

void pgCollection::UpdateChildCount(ctlTree *browser, int substract)
{
    wxString label;
    label.Printf(wxString(wxGetTranslation(GetName())) + wxT(" (%d)"), browser->GetChildrenCount(GetId(), false) -substract);
    browser->SetItemText(GetId(), label);
}


int pgCollection::GetIconId()
{
    pgaFactory *objFactory=pgaFactory::GetFactory(GetType());
    if (objFactory)
        return objFactory->GetIconId();
    return 0;
}


pgObject *pgCollection::FindChild(ctlTree *browser, int index)
{
    wxCookieType cookie;
    pgObject *data;

    wxTreeItemId item = browser->GetFirstChild(GetId(), cookie);
    long pos=0;
    while (item && index >= 0)
    {
        data = browser->GetObject(item);
        if (data && IsCollectionFor(data))
        {
            if (index == pos)
                return data;

            pos++;
        }
        item = browser->GetNextChild(GetId(), cookie);
    }
    return 0;
}



void pgCollection::ShowTreeDetail(ctlTree *browser, frmMain *form, ctlListView *properties, ctlSQLBox *sqlPane)
{
    if (browser->GetChildrenCount(GetId(), false) == 0)
    {
        if (GetFactory())
            GetFactory()->CreateObjects(this, browser);
    }

    UpdateChildCount(browser);
    if (properties)
    {
        ShowList(browser, properties);
    }
}
