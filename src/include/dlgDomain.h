//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
//
// dlgDomain.h - Domain property 
//
//////////////////////////////////////////////////////////////////////////


#ifndef __DLG_DOMAINPROP
#define __DLG_DOMAINPROP

#include "dlgProperty.h"

class pgDomain;

class dlgDomain : public dlgProperty
{
public:
    dlgDomain(frmMain *frame, pgDomain *db);
    int Go(bool modal);

    wxString GetSql();
    pgObject *CreateObject(pgCollection *collection);
    pgObject *GetObject();

private:
    void OnChange(wxNotifyEvent &ev);
    void OnSelChangeTyp(wxNotifyEvent &ev);

    pgDomain *domain;
    wxArrayString typmods;
    bool isVarLen, isVarPrec;

    DECLARE_EVENT_TABLE();
};


#endif
