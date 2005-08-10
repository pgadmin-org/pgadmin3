//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id:  $
// Copyright (C) 2002 - 2005, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// factory.h - Object classes factory
//
//////////////////////////////////////////////////////////////////////////

#ifndef FACTORY_H
#define FACTORY_H

// wxWindows headers
#include <wx/wx.h>

#ifdef WIN32
#pragma warning(disable:4183)
#endif

class pgObject;
class frmMain;
class dlgProperty;
class wxTreeCtrl;
class pgCollection;
class pgSchema;
class pgaCollectionFactory;


class pgaFactory
{
public:
    virtual dlgProperty *CreateDialog(frmMain *frame, pgObject *node, pgObject *parent)=0;
    virtual pgObject *CreateObjects(pgCollection  *obj, wxTreeCtrl *browser, const wxString &restr=wxEmptyString) { return 0; }
    virtual bool IsCollection() { return false; }
    virtual void AppendMenu(wxMenu *menu);
    bool IsCollectionFor(pgaFactory &f) { return f.GetCollectionFactory() == (pgaCollectionFactory*)this; }

    static pgaFactory *GetFactory(int id);
    static pgaFactory *GetFactory(const wxString &name);
    int GetId() { return id; }
    int GetMetaType();
    wxChar *GetTypeName() { return typeName; }
    wxChar *GetNewString() { return newString; }
    wxChar *GetNewLongString() { return newLongString; }
    pgaCollectionFactory *GetCollectionFactory() { return collectionFactory; }

    virtual int GetIconId() { return iconId; }
    static void RegisterMenu(wxWindow *w, wxObjectEventFunction func);
    static void RealizeImages();
    char **GetImage() const { return image; }

protected:
    pgaFactory(wxChar *tn=0, wxChar *ns=0, wxChar *nls=0, char **img=0);

    int addImage(char **img);

    int id, metaType;
    wxChar *typeName;
    wxChar *newString, *newLongString;
    int iconId;
    char **image;

    pgaCollectionFactory *collectionFactory;
    friend class pgaCollectionFactory;
};


class pgaCollectionFactory : public pgaFactory
{
public:
    pgaCollectionFactory(pgaFactory *f, wxChar *tn=0, char **img=0);
    wxChar *GetItemTypeName() { return itemFactory->GetTypeName(); }
    pgaFactory *GetItemFactory() { return itemFactory; }
    pgObject *CreateObjects(pgCollection  *obj, wxTreeCtrl *browser, const wxString &restr=wxEmptyString);
    
protected:
    virtual bool IsCollection() { return true; }
    dlgProperty *CreateDialog(frmMain *frame, pgObject *node, pgObject *parent);

    pgaFactory *itemFactory;
};


class menuFactory
{
public:
    virtual bool IsAction() { return false; }

protected:
    menuFactory() {}
};


class actionFactory : public menuFactory
{
public:
    virtual bool IsAction() { return true; }
    virtual wxWindow *StartDialog(pgFrame *form, pgObject *obj)=0;
    virtual bool CheckEnable(pgObject *obj) { return true; }
    int GetId() { return id; }
    bool GetContext() { return context; }


    static void CheckMenu(pgObject *obj, wxMenuBar *menubar, wxToolBar *toolbar);
    static void AppendEnabledMenus(wxMenuBar *menuBar, wxMenu *treeContextMenu);
    static actionFactory *GetFactory(int id, bool actionOnly=true);
    static void RegisterMenu(wxWindow *w, wxObjectEventFunction func);

protected:
    actionFactory();

    int id;
    bool context;
};


class contextActionFactory : public actionFactory
{
protected:
    contextActionFactory() { context=true; }
};

class separatorFactory : public menuFactory
{
public:
    separatorFactory() {}
};
#endif
