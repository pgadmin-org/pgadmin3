//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2005, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// dlgRepSet.h - Slony-I Set property
//
//////////////////////////////////////////////////////////////////////////


#ifndef __DLG_REPSETPROP
#define __DLG_REPSETPROP

#include "dlgRepProperty.h"

class slCluster;
class slSet;

class dlgRepSet : public dlgRepProperty
{
public:
    dlgRepSet(frmMain *frame, slSet *set, slCluster *c);
    int Go(bool modal);

    void CheckChange();
    wxString GetSql();
    pgObject *CreateObject(pgCollection *collection);
    pgObject *GetObject();

private:
    slSet *set;

    DECLARE_EVENT_TABLE();
};


#endif
