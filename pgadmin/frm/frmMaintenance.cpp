//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2008, The pgAdmin Development Team
// This software is released under the Artistic Licence
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
END_EVENT_TABLE()

#define nbNotebook              CTRL_NOTEBOOK("nbNotebook")
#define rbxAction               CTRL_RADIOBOX("rbxAction")
#define sbxOptions              CTRL_STATICBOX("sbxOptions")
#define chkFull                 CTRL_CHECKBOX("chkFull")
#define chkFreeze               CTRL_CHECKBOX("chkFreeze")
#define chkAnalyze              CTRL_CHECKBOX("chkAnalyze")
#define sbxReindexOptions       CTRL_STATICBOX("sbxReindexOptions")
#define chkForce                CTRL_CHECKBOX("chkForce")
#define chkRecreate             CTRL_CHECKBOX("chkRecreate")
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
    sbxOptions->Enable(isVacuum);
    chkFull->Enable(isVacuum);
    chkFreeze->Enable(isVacuum);
    chkAnalyze->Enable(isVacuum);

    bool isReindex = (rbxAction->GetSelection() == 2);
    sbxReindexOptions->Enable(isReindex && object->GetMetaType() == PGM_DATABASE || object->GetMetaType() == PGM_INDEX || object->GetMetaType() == PGM_PRIMARYKEY || object->GetMetaType() == PGM_UNIQUE);
    chkForce->Enable(isReindex && object->GetMetaType() == PGM_DATABASE);
    chkRecreate->Enable(isReindex && object->GetMetaType() == PGM_INDEX);
}



wxString frmMaintenance::GetSql()
{
    wxString sql;

    switch (rbxAction->GetSelection())
    {
        case 0:
        {
            sql=wxT("VACUUM ");

            if (chkFull->GetValue())
                sql += wxT("FULL ");
            if (chkFreeze->GetValue())
                sql += wxT("FREEZE ");
            if (chkVerbose->GetValue())
                sql += wxT("VERBOSE ");
            if (chkAnalyze->GetValue())
                sql += wxT("ANALYZE ");

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
            else
            {
                if (chkRecreate->GetValue())
                {
                    sql = wxT("DROP INDEX ") + object->GetQuotedFullIdentifier() + wxT(";\n")
                        + ((pgIndex*)object)->GetCreate()
                        + object->GetCommentSql();
                }
                else
                {
                    sql = wxT("REINDEX ") + object->GetTypeName().Upper()
                        + wxT(" ") + object->GetQuotedFullIdentifier();
                    if (chkForce->GetValue())
                        sql += wxT(" FORCE");
                }
            }
            break;
        }
    }

    return sql;
}



void frmMaintenance::Go()
{
    chkFull->SetFocus();
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
