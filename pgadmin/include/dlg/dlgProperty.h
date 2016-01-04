//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgProperty.h - common property dialog class
//
//////////////////////////////////////////////////////////////////////////


#ifndef __DLG_PROP
#define __DLG_PROP


#include <wx/notebook.h>
#include <wx/dynarray.h>
#include "schema/pgObject.h"
#include "db/pgConn.h"
#include "ctl/ctlSecurityPanel.h"

class pgSchema;
class pgTable;
class frmMain;
class pgaFactory;

#define stComment       CTRL_STATIC("stComment")
#define lstColumns      CTRL_LISTVIEW("lstColumns")
#define cbColumns       CTRL_COMBOBOX("cbColumns")

class dataType
{
public:
	void SetTypename(wxString name);
	wxString GetTypename();

	void SetOid(OID id);
	OID GetOid();

private:
	wxString typeName;
	OID      oid;

};


/*
 * We need the definition of replClientData in dlgDatabase too, to hack
 * the execution of default privileges statements (sqls) before getting
 * disconnected.
 */
class replClientData : public wxClientData
{
public:
	replClientData(const wxString &c, long s, long ma, long mi)
	{
		cluster = c;
		setId = s;
		majorVer = ma;
		minorVer = mi;
	}
	wxString cluster;
	long setId;
	long majorVer;
	long minorVer;
};


WX_DEFINE_ARRAY(dataType *, dataTypeCache);

class dlgProperty : public DialogWithHelp
{
public:
	static bool CreateObjectDialog(frmMain *frame, pgObject *node, pgaFactory *factory = 0);
	static bool EditObjectDialog(frmMain *frame, ctlSQLBox *sqlbox, pgObject *node);
	void InitDialog(frmMain *frame, pgObject *node);

	wxString GetName();
	virtual wxString GetDisplayName()
	{
		return GetName();
	};

	virtual wxString GetSql() = 0;
	virtual wxString GetSql2()
	{
		return wxEmptyString;
	};
	virtual bool GetDisconnectFirst()
	{
		return false;
	};
	virtual pgObject *CreateObject(pgCollection *collection) = 0;
	virtual pgObject *GetObject() = 0;
	virtual void SetObject(pgObject *obj) {} // only necessary if apply is implemented

	virtual void CreateAdditionalPages();
	virtual wxString GetHelpPage() const;
	virtual wxString GetHelpPage(bool forCreate) const
	{
		return wxEmptyString;
	}
	virtual void SetConnection(pgConn *conn)
	{
		connection = conn;
	}
	void SetDatabase(pgDatabase *db);
	void SetDatatypeCache(dataTypeCache cache);
	virtual int Go(bool modal = false);
	virtual void CheckChange() = 0;

	virtual bool WannaSplitQueries()
	{
		return false;
	}

protected:
	dlgProperty(pgaFactory *factory, frmMain *frame, const wxString &resName);
	~dlgProperty();

	void EnableOK(bool enable);
	void SetSqlReadOnly(bool readonly);
	virtual bool IsUpToDate()
	{
		return true;
	};
	void ShowObject();

	void FillSQLTextfield();

	void CheckValid(bool &enable, const bool condition, const wxString &msg);
	static dlgProperty *CreateDlg(frmMain *frame, pgObject *node, bool asNew, pgaFactory *factory = 0);
	void AppendNameChange(wxString &sql, const wxString &objname = wxEmptyString);
	void AppendOwnerChange(wxString &sql, const wxString &objName = wxEmptyString);
	void AppendOwnerNew(wxString &sql, const wxString &objname);
	void AppendSchemaChange(wxString &sql, const wxString &objname = wxEmptyString);
	void AppendComment(wxString &sql, const wxString &objType, pgSchema *schema, pgObject *obj);
	void AppendComment(wxString &sql, const wxString &objName, pgObject *obj);
	void AppendQuoted(wxString &sql, const wxString &name);
	void AppendQuotedType(wxString &sql, const wxString &name);
	wxString qtDbString(const wxString &str);

#if __GNUC__ >= 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 4)
	// ANSI spec 11.5 is quite brain dead about pointers of protected members: In order to access
	// them using the base class name, they can't be protected.
	// apparently, only gcc 3.4 knows that; other compilers take protected as protected.
public:

#endif

