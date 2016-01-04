//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// pgTextSearchConfiguration.h PostgreSQL Text Search Configuration
//
//////////////////////////////////////////////////////////////////////////

#ifndef PG_TSCONFIGURATION_H
#define PG_TSCONFIGURATION_H


#include "pgSchema.h"


class pgTextSearchConfigurationFactory : public pgSchemaObjFactory
{
public:
	pgTextSearchConfigurationFactory();
	virtual dlgProperty *CreateDialog(frmMain *frame, pgObject *node, pgObject *parent);
	virtual pgObject *CreateObjects(pgCollection *obj, ctlTree *browser, const wxString &restr = wxEmptyString);
	virtual pgCollection *CreateCollection(pgObject *obj);
};
extern pgTextSearchConfigurationFactory textSearchConfigurationFactory;

class pgTextSearchConfiguration : public pgSchemaObject
{
public:
	pgTextSearchConfiguration(pgSchema *newSchema, const wxString &newName = wxT(""));
	~pgTextSearchConfiguration();

	wxString GetTranslatedMessage(int kindOfMessage) const;
	void ShowTreeDetail(ctlTree *browser, frmMain *form = 0, ctlListView *properties = 0, ctlSQLBox *sqlPane = 0);
	bool CanDropCascaded()
	{
		return GetSchema()->GetMetaType() != PGM_CATALOG;
	}

	wxString GetParser() const
	{
		return parser;
	}
	void iSetParser(const wxString &s)
	{
		parser = s;
	}
	wxString GetCopy() const
	{
		return copy;
	}
	void iSetCopy(const wxString &s)
	{
		copy = s;
	}
	OID GetParserOid() const
	{
		return parserOid;
	}
	void iSetParserOid(const OID o)
	{
		parserOid = o;
	}
	wxString GetParserOidStr() const
	{
		return NumToStr(GetParserOid()) + wxT("::oid");
	}
	wxArrayString &GetTokens()
	{
		return tokens;
	}
	virtual wxString GetHelpPage(bool forCreate) const
	{
		return wxT("pg/sql-createtsconfig");
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
	wxString parser, copy;
	OID parserOid;
	wxArrayString tokens;
};

class pgTextSearchConfigurationCollection : public pgSchemaObjCollection
{
public:
	pgTextSearchConfigurationCollection(pgaFactory *factory, pgSchema *sch);
	wxString GetTranslatedMessage(int kindOfMessage) const;
};

#endif
