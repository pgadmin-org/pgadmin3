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


#define CTRL(id, typ) (XRCCTRL(*this, id, typ))


class dlgProperty : public wxDialog
{
public:
    static void CreateObjectDialog(frmMain *frame, wxTreeCtrl *browser, wxListCtrl *properties, pgObject *node, pgConn *conn);
    static void EditObjectDialog(frmMain *frame, wxTreeCtrl *browser, wxListCtrl *properties, wxListCtrl *statistics, ctlSQLBox *sqlbox, pgObject *node, pgConn *conn);

    virtual wxString GetSql() =0;
    virtual pgObject *CreateObject(pgCollection *collection) =0;
    virtual pgObject *GetObject() =0;

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

#endif
