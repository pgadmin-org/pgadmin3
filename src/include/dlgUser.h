//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2003, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// dlgUser.h - User property 
//
//////////////////////////////////////////////////////////////////////////


#ifndef __DLG_USERPROP
#define __DLG_USERPROP

#include "dlgProperty.h"

class pgUser;

class dlgUser : public dlgProperty
{
public:
    dlgUser(frmMain *frame, pgUser *node=0);
    wxString GetSql();
    pgObject *CreateObject(pgCollection *collection);
    pgObject *GetObject();

    int Go(bool modal);

private:
    pgUser *user;
    wxArrayString varInfo;

    void OnChange(wxNotifyEvent &ev);
    void OnGroupAdd(wxNotifyEvent &ev);
    void OnGroupRemove(wxNotifyEvent &ev);

    void OnVarAdd(wxNotifyEvent &ev);
    void OnVarRemove(wxNotifyEvent &ev);
    void OnVarSelChange(wxListEvent &ev);

    void OnVarnameSelChange(wxNotifyEvent &ev);

    wxArrayString groupsIn;

    DECLARE_EVENT_TABLE();
};


#endif
