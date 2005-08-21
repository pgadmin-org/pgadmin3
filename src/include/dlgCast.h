//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2005, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// dlgCast.h - Cast property 
//
//////////////////////////////////////////////////////////////////////////


#ifndef __DLG_CASTPROP
#define __DLG_CASTPROP

#include "dlgProperty.h"

class pgCast;

class dlgCast : public dlgTypeProperty
{
public:
    dlgCast(pgaFactory *factory, frmMain *frame, pgCast *ca);
    int Go(bool modal);

    void CheckChange();
    wxString GetSql();
    pgObject *CreateObject(pgCollection *collection);
    pgObject *GetObject();

private:
    void OnChangeType(wxCommandEvent &ev);
    void OnChangeTypeSrc(wxCommandEvent &ev);
    void OnChangeTypeTrg(wxCommandEvent &ev);

    pgCast *cast;
    wxArrayString functions;

    DECLARE_EVENT_TABLE();
};


#endif
