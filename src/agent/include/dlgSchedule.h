//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2004, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// dlgSchedule.h - Job property
//
//////////////////////////////////////////////////////////////////////////


#ifndef __DLG_SCHEDULEPROP
#define __DLG_SCHEDULEPROP

#include "dlgProperty.h"
#include "timespin.h"
#include "calbox.h"

class pgaSchedule;
class pgaJob;

class dlgSchedule : public dlgOidProperty
{
public:
    dlgSchedule(frmMain *frame, pgaSchedule *s, pgaJob *j);
    int Go(bool modal);

    wxString GetComment();
    wxString GetUpdateSql();
    wxString GetInsertSql();
    pgObject *CreateObject(pgCollection *collection);
    pgObject *GetObject();
    void SetJobOid(OID oid) { jobOid = oid; }

private:
    OID jobOid;
    pgaSchedule *schedule;
    pgaJob *job;
    void OnChange(wxCommandEvent &ev);
    void OnChangeCal(wxCalendarEvent &ev);
    void OnChangeSpin(wxSpinEvent &ev);
    void OnChangeKind(wxCommandEvent &ev);
    void OnSelChangeInterval(wxListEvent &ev);
    void OnAddInterval(wxCommandEvent &ev);
    void OnChangeInterval(wxCommandEvent &ev);
    void OnRemoveInterval(wxCommandEvent &ev);

    DECLARE_EVENT_TABLE();
};


#endif
