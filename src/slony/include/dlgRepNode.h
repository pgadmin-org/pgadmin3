//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2005, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// dlgRepNode.h - Slony-I Node property
//
//////////////////////////////////////////////////////////////////////////


#ifndef __DLG_REPNODEPROP
#define __DLG_REPNODEPROP

#include "dlgRepProperty.h"


class slCluster;
class slNode;

class dlgRepNode : public dlgRepProperty
{
public:
    dlgRepNode(frmMain *frame, slNode *node, slCluster *c);
    int Go(bool modal);

    void CheckChange();
    wxString GetSql();
    pgObject *CreateObject(pgCollection *collection);
    pgObject *GetObject();

private:
    slNode *node;

    DECLARE_EVENT_TABLE();
};

#endif
