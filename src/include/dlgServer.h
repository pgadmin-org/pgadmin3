//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2004, The pgAdmin Development Team
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
    int Go(bool modal);

    void CheckChange();
    wxString GetSql();
    pgObject *CreateObject(pgCollection *collection);
    pgObject *GetObject();
    wxString GetHelpPage() const;

private:
    pgServer *server;

    void OnOK(wxCommandEvent &ev);
    void OnPageSelect(wxNotebookEvent &event);

    DECLARE_EVENT_TABLE();
};


#endif
