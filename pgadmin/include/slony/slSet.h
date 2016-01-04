//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// slSet.h PostgreSQL Slony-I Set
//
//////////////////////////////////////////////////////////////////////////

#ifndef SLSET_H
#define SLSET_H

#include "slony/slCluster.h"


class slSetFactory : public slObjFactory
{
public:
	slSetFactory();
	virtual dlgProperty *CreateDialog(frmMain *frame, pgObject *node, pgObject *parent);
	virtual pgObject *CreateObjects(pgCollection *obj, ctlTree *browser, const wxString &restr = wxEmptyString);
	virtual pgCollection *CreateCollection(pgObject *obj);
	int GetExportedIconId()
	{
		return exportedIconId;
	}

protected:
	int exportedIconId;
};
extern slSetFactory setFactory;


class slSet : public slObject
{
public:
	slSet(slCluster *_cluster, const wxString &newName = wxT(""));

	int GetIconId();
	void ShowTreeDetail(ctlTree *browser, frmMain *form = 0, ctlListView *properties = 0, ctlSQLBox *sqlPane = 0);

	wxString GetTranslatedMessage(int kindOfMessage) const;

	bool CanDrop();
	bool RequireDropConfirm()
	{
		return true;
	}
	bool WantDummyChild()
	{
		return true;
	}

	long GetOriginId() const
	{
		return originId;
	}
	void iSetOriginId(long l)
	{
		originId = l;
	}
	wxString GetOriginNode() const
	{
		return originNode;
	}
	void iSetOriginNode(const wxString s)
	{
		originNode = s;
	}
	long GetSubscriptionCount()
	{
		return subscriptionCount;
	}
	void iSetSubscriptionCount(long l)
	{
		subscriptionCount = l;
	}

	wxString GetLockXXID();
	bool Lock();
	bool Unlock();

	bool DropObject(wxFrame *frame, ctlTree *browser, bool cascaded);
	wxMenu *GetNewMenu();
	wxString GetSql(ctlTree *browser);
	pgObject *Refresh(ctlTree *browser, const wxTreeItemId item);

	void ShowDependents(frmMain *form, ctlListView *referencedBy, const wxString &wh);
	void ShowDependencies(frmMain *form, ctlListView *Dependencies, const wxString &wh);
	void ShowStatistics(pgCollection *collection, ctlListView *statistics);

private:
	long subscriptionCount;
	long originId;
	wxString originNode;
};


class slSetCollection : public slObjCollection
{
public:
	slSetCollection(pgaFactory *factory, slCluster *cl) : slObjCollection(factory, cl) {}
	wxString GetTranslatedMessage(int kindOfMessage) const;
};

// Object in a Slony-I set
class slSetObject : public slObject
{
public:
	slSetObject(slSet *s, pgaFactory &factory, const wxString &newName = wxT(""));
	slSet *GetSet()
	{
		return set;
	}

	bool CanDrop();
	bool CanCreate();

private:
	slSet *set;
};


// Collection of set objects
class slSubscription;
class slSetObjCollection : public slObjCollection
{
public:
	slSetObjCollection(pgaFactory *factory, slSet *_set);
	bool CanCreate();

	slSet *GetSet()
	{
		return set;
	}

private:
	slSet *set;
	slSubscription *subscription;
};


class slSetObjFactory : public slObjFactory
{
public:
	slSetObjFactory(const wxChar *tn, const wxChar *ns, const wxChar *nls, wxImage *img, wxImage *imgSm = 0) : slObjFactory(tn, ns, nls, img, imgSm) {}
	virtual pgCollection *CreateCollection(pgObject *obj);
};

class slonyMoveSetFactory : public contextActionFactory
{
public:
	slonyMoveSetFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar);
	wxWindow *StartDialog(frmMain *form, pgObject *obj);
	bool CheckEnable(pgObject *obj);
};


class slonyMergeSetFactory : public contextActionFactory
{
public:
	slonyMergeSetFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar);
	wxWindow *StartDialog(frmMain *form, pgObject *obj);
	bool CheckEnable(pgObject *obj);
};


class slonyLockSetFactory : public contextActionFactory
{
public:
	slonyLockSetFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar);
	wxWindow *StartDialog(frmMain *form, pgObject *obj);
	bool CheckEnable(pgObject *obj);
};


class slonyUnlockSetFactory : public contextActionFactory
{
public:
	slonyUnlockSetFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar);
	wxWindow *StartDialog(frmMain *form, pgObject *obj);
	bool CheckEnable(pgObject *obj);
};

#endif


