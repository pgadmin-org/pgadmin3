//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002 - 2003, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// dlgGroup.h - Group property 
//
//////////////////////////////////////////////////////////////////////////


#ifndef __DLG_GROUPPROP
#define __DLG_GROUPPROP

#include "dlgProperty.h"

class pgGroup;

class dlgGroup : public dlgProperty
{
public:
    dlgGroup(frmMain *frame, pgGroup *node=0);
    wxString GetSql();
    pgObject *CreateObject(pgCollection *collection);
    pgObject *GetObject();

    int Go(bool modal);

private:
    pgGroup *group;
    void OnChange(wxNotifyEvent &ev);
    void OnUserAdd(wxNotifyEvent &ev);
    void OnUserRemove(wxNotifyEvent &ev);

    wxArrayString usersIn;

    DECLARE_EVENT_TABLE();
};


#endif
