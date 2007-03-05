//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id: dlgCast.h 5828 2007-01-04 16:41:08Z dpage $
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// dlgPackage.h - EnterpriseDB Package property
//
//////////////////////////////////////////////////////////////////////////


#ifndef __DLG_PACKAGEPROP
#define __DLG_PACKAGEPROP

#include "dlg/dlgProperty.h"

class edbPackage;

class dlgPackage : public dlgSecurityProperty
{
public:
    dlgPackage(pgaFactory *f, frmMain *frame, edbPackage *node, pgSchema *sch);
    int Go(bool modal);

    void CheckChange();
    wxString GetSql();
    pgObject *CreateObject(pgCollection *collection);
    pgObject *GetObject();

private:
    edbPackage *package;
    pgSchema *schema;

    DECLARE_EVENT_TABLE()
};


#endif
