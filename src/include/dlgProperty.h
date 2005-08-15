//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2005, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// dlgProperty.h - common property dialog class
//
//////////////////////////////////////////////////////////////////////////


#ifndef __DLG_PROP
#define __DLG_PROP


#include <wx/notebook.h>
#include "pgObject.h"
#include "pgConn.h"

class pgSchema;
class pgTable;
class frmMain;
class ctlSecurityPanel;
class pgaFactory;

#define stComment       CTRL_STATIC("stComment")
#define lstColumns      CTRL_LISTVIEW("lstColumns")
#define cbColumns       CTRL_COMBOBOX("cbColumns")

class dlgProperty : public DialogWithHelp
{
public:
    static bool CreateObjectDialog(frmMain *frame, pgObject *node, int type);
    static bool EditObjectDialog(frmMain *frame, ctlSQLBox *sqlbox, pgObject *node);
    void InitDialog(frmMain *frame, pgObject *node);

    wxString GetName();

    virtual wxString GetSql() =0;
    virtual pgObject *CreateObject(pgCollection *collection) =0;
    virtual pgObject *GetObject() =0;
    virtual void SetObject(pgObject *obj) {} // only necessary if apply is implemented

    virtual void CreateAdditionalPages();
    virtual wxString GetHelpPage() const;
    void SetConnection(pgConn *conn) { connection=conn; }
    void SetDatabase(pgDatabase *db);
    virtual int Go(bool modal=false);
    virtual void CheckChange() =0;

protected:
    dlgProperty(frmMain *frame, const wxString &resName);
    ~dlgProperty();

    void EnableOK(bool enable);
    void ShowObject();

    void CheckValid(bool &enable, const bool condition, const wxString &msg);
    static dlgProperty *CreateDlg(frmMain *frame, pgObject *node, bool asNew, int type=-1);
    void AppendNameChange(wxString &sql, const wxString &objname=wxEmptyString);
    void AppendOwnerChange(wxString &sql, const wxString &objName=wxEmptyString);
    void AppendOwnerNew(wxString &sql, const wxString &objname);
    void AppendComment(wxString &sql, const wxString &objType, pgSchema *schema, pgObject *obj);
    void AppendComment(wxString &sql, const wxString &objName, pgObject *obj);
    void AppendQuoted(wxString &sql, const wxString &name);


#if __GNUC__ >= 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 4)
    // ANSI spec 11.5 is quite brain dead about pointers of protected members: In order to access 
    // them using the base class name, they can't be protected.
    // apparently, only gcc 3.4 knows that; other compilers take protected as protected.
public:

#endif

    void OnPageSelect(wxNotebookEvent& event);
    void OnOK(wxCommandEvent &ev);
    void OnApply(wxCommandEvent &ev);
    void OnChange(wxCommandEvent &ev);
    void OnChangeOwner(wxCommandEvent &ev);
    void OnChangeStc(wxStyledTextEvent& event);

protected:
    void AddUsers(wxComboBox *cb1, wxComboBox *cb2=0);
    void FillCombobox(const wxString &query, wxComboBox *cb1, wxComboBox *cb2=0);
    void PrepareTablespace(wxComboBox *cb, const wxChar *current=0);

    pgConn *connection;
    pgDatabase *database;

    frmMain *mainForm;
    ctlSQLBox *sqlPane;

    wxTextValidator numericValidator;

    wxNotebook *nbNotebook;
    wxTextCtrl *txtName, *txtOid, *txtComment;
    ctlComboBox *cbOwner;

    int width, height;
    wxTreeItemId item;
    int objectType;
    bool readOnly;
    bool processing;
    pgaFactory *factory;

private:
    bool tryUpdate(wxTreeItemId collectionItem);
    bool apply(const wxString &sql);

    DECLARE_EVENT_TABLE();
};


#define cbDatatype      CTRL_COMBOBOX2("cbDatatype")


class dlgTypeProperty : public dlgProperty
{
public:
    wxString GetQuotedTypename(int sel);
    wxString GetTypeOid(int sel);
    wxString GetTypeInfo(int sel);
    void AddType(const wxString &typ, const OID oid, const wxString quotedName=wxEmptyString);


    int Go(bool modal);

protected:
    dlgTypeProperty(frmMain *frame, const wxString &resName);
    void CheckLenEnable();
    void FillDatatype(ctlComboBox *cb, bool withDomains=true);
    void FillDatatype(ctlComboBox *cb, ctlComboBox *cb2, bool withDomains=true);

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
    dlgCollistProperty(frmMain *frame, const wxString &resName, pgTable *table);
    dlgCollistProperty(frmMain *frame, const wxString &resName, ctlListView *colList);

    ctlListView *columns;
    pgTable *table;
};


class dlgSecurityProperty : public dlgProperty
{
protected:
    dlgSecurityProperty(frmMain *frame, pgObject *obj, const wxString &resName, const wxString& privilegeList, char *privilegeChar);
    ~dlgSecurityProperty();
    void AddGroups(ctlComboBox *comboBox=0);
    void AddUsers(ctlComboBox *comboBox=0);

    wxString GetGrant(const wxString &allPattern, const wxString &grantObject);
    void EnableOK(bool enable);
    virtual wxString GetHelpPage() const;
    virtual int Go(bool modal=false);

private:

    void OnAddPriv(wxCommandEvent& ev);
    void OnDelPriv(wxCommandEvent& ev);
    bool securityChanged;

    ctlSecurityPanel *securityPage;
    wxArrayString currentAcl;

    DECLARE_EVENT_TABLE();
};



class dlgAgentProperty : public dlgProperty
{
public:

protected:
    dlgAgentProperty(frmMain *frame, const wxString &resName);
    void OnOK(wxCommandEvent &ev);
    bool executeSql();
    virtual wxString GetInsertSql() =0;
    virtual wxString GetUpdateSql() =0;
    wxString GetSql();
    long GetRecId() { return recId; }

    DECLARE_EVENT_TABLE();

    long recId;
};


class propertyFactory : public contextActionFactory
{
public:
    propertyFactory(menuFactoryList *list, wxMenu *mnu, wxToolBar *toolbar);
    wxWindow *StartDialog(frmMain *form, pgObject *obj);
    bool CheckEnable(pgObject *obj);
};


class createFactory : public actionFactory
{
public:
    createFactory(menuFactoryList *list, wxMenu *mnu, wxToolBar *toolbar);
    wxWindow *StartDialog(frmMain *form, pgObject *obj);
    bool CheckEnable(pgObject *obj);
};

class dropFactory : public contextActionFactory
{
public:
    dropFactory(menuFactoryList *list, wxMenu *mnu, wxToolBar *toolbar);
    wxWindow *StartDialog(frmMain *form, pgObject *obj);
    bool CheckEnable(pgObject *obj);
};


class dropCascadedFactory : public contextActionFactory
{
public:
    dropCascadedFactory(menuFactoryList *list, wxMenu *mnu, wxToolBar *toolbar);
    wxWindow *StartDialog(frmMain *form, pgObject *obj);
    bool CheckEnable(pgObject *obj);
};

class refreshFactory : public contextActionFactory
{
public:
    refreshFactory(menuFactoryList *list, wxMenu *mnu, wxToolBar *toolbar);
    wxWindow *StartDialog(frmMain *form, pgObject *obj);
    bool CheckEnable(pgObject *obj);
};


#endif
