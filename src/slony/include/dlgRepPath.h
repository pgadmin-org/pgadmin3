//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// dlgRepPath.h - Slony-I path property
//
//////////////////////////////////////////////////////////////////////////


#ifndef __DLG_REPPATHPROP
#define __DLG_REPPATHPROP

#include "dlgRepProperty.h"

class slNode;
class slPath;

class dlgRepPath : public dlgRepProperty
{
public:
    dlgRepPath(pgaFactory *factory, frmMain *frame, slPath *p, slNode *n);
    int Go(bool modal);
    wxString GetHelpPage() const { return wxT("slony-path"); }

    void CheckChange();
    wxString GetSql();
    pgObject *CreateObject(pgCollection *collection);
    pgObject *GetObject();

private:

    slPath *path;
    slNode *node;

    DECLARE_EVENT_TABLE()
};


#endif
