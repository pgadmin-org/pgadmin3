//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2003, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// dlgStep.h - Job property
//
//////////////////////////////////////////////////////////////////////////


#ifndef __DLG_STEPPROP
#define __DLG_STEPPROP

#include "dlgProperty.h"

class pgaStep;
class pgaJob;

class dlgStep : public dlgOidProperty
{
public:
    dlgStep(frmMain *frame, pgaStep *s, pgaJob *j);
    int Go(bool modal);

    wxString GetUpdateSql();
    wxString GetInsertSql();
    pgObject *CreateObject(pgCollection *collection);
    pgObject *GetObject();
    void SetJobOid(OID oid) { jobOid = oid; }

private:
    OID jobOid;
    ctlSQLBox *sqlBox;
    pgaStep *step;
    pgaJob *job;
    void OnChange(wxNotifyEvent &ev);

    DECLARE_EVENT_TABLE();
};


#endif
