//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id: dlgUser.h 4875 2006-01-06 21:06:46Z dpage $
// Copyright (C) 2002 - 2006, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// dlgUser.h - User property 
//
//////////////////////////////////////////////////////////////////////////


#ifndef __DLG_USERPROP
#define __DLG_USERPROP

#include "dlgProperty.h"
#include "ctl/calbox.h"
#include "ctl/timespin.h"

class pgUser;

class dlgUser : public dlgProperty
{
public:
    dlgUser(pgaFactory *factory, frmMain *frame, pgUser *node=0);

    void CheckChange();
    wxString GetSql();
    pgObject *CreateObject(pgCollection *collection);
    pgObject *GetObject();
    wxString GetHelpPage() const;

    int Go(bool modal);

private:
    pgUser *user;
    wxArrayString varInfo;

    void OnChangeSuperuser(wxCommandEvent &ev);
    void OnChangeSpin(wxSpinEvent &ev);
    void OnChangeCal(wxCalendarEvent &ev);
    void OnChangeDate(wxDateEvent &ev);
    void OnGroupAdd(wxCommandEvent &ev);
    void OnGroupRemove(wxCommandEvent &ev);

    void OnVarAdd(wxCommandEvent &ev);
    void OnVarRemove(wxCommandEvent &ev);
    void OnVarSelChange(wxListEvent &ev);

    void OnVarnameSelChange(wxCommandEvent &ev);
    void OnChangePasswd(wxCommandEvent &ev);

    wxArrayString groupsIn;

    DECLARE_EVENT_TABLE();
};


#endif
