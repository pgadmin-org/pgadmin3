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
        return (pgaFactory*)factoryArray->Item(id);;

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


#include "images/check.xpm"
#include "images/column.xpm"
#include "images/index.xpm"
#include "images/foreignkey.xpm"
#include "images/property.xpm"
#include "images/public.xpm"
#include "images/rule.xpm"
#include "images/server.xpm"
#include "images/servers.xpm"
#include "images/serverbad.xpm"
#include "images/statistics.xpm"
#include "images/trigger.xpm"
#include "images/constraints.xpm"
#include "images/primarykey.xpm"
#include "images/unique.xpm"
#include "images/job.xpm"
#include "images/jobdisabled.xpm"
#include "images/step.xpm"
#include "images/schedule.xpm"
#include "images/slnode.xpm"
#include "images/slpath.xpm"
#include "images/sllisten.xpm"
#include "images/slset.xpm"
#include "images/slset2.xpm"
#include "images/slsubscription.xpm"
#include "images/slsubscription2.xpm"


wxArrayPtrVoid *deferredImagesArray=0;

int pgaFactory::addImage(char **img)
{
    if (!imageList)
    {
        if (!deferredImagesArray)
        {
            deferredImagesArray = new wxArrayPtrVoid;
            //Setup the global imagelist
            deferredImagesArray->Add(property_xpm);
            deferredImagesArray->Add(statistics_xpm);
            deferredImagesArray->Add(servers_xpm);
            deferredImagesArray->Add(server_xpm);
            deferredImagesArray->Add(serverbad_xpm);
            deferredImagesArray->Add(check_xpm);
            deferredImagesArray->Add(column_xpm);
            deferredImagesArray->Add(index_xpm);
            deferredImagesArray->Add(rule_xpm);
            deferredImagesArray->Add(trigger_xpm);
            deferredImagesArray->Add(foreignkey_xpm);
            deferredImagesArray->Add(constraints_xpm);
            deferredImagesArray->Add(primarykey_xpm);
            deferredImagesArray->Add(unique_xpm);
            deferredImagesArray->Add(public_xpm);

            // job, jobdisabled, step, schedule
            deferredImagesArray->Add(job_xpm);
            deferredImagesArray->Add(jobdisabled_xpm);
            deferredImagesArray->Add(step_xpm);
            deferredImagesArray->Add(schedule_xpm);

            // slony cluster, node, path, listen, set, subscription
            deferredImagesArray->Add(slnode_xpm);
            deferredImagesArray->Add(slpath_xpm);
            deferredImagesArray->Add(sllisten_xpm);
            deferredImagesArray->Add(slset_xpm);
            deferredImagesArray->Add(slset2_xpm);
            deferredImagesArray->Add(slsubscription_xpm);
            deferredImagesArray->Add(slsubscription2_xpm);
        }

        deferredImagesArray->Add(img);

        return deferredImagesArray->GetCount() -1;
    }
    else
        return imageList->Add(wxIcon(img));
}


void pgaFactory::RealizeImages()
{
    if (!imageList && deferredImagesArray)
    {
	    imageList = new wxImageList(16, 16, true, deferredImagesArray->GetCount());
        size_t i;
        for (i=0 ; i < deferredImagesArray->GetCount() ; i++)
            imageList->Add(wxIcon((char**)deferredImagesArray->Item(i)));

        delete deferredImagesArray;
        deferredImagesArray=0;
    }
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
    context=false;
    if (!actionFactoryArray)
        actionFactoryArray = new wxArrayPtrVoid;

    id = actionFactoryArray->GetCount()+MNU_ACTION;
    actionFactoryArray->Add(this);
}


actionFactory *actionFactory::GetFactory(int id, bool actionOnly)
{
    id -= MNU_ACTION;
    if (id >= 0 && id < (int)actionFactoryArray->GetCount())
    {
        actionFactory *f=(actionFactory*)actionFactoryArray->Item(id);
        if (f->IsAction() || !actionOnly) 
            return f;
    }
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
        actionFactory *f=GetFactory(id);
        if (f)
        {
            bool how=f->CheckEnable(obj);
            menubar->Enable(id, how);
            if (toolbar)
                toolbar->EnableTool(id, how);
        }
    }
}


void actionFactory::AppendEnabledMenus(wxMenuBar *menuBar, wxMenu *treeContextMenu)
{
    size_t id;
    wxMenuItem *lastItem=0;
    for (id=MNU_ACTION ; id < actionFactoryArray->GetCount()+MNU_ACTION ; id++)
    {
        actionFactory *f=GetFactory(id, false);
        if (f->IsAction())
        {
            if (f->GetContext())
            {
                if (menuBar->IsEnabled(id))
                {
                    wxMenuItem *menuItem=menuBar->FindItem(id);
                    if (menuItem)
                        lastItem = treeContextMenu->Append(id, menuItem->GetLabel(), menuItem->GetHelp());
                }
            }
        }
        else
        {
            // check here if last menu is already separator
            if (!lastItem || lastItem->GetId() >= 0)
                lastItem = treeContextMenu->AppendSeparator();
        }
    }
    lastItem = treeContextMenu->AppendSeparator();
    // check here if last menu is separator and remove it
    if (lastItem && lastItem->GetId() < 0)
    {
        treeContextMenu->Remove(lastItem);
        delete lastItem;
    }
}
