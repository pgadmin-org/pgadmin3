//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2006, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// dlgSchema.h - Schemaproperty 
//
//////////////////////////////////////////////////////////////////////////


#ifndef __DLG_SCHEMAPROP
#define __DLG_SCHEMAPROP

#include "dlgProperty.h"

class pgSchema;

class dlgSchema : public dlgSecurityProperty
{
public:
    dlgSchema(pgaFactory *factory, frmMain *frame, pgSchema *db);
    int Go(bool modal);

    void CheckChange();
    wxString GetSql();
    pgObject *CreateObject(pgCollection *collection);
    pgObject *GetObject();

private:
    pgSchema *schema;

    DECLARE_EVENT_TABLE();
};


#endif