	void OnPageSelect(wxNotebookEvent &event);
	void OnOK(wxCommandEvent &ev);
	void OnChange(wxCommandEvent &ev);
	void OnChangeOwner(wxCommandEvent &ev);
	void OnChangeStc(wxStyledTextEvent &event);
	void OnChangeReadOnly(wxCommandEvent &event);

protected:
	void AddDatabases(ctlComboBoxFix *cb = 0);
	void AddGroups(ctlComboBoxFix *comboBox = 0);
	void AddUsers(ctlComboBoxFix *cb1, ctlComboBoxFix *cb2 = 0);
	void AddSchemas(ctlComboBoxFix *comboBox = 0);
	void FillCombobox(const wxString &query, ctlComboBoxFix *cb1, ctlComboBoxFix *cb2 = 0);
	void PrepareTablespace(ctlComboBoxFix *cb, const OID current = 0);
	void OnHelp(wxCommandEvent &ev);

	pgConn *connection;
	pgDatabase *database;
	pgObject *obj;

	frmMain *mainForm;
	wxPanel *sqlPane;

	wxTextValidator numericValidator;

	wxNotebook *nbNotebook;
	wxTextCtrl *txtName, *txtOid, *txtComment;
	ctlComboBox *cbOwner;
	ctlComboBox *cbSchema;
	wxComboBox *cbClusterSet;
	wxCheckBox *chkReadOnly;
	ctlSQLBox *sqlTextField1;
	ctlSQLBox *sqlTextField2;
	bool enableSQL2;

	int width, height;
	wxTreeItemId item, owneritem;
	bool readOnly;
	bool processing;
	pgaFactory *factory;
	dataTypeCache dtCache;

private:
	bool tryUpdate(wxTreeItemId collectionItem);
	bool apply(const wxString &sql, const wxString &sql2);
	wxString BuildSql(const wxString &sql);
	wxArrayString SplitQueries(const wxString &sql);

	DECLARE_EVENT_TABLE()
};


#define cbDatatype      CTRL_COMBOBOX2("cbDatatype")


class dlgTypeProperty : public dlgProperty
{
public:
	wxString GetQuotedTypename(int sel);
	wxString GetTypeOid(int sel);
	wxString GetTypeInfo(int sel);
	void AddType(const wxString &typ, const OID oid, const wxString quotedName = wxEmptyString);


	int Go(bool modal);

protected:
	dlgTypeProperty(pgaFactory *factory, frmMain *frame, const wxString &resName);
	void CheckLenEnable();
	void FillDatatype(ctlComboBox *cb, bool withDomains = true, bool addSerials = false);
	void FillDatatype(ctlComboBox *cb, ctlComboBox *cb2, bool withDomains = true, bool addSerials = false);

	bool isVarLen, isVarPrec;
	long minVarLen, maxVarLen;
	wxTextCtrl *txtLength, *txtPrecision;

private:
	wxArrayString types;
};


class dlgCollistProperty : public dlgProperty
{
public:
	int Go(bool modal);

protected:
	dlgCollistProperty(pgaFactory *factory, frmMain *frame, const wxString &resName, pgTable *table);
	dlgCollistProperty(pgaFactory *factory, frmMain *frame, const wxString &resName, ctlListView *colList);

	ctlListView *columns;
	pgTable *table;
};


class dlgSecurityProperty : public dlgProperty
{
protected:
	dlgSecurityProperty(pgaFactory *factory, frmMain *frame, pgObject *obj, const wxString &resName, const wxString &privilegeList, const char *privilegeChar);
	~dlgSecurityProperty();
	void AddGroups(ctlComboBox *comboBox = 0);
	void AddUsers(ctlComboBox *comboBox = 0);

