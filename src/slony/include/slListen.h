//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id: slListen.h 4875 2006-01-06 21:06:46Z dpage $
// Copyright (C) 2002 - 2006, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// slListen.h PostgreSQL Slony-I Node
//
//////////////////////////////////////////////////////////////////////////

#ifndef SLLISTEN_H
#define SLLISTEN_H

#include "slNode.h"


class slListenFactory : public slNodeObjFactory
{
public:
    slListenFactory();
    virtual dlgProperty *CreateDialog(frmMain *frame, pgObject *node, pgObject *parent);
    virtual pgObject *CreateObjects(pgCollection *obj, ctlTree *browser, const wxString &restr=wxEmptyString);
};
extern slListenFactory listenFactory;


class slListen : public slNodeObject
{
public:
    slListen(slNode *n, const wxString& newName = wxT(""));
    ~slListen();

    void ShowTreeDetail(ctlTree *browser, frmMain *form=0, ctlListView *properties=0, ctlSQLBox *sqlPane=0);

    long GetProviderId() const { return providerId; }
    void iSetProviderId(long l) { providerId = l; }
    wxString GetProviderName() const { return providerName; }
    void iSetProviderName(const wxString &s) { providerName = s; }
    wxString GetOriginName() const { return originName; }
    void iSetOriginName(const wxString &s) { originName = s; }

    bool DropObject(wxFrame *frame, ctlTree *browser, bool cascaded);
    wxString GetSql(ctlTree *browser);
    pgObject *Refresh(ctlTree *browser, const wxTreeItemId item);

private:
    long providerId;
    wxString providerName, originName;
};

#endif

