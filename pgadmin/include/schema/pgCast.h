//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// pgCast.h PostgreSQL Cast
//
//////////////////////////////////////////////////////////////////////////

#ifndef PGCAST_H
#define PGCAST_H


// App headers
#include "pgDatabase.h"

class pgCollection;
class pgCastFactory : public pgDatabaseObjFactory
{
public:
	pgCastFactory();
	virtual dlgProperty *CreateDialog(frmMain *frame, pgObject *node, pgObject *parent);
	virtual pgObject *CreateObjects(pgCollection *obj, ctlTree *browser, const wxString &restr = wxEmptyString);
	virtual pgCollection *CreateCollection(pgObject *obj);
};
extern pgCastFactory castFactory;


class pgCast : public pgDatabaseObject
{
public:
	pgCast(const wxString &newName = wxT(""));
	~pgCast();

	wxString GetTranslatedMessage(int kindOfMessage) const;
	void ShowTreeDetail(ctlTree *browser, frmMain *form = 0, ctlListView *properties = 0, ctlSQLBox *sqlPane = 0);

	bool CanDropCascaded()
	{
		return true;
	}

	bool GetSystemObject() const
	{
		return GetOid() <= GetConnection()->GetLastSystemOID();
	}
	wxString GetSourceType() const
	{
		return sourceType;
	}
	void iSetSourceType(const wxString &s)
	{
		sourceType = s;
	}
	wxString GetTargetType() const
	{
		return targetType;
	}
	void iSetTargetType(const wxString &s)
	{
		targetType = s;
	}
	wxString GetSourceNamespace() const
	{
		return sourceNamespace;
	}
	void iSetSourceNamespace(const wxString &s)
	{
		sourceNamespace = s;
	}
	wxString GetTargetNamespace() const
	{
		return targetNamespace;
	}
	void iSetTargetNamespace(const wxString &s)
	{
		targetNamespace = s;
	}
	OID GetSourceTypeOid() const
	{
		return sourceTypeOid;
	}
	void iSetSourceTypeOid(const OID o)
	{
		sourceTypeOid = o;
	}
	OID GetTargetTypeOid() const
	{
		return targetTypeOid;
	}
	void iSetTargetTypeOid(const OID o)
	{
		targetTypeOid = o;
	}
	wxString GetCastFunction() const
	{
		return castFunction;
	}
	void iSetCastFunction(const wxString &s)
	{
		castFunction = s;
	}
	wxString GetCastNamespace() const
	{
		return castNamespace;
	}
	void iSetCastNamespace(const wxString &s)
	{
		castNamespace = s;
	}

	wxString GetCastContext() const
	{
		return castContext;
	}
	void iSetCastContext(const wxString &s)
	{
		castContext = s;
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
	wxString sourceType, sourceNamespace, targetType, targetNamespace,
	         castFunction, castContext, castNamespace;
	OID sourceTypeOid, targetTypeOid;
};

class pgCastCollection : public pgDatabaseObjCollection
{
public:
	pgCastCollection(pgaFactory *factory, pgDatabase *db);
	wxString GetTranslatedMessage(int kindOfMessage) const;
};

#endif
