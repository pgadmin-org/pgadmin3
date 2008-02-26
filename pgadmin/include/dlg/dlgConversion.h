//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2008, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// dlgConversion.h - Conversion property 
//
//////////////////////////////////////////////////////////////////////////


#ifndef __DLG_CONVERSIONPROP
#define __DLG_CONVERSIONPROP

#include "dlg/dlgProperty.h"

class pgSchema;
class pgConversion;

class dlgConversion : public dlgProperty
{
public:
    dlgConversion(pgaFactory *factory, frmMain *frame, pgConversion *cc, pgSchema *sch);
    int Go(bool modal);

    void CheckChange();
    wxString GetSql();
    pgObject *CreateObject(pgCollection *collection);
    pgObject *GetObject();

private:
    pgConversion *conversion;
    pgSchema *schema;
    wxArrayString functions;

    DECLARE_EVENT_TABLE()
};


#endif
