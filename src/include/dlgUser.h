//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2004, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// dlgUser.h - User property 
//
//////////////////////////////////////////////////////////////////////////


#ifndef __DLG_USERPROP
#define __DLG_USERPROP

#include "dlgProperty.h"
#include "calbox.h"
#include "timespin.h"

class pgUser;

class dlgUser : public dlgProperty
{
public:
    dlgUser(frmMain *frame, pgUser *node=0);
    wxString GetSql();
    pgObject *CreateObject(pgCollection *collection);
    pgObject *GetObject();
    wxString GetHelpPage() const;

    int Go(bool modal);

private:
    pgUser *user;
    wxArrayString varInfo;

    void OnChange(wxCommandEvent &ev);
    void OnChangeSuperuser(wxCommandEvent &ev);
    void OnChangeSpin(wxSpinEvent &ev);
    void OnChangeCal(wxCalendarEvent &ev);
    void OnGroupAdd(wxCommandEvent &ev);
    void OnGroupRemove(wxCommandEvent &ev);

    void OnVarAdd(wxCommandEvent &ev);
    void OnVarRemove(wxCommandEvent &ev);
    void OnVarSelChange(wxListEvent &ev);

    void OnVarnameSelChange(wxCommandEvent &ev);

    wxArrayString groupsIn;

    DECLARE_EVENT_TABLE();
};


#endif
