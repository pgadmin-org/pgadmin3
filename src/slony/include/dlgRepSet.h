//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2005, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// dlgRepSet.h - Slony-I Set property
//
//////////////////////////////////////////////////////////////////////////


#ifndef __DLG_REPSETPROP
#define __DLG_REPSETPROP

#include "dlgRepProperty.h"

class slCluster;
class slSet;

class dlgRepSet : public dlgRepProperty
{
public:
    dlgRepSet(pgaFactory *factory, frmMain *frame, slSet *set, slCluster *c);
    int Go(bool modal);
    wxString GetHelpPage() const { return wxT("slony-set"); }

    void CheckChange();
    wxString GetSql();
    pgObject *CreateObject(pgCollection *collection);
    pgObject *GetObject() { return (pgObject*)set; }

private:
    slSet *set;

    DECLARE_EVENT_TABLE();
};


class dlgRepSetMerge : public dlgRepProperty
{
public:
    dlgRepSetMerge(pgaFactory *factory, frmMain *frame, slSet *set);
    int Go(bool modal);
    wxString GetHelpPage() const { return wxT("slony-functions"); }

    void CheckChange();
    wxString GetSql();
    pgObject *GetObject() { return (pgObject*)set; }
    pgObject *CreateObject(pgCollection *collection) { return 0; }

private:
    slSet *set;

    DECLARE_EVENT_TABLE();
};

class dlgRepSetMove : public dlgRepProperty
{
public:
    dlgRepSetMove(pgaFactory *f, frmMain *frame, slSet *set);
    int Go(bool modal);
    wxString GetHelpPage() const { return wxT("slony-functions"); }

    void CheckChange();
    wxString GetSql();
    pgObject *GetObject() { return (pgObject*)set; }
    pgObject *CreateObject(pgCollection *collection) { return 0; }

private:
    slSet *set;

    DECLARE_EVENT_TABLE();
};


class slonyMoveSetFactory : public contextActionFactory
{
public:
    slonyMoveSetFactory(menuFactoryList *list, wxMenu *mnu, wxToolBar *toolbar);
    wxWindow *StartDialog(frmMain *form, pgObject *obj);
    bool CheckEnable(pgObject *obj);
};


class slonyMergeSetFactory : public contextActionFactory
{
public:
    slonyMergeSetFactory(menuFactoryList *list, wxMenu *mnu, wxToolBar *toolbar);
    wxWindow *StartDialog(frmMain *form, pgObject *obj);
    bool CheckEnable(pgObject *obj);
};


class slonyLockSetFactory : public contextActionFactory
{
public:
    slonyLockSetFactory(menuFactoryList *list, wxMenu *mnu, wxToolBar *toolbar);
    wxWindow *StartDialog(frmMain *form, pgObject *obj);
    bool CheckEnable(pgObject *obj);
};


class slonyUnlockSetFactory : public contextActionFactory
{
public:
    slonyUnlockSetFactory(menuFactoryList *list, wxMenu *mnu, wxToolBar *toolbar);
    wxWindow *StartDialog(frmMain *form, pgObject *obj);
    bool CheckEnable(pgObject *obj);
};

#endif
