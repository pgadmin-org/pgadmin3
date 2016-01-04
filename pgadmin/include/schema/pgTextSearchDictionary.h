//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// pgTextSearchDictionary.h PostgreSQL Text Search Dictionary
//
//////////////////////////////////////////////////////////////////////////

#ifndef PG_TSDICTIONARY_H
#define PG_TSDICTIONARY_H


#include "pgSchema.h"


class pgTextSearchDictionaryFactory : public pgSchemaObjFactory
{
public:
	pgTextSearchDictionaryFactory();
	virtual dlgProperty *CreateDialog(frmMain *frame, pgObject *node, pgObject *parent);
	virtual pgObject *CreateObjects(pgCollection *obj, ctlTree *browser, const wxString &restr = wxEmptyString);
	virtual pgCollection *CreateCollection(pgObject *obj);
};
extern pgTextSearchDictionaryFactory textSearchDictionaryFactory;

class pgTextSearchDictionary : public pgSchemaObject
{
public:
	pgTextSearchDictionary(pgSchema *newSchema, const wxString &newName = wxT(""));
	~pgTextSearchDictionary();

	wxString GetTranslatedMessage(int kindOfMessage) const;
	void ShowTreeDetail(ctlTree *browser, frmMain *form = 0, ctlListView *properties = 0, ctlSQLBox *sqlPane = 0);
	bool CanDropCascaded()
	{
		return GetSchema()->GetMetaType() != PGM_CATALOG;
	}

	wxString GetTemplate() const
	{
		return tmpl;
	}
	void iSetTemplate(const wxString &s)
	{
		tmpl = s;
	}
	wxString GetOptions() const
	{
		return options;
	}
	void iSetOptions(const wxString &s)
	{
		options = s;
	}
	virtual wxString GetHelpPage(bool forCreate) const
	{
		return wxT("pg/sql-createtsdictionary");
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

private:
	wxString tmpl, options;
};

class pgTextSearchDictionaryCollection : public pgSchemaObjCollection
{
public:
	pgTextSearchDictionaryCollection(pgaFactory *factory, pgSchema *sch);
	wxString GetTranslatedMessage(int kindOfMessage) const;
};

#endif
