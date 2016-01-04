//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// pgIndex.h PostgreSQL Index
//
//////////////////////////////////////////////////////////////////////////

#ifndef PGINDEX_H
#define PGINDEX_H

#include "pgTable.h"
#include <wx/arrstr.h>




class pgIndexBase : public pgSchemaObject
{
protected:
	pgIndexBase(pgSchema *newSchema, pgaFactory &factory, const wxString &newName = wxT(""));

public:
	wxString GetTranslatedMessage(int kindOfMessage) const;
	void ShowTreeDetail(ctlTree *browser, frmMain *form = 0, ctlListView *properties = 0, ctlSQLBox *sqlPane = 0);
	void ShowStatistics(frmMain *form, ctlListView *statistics);
	bool CanDropCascaded()
	{
		return GetSchema()->GetMetaType() != PGM_CATALOG;
	}

	wxString GetProcArgs() const
	{
		return procArgs;
	}
	wxString GetQuotedTypedColumns() const
	{
		return quotedTypedColumns;
	}
	wxString GetTypedColumns() const
	{
		return typedColumns;
	}
	wxString GetOperatorClasses() const
	{
		return operatorClasses;
	}
	wxString GetQuotedColumns() const
	{
		return quotedColumns;
	}
	wxString GetColumns() const
	{
		return columns;
	}
	wxArrayString GetColumnList() const
	{
		return columnList;
	}

	wxString GetColumnNumbers() const
	{
		return columnNumbers;
	}
	void iSetColumnNumbers(const wxString &s)
	{
		columnNumbers = s;
	}
	wxString GetConstraint() const
	{
		return constraint;
	}
	void iSetConstraint(const wxString &s)
	{
		constraint = s;
	}
	wxString GetIndexType() const
	{
		return indexType;
	}
	void iSetIndexType(const wxString &s)
	{
		indexType = s;
	}
	long GetColumnCount() const
	{
		return columnCount;
	}
	void iSetColumnCount(const long l)
	{
		columnCount = l;
	}
	bool GetIsUnique() const
	{
		return isUnique;
	}
	void iSetIsUnique(const bool b)
	{
		isUnique = b;
	}
	bool GetIsExclude() const
	{
		return isExclude;
	}
	void iSetIsExclude(const bool b)
	{
		isExclude = b;
	}
	bool GetIsPrimary() const
	{
		return isPrimary;
	}
	void iSetIsPrimary(const bool b)
	{
		isPrimary = b;
	}
	bool GetIsClustered() const
	{
		return isClustered;
	}
	void iSetIsClustered(const bool b)
	{
		isClustered = b;
	}
	bool GetIsValid() const
	{
		return isValid;
	}
	void iSetIsValid(const bool b)
	{
		isValid = b;
	}
	wxString GetIdxTable() const
	{
		return idxTable;
	}
	void iSetIdxTable(const wxString &s)
	{
		idxTable = s;
	}
	wxString GetIdxSchema() const
	{
		return idxSchema;
	}
	void iSetIdxSchema(const wxString &s)
	{
		idxSchema = s;
	}
	OID GetRelTableOid() const
	{
		return relTableOid;
	}
	void iSetRelTableOid(const OID d)
	{
		relTableOid = d;
	}
	wxString GetTablespace() const
	{
		return tablespace;
	};
	void iSetTablespace(const wxString &newVal)
	{
		tablespace = newVal;
	}
	OID GetTablespaceOid() const
	{
		return tablespaceOid;
	};
	void iSetTablespaceOid(const OID newVal)
	{
		tablespaceOid = newVal;
	}
	wxString GetFillFactor()
	{
		return fillFactor;
	}
	void iSetFillFactor(const wxString &s)
	{
		fillFactor = s;
	}

	wxString GetProcName() const
	{
		return procName;
	}
	void iSetProcName(const wxString &s)
	{
		procName = s;
	}
	wxString GetProcNamespace() const
	{
		return procNamespace;
	}
	void iSetProcNamespace(const wxString &s)
	{
		procNamespace = s;
	}

	bool GetDeferrable() const
	{
		return deferrable;
	}
	void iSetDeferrable(const bool b)
	{
		deferrable = b;
	}
	bool GetDeferred() const
	{
		return deferred;
	}
	void iSetDeferred(const bool b)
	{
		deferred = b;
	}

	void iSetOperatorClassList(const wxString &s)
	{
		operatorClassList = s;
	}
	void iSetProcArgTypeList(const wxString &s)
	{
		procArgTypeList = s;
	}

	const wxArrayString &GetOrdersArray()
	{
		return ordersArray;
	}

	const wxArrayString &GetNullsArray()
	{
		return nullsArray;
	}

	const wxArrayString &GetOpClassesArray()
	{
		return opclassesArray;
	}

	const wxArrayString &GetCollationsArray()
	{
		return collationsArray;
	}

	bool DropObject(wxFrame *frame, ctlTree *browser, bool cascaded);
	wxString GetCreate();
	bool CanRestore()
	{
		return true;
	}
	wxString GetSql(ctlTree *browser);
	pgObject *Refresh(ctlTree *browser, const wxTreeItemId item);
	bool CanMaintenance()
	{
		return true;
	}
	bool GetShowExtendedStatistics()
	{
		return showExtendedStatistics;
	}
	void iSetShowExtendedStatistics(bool b)
	{
		showExtendedStatistics = b;
	}

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
	bool HasPgstatindex();

protected:
	void ReadColumnDetails();

private:
	wxString columnNumbers, columns, quotedColumns, indexType, idxTable, idxSchema, constraint, tablespace;
	wxString procName, procNamespace, procArgs, procArgTypeList, typedColumns, quotedTypedColumns, operatorClasses, operatorClassList;
	long columnCount;
	wxArrayString columnList, ordersArray, nullsArray, opclassesArray, collationsArray;
	bool isUnique, isPrimary, isExclude, isClustered, isValid;
	bool deferrable, deferred, showExtendedStatistics;
	OID relTableOid, tablespaceOid;
	wxString fillFactor;
};


class pgIndex : public pgIndexBase
{
public:
	pgIndex(pgSchema *newSchema, const wxString &newName = wxT(""));
};


class pgIndexBaseFactory : public pgSchemaObjFactory
{
public:
	virtual pgObject *CreateObjects(pgCollection *obj, ctlTree *browser, const wxString &restr = wxEmptyString);
	virtual pgCollection *CreateCollection(pgObject *obj);
protected:
	pgIndexBaseFactory(const wxChar *tn, const wxChar *ns, const wxChar *nls, wxImage *img = 0) : pgSchemaObjFactory(tn, ns, nls, img) {}
};

class pgIndexFactory : public pgIndexBaseFactory
{
public:
	pgIndexFactory();
	virtual dlgProperty *CreateDialog(frmMain *frame, pgObject *node, pgObject *parent);
	virtual pgObject *CreateObjects(pgCollection *obj, ctlTree *browser, const wxString &restr = wxEmptyString);
};
extern pgIndexFactory indexFactory;

class pgIndexBaseCollection : public pgSchemaObjCollection
{
public:
	pgIndexBaseCollection(pgaFactory *factory, pgSchema *sch);
	wxString GetTranslatedMessage(int kindOfMessage) const;
	void ShowStatistics(frmMain *form, ctlListView *statistics);
};


class executePgstatindexFactory : public contextActionFactory
{
public:
	executePgstatindexFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar);
	wxWindow *StartDialog(frmMain *form, pgObject *obj);
	bool CheckEnable(pgObject *obj);
	bool CheckChecked(pgObject *obj);
};


#endif
