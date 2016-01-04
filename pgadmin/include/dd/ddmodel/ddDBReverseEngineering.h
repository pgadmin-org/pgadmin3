//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// ddDBReverseEngineering.h - Reverse engineering database functions for database designer.
//
//////////////////////////////////////////////////////////////////////////

#ifndef DDDBREVERSEENGINEERING_H
#define DDDBREVERSEENGINEERING_H
#include "wx/wizard.h"
#include "hotdraw/main/hdObject.h"
#include "dd/dditems/utilities/ddDataType.h"
#include "schema/pgDatatype.h"
#include "dd/dditems/figures/ddTableFigure.h"
#include "dd/ddmodel/ddDatabaseDesign.h"

class SelSchemaPage;
class SelTablesPage;
class ReportPage;
class ddStubTable;

enum
{
	DDALLSCHEMAS = 7001,
	DDALLTABS,
	DDSELTABS,
	DDADD,
	DDADDALL,
	DDREMOVE,
	DDREMOVEALL
};

WX_DECLARE_STRING_HASH_MAP( ddStubTable *, stubTablesHashMap);
WX_DEFINE_SORTED_ARRAY_INT(int, wxSortedArrayInt);

// Stub tables related classes
class ddImportDBUtils : public hdObject
{
public:
	// Implement and replace at this and other dd related classes as Generation Wizard static wxArrayString getSchemasNames(...);
	static wxArrayString getTablesNames(pgConn *connection, wxString schemaName);
	static ddStubTable *getTable(pgConn *connection, wxString tableName, OID tableOid);
	static ddTableFigure *getTableFigure(ddStubTable *table);
	static void getAllRelationships(pgConn *connection, stubTablesHashMap &tables, ddDatabaseDesign *design);
	static int getPgColumnNum(pgConn *connection, wxString schemaName, wxString tableName, wxString columnName);
	static OID getTableOID(pgConn *connection, wxString schemaName, wxString tableName);
	static OID getSchemaOID(pgConn *connection, wxString schemaName);
	static bool existsFk(pgConn *connection, OID destTableOid, wxString schemaName, wxString fkName, wxString sourceTableName);
	static wxArrayString getFkAtDbNotInModel(pgConn *connection, OID destTableOid, wxString schemaName, wxArrayString existingFkList, ddDatabaseDesign *design);
	static bool isModelSameDbFk(pgConn *connection, OID destTableOid, wxString schemaName, wxString fkName, wxString sourceTableName, wxString destTableName, ddStubTable *destStubTable, ddRelationshipFigure *relation);

private:
	static bool setUniqueConstraints(pgConn *connection, ddStubTable *table);
	static bool setPkName(pgConn *connection, ddStubTable *table);
	static int sortFunc(int n1, int n2)
	{
		return n1 - n2;
	}
};

class ddStubColumn : public hdObject
{
public:
	ddStubColumn();
	ddStubColumn(const ddStubColumn &copy);
	ddStubColumn(wxString name, OID oidSource, bool notNull, bool pk, pgDatatype *type, int ukIndex = -1);
	ddStubColumn(wxString name, OID oidSource);
	~ddStubColumn();
	wxString columnName;
	bool isNotNull;
	bool isPrimaryKey;
	bool isUniqueKey();
	int uniqueKeyIndex;
	OID OIDTable;
	pgDatatype *typeColumn;
	int pgColNumber;
};

WX_DECLARE_STRING_HASH_MAP( ddStubColumn *, stubColsHashMap);

class ddStubTable : public hdObject
{
public:
	ddStubTable();
	ddStubTable(wxString name, OID tableOID);
	ddStubColumn *getColumnByNumber(int pgColNumber);
	~ddStubTable();
	wxString tableName;
	OID OIDTable;
	stubColsHashMap cols;
	wxString PrimaryKeyName;
	wxArrayString UniqueKeysNames;
};

//
//
// Wizard related classes
//
//
//

WX_DECLARE_STRING_HASH_MAP( OID, oidsHashMap);

