//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// pgDomain.h PostgreSQL Domain
//
//////////////////////////////////////////////////////////////////////////

#ifndef PGDOMAIN_H
#define PGDOMAIN_H

#include "pgSchema.h"

class pgCollection;

class pgDomainFactory : public pgSchemaObjFactory
{
public:
	pgDomainFactory();
	virtual dlgProperty *CreateDialog(frmMain *frame, pgObject *node, pgObject *parent);
	virtual pgObject *CreateObjects(pgCollection *obj, ctlTree *browser, const wxString &restr = wxEmptyString);
	virtual pgCollection *CreateCollection(pgObject *obj);
};
extern pgDomainFactory domainFactory;


class pgDomain : public pgSchemaObject
{
public:
	pgDomain(pgSchema *newSchema, const wxString &newName = wxT(""));
	~pgDomain();

	wxString GetTranslatedMessage(int kindOfMessage) const;
	void ShowTreeDetail(ctlTree *browser, frmMain *form = 0, ctlListView *properties = 0, ctlSQLBox *sqlPane = 0);
	bool CanDropCascaded()
	{
		return GetSchema()->GetMetaType() != PGM_CATALOG;
	}

	wxString GetBasetype() const
	{
		return basetype;
	}
	void iSetBasetype(const wxString &s)
	{
		basetype = s;
	}
	wxString GetQuotedBasetype() const
	{
		return quotedBasetype;
	}
	void iSetQuotedBasetype(const wxString &s)
	{
		quotedBasetype = s;
	}
	wxString GetCollation() const
	{
		return collation;
	}
	void iSetCollation(const wxString &s)
	{
		collation = s;
	}
	wxString GetQuotedCollation() const
	{
		return quotedCollation;
	}
	void iSetQuotedCollation(const wxString &s)
	{
		quotedCollation = s;
	}
	void iSetIsDup(bool b)
	{
		isDup = b;
	}
	long GetLength() const
	{
		return length;
	}
	void iSetLength(long l)
	{
		length = l;
	}
	long GetPrecision() const
	{
		return precision;
	}
	void iSetPrecision(long l)
	{
		precision = l;
	}
	wxString GetCheck() const
	{
		return check;
	}
	void iSetCheck(const wxString &s)
	{
		check = s;
	}
	wxString GetDefault() const
	{
		return defaultVal;
	}
	void iSetDefault(const wxString &s)
	{
		defaultVal = s;
	}
	bool GetNotNull() const
	{
		return notNull;
	}
	void iSetNotNull(bool b)
	{
		notNull = b;
	}
	long GetDimensions() const
	{
		return dimensions;
	}
	void iSetDimensions(long l)
	{
		dimensions = l;
	}
	wxString GetDelimiter() const
	{
		return delimiter;
	}
	void iSetDelimiter(const wxString &s)
	{
		delimiter = s;
	}
	OID GetBasetypeOid() const
	{
		return basetypeOid;
	}
	void iSetBasetypeOid(OID d)
	{
		basetypeOid = d;
	}
	OID GetCollationOid() const
	{
		return collationOid;
	}
	void iSetCollationOid(OID d)
	{
		collationOid = d;
	}
	long GetTyplen() const
	{
		return typlen;
	}
	void iSetTyplen(const long l)
	{
		typlen = l;
	}
	long GetTypmod() const
	{
		return typmod;
	}
	void iSetTypmod(const long l)
	{
		typmod = l;
	}
	bool GetValid() const
	{
		return constraintvalid;
	}
	void iSetValid(const bool b)
	{
		constraintvalid = b;
	}
	wxString GetCheckConstraintName() const
	{
		return checkconstraintname;
	}
	void iSetCheckConstraintName(const wxString &s)
	{
		checkconstraintname = s;
	}

	bool DropObject(wxFrame *frame, ctlTree *browser, bool cascaded);
	wxString GetSql(ctlTree *browser);
	pgObject *Refresh(ctlTree *browser, const wxTreeItemId item);
	wxMenu *GetNewMenu();

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
	void Validate(frmMain *form);

private:
	wxString basetype, quotedBasetype, defaultVal, delimiter, check, collation, quotedCollation, checkconstraintname;
	long length, precision, dimensions;
	long typlen, typmod;
	bool notNull, isDup, constraintvalid;
	OID basetypeOid, collationOid;
};

class pgDomainCollection : public pgSchemaObjCollection
{
public:
	pgDomainCollection(pgaFactory *factory, pgSchema *sch);
	wxString GetTranslatedMessage(int kindOfMessage) const;
};

class validateDomainCheckFactory : public contextActionFactory
{
public:
	validateDomainCheckFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar);
	wxWindow *StartDialog(frmMain *form, pgObject *obj);
	bool CheckEnable(pgObject *obj);
};

#endif
