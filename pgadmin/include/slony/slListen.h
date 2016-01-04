//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// slListen.h PostgreSQL Slony-I Node
//
//////////////////////////////////////////////////////////////////////////

#ifndef SLLISTEN_H
#define SLLISTEN_H

#include "slony/slNode.h"


class slListenFactory : public slNodeObjFactory
{
public:
	slListenFactory();
	virtual dlgProperty *CreateDialog(frmMain *frame, pgObject *node, pgObject *parent);
	virtual pgObject *CreateObjects(pgCollection *obj, ctlTree *browser, const wxString &restr = wxEmptyString);
	virtual pgCollection *CreateCollection(pgObject *obj);
};
extern slListenFactory listenFactory;


class slListen : public slNodeObject
{
public:
	slListen(slNode *n, const wxString &newName = wxT(""));

	void ShowTreeDetail(ctlTree *browser, frmMain *form = 0, ctlListView *properties = 0, ctlSQLBox *sqlPane = 0);

	wxString GetTranslatedMessage(int kindOfMessage) const;

	long GetProviderId() const
	{
		return providerId;
	}
	void iSetProviderId(long l)
	{
		providerId = l;
	}
	wxString GetProviderName() const
	{
		return providerName;
	}
	void iSetProviderName(const wxString &s)
	{
		providerName = s;
	}
	wxString GetOriginName() const
	{
		return originName;
	}
	void iSetOriginName(const wxString &s)
	{
		originName = s;
	}

	bool DropObject(wxFrame *frame, ctlTree *browser, bool cascaded);
	wxString GetSql(ctlTree *browser);
	pgObject *Refresh(ctlTree *browser, const wxTreeItemId item);

private:
	long providerId;
	wxString providerName, originName;
};

class slListenCollection : public slNodeObjCollection
{
public:
	slListenCollection(pgaFactory *factory, slNode *nd) : slNodeObjCollection(factory, nd) {}
	wxString GetTranslatedMessage(int kindOfMessage) const;
};

#endif

