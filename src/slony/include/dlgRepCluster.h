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

class dlgRepCluster : public dlgProperty
{
public:
    dlgRepCluster(frmMain *frame, slCluster *cl, pgDatabase *obj);
    ~dlgRepCluster();
    int Go(bool modal);

    void CheckChange();
    wxString GetSql();
    pgObject *CreateObject(pgCollection *collection);
    pgObject *GetObject();

private:
    void OnOK(wxCommandEvent &ev);
    void OnChangeJoin(wxCommandEvent &ev);
    void OnChangeServer(wxCommandEvent &ev);
    void OnChangeDatabase(wxCommandEvent &ev);
    void OnChangeCluster(wxCommandEvent &ev);
    void OnEndProcess(wxProcessEvent& event);

    bool CopyTable(pgConn *from, pgConn *to, const wxString &table);
    slCluster *cluster;
    sysProcess *process;

    wxArrayLong usedNodes;
    wxTreeItemId servers;
    wxString clusterBackup;

    pgServer *remoteServer;
    pgConn *remoteConn;

    DECLARE_EVENT_TABLE();
};


#endif
