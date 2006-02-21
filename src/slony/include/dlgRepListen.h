//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id: dlgRepListen.h 4875 2006-01-06 21:06:46Z dpage $
// Copyright (C) 2002 - 2006, The pgAdmin Development Team
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
    dlgRepListen(pgaFactory *factory, frmMain *frame, slListen *l, slNode *n);
    int Go(bool modal);
    wxString GetHelpPage() const { return wxT("slony-path#listen"); }

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
