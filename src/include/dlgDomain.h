//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id: dlgDomain.h 4875 2006-01-06 21:06:46Z dpage $
// Copyright (C) 2002 - 2006, The pgAdmin Development Team
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
    dlgDomain(pgaFactory *factory, frmMain *frame, pgDomain *dom, pgSchema *sch);
    int Go(bool modal);

    void CheckChange();
    wxString GetSql();
    pgObject *CreateObject(pgCollection *collection);
    pgObject *GetObject();

private:
    void OnSelChangeTyp(wxCommandEvent &ev);

    pgSchema *schema;
    pgDomain *domain;

    DECLARE_EVENT_TABLE()
};


#endif
