//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002 - 2003, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// dlgCast.h - Cast property 
//
//////////////////////////////////////////////////////////////////////////


#ifndef __DLG_CASTPROP
#define __DLG_CASTPROP

#include "dlgProperty.h"

class pgSchema;
class pgCast;

class dlgCast : public dlgTypeProperty
{
public:
    dlgCast(frmMain *frame, pgCast *ca);
    int Go(bool modal);

    wxString GetSql();
    pgObject *CreateObject(pgCollection *collection);
    pgObject *GetObject();

private:
    void OnChange(wxNotifyEvent &ev);
    void OnChangeType(wxNotifyEvent &ev);

    pgCast *cast;
    wxArrayString functions;

    DECLARE_EVENT_TABLE();
};


#endif
