//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2004, The pgAdmin Development Team
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
    void OnChange(wxCommandEvent &ev);
    void OnUserAdd(wxCommandEvent &ev);
    void OnUserRemove(wxCommandEvent &ev);

    wxArrayString usersIn;

    DECLARE_EVENT_TABLE();
};


#endif
