//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2005, The pgAdmin Development Team
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

class dlgStep : public dlgAgentProperty
{
public:
    dlgStep(frmMain *frame, pgaStep *s, pgaJob *j);

    void CheckChange();
    int Go(bool modal);

    wxString GetUpdateSql();
    wxString GetInsertSql();
    wxString GetComment();
    pgObject *CreateObject(pgCollection *collection);
    pgObject *GetObject();
    void SetJobId(long id) { jobId = id; }
    wxString GetHelpPage() const;

private:
    long jobId;
    ctlSQLBox *sqlBox;
    pgaStep *step;
    pgaJob *job;

    DECLARE_EVENT_TABLE();
};


#endif
