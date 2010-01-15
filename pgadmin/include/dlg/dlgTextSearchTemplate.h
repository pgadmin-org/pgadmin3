//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2010, The pgAdmin Development Team
// This software is released under the BSD Licence
//
// dlgTextSearchTemplate.h - Text Search Template property 
//
//////////////////////////////////////////////////////////////////////////


#ifndef __DLG_TSTEMPLATEPROP
#define __DLG_TSTEMPLATEPROP

#include "dlg/dlgProperty.h"

class pgSchema;
class pgTextSearchTemplate;

class dlgTextSearchTemplate : public dlgTypeProperty
{
public:
    dlgTextSearchTemplate(pgaFactory *factory, frmMain *frame, pgTextSearchTemplate *cfg, pgSchema *sch);
    int Go(bool modal);

    void CheckChange();
    wxString GetSql();
    pgObject *CreateObject(pgCollection *collection);
    pgObject *GetObject();

private:
    void OnChange(wxCommandEvent &ev);

    pgSchema *schema;
    pgTextSearchTemplate *tmpl;

    DECLARE_EVENT_TABLE()
};


#endif
