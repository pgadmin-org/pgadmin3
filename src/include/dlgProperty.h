//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002 - 2003, The pgAdmin Development Team
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

#define txtName         CTRL("txtName",         wxTextCtrl)
#define txtOID          CTRL("txtOID",          wxTextCtrl)
#define txtComment      CTRL("txtComment",      wxTextCtrl)
#define lstColumns      CTRL("lstColumns",      wxListCtrl)
#define cbColumns       CTRL("cbColumns",       wxComboBox)
#define btnOK           CTRL("btnOK",           wxButton)
#define btnCancel       CTRL("btnCancel",       wxButton)


class dlgProperty : public DialogWithHelp
{
public:
    static void CreateObjectDialog(frmMain *frame, wxListCtrl *properties, pgObject *node, int type);
    static void EditObjectDialog(frmMain *frame, wxListCtrl *properties, wxListCtrl *statistics, ctlSQLBox *sqlbox, pgObject *node);

    wxString GetName();

    virtual wxString GetSql() =0;
    virtual pgObject *CreateObject(pgCollection *collection) =0;
    virtual pgObject *GetObject() =0;

    virtual void CreateAdditionalPages();
    virtual wxString GetHelpPage() const;
    void SetConnection(pgConn *conn) { connection=conn; }
    virtual int Go(bool modal=false);

protected:
    dlgProperty(frmMain *frame, const wxString &resName);

    void EnableOK(bool enable);

    void CreateListColumns(wxListCtrl *list, const wxString &left, const wxString &right, int leftSize=100);
    int AppendListItem(wxListCtrl *list, const wxString& str1, const wxString& str2, int icon);

    void CheckValid(bool &enable, const bool condition, const wxString &msg);
    static dlgProperty *CreateDlg(frmMain *frame, pgObject *node, bool asNew, int type=-1);
    void AppendComment(wxString &sql, const wxString &objType, pgSchema *schema, pgObject *obj);
    void AppendComment(wxString &sql, const wxString &objName, pgObject *obj);
    void AppendQuoted(wxString &sql, const wxString &name);

    void OnPageSelect(wxNotebookEvent& event);
    void OnOK(wxNotifyEvent &ev);
    void OnCancel(wxNotifyEvent &ev);
    void OnClose(wxCloseEvent &ev);


    pgConn *connection;

    ctlSQLBox *sqlPane;
    wxListCtrl *properties, *statistics;
    ctlSQLBox *sqlFormPane;

    wxTextValidator numericValidator;

    wxTextCtrl *statusBox;
    wxNotebook *nbNotebook;
    int width, height;
    wxTreeItemId item;
    int objectType;
    bool readOnly;

private:
    bool tryUpdate(wxTreeItemId collectionItem);

    DECLARE_EVENT_TABLE();
};


#define cbDatatype      CTRL("cbDatatype",      wxComboBox)


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
    void FillDatatype(wxComboBox *cb, bool withDomains=true);
    void FillDatatype(wxComboBox *cb, wxComboBox *cb2, bool withDomains=true);

    bool isVarLen, isVarPrec;
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
    dlgCollistProperty(frmMain *frame, const wxString &resName, wxListCtrl *colList);

    wxListCtrl *columns;
    pgTable *table;
};


class dlgSecurityProperty : public dlgProperty
{
protected:
    dlgSecurityProperty(frmMain *frame, pgObject *obj, const wxString &resName, const wxString& privilegeList, char *privilegeChar);
    ~dlgSecurityProperty();
    void AddGroups(wxComboBox *comboBox=0);
    void AddUsers(wxComboBox *comboBox=0);

    wxString GetGrant(const wxString &allPattern, const wxString &grantObject);
    void EnableOK(bool enable);
    virtual wxString GetHelpPage() const;

private:
    void OnPrivSelChange(wxListEvent &ev);
    void OnAddPriv(wxNotifyEvent& ev);
    void OnDelPriv(wxNotifyEvent& ev);
    void OnPrivCheck(wxCommandEvent& ev);
    void OnPrivCheckAll(wxCommandEvent& ev);
    void OnPrivCheckAllGrant(wxCommandEvent& ev);

    void ExecPrivCheck(int index);
    bool GrantAllowed() const;

    bool securityChanged;
    wxArrayString currentAcl;
    wxListView *lbPrivileges;
    wxComboBox *cbGroups;
    wxStaticText *stGroup;
    wxButton *btnAddPriv, *btnDelPriv;
    int privilegeCount;
    char *privilegeChars;
    wxCheckBox **privCheckboxes;
    wxCheckBox *allPrivileges, *allPrivilegesGrant;
    DECLARE_EVENT_TABLE();
};


#endif
