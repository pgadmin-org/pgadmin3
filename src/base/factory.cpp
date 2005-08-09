//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id:  $
// Copyright (C) 2002 - 2005, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// factory.cpp - Object classes factory
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include "pgAdmin3.h"

#include "pgCollection.h"
#include "menu.h"


wxArrayPtrVoid *factoryArray=0;

#define FACTORY_OFFSET 100

pgaFactory::pgaFactory(wxChar *tn, wxChar *ns, wxChar *nls, char **img)
{
    if (!factoryArray)
        factoryArray = new wxArrayPtrVoid;
    id=factoryArray->GetCount()+FACTORY_OFFSET;
    factoryArray->Add(this);
    collectionFactory=0;
    typeName=tn;
    newString=ns;
    newLongString=nls;
    metaType=PGM_UNKNOWN;
    image=img;
    if (image)
        iconId = addImage(image);
    else
        iconId=-1;
}


pgaFactory *pgaFactory::GetFactory(int id)
{
    id -= FACTORY_OFFSET;
    if (id >= 0 && id < (int)factoryArray->GetCount())
        return (pgaFactory*)factoryArray->Item(id);
    return 0;
}


pgaFactory *pgaFactory::GetFactory(const wxString &name)
{
    int i;
    pgaFactory *factory;

    for (i=FACTORY_OFFSET ; (factory=GetFactory(i)) != 0 ; i++)
    {
        if (name.Matches(factory->GetTypeName()))
            return factory;
    }
    return 0;
}


#include "images/aggregate.xpm"
#include "images/baddatabase.xpm"
#include "images/check.xpm"
#include "images/closeddatabase.xpm"
#include "images/tablespace.xpm"
#include "images/cast.xpm"
#include "images/conversion.xpm"
#include "images/column.xpm"
#include "images/database.xpm"
#include "images/group.xpm"
#include "images/index.xpm"
#include "images/language.xpm"
#include "images/foreignkey.xpm"
#include "images/namespace.xpm"
#include "images/property.xpm"
#include "images/public.xpm"
#include "images/rule.xpm"
#include "images/server.xpm"
#include "images/servers.xpm"
#include "images/serverbad.xpm"
#include "images/statistics.xpm"
#include "images/trigger.xpm"
#include "images/user.xpm"
#include "images/constraints.xpm"
#include "images/primarykey.xpm"
#include "images/unique.xpm"
#include "images/job.xpm"
#include "images/jobdisabled.xpm"
#include "images/step.xpm"
#include "images/schedule.xpm"
#include "images/slcluster.xpm"
#include "images/slnode.xpm"
#include "images/slpath.xpm"
#include "images/sllisten.xpm"
#include "images/slset.xpm"
#include "images/slset2.xpm"
#include "images/slsubscription.xpm"
#include "images/slsubscription2.xpm"

int pgaFactory::addImage(char **img)
{
    if (!imageList)
    {
        //Setup the global imagelist
	    imageList = new wxImageList(16, 16, true, 50);
        imageList->Add(wxIcon(property_xpm));
        imageList->Add(wxIcon(statistics_xpm));
        imageList->Add(wxIcon(servers_xpm));
        imageList->Add(wxIcon(server_xpm));
        imageList->Add(wxIcon(serverbad_xpm));
        imageList->Add(wxIcon(database_xpm));
        imageList->Add(wxIcon(language_xpm));
        imageList->Add(wxIcon(namespace_xpm));
        imageList->Add(wxIcon(tablespace_xpm));
        imageList->Add(wxIcon(user_xpm));
        imageList->Add(wxIcon(group_xpm));
        imageList->Add(wxIcon(baddatabase_xpm));
        imageList->Add(wxIcon(closeddatabase_xpm));
        imageList->Add(wxIcon(check_xpm));
        imageList->Add(wxIcon(column_xpm));
        imageList->Add(wxIcon(index_xpm));
        imageList->Add(wxIcon(rule_xpm));
        imageList->Add(wxIcon(trigger_xpm));
        imageList->Add(wxIcon(foreignkey_xpm));
        imageList->Add(wxIcon(cast_xpm));
        imageList->Add(wxIcon(conversion_xpm));
        imageList->Add(wxIcon(constraints_xpm));
        imageList->Add(wxIcon(primarykey_xpm));
        imageList->Add(wxIcon(unique_xpm));
        imageList->Add(wxIcon(public_xpm));

        // job, jobdisabled, step, schedule
        imageList->Add(wxIcon(job_xpm));
        imageList->Add(wxIcon(jobdisabled_xpm));
        imageList->Add(wxIcon(step_xpm));
        imageList->Add(wxIcon(schedule_xpm));

        // slony cluster, node, path, listen, set, subscription
        imageList->Add(wxIcon(slcluster_xpm));
        imageList->Add(wxIcon(slnode_xpm));
        imageList->Add(wxIcon(slpath_xpm));
        imageList->Add(wxIcon(sllisten_xpm));
        imageList->Add(wxIcon(slset_xpm));
        imageList->Add(wxIcon(slset2_xpm));
        imageList->Add(wxIcon(slsubscription_xpm));
        imageList->Add(wxIcon(slsubscription2_xpm));
    }
    return imageList->Add(wxIcon(img));
}