	wxString GetGrant(const wxString &allPattern, const wxString &grantObject);
	void EnableOK(bool enable, bool ignoreSql = false);
	virtual wxString GetHelpPage() const;
	virtual int Go(bool modal = false);
	bool DisablePrivilege(const wxString &priv);
	void SetPrivilegesLayout();
	void AppendCurrentAcl(const wxString &name, const wxString &value);

#ifdef __WXMAC__
	void OnChangeSize(wxSizeEvent &ev);
#endif

	ctlSecurityPanel *securityPage;

private:

	void OnAddPriv(wxCommandEvent &ev);
	void OnDelPriv(wxCommandEvent &ev);
	bool securityChanged;

	wxArrayString currentAcl;

	DECLARE_EVENT_TABLE()
};

class ctlDefaultSecurityPanel;

class dlgDefaultSecurityProperty : public dlgSecurityProperty
{

protected:
	dlgDefaultSecurityProperty(pgaFactory *factory, frmMain *frame, pgObject *obj, const wxString &resName,
	                           const wxString &privilegeList, const char *privilegeChar, bool createDefPrivPanel = true);

	virtual int      Go(bool modal = false, bool createDefPrivs = false,
	                    const wxString &defPrivsOnTables = wxT(""),
	                    const wxString &defPrivsOnSeqs   = wxT(""),
	                    const wxString &defPrivsOnFuncs  = wxT(""),
	                    const wxString &defPrivsOnTypes  = wxT(""));

	virtual wxString GetHelpPage() const;

	void     EnableOK(bool enable, bool ignoreSql = false);
	wxString GetDefaultPrivileges(const wxString &schemaName = wxT(""));
	void     AddGroups(ctlComboBox *comboBox = 0);
	void     AddUsers(ctlComboBox *comboBox = 0);
#ifdef __WXMAC__
	void        OnChangeSize(wxSizeEvent &ev);
#endif

	bool defaultSecurityChanged;

private:
	void OnAddPriv(wxCommandEvent &ev);
	void OnDelPriv(wxCommandEvent &ev);

	ctlDefaultSecurityPanel *defaultSecurityPage;

	DECLARE_EVENT_TABLE()
};



class dlgAgentProperty : public dlgProperty
{
public:

protected:
	dlgAgentProperty(pgaFactory *factory, frmMain *frame, const wxString &resName);
	void OnOK(wxCommandEvent &ev);
	bool executeSql();
	virtual wxString GetInsertSql() = 0;
	virtual wxString GetUpdateSql() = 0;
	wxString GetSql();
	long GetRecId()
	{
		return recId;
	}

	DECLARE_EVENT_TABLE()

	long recId;
};


class propertyFactory : public contextActionFactory
{
public:
	propertyFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar);
	wxWindow *StartDialog(frmMain *form, pgObject *obj);
	bool CheckEnable(pgObject *obj);
};


class createFactory : public actionFactory
{
public:
	createFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar);
	wxWindow *StartDialog(frmMain *form, pgObject *obj);
	bool CheckEnable(pgObject *obj);
};

class dropFactory : public contextActionFactory
{
public:
	dropFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar);
	wxWindow *StartDialog(frmMain *form, pgObject *obj);
	bool CheckEnable(pgObject *obj);
};


class dropCascadedFactory : public contextActionFactory
{
public:
	dropCascadedFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar);
	wxWindow *StartDialog(frmMain *form, pgObject *obj);
	bool CheckEnable(pgObject *obj);
};

class refreshFactory : public contextActionFactory
{
public:
	refreshFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar);
	wxWindow *StartDialog(frmMain *form, pgObject *obj);
	bool CheckEnable(pgObject *obj);
};


#endif
