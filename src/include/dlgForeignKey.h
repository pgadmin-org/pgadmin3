//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2003, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// dlgForeignKey.h - ForeignKey property 
//
//////////////////////////////////////////////////////////////////////////


#ifndef __DLG_FOREIGNKEYPROP
#define __DLG_FOREIGNKEYPROP

#include "dlgProperty.h"


class pgForeignKey;
class pgTable;

class dlgForeignKey : public dlgCollistProperty
{
public:
    dlgForeignKey(frmMain *frame, pgForeignKey *node, pgTable *parentNode);
    dlgForeignKey(frmMain *frame, ctlListView *colList);
    wxString GetSql();
    wxString GetDefinition();
    pgObject *CreateObject(pgCollection *collection);
    pgObject *GetObject();

    int Go(bool modal);

private:
    pgForeignKey *foreignKey;

    void OnChange(wxNotifyEvent &ev);
    void OnCheckDeferrable(wxNotifyEvent &ev);
    void OnSelChangeCol(wxNotifyEvent &ev);
    void OnSelChangeRef(wxNotifyEvent &ev);
    void OnSelChangeRefCol(wxNotifyEvent &ev);
    void OnAddRef(wxNotifyEvent &ev);
    void OnRemoveRef(wxNotifyEvent &ev);

    DECLARE_EVENT_TABLE();
};


#endif
