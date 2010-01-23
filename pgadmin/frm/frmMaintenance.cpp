//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2010, The pgAdmin Development Team
// This software is released under the BSD Licence
//
// frmMaintenance.cpp - Maintenance options selection dialogue
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>
#include <wx/settings.h>
#include <wx/xrc/xmlres.h>


// App headers
#include "pgAdmin3.h"
#include "ctl/ctlMenuToolbar.h"
#include "frm/frmMaintenance.h"
#include "frm/frmMain.h"
#include "utils/sysLogger.h"
#include "schema/pgIndex.h"

// Icons
#include "images/vacuum.xpm"


BEGIN_EVENT_TABLE(frmMaintenance, ExecutionDialog)
    EVT_RADIOBOX(XRCID("rbxAction"),    frmMaintenance::OnAction)
    EVT_CHECKBOX(XRCID("chkFull"),      frmMaintenance::OnChange)
END_EVENT_TABLE()

#define nbNotebook              CTRL_NOTEBOOK("nbNotebook")
#define rbxAction               CTRL_RADIOBOX("rbxAction")
#define chkFull                 CTRL_CHECKBOX("chkFull")
#define chkInPlace              CTRL_CHECKBOX("chkInPlace")
#define chkFreeze               CTRL_CHECKBOX("chkFreeze")
#define chkAnalyze              CTRL_CHECKBOX("chkAnalyze")
#define chkVerbose              CTRL_CHECKBOX("chkVerbose")

#define stBitmap                CTRL("stBitmap", wxStaticBitmap)



frmMaintenance::frmMaintenance(frmMain *form, pgObject *obj) : ExecutionDialog(form, obj)
{
    wxWindowBase::SetFont(settings->GetSystemFont());
    LoadResource(form, wxT("frmMaintenance"));
    RestorePosition();

    SetTitle(wxString::Format(_("Maintain %s %s"), object->GetTranslatedTypeName().c_str(), object->GetFullIdentifier().c_str()));

    txtMessages = CTRL_TEXT("txtMessages");

    // Icon
    SetIcon(wxIcon(vacuum_xpm));

    txtMessages->SetMaxLength(0L);

    if (object->GetMetaType() == PGM_INDEX || object->GetMetaType() == PGM_PRIMARYKEY || object->GetMetaType() == PGM_UNIQUE)
    {
        rbxAction->SetSelection(2);
        rbxAction->Enable(0, false);
        rbxAction->Enable(1, false);
    }
    wxCommandEvent ev;
    OnAction(ev);
}


frmMaintenance::~frmMaintenance()
{
    SavePosition();
    Abort();
}


wxString frmMaintenance::GetHelpPage() const
{
    wxString page;
    switch ((XRCCTRL(*(frmMaintenance*)this, "rbxAction", wxRadioBox))->GetSelection())
    {
        case 0: page = wxT("pg/sql-vacuum"); break;
        case 1: page = wxT("pg/sql-analyze"); break;
        case 2: page = wxT("pg/sql-reindex"); break;
    }
    return page;
}



void frmMaintenance::OnAction(wxCommandEvent& ev)
{
    bool isVacuum = (rbxAction->GetSelection() == 0);
    chkFull->Enable(isVacuum);
    chkFreeze->Enable(isVacuum);
    chkAnalyze->Enable(isVacuum);

    bool isReindex = (rbxAction->GetSelection() == 2);
    if (isReindex)
    {
        chkVerbose->SetValue(false);
    }
    chkVerbose->Enable(!isReindex);
}



void frmMaintenance::OnChange(wxCommandEvent &ev)
{
    chkInPlace->Enable(object->GetConnection()->BackendMinimumVersion(8, 5) && chkFull->GetValue());
}


wxString frmMaintenance::GetSql()
{
    wxString sql;

    switch (rbxAction->GetSelection())
    {
        case 0:
        {
            sql=wxT("VACUUM ");

            if (object->GetConnection()->BackendMinimumVersion(8, 5) && chkInPlace->GetValue())
            {
                sql += wxT("(FULL INPLACE");
                if (chkFreeze->GetValue())
                    sql += wxT(", FREEZE");
                if (chkVerbose->GetValue())
                    sql += wxT(", VERBOSE");
                if (chkAnalyze->GetValue())
                    sql += wxT(", ANALYZE");
                sql += wxT(")");
            }
            else
            {
                if (chkFull->GetValue())
                    sql += wxT("FULL ");
                if (chkFreeze->GetValue())
                    sql += wxT("FREEZE ");
                if (chkVerbose->GetValue())
                    sql += wxT("VERBOSE ");
                if (chkAnalyze->GetValue())
                    sql += wxT("ANALYZE ");
            }

            if (object->GetMetaType() != PGM_DATABASE)
                sql += object->GetQuotedFullIdentifier();
            
            break;
        }
        case 1:
        {
            sql = wxT("ANALYZE ");
            if (chkVerbose->GetValue())
                sql += wxT("VERBOSE ");
            
            if (object->GetMetaType() != PGM_DATABASE)
                sql += object->GetQuotedFullIdentifier();

            break;
        }
        case 2:
        {
            if (object->GetMetaType() == PGM_UNIQUE || object->GetMetaType() == PGM_PRIMARYKEY)
            {
                sql = wxT("REINDEX INDEX ") + object->GetQuotedFullIdentifier();
            }
            else // Database, Tables, and Index (but not Constraintes ones)
            {
                sql = wxT("REINDEX ") + object->GetTypeName().Upper()
                    + wxT(" ") + object->GetQuotedFullIdentifier();
            }
            break;
        }
    }

    return sql;
}



void frmMaintenance::Go()
{
    chkFull->SetFocus();

    chkInPlace->Enable(object->GetConnection()->BackendMinimumVersion(8, 5) && chkFull->GetValue());

    Show(true);
}



maintenanceFactory::maintenanceFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar) : contextActionFactory(list)
{
    mnu->Append(id, _("&Maintenance..."), _("Maintain the current database or table."));
    toolbar->AddTool(id, _("Maintenance"), wxBitmap(vacuum_xpm), _("Maintain the current database or table."), wxITEM_NORMAL);
}


wxWindow *maintenanceFactory::StartDialog(frmMain *form, pgObject *obj)
{
    frmMaintenance *frm=new frmMaintenance(form, obj);
    frm->Go();
    return 0;
}


bool maintenanceFactory::CheckEnable(pgObject *obj)
{
    return obj && obj->CanMaintenance();
}
