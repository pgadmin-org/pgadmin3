//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
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
    EVT_TEXT(XRCID("txtComment"),                   dlgTrigger::OnChange)
END_EVENT_TABLE();


dlgTrigger::dlgTrigger(frmMain *frame, pgTrigger *node, pgTable *parentNode)
: dlgProperty(frame, wxT("dlgTrigger"))
{
    SetIcon(wxIcon(trigger_xpm));
    trigger=node;
    table=parentNode;
    wxASSERT(!table || table->GetType() == PG_TABLE);

    txtArguments->Disable();
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
        txtName->Disable();
        chkRow->Disable();
        cbFunction->Append(trigger->GetFunction());
        cbFunction->SetSelection(0);
        // txtArguments
        cbFunction->Disable();
        rdbFires->Disable();
        chkInsert->Disable();
        chkUpdate->Disable();
        chkDelete->Disable();
    }
    else
    {
        // create mode
        pgSet *set=connection->ExecuteSet(wxT(
            "SELECT proname, proargtypes FROM pg_proc WHERE prorettype=") + NumToStr(PGOID_TYPE_TRIGGER));
        if (set)
        {
            while (!set->Eof())
            {
                wxString proname=set->GetVal(0);
                wxString argtype=set->GetVal(1);
                argtype=argtype.Mid(1, argtype.Length()-2);
                argtypes.Add(argtype);
                cbFunction->Append(proname);
                set->MoveNext();
            }
            delete set;
        }
        if (connection->GetVersionNumber() <= 7.3)
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
        sql = wxT("CREATE TRIGGER ") + qtIdent(txtName->GetValue());
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
    AppendComment(sql, wxT("TRIGGER ") + qtIdent(txtName->GetValue()) + wxT(" ON ") + table->GetQuotedFullIdentifier(), trigger);

    return sql;
}


pgObject *dlgTrigger::CreateObject(pgCollection *collection)
{
    pgObject *obj=pgTrigger::ReadObjects(collection, 0, 
        wxT("\n   AND tgname=") + qtString(txtName->GetValue()) +
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
        wxString name=txtName->GetValue();

        bool enable=true;
        CheckValid(enable, !name.IsEmpty(), wxT("Please specify name."));
        CheckValid(enable, !function.IsEmpty(), wxT("Please specify trigger function."));
        CheckValid(enable, chkInsert->GetValue() || chkUpdate->GetValue() ||chkDelete->GetValue(),
            wxT("Please specify at least one action."));

        if (enable)
        {
            int argCount=0, varCount=0;
            int parenPos=function.Find('(');
            if (parenPos > 0 && function.GetChar(parenPos+1) != ')')
            {
                int commaPos=0;
                while (commaPos >= 0)
                {
                    parenPos += commaPos;
                    argCount++;
                    commaPos = function.Mid(parenPos).Find(',');
                }
            }
            txtArguments->Enable(argCount != 0);
            if (argCount)
            {
                int pos=0;
                int commaPos=0;
                wxString str=txtArguments->GetValue();
                while (commaPos >= 0)
                {
                    pos+= commaPos;
                    varCount++;
                    commaPos = str.Mid(pos).Find(',');
                }
            }
            // we compare for less/equal, maybe a string arg contains ','
            CheckValid(enable, argCount <= varCount, wxT("Please specify correct parameters."));
        }
        EnableOK(enable);
    }
}
