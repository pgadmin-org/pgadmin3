//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// pgTextSearchParser.h PostgreSQL Text Search Parser
//
//////////////////////////////////////////////////////////////////////////

#ifndef PG_TSPARSER_H
#define PG_TSPARSER_H


#include "pgSchema.h"


class pgTextSearchParserFactory : public pgSchemaObjFactory
{
public:
	pgTextSearchParserFactory();
	virtual dlgProperty *CreateDialog(frmMain *frame, pgObject *node, pgObject *parent);
	virtual pgObject *CreateObjects(pgCollection *obj, ctlTree *browser, const wxString &restr = wxEmptyString);
	virtual pgCollection *CreateCollection(pgObject *obj);
};
extern pgTextSearchParserFactory textSearchParserFactory;

class pgTextSearchParser : public pgSchemaObject
{
public:
	pgTextSearchParser(pgSchema *newSchema, const wxString &newName = wxT(""));
	~pgTextSearchParser();

	wxString GetTranslatedMessage(int kindOfMessage) const;
	void ShowTreeDetail(ctlTree *browser, frmMain *form = 0, ctlListView *properties = 0, ctlSQLBox *sqlPane = 0);
	virtual wxString GetHelpPage(bool forCreate) const
	{
		return wxT("pg/sql-createtsparser");
	}
	bool CanDropCascaded()
	{
		return GetSchema()->GetMetaType() != PGM_CATALOG;
	}

	wxString GetStart() const
	{
		return start;
	}
	void iSetStart(const wxString &s)
	{
		start = s;
	}
	wxString GetGettoken() const
	{
		return gettoken;
	}
	void iSetGettoken(const wxString &s)
	{
		gettoken = s;
	}
	wxString GetEnd() const
	{
		return end;
	}
	void iSetEnd(const wxString &s)
	{
		end = s;
	}
	wxString GetLextypes() const
	{
		return lextypes;
	}
	void iSetLextypes(const wxString &s)
	{
		lextypes = s;
	}
	wxString GetHeadline() const
	{
		return headline;
	}
	void iSetHeadline(const wxString &s)
	{
		headline = s;
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
	wxString start, gettoken, end, lextypes, headline;
};

class pgTextSearchParserCollection : public pgSchemaObjCollection
{
public:
	pgTextSearchParserCollection(pgaFactory *factory, pgSchema *sch);
	wxString GetTranslatedMessage(int kindOfMessage) const;
};

#endif
