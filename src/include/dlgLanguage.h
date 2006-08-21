//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id: dlgLanguage.h 4875 2006-01-06 21:06:46Z dpage $
// Copyright (C) 2002 - 2006, The pgAdmin Development Team
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
    dlgLanguage(pgaFactory *factory, frmMain *frame, pgLanguage *db);
    int Go(bool modal);

    void CheckChange();
    wxString GetSql();
    pgObject *CreateObject(pgCollection *collection);
    pgObject *GetObject();

private:
    pgLanguage *language;
    void OnChangeName(wxCommandEvent &ev);

    DECLARE_EVENT_TABLE()
};


#endif
