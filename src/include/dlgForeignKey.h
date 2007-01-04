//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
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
    dlgForeignKey(pgaFactory *factory, frmMain *frame, pgForeignKey *node, pgTable *parentNode);
    dlgForeignKey(pgaFactory *factory, frmMain *frame, ctlListView *colList);

    void CheckChange();
    wxString GetSql();
    wxString GetDefinition();
    pgObject *CreateObject(pgCollection *collection);
    pgObject *GetObject();
    wxString GetHelpPage() const { return wxT("pg/sql-altertable"); }

    int Go(bool modal);

private:
    pgForeignKey *foreignKey;
    wxString savedIndexName, savedFKName;

    wxString DefaultIndexName(const wxString &name);

    void OnCheckDeferrable(wxCommandEvent &ev);
    void OnSelChangeCol(wxListEvent &ev);
    void OnSelChangeRef(wxCommandEvent &ev);
    void OnSelChangeRefCol(wxCommandEvent &ev);
    void OnAddRef(wxCommandEvent &ev);
    void OnRemoveRef(wxCommandEvent &ev);
    void OnOK(wxCommandEvent &ev);

    DECLARE_EVENT_TABLE()
};


#endif