void pgaFactory::RegisterMenu(wxWindow *w, wxObjectEventFunction func)
{
    w->Connect(GetFactory(FACTORY_OFFSET)->GetId() + MNU_NEW, 
        GetFactory(factoryArray->GetCount()+FACTORY_OFFSET-1)->GetId() + MNU_NEW,
        wxEVT_COMMAND_MENU_SELECTED, func);
}


void pgaFactory::AppendMenu(wxMenu *menu)
{
    if (menu)
    {
        menu->Append(MNU_NEW+GetId(), 
            wxGetTranslation(GetNewString()),
            wxGetTranslation(GetNewLongString()));
    }
}


int pgaFactory::GetMetaType()
{
    if (IsCollection())
        return ((pgaCollectionFactory*)this)->GetItemFactory()->GetMetaType();
    return metaType;
}


pgaCollectionFactory::pgaCollectionFactory(pgaFactory *f, wxChar *tn, char **img) 
: pgaFactory(tn, f->GetNewString(), f->GetNewLongString())
{
    itemFactory=f;
    f->collectionFactory = this;
    if (img)
    {
        image=img;
        iconId=addImage(image);
    }
    else
        iconId=f->GetIconId();
}


pgObject *pgaCollectionFactory::CreateObjects(pgCollection  *obj, wxTreeCtrl *browser, const wxString &restr)
{
    if (itemFactory)
        return itemFactory->CreateObjects(obj, browser, restr);
    return 0;
}


dlgProperty *pgaCollectionFactory::CreateDialog(frmMain *frame, pgObject *node, pgObject *parent)
{
    if (itemFactory)
        return itemFactory->CreateDialog(frame, node, parent);
    return 0;
}


////////////////////////////////////////////////

wxArrayPtrVoid *actionFactoryArray=0;


actionFactory::actionFactory()
{
    if (!actionFactoryArray)
        actionFactoryArray = new wxArrayPtrVoid;

    id = actionFactoryArray->GetCount()+MNU_ACTION;
    actionFactoryArray->Add(this);
}


actionFactory *actionFactory::GetFactory(int id)
{
    id -= MNU_ACTION;
    if (id >= 0 && id < (int)actionFactoryArray->GetCount())
        return (actionFactory*)actionFactoryArray->Item(id);
    return 0;
}


void actionFactory::RegisterMenu(wxWindow *w, wxObjectEventFunction func)
{
    w->Connect(MNU_ACTION, MNU_ACTION+actionFactoryArray->GetCount()-1, 
        wxEVT_COMMAND_MENU_SELECTED, func);
}


void actionFactory::CheckMenu(pgObject *obj, wxMenuBar *menubar, wxToolBar *toolbar)
{
    size_t id;
    for (id=MNU_ACTION ; id < actionFactoryArray->GetCount()+MNU_ACTION ; id++)
    {
        bool how=GetFactory(id)->CheckEnable(obj);
        menubar->Enable(id, how);
        if (toolbar)
            toolbar->EnableTool(id, how);
    }
}


void actionFactory::AppendEnabledMenus(wxMenuBar *menuBar, wxMenu *treeContextMenu)
{
    size_t id;
    for (id=MNU_ACTION ; id < actionFactoryArray->GetCount()+MNU_ACTION ; id++)
    {
        if (menuBar->IsEnabled(id))
        {
            wxMenuItem *menuItem=menuBar->FindItem(id);
            if (menuItem)
                treeContextMenu->Append(id, menuItem->GetLabel(), menuItem->GetHelp());
        }
    }
}