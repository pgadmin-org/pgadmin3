//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// slNode.h PostgreSQL Slony-I Node
//
//////////////////////////////////////////////////////////////////////////

#ifndef SLNODE_H
#define SLNODE_H

#include "slony/slCluster.h"


class slNodeFactory : public slObjFactory
{
public:
	slNodeFactory();
	virtual dlgProperty *CreateDialog(frmMain *frame, pgObject *node, pgObject *parent);
	virtual pgObject *CreateObjects(pgCollection *obj, ctlTree *browser, const wxString &restr = wxEmptyString);
	virtual pgCollection *CreateCollection(pgObject *obj);
	int GetLocalIconId()
	{
		return localIconId;
	}
	int GetDisabledIconId()
	{
		return disabledIconId;
	}

private:
	int localIconId, disabledIconId;
};
extern slNodeFactory nodeFactory;


class slNode : public slObject
{
public:
	slNode(slCluster *_cluster, const wxString &newName = wxT(""));

	int GetIconId();

	void ShowTreeDetail(ctlTree *browser, frmMain *form = 0, ctlListView *properties = 0, ctlSQLBox *sqlPane = 0);
	void ShowStatistics(frmMain *form, ctlListView *statistics);

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

	long GetOutstandingAcks();
	bool CheckAcksAndContinue(wxFrame *frame);

	bool GetActive() const
	{
		return active;
	}
	void iSetActive(bool b)
	{
		active = b;
	}
	bool GetSpool() const
	{
		return spool;
	}
	void iSetSpool(bool b)
	{
		spool = b;
	}
	wxString GetConnInfo() const
	{
		return connInfo;
	}
	void iSetConnInfo(const wxString s)
	{
		connInfo = s;
	}
	long GetPid()
	{
		return pid;
	}
	bool WaitForEvent(long evNode);

	bool DropObject(wxFrame *frame, ctlTree *browser, bool cascaded);
	wxMenu *GetNewMenu();
	wxString GetSql(ctlTree *browser);
	pgObject *Refresh(ctlTree *browser, const wxTreeItemId item);

private:
	bool active, spool;
	long pid;
	wxString connInfo;
};


class slNodeCollection : public slObjCollection
{
public:
	slNodeCollection(pgaFactory *factory, slCluster *cl) : slObjCollection(factory, cl) {}
	wxString GetTranslatedMessage(int kindOfMessage) const;
	void ShowStatistics(frmMain *form, ctlListView *statistics);
};

//////////////////////////////////////////

// Object under a Slony-I node
class slNodeObject : public slObject
{
public:
	slNodeObject(slNode *n, pgaFactory &factory, const wxString &newName = wxT(""));
	slNode *GetNode() const
	{
		return node;
	}

private:
	slNode *node;
};

// Collection of node objects
class slNodeObjCollection : public slObjCollection
{
public:
	slNodeObjCollection(pgaFactory *factory, slNode *n);
	slNode *GetNode()
	{
		return node;
	}

private:
	slNode *node;
};


class slNodeObjFactory : public slObjFactory
{
public:
	slNodeObjFactory(const wxChar *tn, const wxChar *ns, const wxChar *nls, wxImage *img = 0) : slObjFactory(tn, ns, nls, img) {}
	virtual pgCollection *CreateCollection(pgObject *obj);
};


#endif

