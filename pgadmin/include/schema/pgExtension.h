//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// pgExtension.h PostgreSQL Extension
//
//////////////////////////////////////////////////////////////////////////

#ifndef PGEXTENSION_H
#define PGEXTENSION_H

#include "pgDatabase.h"

class pgCollection;
class pgExtensionFactory : public pgDatabaseObjFactory
{
public:
	pgExtensionFactory();
	virtual dlgProperty *CreateDialog(frmMain *frame, pgObject *node, pgObject *parent);
	virtual pgObject *CreateObjects(pgCollection *obj, ctlTree *browser, const wxString &restr = wxEmptyString);
	virtual pgCollection *CreateCollection(pgObject *obj);
};
extern pgExtensionFactory extensionFactory;

class pgExtension : public pgDatabaseObject
{
public:
	pgExtension(const wxString &newName = wxT(""));

	wxString GetTranslatedMessage(int kindOfMessage) const;
	void ShowTreeDetail(ctlTree *browser, frmMain *form = 0, ctlListView *properties = 0, ctlSQLBox *sqlPane = 0);
	bool CanDropCascaded()
	{
		return true;
	}

	wxString GetSchemaStr() const
	{
		return schema;
	}
	void iSetSchemaStr(const wxString &s)
	{
		schema = s;
	}
	wxString GetVersion() const
	{
		return version;
	}
	void iSetVersion(const wxString &s)
	{
		version = s;
	}
	bool GetIsRelocatable() const
	{
		return isrelocatable;
	}
	void iSetIsRelocatable(const bool b)
	{
		isrelocatable = b;
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
		return false;
	}
	bool HasReferences()
	{
		return false;
	}

private:
	wxString schema, version;
	bool isrelocatable;
};

class pgExtensionCollection : public pgDatabaseObjCollection
{
public:
	pgExtensionCollection(pgaFactory *factory, pgDatabase *db);
	wxString GetTranslatedMessage(int kindOfMessage) const;
};

#endif
