//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002 - 2003, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// dlgTrigger.cpp - PostgreSQL Trigger Property
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "misc.h"
#include "pgDefs.h"

#include "dlgTrigger.h"
#include "pgTrigger.h"
#include "pgTable.h"
#include "pgSchema.h"

// Images
#include "images/trigger.xpm"


// pointer to controls
#define chkRow          CTRL("chkRow",          wxCheckBox)
#define cbFunction      CTRL("cbFunction",      wxComboBox)
#define txtArguments    CTRL("txtArguments",    wxTextCtrl)
#define rdbFires        CTRL("rdbFires",        wxRadioBox)
#define chkInsert       CTRL("chkInsert",       wxCheckBox)
#define chkUpdate       CTRL("chkUpdate",       wxCheckBox)
#define chkDelete       CTRL("chkDelete",       wxCheckBox)


BEGIN_EVENT_TABLE(dlgTrigger, dlgProperty)
    EVT_TEXT(XRCID("txtName"),                      dlgTrigger::OnChange)
    EVT_CHECKBOX(XRCID("chkInsert"),                dlgTrigger::OnChange)
    EVT_CHECKBOX(XRCID("chkUpdate"),                dlgTrigger::OnChange)
    EVT_CHECKBOX(XRCID("chkDelete"),                dlgTrigger::OnChange)
    EVT_COMBOBOX(XRCID("cbFunction"),               dlgTrigger::OnChange)
    EVT_TEXT(XRCID("txtArguments"),                 dlgTrigger::OnChange)
    EVT_TEXT(XRCID("txtComment"),                   dlgTrigger::OnChange)
END_EVENT_TABLE();


dlgTrigger::dlgTrigger(frmMain *frame, pgTrigger *node, pgTable *parentNode)
: dlgProperty(frame, wxT("dlgTrigger"))
{
    SetIcon(wxIcon(trigger_xpm));
    trigger=node;
    table=parentNode;
    wxASSERT(!table || table->GetType() == PG_TABLE);

    txtOID->Disable();
}


pgObject *dlgTrigger::GetObject()
{
    return trigger;
}


int dlgTrigger::Go(bool modal)
{
    if (trigger)
    {
        // edit mode
        txtName->SetValue(trigger->GetName());

        chkRow->SetValue((trigger->GetTriggerType() & TRIGGER_TYPE_ROW) != 0);
        chkInsert->SetValue((trigger->GetTriggerType() & TRIGGER_TYPE_INSERT) != 0);
        chkUpdate->SetValue((trigger->GetTriggerType() & TRIGGER_TYPE_UPDATE) != 0);
        chkDelete->SetValue((trigger->GetTriggerType() & TRIGGER_TYPE_DELETE) != 0);
        rdbFires->SetSelection(trigger->GetTriggerType() & TRIGGER_TYPE_BEFORE ? 0 : 1);
        txtArguments->SetValue(trigger->GetArguments());
        txtName->Disable();
        chkRow->Disable();
        cbFunction->Append(trigger->GetFunction());
        cbFunction->SetSelection(0);
        txtArguments->Disable();
        cbFunction->Disable();
        rdbFires->Disable();
        chkInsert->Disable();
        chkUpdate->Disable();
        chkDelete->Disable();
    }
    else
    {
        // create mode
        wxString sysRestr;
        if (!settings->GetShowSystemObjects())
            sysRestr = wxT(" AND pronamespace >") + NumToStr(connection->GetLastSystemOID());

        pgSet *set=connection->ExecuteSet(
            wxT("SELECT proname FROM pg_proc WHERE prorettype=") + NumToStr(PGOID_TYPE_TRIGGER) + sysRestr);
        if (set)
        {
            while (!set->Eof())
            {
                cbFunction->Append(set->GetVal(0));
                set->MoveNext();
            }
            delete set;
        }
        if (!connection->BackendMinimumVersion(7, 4))
        {
            chkRow->SetValue(true);
            chkRow->Disable();
        }
    }
    return dlgProperty::Go(modal);
}


wxString dlgTrigger::GetSql()
{
    wxString sql;

    if (trigger)
    {
        // nothing
    }
    else
    {
        sql = wxT("CREATE TRIGGER ") + qtIdent(GetName());
        if (rdbFires->GetSelection())
            sql += wxT(" AFTER");
        else
            sql += wxT(" BEFORE");
        int actionCount=0;
        if (chkInsert->GetValue())
        {
            if (actionCount++)
                sql += wxT(", ");
            sql += wxT(" INSERT");
        }
        if (chkUpdate->GetValue())
        {
            if (actionCount++)
                sql += wxT(", ");
            sql += wxT(" UPDATE");
        }
        if (chkDelete->GetValue())
        {
            if (actionCount++)
                sql += wxT(", ");
            sql += wxT(" DELETE");
        }
        sql += wxT("\n   ON ") + table->GetQuotedFullIdentifier()
            + wxT(" FOR EACH ");
        if (chkRow->GetValue())
            sql += wxT("ROW");
        else
            sql += wxT("STATEMENT");
        sql += wxT("\n   EXECUTE PROCEDURE ") + qtIdent(cbFunction->GetValue())
            + wxT("(") + txtArguments->GetValue()
            + wxT(");\n");
    }
    AppendComment(sql, wxT("TRIGGER ") + qtIdent(GetName()) 
        + wxT(" ON ") + table->GetQuotedFullIdentifier(), trigger);

    return sql;
}


pgObject *dlgTrigger::CreateObject(pgCollection *collection)
{
    pgObject *obj=pgTrigger::ReadObjects(collection, 0, 
        wxT("\n   AND tgname=") + qtString(GetName()) +
        wxT("\n   AND tgreloid=") + table->GetOidStr() +
        wxT("\n   AND relnamespace=") + table->GetSchema()->GetOidStr());
    return obj;
}


void dlgTrigger::OnChange(wxNotifyEvent &ev)
{
    if (trigger)
        btnOK->Enable(txtComment->GetValue() != trigger->GetComment());
    else
    {
        wxString function=cbFunction->GetValue();
        wxString name=GetName();

        bool enable=true;
        CheckValid(enable, !name.IsEmpty(), _("Please specify name."));
        CheckValid(enable, !function.IsEmpty(), _("Please specify trigger function."));
        CheckValid(enable, chkInsert->GetValue() || chkUpdate->GetValue() ||chkDelete->GetValue(),
            _("Please specify at least one action."));

        EnableOK(enable);
    }
}
