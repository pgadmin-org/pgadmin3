//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2005, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// dlgRepSet.h - Slony-I property
//
//////////////////////////////////////////////////////////////////////////


#ifndef __DLG_REPPROP
#define __DLG_REPPROP

#include "dlgProperty.h"

class slCluster;

class dlgRepProperty : public dlgProperty
{
public:
    dlgRepProperty(pgaFactory *f, frmMain *frame, slCluster *cl, const wxString &resName) 
        : dlgProperty(f, frame, resName) { cluster=cl; }

protected:
    slCluster *cluster;
};


#endif
