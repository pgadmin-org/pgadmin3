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
    void OnChange(wxNotifyEvent &ev);
    void OnChangeKind(wxNotifyEvent &ev);
    void OnSelChangeInterval(wxNotifyEvent &ev);
    void OnAddInterval(wxNotifyEvent &ev);
    void OnChangeInterval(wxNotifyEvent &ev);
    void OnRemoveInterval(wxNotifyEvent &ev);

    DECLARE_EVENT_TABLE();
};


#endif
