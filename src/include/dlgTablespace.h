//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2005, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// dlgTablespace.h - Tablespace property 
//
//////////////////////////////////////////////////////////////////////////


#ifndef __DLG_TABLESPACEPROP
#define __DLG_TABLESPACEPROP

#include "dlgProperty.h"

class pgTablespace;

class dlgTablespace : public dlgSecurityProperty
{
public:
    dlgTablespace(pgaFactory *factory, frmMain *frame, pgTablespace *node=0);

    void CheckChange();
    wxString GetSql();
    pgObject *CreateObject(pgCollection *collection);
    pgObject *GetObject();

    int Go(bool modal);

private:
    pgTablespace *tablespace;

    DECLARE_EVENT_TABLE();
};


#endif
