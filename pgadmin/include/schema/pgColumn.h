//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// pgColumn.h PostgreSQL Column
//
//////////////////////////////////////////////////////////////////////////

#ifndef PGCOLUMN_H
#define PGCOLUMN_H

// App headers
#include "pgTable.h"

WX_DECLARE_STRING_HASH_MAP(wxString, inheritHashMap);

class pgCollection;

class pgColumnFactory : public pgTableObjFactory
{
public:
	pgColumnFactory();
	virtual dlgProperty *CreateDialog(frmMain *frame, pgObject *node, pgObject *parent);
	virtual pgObject *CreateObjects(pgCollection *obj, ctlTree *browser, const wxString &restr = wxEmptyString);
	virtual pgCollection *CreateCollection(pgObject *obj);
};
extern pgColumnFactory columnFactory;

class pgColumn : public pgTableObject
{
public:
	pgColumn(pgTable *newTable, const wxString &newName = wxT(""));
	~pgColumn();

	wxString GetTranslatedMessage(int kindOfMessage) const;
	void ShowTreeDetail(ctlTree *browser, frmMain *form = 0, ctlListView *properties = 0, ctlSQLBox *sqlPane = 0);
	void ShowStatistics(frmMain *form, ctlListView *statistics);
	void ShowDependencies(frmMain *form, ctlListView *Dependencies, const wxString &where = wxEmptyString);
	void ShowDependents(frmMain *form, ctlListView *referencedBy, const wxString &where = wxEmptyString);

	wxString GetDefinition();

	bool IsReferenced();

	wxString GetRawTypename() const
	{
		return rawTypename;
	}
	void iSetRawTypename(const wxString &s)
	{
		rawTypename = s;
	}
	wxString GetVarTypename() const
	{
		return varTypename;
	}
	void iSetVarTypename(const wxString &s)
	{
		varTypename = s;
	}
	wxString GetQuotedTypename() const
	{
		return quotedTypename;
	}
	void iSetQuotedTypename(const wxString &s)
	{
		quotedTypename = s;
	}
	wxString GetDefault() const
	{
		return defaultVal;
	}
	void iSetDefault(const wxString &s)
	{
		defaultVal = s;
	}
	long GetColNumber() const
	{
		return colNumber;
	}
	void iSetColNumber(const long l)
	{
		colNumber = l;
	}
	long GetLength() const
	{
		return length;
	}
	void iSetLength(const long l)
	{
		length = l;
	}
	long GetPrecision() const
	{
		return precision;
	}
	void iSetPrecision(const long l)
	{
		precision = l;
	}
	long GetStatistics() const
	{
		return statistics;
	}
	void iSetStatistics(const long l)
	{
		statistics = l;
	}
	bool GetIsPK() const
	{
		return isPK;
	}
	bool GetIsFK() const
	{
		return isFK;
	}
	bool GetNotNull() const
	{
		return notNull;
	}
	void iSetNotNull(const bool b)
	{
		notNull = b;
	}
	bool GetIsArray() const
	{
		return isArray;
	}
	void iSetIsArray(const bool b)
	{
		isArray = b;
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
	wxString GetTableName() const
	{
		return tableName;
	}
	void iSetTableName(const wxString &s)
	{
		tableName = s;
	}
	wxString GetQuotedFullTable() const
	{
		return quotedFullTable;
	}
	void iSetQuotedFullTable(const wxString &s)
	{
		quotedFullTable = s;
	}
	wxString GetDefaultStorage() const
	{
		return defaultStorage;
	}
	void iSetDefaultStorage(const wxString &s)
	{
		defaultStorage = s;
	}
	wxString GetStorage() const
	{
		return storage;
	}
	void iSetStorage(const wxString &s)
	{
		storage = s;
	}
	long GetInheritedCount() const
	{
		return inheritedCount;
	}
	void iSetInheritedCount(const long l)
	{
		inheritedCount = l;
	}
	wxString GetInheritedTableName() const
	{
		return inheritedTableName;
	}
	void iSetInheritedTableName(const wxString &s)
	{
		inheritedTableName = s;
	}
	bool GetIsLocal() const
	{
		return isLocal;
	}
	void iSetIsLocal(const bool b)
	{
		isLocal = b;
	}
	OID  GetAttTypId() const
	{
		return attTypId;
	}
	void iSetAttTypId(const OID o)
	{
		attTypId = o;
	}
	long GetAttstattarget() const
	{
		return attstattarget;
	}
	void iSetAttstattarget(const long l)
	{
		attstattarget = l;
	}
	wxString GetSerialSequence() const
	{
		return serialSequence;
	}
	void iSetSerialSequence(const wxString &s)
	{
		serialSequence = s;
	}
	wxString GetSerialSchema() const
	{
		return serialSchema;
	}
	void iSetSerialSchema(const wxString &s)
	{
		serialSchema = s;
	}
	void iSetPkCols(const wxString &s)
	{
		pkCols = s;
	}
	void iSetIsFK(const bool b)
	{
		isFK = b;
	}
	wxArrayString &GetVariables()
	{
		return variables;
	}
	wxString GetCollation() const
	{
		return collation;
	}
	void iSetCollation(const wxString &s)
	{
		collation = s;
	}

	bool DropObject(wxFrame *frame, ctlTree *browser, bool cascaded);
	bool GetSystemObject() const
	{
		return colNumber < 0;
	}
	wxString GetSql(ctlTree *browser);
	wxString GetCommentSql();
	wxString GetStorageSql();
	wxString GetAttstattargetSql();
	wxString GetVariablesSql();
	wxString GetPrivileges();
	wxString GetHelpPage(bool forCreate) const
	{
		return wxT("pg/sql-altertable");
	}

	virtual bool CanDrop()
	{
		return inheritedCount == 0 && pgSchemaObject::CanDrop() && GetSchema()->GetMetaType() != PGM_CATALOG && GetTable()->GetMetaType() != PGM_VIEW && GetTable()->GetMetaType() != GP_EXTTABLE;
	}
	virtual bool CanCreate()
	{
		return GetTable()->GetMetaType() != PGM_VIEW && GetTable()->GetMetaType() != GP_EXTTABLE && GetSchema()->GetMetaType() != PGM_CATALOG;
	}
	pgObject *Refresh(ctlTree *browser, const wxTreeItemId item);

	bool HasStats()
	{
		return true;
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
	wxString varTypename, quotedTypename, defaultVal, tableName, quotedFullTable, defaultStorage, storage, rawTypename;
	wxString serialSequence, serialSchema, pkCols, inheritedTableName, collation;
	long colNumber, length, precision, statistics, attstattarget;
	long typlen, typmod, inheritedCount;
	bool isPK, isFK, notNull, isArray, isLocal;
	OID attTypId;
	int isReferenced;
	wxArrayString variables;
};

class pgColumnCollection : public pgTableObjCollection
{
public:
	pgColumnCollection(pgaFactory *factory, pgTable *tbl);
	wxString GetTranslatedMessage(int kindOfMessage) const;
};

#endif
