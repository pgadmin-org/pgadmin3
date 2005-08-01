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
    dlgServer(frmMain *frame, pgServer *s);
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

    void OnOK(wxCommandEvent &ev);
    void OnChangeTryConnect(wxCommandEvent &ev);
    void OnPageSelect(wxNotebookEvent &event);

    DECLARE_EVENT_TABLE();
};


#endif
