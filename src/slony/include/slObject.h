//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2005, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// slObject.h PostgreSQL Slony-I Objects
//
//////////////////////////////////////////////////////////////////////////

#ifndef SLOBJECT_H
#define SLOBJECT_H

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "pgObject.h"
#include "pgCollection.h"


class slCluster;
class slNode;
class slSet;
class slSubscription;


// Collection of Slony-I objects
class slCollection : public pgCollection
{
public:
    slCollection(int newType, slCluster *_cluster);

    void ShowTreeDetail(wxTreeCtrl *browser, frmMain *form, ctlListView *properties, ctlSQLBox *sqlPane);
    void ShowStatistics(frmMain *form, ctlListView *statistics);
    int GetIconId();
    bool CanCreate();

    slCluster *GetCluster() { return cluster; }
    long GetSlId() const { return slId; }
    void iSetSlId(long l) { slId = l; }

private:
    slCluster *cluster;
    long slId;
};



// Collection of set objects 
class slSetCollection : public slCollection
{
public:
    slSetCollection(int newType, slSet *_set, slSubscription *sub=0);
    void ShowTreeDetail(wxTreeCtrl *browser, frmMain *form, ctlListView *properties, ctlSQLBox *sqlPane);
    int GetIconId();
    bool CanCreate();

    slSet *GetSet() {return set; }

private:
    slSet *set;
    slSubscription *subscription;
};


// Collection of node objects 
class slNodeCollection : public slCollection
{
public:
    slNodeCollection(int newType, slNode *n);
    void ShowTreeDetail(wxTreeCtrl *browser, frmMain *form, ctlListView *properties, ctlSQLBox *sqlPane);

    slNode *GetNode() {return node; }

private:
    slNode *node;
};

// Slony-I object
class slObject : public pgDatabaseObject
{
public:
    slObject(slCluster *_slCluster, int newType, const wxString& newName = wxT(""));
    slCluster *GetCluster() { return cluster; }

    void iSetSlId(long i) { slId=i; }
    long GetSlId() const { return slId; }

private:
    slCluster *cluster;
    long slId;
};


// Object under a Slony-I node
class slNodeObject : public slObject
{
public:
    slNodeObject(slNode *n, int newType, const wxString& newName = wxT(""));
    slNode *GetNode() const { return node; }

private:
    slNode *node;
};



// Object in a Slony-I set
class slSetObject : public slObject
{
public:
    slSetObject(slSet *s, int newType, const wxString& newName = wxT(""));

    slSet *GetSet() { return set; }

    bool CanDrop();
    bool CanCreate();

private:
    slSet *set;
};


#endif

