//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2006, The pgAdmin Development Team
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
    dlgSequence(pgaFactory *factory, frmMain *frame, pgSequence *seq, pgSchema *sch);
    int Go(bool modal);

    void CheckChange();
    wxString GetSql();
    pgObject *CreateObject(pgCollection *collection);
    pgObject *GetObject();

private:
    pgSchema *schema;
    pgSequence *sequence;

    DECLARE_EVENT_TABLE()
};


#endif
