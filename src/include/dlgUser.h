//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
//
// dlgUser.h - User property 
//
//////////////////////////////////////////////////////////////////////////


#ifndef __DLG_USERPROP
#define __DLG_USERPROP

#include "dlgProperty.h"
#include "pgUser.h"

class dlgUser : public dlgProperty
{
public:
    dlgUser(wxFrame *frame, pgUser *node=0);
    wxString GetSql();
    pgObject *CreateObject(pgCollection *collection);
    pgObject *GetObject() { return user; }

    void Go();

private:
    pgUser *user;
    void OnChange(wxNotifyEvent &ev);
    void OnGroupAdd(wxNotifyEvent &ev);
    void OnGroupRemove(wxNotifyEvent &ev);

    void OnVarAdd(wxNotifyEvent &ev);
    void OnVarRemove(wxNotifyEvent &ev);
    void OnVarSelChange(wxListEvent &ev);

    wxArrayString groupsIn;

    DECLARE_EVENT_TABLE();
};


#endif
