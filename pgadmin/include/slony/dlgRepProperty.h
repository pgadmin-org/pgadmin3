//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2010, The pgAdmin Development Team
// This software is released under the BSD Licence
//
// dlgRepSet.h - Slony-I property
//
//////////////////////////////////////////////////////////////////////////


#ifndef __DLG_REPPROP
#define __DLG_REPPROP

#include "dlg/dlgProperty.h"

class slCluster;

class dlgRepProperty : public dlgProperty
{
public:
    dlgRepProperty(pgaFactory *f, frmMain *frame, slCluster *c, const wxString &resName);

protected:
    slCluster *cluster;
};


#endif
