//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// slTable.h PostgreSQL Slony-I Node
//
//////////////////////////////////////////////////////////////////////////

#ifndef SLTABLE_H
#define SLTABLE_H

#include "slony/slSet.h"

class slSlTableFactory : public slSetObjFactory
{
public:
	slSlTableFactory();
	virtual dlgProperty *CreateDialog(frmMain *frame, pgObject *node, pgObject *parent);
	virtual pgObject *CreateObjects(pgCollection *obj, ctlTree *browser, const wxString &restr = wxEmptyString);
	virtual pgCollection *CreateCollection(pgObject *obj);
};
extern slSlTableFactory slTableFactory;


class slTable : public slSetObject
{
public:
	slTable(slSet *set, const wxString &newName = wxT(""));

	void ShowTreeDetail(ctlTree *browser, frmMain *form = 0, ctlListView *properties = 0, ctlSQLBox *sqlPane = 0);

	wxString GetTranslatedMessage(int kindOfMessage) const;

	bool GetAltered() const
	{
		return altered;
	}
	void iSetAltered(bool b)
	{
		altered = b;
	}
	wxString GetIndexName() const
	{
		return indexName;
	}
	void iSetIndexName(const wxString s)
	{
		indexName = s;
	}
	const wxArrayString &GetTriggers()
	{
		return triggers;
	}

	bool DropObject(wxFrame *frame, ctlTree *browser, bool cascaded);
	wxString GetSql(ctlTree *browser);
	pgObject *Refresh(ctlTree *browser, const wxTreeItemId item);

private:
	bool altered;
	wxString indexName;
	wxArrayString triggers;
};

class slSlTableCollection : public slSetObjCollection
{
public:
	slSlTableCollection(pgaFactory *factory, slSet *set) : slSetObjCollection(factory, set) {}
	wxString GetTranslatedMessage(int kindOfMessage) const;
};

#endif

