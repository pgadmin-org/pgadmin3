//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// slPath.h PostgreSQL Slony-I Node
//
//////////////////////////////////////////////////////////////////////////

#ifndef SLPATH_H
#define SLPATH_H


#include "slony/slNode.h"


class slPathFactory : public slNodeObjFactory
{
public:
	slPathFactory();
	virtual dlgProperty *CreateDialog(frmMain *frame, pgObject *node, pgObject *parent);
	virtual pgObject *CreateObjects(pgCollection *obj, ctlTree *browser, const wxString &restr = wxEmptyString);
	virtual pgCollection *CreateCollection(pgObject *obj);
};
extern slPathFactory pathFactory;


class slPath : public slNodeObject
{
public:
	slPath(slNode *n, const wxString &newName = wxT(""));

	void ShowTreeDetail(ctlTree *browser, frmMain *form = 0, ctlListView *properties = 0, ctlSQLBox *sqlPane = 0);
	static pgObject *ReadObjects(slNodeCollection *coll, ctlTree *browser);

	wxString GetTranslatedMessage(int kindOfMessage) const;

	void iSetConnInfo(const wxString &s)
	{
		connInfo = s;
	}
	wxString GetConnInfo() const
	{
		return connInfo;
	}
	void iSetConnRetry(long l)
	{
		connRetry = l;
	}
	long GetConnRetry()
	{
		return connRetry;
	}

	bool DropObject(wxFrame *frame, ctlTree *browser, bool cascaded);
	wxString GetSql(ctlTree *browser);
	pgObject *Refresh(ctlTree *browser, const wxTreeItemId item);

private:
	long connRetry;
	wxString connInfo;
};

class slPathCollection : public slNodeObjCollection
{
public:
	slPathCollection(pgaFactory *factory, slNode *nd) : slNodeObjCollection(factory, nd) {}
	wxString GetTranslatedMessage(int kindOfMessage) const;
};


#endif

