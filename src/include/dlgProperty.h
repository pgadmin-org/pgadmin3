//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
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


class dlgProperty : public wxDialog
{
public:
    static void CreateObjectDialog(frmMain *frame, wxTreeCtrl *browser, wxListCtrl *properties, pgObject *node, pgConn *conn);
    static void EditObjectDialog(frmMain *frame, wxTreeCtrl *browser, wxListCtrl *properties, wxListCtrl *statistics, ctlSQLBox *sqlbox, pgObject *node, pgConn *conn);

    virtual wxString GetSql() =0;
    virtual pgObject *CreateObject(pgCollection *collection) =0;
    virtual pgObject *GetObject() =0;

    virtual void CreateAdditionalPages();
    virtual void Go() {}

protected:
    static dlgProperty *CreateDlg(wxFrame *frame, pgObject *node, bool asNew);
    dlgProperty(wxFrame *frame, const wxString &resName);
    void OnPageSelect(wxNotebookEvent& event);
    void OnOK(wxNotifyEvent &ev);
    void OnCancel(wxNotifyEvent &ev);

    pgConn *connection;
    ctlSQLBox *sqlPane;
    wxNotebook *notebook;

    frmMain *mainForm;
    wxListCtrl *properties, *statistics;
    ctlSQLBox *sqlFormPane;
    int sqlPageNo;
    wxTreeCtrl *browser;
    wxTreeItemId item;
    int objectType;

private:

    DECLARE_EVENT_TABLE();
};


class dlgSecurityProperty : public dlgProperty
{
protected:
    dlgSecurityProperty(wxFrame *frame, pgObject *obj, const wxString &resName, const wxString& privilegeList, char *privilegeChar);
    ~dlgSecurityProperty();
    void AddGroups(wxComboBox *comboBox=0);
    void AddUsers(wxComboBox *comboBox=0);

    wxString GetGrant(const wxString &allPattern, const wxString &grantObject);

private:
    void OnPrivSelChange(wxListEvent &ev);
    void OnAddPriv(wxNotifyEvent& ev);
    void OnDelPriv(wxNotifyEvent& ev);
    void OnPrivCheck(wxCommandEvent& ev);
    void OnPrivCheckAll(wxCommandEvent& ev);
    void OnPrivCheckAllGrant(wxCommandEvent& ev);

    void ExecPrivCheck(int index);
    bool GrantAllowed() const;

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
