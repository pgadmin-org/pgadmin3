//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2005, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// pgView.h PostgreSQL View
//
//////////////////////////////////////////////////////////////////////////

#ifndef PGView_H
#define PGView_H

#include "pgRule.h"

class pgCollection;

class pgViewFactory : public pgSchemaObjFactory
{
public:
    pgViewFactory();
    virtual dlgProperty *CreateDialog(frmMain *frame, pgObject *node, pgObject *parent);
    virtual pgObject *CreateObjects(pgCollection *obj, ctlTree *browser, const wxString &restr=wxEmptyString);
};
extern pgViewFactory viewFactory;


class pgView : public pgRuleObject
{
public:
    pgView(pgSchema *newSchema, const wxString& newName = wxT(""));
    ~pgView();

    void ShowTreeDetail(ctlTree *browser, frmMain *form=0, ctlListView *properties=0, ctlSQLBox *sqlPane=0);
    bool CanDropCascaded() { return true; }

    bool DropObject(wxFrame *frame, ctlTree *browser, bool cascaded);
    bool CanView() { return true; }
    bool WantDummyChild() { return true; }

    wxMenu *GetNewMenu();
    wxString GetSql(ctlTree *browser);
    pgObject *Refresh(ctlTree *browser, const wxTreeItemId item);
};

#endif
