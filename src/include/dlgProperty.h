//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2004, The pgAdmin Development Team
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
#include <wx/xrc/xmlres.h>

class pgSchema;
class pgTable;
class frmMain;
class ctlSecurityPanel;

#define txtName         CTRL_TEXT("txtName")
#define txtOID          CTRL_TEXT("txtOID")
#define stComment       CTRL_STATIC("stComment")
#define txtComment      CTRL_TEXT("txtComment")
#define lstColumns      CTRL_LISTVIEW("lstColumns")
#define cbColumns       CTRL_COMBOBOX("cbColumns")

class dlgProperty : public DialogWithHelp
{
public:
    static void CreateObjectDialog(frmMain *frame, pgObject *node, int type);
    static void EditObjectDialog(frmMain *frame, ctlSQLBox *sqlbox, pgObject *node);

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

protected:
    dlgProperty(frmMain *frame, const wxString &resName);
    ~dlgProperty();

    void EnableOK(bool enable);
    void ShowObject();

    void CheckValid(bool &enable, const bool condition, const wxString &msg);
    static dlgProperty *CreateDlg(frmMain *frame, pgObject *node, bool asNew, int type=-1);
    void AppendNameChange(wxString &sql);
    void AppendComment(wxString &sql, const wxString &objType, pgSchema *schema, pgObject *obj);
    void AppendComment(wxString &sql, const wxString &objName, pgObject *obj);
    void AppendQuoted(wxString &sql, const wxString &name);

    void OnPageSelect(wxNotebookEvent& event);
    void OnOK(wxCommandEvent &ev);
    void OnCancel(wxCommandEvent &ev);
    void OnApply(wxCommandEvent &ev);
    void OnClose(wxCloseEvent &ev);


    pgConn *connection;
    pgDatabase *database;

    frmMain *mainForm;
    ctlSQLBox *sqlPane;

    wxTextValidator numericValidator;

    wxTextCtrl *statusBox;
    wxNotebook *nbNotebook;
    int width, height;
    wxTreeItemId item;
    int objectType;
    bool readOnly;
    bool processing;

private:
    bool tryUpdate(wxTreeItemId collectionItem);
    bool apply(const wxString &sql);

    DECLARE_EVENT_TABLE();
};


#define cbDatatype      CTRL_COMBOBOX("cbDatatype")


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



class dlgOidProperty : public dlgProperty
{
public:

protected:
    dlgOidProperty(frmMain *frame, const wxString &resName);
    void OnOK(wxCommandEvent &ev);
    bool executeSql();
    virtual wxString GetInsertSql() =0;
    virtual wxString GetUpdateSql() =0;
    wxString GetSql();

    DECLARE_EVENT_TABLE();

    OID oid;
};


#endif
