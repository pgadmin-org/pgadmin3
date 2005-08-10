//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2005, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// frmIndexcheck.cpp - Index check dialogue
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>
#include <wx/settings.h>


// App headers
#include "pgAdmin3.h"
#include "frmIndexcheck.h"
#include "sysLogger.h"
#include "pgTable.h"
#include "ctlSecurityPanel.h"


// Icons
#include "images/index.xpm"


#define chkList     CTRL_CHECKLISTBOX("chkList")


BEGIN_EVENT_TABLE(frmIndexcheck, ExecutionDialog)
    EVT_BUTTON(XRCID("btnChkAll"), frmIndexcheck::OnCheckAll)
    EVT_BUTTON(XRCID("btnUnchkAll"), frmIndexcheck::OnUncheckAll)
END_EVENT_TABLE()




frmIndexcheck::frmIndexcheck(frmMain *form, pgObject *obj) : ExecutionDialog(form, obj)
{
    wxLogInfo(wxT("Creating a Index Check dialogue for %s %s"), object->GetTypeName().c_str(), object->GetFullName().c_str());

    nbNotebook = 0;
    wxWindowBase::SetFont(settings->GetSystemFont());
    LoadResource(form, wxT("frmIndexCheck"));
    RestorePosition();

    SetTitle(wxString::Format(_("Check Foreign Key indexes on %s %s"), object->GetTranslatedTypeName().c_str(), object->GetFullIdentifier().c_str()));

    nbNotebook = CTRL_NOTEBOOK("nbNotebook");

    // Icon
    SetIcon(wxIcon(index_xpm));
    txtMessages = CTRL_TEXT("txtMessages");

    sqlPane = new ctlSQLBox(nbNotebook, CTL_PROPSQL, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxSUNKEN_BORDER | wxTE_READONLY | wxTE_RICH2);
    nbNotebook->AddPage(sqlPane, wxT("SQL"));

    txtMessages = new wxTextCtrl(nbNotebook, CTL_MSG, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY|wxHSCROLL);
    nbNotebook->AddPage(txtMessages, _("Messages"));

    CenterOnParent();
}


frmIndexcheck::~frmIndexcheck()
{
    wxLogInfo(wxT("Destroying a Index Check dialogue"));
    SavePosition();
    Abort();
}


wxString frmIndexcheck::GetHelpPage() const
{
    wxString page;

    return page;
}



wxString frmIndexcheck::GetSql()
{
    wxString sql;

    return sql;
}



void frmIndexcheck::OnUncheckAll(wxCommandEvent& event)
{
    int i;
    for (i=0 ; i < chkList->GetCount() ; i++)
        chkList->Check(i, false);
}


void frmIndexcheck::OnCheckAll(wxCommandEvent& event)
{
    int i;
    for (i=0 ; i < chkList->GetCount() ; i++)
        chkList->Check(i, true);
}


void frmIndexcheck::OnPageSelect(wxNotebookEvent& event)
{
    if (nbNotebook && sqlPane && event.GetSelection() == (int)nbNotebook->GetPageCount()-2)
    {
        sqlPane->SetReadOnly(false);
        sqlPane->SetText(GetSql());
        sqlPane->SetReadOnly(true);
    }
}


void frmIndexcheck::AddObjects(const wxString &where)
{
    pgSet *set=object->GetConnection()->ExecuteSet(
        wxT("SELECT nl.nspname, cl.relname, ct.conname\n")
        wxT("  FROM pg_constraint ct\n")
        wxT("  JOIN pg_class cl ON cl.oid=conrelid\n")
        wxT("  JOIN pg_namespace nl ON nl.oid=cl.relnamespace\n")
        wxT(" WHERE ct.contype='f' AND ") + where);

    if (set)
    {
        while (!set->Eof())
        {
            chkList->Append(qtIdent(set->GetVal(wxT("conname"))) +
                    wxT(" ON ") + qtIdent(set->GetVal(wxT("nspname"))) +
                    wxT(".") + qtIdent(set->GetVal(wxT("relname"))));
            set->MoveNext();
        }
        delete set;
    }
}


void frmIndexcheck::Go()
{
    chkList->SetFocus();

    switch (object->GetType())
    {
        case PG_FOREIGNKEY:
            AddObjects(wxT("ct.oid = ") + object->GetOidStr());
            break;
        case PG_CONSTRAINTS:
            AddObjects(wxT("cl.oid = ") + object->GetOidStr());
            break;
        default:
        {
            break;
        }
    }
    switch (object->GetMetaType())
    {
        case PGM_SCHEMA:
            AddObjects(wxT("nl.oid = ") + object->GetOidStr());
            break;
        case PGM_TABLE:
        {
            if (object->IsCollection())
                AddObjects(wxT("nl.oid = ") + ((pgTable*)object)->GetSchema()->GetOidStr());
            else
            AddObjects(wxT("cl.oid = ") + object->GetOidStr());
            break;
        }
        default:
            break;
    }
    Show(true);
}



indexCheckFactory::indexCheckFactory(wxMenu *mnu, wxToolBar *toolbar)
{
    mnu->Append(id, _("&FK Index check"), _("Checks existence of foreign key indexes"));
}


wxWindow *indexCheckFactory::StartDialog(pgFrame *form, pgObject *obj)
{
    frmIndexcheck *frm=new frmIndexcheck((frmMain*)form, obj);
    frm->Go();
    return frm;
}


bool indexCheckFactory::CheckEnable(pgObject *obj)
{
    if (obj)
    {
        switch (obj->GetType())
        {
            case PG_FOREIGNKEY:
            case PG_CONSTRAINTS:
                return true;
        }
        switch (obj->GetMetaType())
        {
            case PGM_SCHEMA:
            case PGM_TABLE:
                return true;
        }
    }
    return false;
}
