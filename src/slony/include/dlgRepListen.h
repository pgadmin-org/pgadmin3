//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2005, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// dlgRepListen.h - Slony-I listen property
//
//////////////////////////////////////////////////////////////////////////


#ifndef __DLG_REPLISTENPROP
#define __DLG_REPLISTENPROP

#include "dlgRepProperty.h"

class slNode;
class slListen;

class dlgRepListen : public dlgRepProperty
{
public:
    dlgRepListen(frmMain *frame, slListen *l, slNode *n);
    int Go(bool modal);

    void CheckChange();
    wxString GetSql();
    pgObject *CreateObject(pgCollection *collection);
    pgObject *GetObject();

private:

    slListen *listen;
    slNode *node;

    DECLARE_EVENT_TABLE();
};


#endif
