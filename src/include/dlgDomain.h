//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002 - 2003, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// dlgDomain.h - Domain property 
//
//////////////////////////////////////////////////////////////////////////


#ifndef __DLG_DOMAINPROP
#define __DLG_DOMAINPROP

#include "dlgProperty.h"

class pgSchema;
class pgDomain;

class dlgDomain : public dlgTypeProperty
{
public:
    dlgDomain(frmMain *frame, pgDomain *dom, pgSchema *sch);
    int Go(bool modal);

    wxString GetSql();
    pgObject *CreateObject(pgCollection *collection);
    pgObject *GetObject();

private:
    void OnChange(wxNotifyEvent &ev);
    void OnSelChangeTyp(wxNotifyEvent &ev);

    pgSchema *schema;
    pgDomain *domain;

    DECLARE_EVENT_TABLE();
};


#endif
