//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2004, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// dlgConversion.h - Conversion property 
//
//////////////////////////////////////////////////////////////////////////


#ifndef __DLG_CONVERSIONPROP
#define __DLG_CONVERSIONPROP

#include "dlgProperty.h"

class pgSchema;
class pgConversion;

class dlgConversion : public dlgProperty
{
public:
    dlgConversion(frmMain *frame, pgConversion *cc, pgSchema *sch);
    int Go(bool modal);

    wxString GetSql();
    pgObject *CreateObject(pgCollection *collection);
    pgObject *GetObject();

private:
    void OnChange(wxCommandEvent &ev);

    pgConversion *conversion;
    pgSchema *schema;
    wxArrayString functions;

    DECLARE_EVENT_TABLE();
};


#endif
