//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2004, The pgAdmin Development Team
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
    dlgTablespace(frmMain *frame, pgTablespace *node=0);
    wxString GetSql();
    pgObject *CreateObject(pgCollection *collection);
    pgObject *GetObject();

    int Go(bool modal);

private:
    pgTablespace *tablespace;

    void OnChange(wxCommandEvent &ev);
    void OnOwnerChange(wxCommandEvent &ev);
    DECLARE_EVENT_TABLE();
};


#endif
