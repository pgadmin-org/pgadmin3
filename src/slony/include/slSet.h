//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2005, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// slSet.h PostgreSQL Slony-I Set
//
//////////////////////////////////////////////////////////////////////////

#ifndef SLSET_H
#define SLSET_H

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "pgObject.h"
#include "pgServer.h"
#include "pgDatabase.h"
#include "slObject.h"


class slSet : public slObject
{
public:
    slSet(slCluster *_cluster, const wxString& newName = wxT(""));
    ~slSet();

    int GetIcon() { return SLICON_SET; }
    void ShowTreeDetail(wxTreeCtrl *browser, frmMain *form=0, ctlListView *properties=0, ctlSQLBox *sqlPane=0);
    static pgObject *ReadObjects(slCollection *coll, wxTreeCtrl *browser, const wxString &restriction);
    static pgObject *ReadObjects(slCollection *coll, wxTreeCtrl *browser);

    bool CanDrop();
    bool RequireDropConfirm() { return true; }
    bool WantDummyChild() { return true; }

    long GetOriginId() const { return originId; }
    void iSetOriginId(long l) { originId=l; }
    wxString GetOriginNode() const { return originNode; }
    void iSetOriginNode(const wxString s) { originNode = s; }
    long GetSubscriptionCount() { return subscriptionCount; }
    void iSetSubscriptionCount(long l) { subscriptionCount=l; }

    bool DropObject(wxFrame *frame, wxTreeCtrl *browser);
    wxMenu *GetNewMenu();
    wxString GetSql(wxTreeCtrl *browser);
    pgObject *Refresh(wxTreeCtrl *browser, const wxTreeItemId item);

    void ShowReferencedBy(frmMain *form, ctlListView *referencedBy, const wxString &wh);
    void ShowDependsOn(frmMain *form, ctlListView *dependsOn, const wxString &wh);
    void ShowStatistics(pgCollection *collection, ctlListView *statistics);

private:
    long subscriptionCount;
    long originId;
    wxString originNode;
};

#endif

