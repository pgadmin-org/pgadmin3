//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2005, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// slPath.h PostgreSQL Slony-I Node
//
//////////////////////////////////////////////////////////////////////////

#ifndef SLPATH_H
#define SLPATH_H


#include "slNode.h"


class slPathFactory : public slNodeObjFactory
{
public:
    slPathFactory();
    virtual dlgProperty *CreateDialog(frmMain *frame, pgObject *node, pgObject *parent);
    virtual pgObject *CreateObjects(pgCollection *obj, ctlTree *browser, const wxString &restr=wxEmptyString);
};
extern slPathFactory pathFactory;


class slPath : public slNodeObject
{
public:
    slPath(slNode *n, const wxString& newName = wxT(""));
    ~slPath();

    void ShowTreeDetail(ctlTree *browser, frmMain *form=0, ctlListView *properties=0, ctlSQLBox *sqlPane=0);
    static pgObject *ReadObjects(slNodeCollection *coll, ctlTree *browser);

    void iSetConnInfo(const wxString &s) { connInfo = s;}
    wxString GetConnInfo() const { return connInfo; }
    void iSetConnRetry(long l) { connRetry = l; }
    long GetConnRetry() { return connRetry; }

    bool DropObject(wxFrame *frame, ctlTree *browser, bool cascaded);
    wxString GetSql(ctlTree *browser);
    pgObject *Refresh(ctlTree *browser, const wxTreeItemId item);

private:
    long connRetry;
    wxString connInfo;
};

#endif

