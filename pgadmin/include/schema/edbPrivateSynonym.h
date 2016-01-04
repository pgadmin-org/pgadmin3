//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// edbPrivateSynonym.h PostgreSQL Table
//
//////////////////////////////////////////////////////////////////////////

#ifndef EDBPRIVATESYNONYM_H
#define EDBPRIVATESYNONYM_H

#include "schema/pgSchema.h"


class edbPrivateSynonymFactory : public pgSchemaObjFactory
{
public:
	edbPrivateSynonymFactory();
	virtual dlgProperty *CreateDialog(frmMain *frame, pgObject *node, pgObject *parent);
	virtual pgObject *CreateObjects(pgCollection *obj, ctlTree *browser, const wxString &restr = wxEmptyString);
	pgCollection *CreateCollection(pgObject *obj);
private:
};
extern edbPrivateSynonymFactory edbPrivFactory;

class edbPrivateSynonym  : public pgSchemaObject
{
public:
	edbPrivateSynonym(pgSchema *newSchema, const wxString &newName = wxT(""));

	void ShowTreeDetail(ctlTree *browser, frmMain *form = 0, ctlListView *properties = 0, ctlSQLBox *sqlPane = 0);

	virtual wxString GetSql(ctlTree *browser);

	bool DropObject(wxFrame *, ctlTree *, bool);
	pgObject *Refresh(ctlTree *browser, const wxTreeItemId item);
	wxString GetTranslatedMessage(int kindOfMessage) const;

	bool HasStats()
	{
		return false;
	}
	bool HasDepends()
	{
		return true;
	}
	bool HasReferences()
	{
		return true;
	}
	bool GetSystemObject() const
	{
		return GetSchema()->GetSystemObject();
	}

	wxString GetHelpPage(bool forCreate) const
	{
		return wxT("pg/sql-createpubsynonym");
	}

	wxString GetTargetType() const
	{
		return targetType;
	}
	void iSetTargetType(const wxString &s)
	{
		targetType = s;
	}
	wxString GetTargetSchema() const
	{
		return targetSchema;
	}
	void iSetTargetSchema(const wxString &s)
	{
		targetSchema = s;
	}
	wxString GetTargetObject() const
	{
		return targetObject;
	}
	void iSetTargetObject(const wxString &s)
	{
		targetObject = s;
	}

private:
	wxString targetType, targetSchema, targetObject;
	bool isPublic;
};

class edbPrivateSynonymCollection : public pgSchemaObjCollection
{
public:
	edbPrivateSynonymCollection(pgaFactory *factory, pgSchema *sch);
	wxString GetTranslatedMessage(int kindOfMessage) const;
};

#endif
