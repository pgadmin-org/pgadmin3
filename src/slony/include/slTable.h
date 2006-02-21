//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id: slTable.h 4875 2006-01-06 21:06:46Z dpage $
// Copyright (C) 2002 - 2006, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// slTable.h PostgreSQL Slony-I Node
//
//////////////////////////////////////////////////////////////////////////

#ifndef SLTABLE_H
#define SLTABLE_H

#include "slSet.h"

class slSlTableFactory : public slSetObjFactory
{
public:
    slSlTableFactory();
    virtual dlgProperty *CreateDialog(frmMain *frame, pgObject *node, pgObject *parent);
    virtual pgObject *CreateObjects(pgCollection *obj, ctlTree *browser, const wxString &restr=wxEmptyString);
};
extern slSlTableFactory slTableFactory;


class slTable : public slSetObject
{
public:
    slTable(slSet *set, const wxString& newName = wxT(""));
    ~slTable();

    void ShowTreeDetail(ctlTree *browser, frmMain *form=0, ctlListView *properties=0, ctlSQLBox *sqlPane=0);

    bool GetAltered() const { return altered; }
    void iSetAltered(bool b) { altered=b; }
    wxString GetIndexName() const { return indexName; }
    void iSetIndexName(const wxString s) { indexName = s; }
    const wxArrayString &GetTriggers() { return triggers; }

    bool DropObject(wxFrame *frame, ctlTree *browser, bool cascaded);
    wxString GetSql(ctlTree *browser);
    pgObject *Refresh(ctlTree *browser, const wxTreeItemId item);

private:
    bool altered;
    wxString indexName;
    wxArrayString triggers;
};

#endif

