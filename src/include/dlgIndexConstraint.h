//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
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
    dlgIndexConstraint(frmMain *frame, const wxString &resName, wxListCtrl *colList);

public:
    wxListCtrl *columns;


    int Go(bool modal);
    wxString GetDefinition();
    wxString GetSql();

private:
    void OnCheckDeferrable(wxNotifyEvent &ev);
    DECLARE_EVENT_TABLE();
};


class dlgPrimaryKey : public dlgIndexConstraint
{
public:
    dlgPrimaryKey(frmMain *frame, pgIndex *index, pgTable *parentNode);
    dlgPrimaryKey(frmMain *frame, wxListCtrl *colList);

    pgObject *CreateObject(pgCollection *collection);
};


class dlgUnique : public dlgIndexConstraint
{
public:
    dlgUnique(frmMain *frame, pgIndex *index, pgTable *parentNode);
    dlgUnique(frmMain *frame, wxListCtrl *colList);

    pgObject *CreateObject(pgCollection *collection);
};


#endif