//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2005, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// dlgServer.h - Server property 
//
//////////////////////////////////////////////////////////////////////////


#ifndef __DLG_SERVERPROP
#define __DLG_SERVERPROP

#include "dlgProperty.h"

class pgServer;

class dlgServer : public dlgProperty
{
public:
    dlgServer(pgaFactory *factory, frmMain *frame, pgServer *s);
    ~dlgServer();
    int Go(bool modal);
    int GoNew();

    void CheckChange();
    wxString GetSql();
    pgObject *CreateObject(pgCollection *collection);
    pgObject *GetObject();
    wxString GetHelpPage() const;

    wxString GetPassword();
    bool GetTryConnect();

private:
    pgServer *server;
    bool dbRestrictionOk;

    void OnOK(wxCommandEvent &ev);
    void OnChangeRestr(wxCommandEvent &ev);
    void OnChangeTryConnect(wxCommandEvent &ev);
    void OnPageSelect(wxNotebookEvent &event);

    DECLARE_EVENT_TABLE();
};


class addServerFactory : public actionFactory
{
public:
    addServerFactory(menuFactoryList *list, wxMenu *mnu, wxToolBar *toolbar);
    wxWindow *StartDialog(frmMain *form, pgObject *obj);
};


class startServiceFactory : public contextActionFactory
{
public:
    startServiceFactory (menuFactoryList *list, wxMenu *mnu, wxToolBar *toolbar);
    wxWindow *StartDialog(frmMain *form, pgObject *obj);
    bool CheckEnable(pgObject *obj);
};

class stopServiceFactory : public contextActionFactory
{
public:
    stopServiceFactory (menuFactoryList *list, wxMenu *mnu, wxToolBar *toolbar);
    wxWindow *StartDialog(frmMain *form, pgObject *obj);
    bool CheckEnable(pgObject *obj);
};


class connectServerFactory : public contextActionFactory
{
public:
    connectServerFactory(menuFactoryList *list, wxMenu *mnu, wxToolBar *toolbar);
    wxWindow *StartDialog(frmMain *form, pgObject *obj);
    bool CheckEnable(pgObject *obj);
};


class disconnectServerFactory : public contextActionFactory
{
public:
    disconnectServerFactory(menuFactoryList *list, wxMenu *mnu, wxToolBar *toolbar);
    wxWindow *StartDialog(frmMain *form, pgObject *obj);
    bool CheckEnable(pgObject *obj);
};

#endif
