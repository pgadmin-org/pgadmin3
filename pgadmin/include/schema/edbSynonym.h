//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// edbSynonym.h - EnterpriseDB Synonym class
//
//////////////////////////////////////////////////////////////////////////

#ifndef EDBSYNONYM_H
#define EDBSYNONYM_H


// App headers
#include "pgDatabase.h"

class pgCollection;
class edbSynonymFactory : public pgDatabaseObjFactory
{
public:
	edbSynonymFactory();
	virtual dlgProperty *CreateDialog(frmMain *frame, pgObject *node, pgObject *parent);
	virtual pgObject *CreateObjects(pgCollection *obj, ctlTree *browser, const wxString &restr = wxEmptyString);
	pgCollection *CreateCollection(pgObject *obj);
};
extern edbSynonymFactory synonymFactory;


class edbSynonym : public pgDatabaseObject
{
public:
	edbSynonym(const wxString &newName = wxT(""));

	wxString GetTranslatedMessage(int kindOfMessage) const;
	void ShowTreeDetail(ctlTree *browser, frmMain *form = 0, ctlListView *properties = 0, ctlSQLBox *sqlPane = 0);

	bool GetSystemObject() const
	{
		return false;
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

	bool DropObject(wxFrame *frame, ctlTree *browser, bool cascaded);
	wxString GetSql(ctlTree *browser);
	pgObject *Refresh(ctlTree *browser, const wxTreeItemId item);

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

	wxString GetHelpPage(bool forCreate) const
	{
		return wxT("pg/sql-createpubsynonym");
	}

private:
	wxString targetType, targetSchema, targetObject;
};

class edbSynonymCollection : public pgDatabaseObjCollection
{
public:
	edbSynonymCollection(pgaFactory *factory, pgDatabase *db);
	wxString GetTranslatedMessage(int kindOfMessage) const;
};


#endif
