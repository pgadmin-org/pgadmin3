//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2005, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// dlgRepCluster.h - Slony-I cluster property
//
//////////////////////////////////////////////////////////////////////////


#ifndef __DLG_REPCLUSTERPROP
#define __DLG_REPCLUSTERPROP

#include "dlgProperty.h"


class slCluster;
class sysProcess;
class wxProcessEvent;

class dlgRepClusterBase : public dlgProperty
{
public:
    dlgRepClusterBase(pgaFactory *factory, frmMain *frame, const wxString &dlgName, slCluster *cl, pgDatabase *obj);
    ~dlgRepClusterBase();
    pgObject *GetObject();
    int Go(bool modal);

private:
    virtual void OnChangeCluster(wxCommandEvent &ev)=0;

protected:
    void OnChangeServer(wxCommandEvent &ev);
    void OnChangeDatabase(wxCommandEvent &ev);

    bool AddScript(wxString &sql, const wxString &filename);
    slCluster *cluster;
    wxTreeItemId servers;
    pgServer *remoteServer;
    pgConn *remoteConn;
    wxString remoteVersion;
    wxString createScript;

    DECLARE_EVENT_TABLE();
};


class dlgRepCluster : public dlgRepClusterBase
{
public:
    dlgRepCluster(pgaFactory *factory, frmMain *frame, slCluster *cl, pgDatabase *obj);
    int Go(bool modal);

    void CheckChange();
    wxString GetSql();
    pgObject *CreateObject(pgCollection *collection);

private:
    void OnOK(wxCommandEvent &ev);
    void OnChangeJoin(wxCommandEvent &ev);
    void OnChangeCluster(wxCommandEvent &ev);
    void OnEndProcess(wxProcessEvent& event);

    bool CopyTable(pgConn *from, pgConn *to, const wxString &table);
    sysProcess *process;

    wxArrayLong usedNodes;
    wxString clusterBackup;


    DECLARE_EVENT_TABLE();
};



class dlgRepClusterUpgrade : public dlgRepClusterBase
{
public:
    dlgRepClusterUpgrade(pgaFactory *factory, frmMain *frame, slCluster *cl);
    int Go(bool modal);

    void CheckChange();
    wxString GetSql();
    pgObject *CreateObject(pgCollection *collection);

private:
    void OnChangeCluster(wxCommandEvent &ev);

    wxString version;
    wxString sql;
    DECLARE_EVENT_TABLE();
};


class clusterActionFactory : public contextActionFactory
{
public:
    clusterActionFactory(menuFactoryList *list) : contextActionFactory(list) {}
    bool CheckEnable(pgObject *obj);
};


class slonyRestartFactory : public clusterActionFactory
{
public:
    slonyRestartFactory(menuFactoryList *list, wxMenu *mnu, wxToolBar *toolbar);
    wxWindow *StartDialog(frmMain *form, pgObject *obj);
};


class slonyUpgradeFactory : public clusterActionFactory
{
public:
    slonyUpgradeFactory(menuFactoryList *list, wxMenu *mnu, wxToolBar *toolbar);
    wxWindow *StartDialog(frmMain *form, pgObject *obj);
};


class slonyFailoverFactory : public clusterActionFactory
{
public:
    slonyFailoverFactory(menuFactoryList *list, wxMenu *mnu, wxToolBar *toolbar);
    wxWindow *StartDialog(frmMain *form, pgObject *obj);
};

#endif
