//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// slSequence.h PostgreSQL Slony-I Node
//
//////////////////////////////////////////////////////////////////////////

#ifndef SLSEQUENCE_H
#define SLSEQUENCE_H

#include "schema/pgDatabase.h"
#include "slony/slSet.h"

class slSlSequenceFactory : public slSetObjFactory
{
public:
	slSlSequenceFactory();
	virtual dlgProperty *CreateDialog(frmMain *frame, pgObject *node, pgObject *parent);
	virtual pgObject *CreateObjects(pgCollection *obj, ctlTree *browser, const wxString &restr = wxEmptyString);
	virtual pgCollection *CreateCollection(pgObject *obj);
};
extern slSlSequenceFactory slSequenceFactory;


class slSequence : public slSetObject
{
public:
	slSequence(slSet *set, const wxString &newName = wxT(""));

	void ShowTreeDetail(ctlTree *browser, frmMain *form = 0, ctlListView *properties = 0, ctlSQLBox *sqlPane = 0);

	wxString GetTranslatedMessage(int kindOfMessage) const;

	bool GetActive() const
	{
		return active;
	}
	void iSetActive(bool b)
	{
		active = b;
	}

	bool DropObject(wxFrame *frame, ctlTree *browser, bool cascaded);
	wxString GetSql(ctlTree *browser);
	pgObject *Refresh(ctlTree *browser, const wxTreeItemId item);

private:
	bool active;
};

class slSlSequenceCollection : public slSetObjCollection
{
public:
	slSlSequenceCollection(pgaFactory *factory, slSet *set) : slSetObjCollection(factory, set) {}
	wxString GetTranslatedMessage(int kindOfMessage) const;
};

#endif

