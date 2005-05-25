//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2005, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// dlgIndexConstraint.h - IndexConstraint property 
//
//////////////////////////////////////////////////////////////////////////


#ifndef __DLG_INDEXCONSTRAINTPROP
#define __DLG_INDEXCONSTRAINTPROP


#include "dlgIndex.h"


class dlgIndexConstraint : public dlgIndexBase
{
protected:
    dlgIndexConstraint(frmMain *frame, const wxString &resName, pgIndex *index, pgTable *parentNode);
    dlgIndexConstraint(frmMain *frame, const wxString &resName, ctlListView *colList);

public:
    ctlListView *columns;


    int Go(bool modal);
    wxString GetDefinition();
    wxString GetSql();
    wxString GetHelpPage() const { return wxT("pg/sql-altertable"); }

private:
    void OnCheckDeferrable(wxCommandEvent &ev);
    DECLARE_EVENT_TABLE();
};


class dlgPrimaryKey : public dlgIndexConstraint
{
public:
    dlgPrimaryKey(frmMain *frame, pgIndex *index, pgTable *parentNode);
    dlgPrimaryKey(frmMain *frame, ctlListView *colList);

    pgObject *CreateObject(pgCollection *collection);
};


class dlgUnique : public dlgIndexConstraint
{
public:
    dlgUnique(frmMain *frame, pgIndex *index, pgTable *parentNode);
    dlgUnique(frmMain *frame, ctlListView *colList);

    pgObject *CreateObject(pgCollection *collection);
};


#endif
