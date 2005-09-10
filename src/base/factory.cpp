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

pgaFactory::pgaFactory(const wxChar *tn, const wxChar *ns, const wxChar *nls, char **img, char **smImg)
{
    if (!factoryArray)
        factoryArray = new wxArrayPtrVoid;
    id=factoryArray->GetCount()+FACTORY_OFFSET;
    factoryArray->Add(this);
    collectionFactory=0;
    smallIconId=-1;
    typeName=(wxChar*)tn;
    if (ns)
        newString=(wxChar*)ns;
    else
        newString=typeName;
    if (nls)
        newLongString=(wxChar*)nls;
    else
        newLongString=newString;
    metaType=PGM_UNKNOWN;
    image=img;
    if (image)
    {
        iconId = addIcon(image);
        if (smImg)
            smallIconId = addIcon(smImg);
    }
    else
        iconId=-1;
}


bool pgaFactory::WantSmallIcon()
{
#ifdef __WXMSW__
    return true;
#else
    return false;
#endif
}

int pgaFactory::GetIconId()
{
    if (WantSmallIcon() && smallIconId >= 0)
        return smallIconId;

    return iconId;
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


#include "images/property.xpm"
#include "images/public.xpm"
#include "images/statistics.xpm"


wxArrayPtrVoid *deferredImagesArray=0;

int pgaFactory::addIcon(char **img)
{
    if (!imageList)
    {
        if (!deferredImagesArray)
        {
            //Setup the global imagelist
            deferredImagesArray = new wxArrayPtrVoid;

            deferredImagesArray->Add(property_xpm);
            deferredImagesArray->Add(statistics_xpm);
            deferredImagesArray->Add(public_xpm);
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
    if (menu && GetNewString())
    {
        wxMenuItem *item=menu->Append(MNU_NEW+GetId(), GetNewString(), GetNewLongString());
        if (image)
        {
            (void)item;
            // doesn't work?
            // item->SetBitmap(wxBitmap(image));
        }
    }
}


int pgaFactory::GetMetaType()
{
    if (IsCollection())
        return ((pgaCollectionFactory*)this)->GetItemFactory()->GetMetaType();
    return metaType;
}


pgaCollectionFactory::pgaCollectionFactory(pgaFactory *f, wxChar *tn, char **img, char **imgSm) 
: pgaFactory(tn, f->GetNewString(), f->GetNewLongString())
{
    itemFactory=f;
    f->collectionFactory = this;
    if (img)
    {
        image=img;
        iconId=addIcon(image);
        if (imgSm)
            smallIconId = addIcon(imgSm);
    }
    else
        iconId=f->GetIconId();
}


pgObject *pgaCollectionFactory::CreateObjects(pgCollection  *obj, ctlTree *browser, const wxString &restr)
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


menuFactoryList::~menuFactoryList()
{
    while (GetCount())
    {
        delete (actionFactory*)Item(0);
        RemoveAt(0);
    }
}

actionFactory *menuFactoryList::GetFactory(int id, bool actionOnly)
{
    id -= MNU_ACTION;
    if (id >= 0 && id < (int)GetCount())
    {
        actionFactory *f=(actionFactory*)Item(id);
        if (f->IsAction() || !actionOnly) 
            return f;
    }
    return 0;
}


void menuFactoryList::RegisterMenu(wxWindow *w, wxObjectEventFunction func)
{
    w->Connect(MNU_ACTION, MNU_ACTION+GetCount()-1, 
        wxEVT_COMMAND_MENU_SELECTED, func);
}


void menuFactoryList::CheckMenu(pgObject *obj, wxMenuBar *menubar, wxToolBar *toolbar)
{
    size_t id;
    for (id=MNU_ACTION ; id < GetCount()+MNU_ACTION ; id++)
    {
        actionFactory *f=GetFactory(id);
        if (f)
        {
            bool how=f->CheckEnable(obj);
            if (menubar->FindItem(id))
                menubar->Enable(id, how);
            if (toolbar)
                toolbar->EnableTool(id, how);
        }
    }
}


void menuFactoryList::AppendEnabledMenus(wxMenuBar *menuBar, wxMenu *treeContextMenu)
{
    size_t id;
    wxMenuItem *lastItem=0;
    for (id=MNU_ACTION ; id < GetCount()+MNU_ACTION ; id++)
    {
        actionFactory *f=GetFactory(id, false);
        if (f->IsAction())
        {
            if (f->GetContext())
            {
                wxMenuItem *menuItem=menuBar->FindItem(id);
                if (menuItem && menuItem->IsEnabled())
                    lastItem = treeContextMenu->Append(id, menuItem->GetLabel(), menuItem->GetHelp());
            }
        }
        else
        {
            if (lastItem && lastItem->GetId() >= 0)
                lastItem = treeContextMenu->AppendSeparator();
        }
    }

    if (lastItem && lastItem->GetId() < 0)
    {
        treeContextMenu->Remove(lastItem);
        delete lastItem;
    }
}


menuFactory::menuFactory(menuFactoryList *list)
{
    if (list)
        list->Add(this);
}


actionFactory::actionFactory(menuFactoryList *list) : menuFactory(list)
{
    if (list)
        id = list->GetCount()+MNU_ACTION -1;
	else
        id=0;
    context=false;
}