class ddDBReverseEngineering : public wxWizard
{
public:
	ddDBReverseEngineering(wxFrame *frame, ddDatabaseDesign *design, pgConn *connection, bool useSizer = true);
	~ddDBReverseEngineering();

	//Wizard related functions
	wxWizardPage *GetFirstPage() const
	{
		return initialPage;
	}

	// Reverse Enginnering related functions
	wxArrayString getTables();
	pgConn *getConnection()
	{
		return conn;
	};

	//transfer data between pages
	OID OIDSelectedSchema;
	wxString schemaName;
	oidsHashMap tablesOIDHM;
	stubTablesHashMap stubsHM;
	SelSchemaPage *page2;
	SelTablesPage *page3;
	ReportPage *page4;
	ddDatabaseDesign *getDesign()
	{
		return figuresDesign;
	};

private:
	void OnFinishPressed(wxWizardEvent &event);
	pgConn *conn;
	wxWizardPageSimple *initialPage;
	wxStaticText *frontText;
	ddDatabaseDesign *figuresDesign;
	DECLARE_EVENT_TABLE()
};

class SelSchemaPage : public wxWizardPage
{
public:
	SelSchemaPage(wxWizard *parent, wxWizardPage *prev);
	~SelSchemaPage();
	virtual wxWizardPage *GetPrev() const
	{
		return m_prev;
	};
	virtual wxWizardPage *GetNext() const
	{
		return m_next;
	};
	void SetPrev(wxWizardPage *prev)
	{
		m_prev = prev;
	}
	void SetNext(wxWizardPage *next)
	{
		m_next = next;
	}
private:
	void OnWizardPageChanging(wxWizardEvent &event);
	void refreshSchemas(pgConn *connection);
	wxStaticText *message;
	wxWizardPage *m_prev, *m_next;
	wxListBox *m_allSchemas;
	wxArrayString schemasNames;
	oidsHashMap schemasHM;
	ddDBReverseEngineering *wparent;
	DECLARE_EVENT_TABLE()
};


class SelTablesPage : public wxWizardPage
{
public:
	SelTablesPage(wxWizard *parent, wxWizardPage *prev);
	~SelTablesPage();
	virtual wxWizardPage *GetPrev() const
	{
		return m_prev;
	};
	virtual wxWizardPage *GetNext() const
	{
		return m_next;
	};
	void SetPrev(wxWizardPage *prev)
	{
		m_prev = prev;
	}
	void SetNext(wxWizardPage *next)
	{
		m_next = next;
	}
	void RefreshTablesList();
	void OnButtonAdd(wxCommandEvent &);
	void OnButtonAddAll(wxCommandEvent &);
	void OnButtonRemove(wxCommandEvent &);
	void OnButtonRemoveAll(wxCommandEvent &);
private:
	void OnWizardPageChanging(wxWizardEvent &event);
	wxStaticText *leftText, *rightText, *centerText;
	wxWizardPage *m_prev, *m_next;
	wxListBox *m_allTables, *m_selTables;
	ddDBReverseEngineering *wparent;
	wxArrayString tablesNames;
	wxBitmapButton *buttonAdd, *buttonAddAll, *buttonRemove, *buttonRemoveAll;
	wxBitmap addBitmap, addAllBitmap, removeBitmap, removeAllBitmap;
	DECLARE_EVENT_TABLE()
};

class ReportPage : public wxWizardPage
{
public:
	ReportPage(wxWizard *parent, wxWizardPage *prev);
	~ReportPage();
	virtual wxWizardPage *GetPrev() const
	{
		return m_prev;
	};
	virtual wxWizardPage *GetNext() const
	{
		return m_next;
	};
	void SetPrev(wxWizardPage *prev)
	{
		m_prev = prev;
	}
	void SetNext(wxWizardPage *next)
	{
		m_next = next;
	}
	wxTextCtrl *results;
private:
	void OnWizardPageChanging(wxWizardEvent &event);
	wxWizardPage *m_prev, *m_next;
	ddDBReverseEngineering *wparent;
	DECLARE_EVENT_TABLE()
};
#endif
