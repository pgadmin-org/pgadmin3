//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2005, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// dlgLanguage.h - Language property
//
//////////////////////////////////////////////////////////////////////////


#ifndef __DLG_LANGUAGEPROP
#define __DLG_LANGUAGEPROP

#include "dlgProperty.h"

class pgLanguage;

class dlgLanguage : public dlgSecurityProperty
{
public:
    dlgLanguage(frmMain *frame, pgLanguage *db);
    int Go(bool modal);

    void CheckChange();
    wxString GetSql();
    pgObject *CreateObject(pgCollection *collection);
    pgObject *GetObject();

private:
    pgLanguage *language;

    DECLARE_EVENT_TABLE();
};


#endif
