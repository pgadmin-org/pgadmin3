//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2006, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// slSubscription.h PostgreSQL Slony-I Node
//
//////////////////////////////////////////////////////////////////////////

#ifndef SLSUBSCRIPTION_H
#define SLSUBSCRIPTION_H

#include "slony/slSet.h"


class slSubscriptionFactory : public slSetObjFactory
{
public:
    slSubscriptionFactory();
    virtual dlgProperty *CreateDialog(frmMain *frame, pgObject *node, pgObject *parent);
    virtual pgObject *CreateObjects(pgCollection *obj, ctlTree *browser, const wxString &restr=wxEmptyString);
    int GetExportedIconId() { return exportedIconId; }
    
protected:
    int exportedIconId;
};
extern slSubscriptionFactory subscriptionFactory;


class slSubscription : public slSetObject
{
public:
    slSubscription(slSet *set, const wxString& newName = wxT(""));
    ~slSubscription();

    int GetIconId();
    void ShowTreeDetail(ctlTree *browser, frmMain *form=0, ctlListView *properties=0, ctlSQLBox *sqlPane=0);

    bool WantDummyChild();
    bool RequireDropConfirm() { return true; }

    bool GetActive() const { return active; }
    void iSetActive(bool b) { active=b; }
    bool GetForward() const { return forward; }
    void iSetForward(bool b) { forward=b; }
    wxString GetProviderNode() const { return providerNode; }
    void iSetProviderNode(const wxString &s) { providerNode=s; }
    wxString GetReceiverNode() const { return receiverNode; }
    void iSetReceiverNode(const wxString &s) { receiverNode=s; }
    long GetProviderId() const { return providerId; }
    void iSetProviderId(long l) { providerId=l; }
    long GetReceiverId() const { return receiverId; }
    void iSetReceiverId(long l) { receiverId=l; }
    bool GetIsSubscribed() { return isSubscribed; }
    void iSetIsSubscribed(bool b) { isSubscribed=b; }


  
    bool CanCreate();
    bool CanDrop();

    bool DropObject(wxFrame *frame, ctlTree *browser, bool cascaded);
    wxString GetSql(ctlTree *browser);
    pgObject *Refresh(ctlTree *browser, const wxTreeItemId item);

private:
    bool active, forward, isSubscribed;
    long providerId, receiverId;
    wxString providerNode, receiverNode;
};

#endif

