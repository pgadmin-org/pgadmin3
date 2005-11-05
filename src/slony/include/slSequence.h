//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2005, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// slSequence.h PostgreSQL Slony-I Node
//
//////////////////////////////////////////////////////////////////////////

#ifndef SLSEQUENCE_H
#define SLSEQUENCE_H

#include "pgDatabase.h"
#include "slSet.h"

class slSlSequenceFactory : public slSetObjFactory
{
public:
    slSlSequenceFactory();
    virtual dlgProperty *CreateDialog(frmMain *frame, pgObject *node, pgObject *parent);
    virtual pgObject *CreateObjects(pgCollection *obj, ctlTree *browser, const wxString &restr=wxEmptyString);
};
extern slSlSequenceFactory slSequenceFactory;


class slSequence : public slSetObject
{
public:
    slSequence(slSet *set, const wxString& newName = wxT(""));
    ~slSequence();

    void ShowTreeDetail(ctlTree *browser, frmMain *form=0, ctlListView *properties=0, ctlSQLBox *sqlPane=0);

    bool GetActive() const { return active; }
    void iSetActive(bool b) { active=b; }

    bool DropObject(wxFrame *frame, ctlTree *browser, bool cascaded);
    wxString GetSql(ctlTree *browser);
    pgObject *Refresh(ctlTree *browser, const wxTreeItemId item);

private:
    bool active;
};

#endif

