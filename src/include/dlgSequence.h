//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2003, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// dlgSequence.h - Sequence property 
//
//////////////////////////////////////////////////////////////////////////


#ifndef __DLG_SEQUENCEPROP
#define __DLG_SEQUENCEPROP

#include "dlgProperty.h"

class pgSchema;
class pgSequence;

class dlgSequence : public dlgSecurityProperty
{
public:
    dlgSequence(frmMain *frame, pgSequence *seq, pgSchema *sch);
    int Go(bool modal);

    wxString GetSql();
    pgObject *CreateObject(pgCollection *collection);
    pgObject *GetObject();

private:
    pgSchema *schema;
    pgSequence *sequence;

    void OnChange(wxNotifyEvent &ev);

    DECLARE_EVENT_TABLE();
};


#endif
