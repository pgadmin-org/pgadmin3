//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2005, The pgAdmin Development Team
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

class dlgSchedule : public dlgAgentProperty
{
public:
    dlgSchedule(frmMain *frame, pgaSchedule *s, pgaJob *j);

    void CheckChange();
    int Go(bool modal);

    wxString GetComment();
    wxString GetUpdateSql();
    wxString GetInsertSql();
    pgObject *CreateObject(pgCollection *collection);
    pgObject *GetObject();
    void SetJobId(long id) { jobId = id; }

private:
    long jobId;
    pgaSchedule *schedule;
    pgaJob *job;
    
    void OnChangeCal(wxCalendarEvent &ev);
    void OnChangeSpin(wxSpinEvent &ev);
    void OnSelChangeException(wxListEvent &ev);
    void OnAddException(wxCommandEvent &ev);
    void OnChangeException(wxCommandEvent &ev);
    void OnRemoveException(wxCommandEvent &ev);
	const wxString ChkListBox2PgArray(wxCheckListBox *lb);
	const wxString ChkListBox2StrArray(wxCheckListBox *lb);

    DECLARE_EVENT_TABLE();
};


#endif
