//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002 - 2003, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// dlgRule.cpp - PostgreSQL View Property
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "misc.h"
#include "pgDefs.h"

#include "ctlSQLBox.h"
#include "dlgRule.h"
#include "pgRule.h"
#include "pgTable.h"
#include "pgCollection.h"


// Images
#include "images/rule.xpm"


// pointer to controls
#define rbxEvent        CTRL("rbxEvent", wxRadioBox)
#define chkDoInstead    CTRL("chkDoInstead", wxCheckBox)
#define txtCondition    CTRL("txtCondition", wxTextCtrl)

#define pnlDefinition   CTRL("pnlDefinition", wxPanel)
#define txtSqlBox       CTRL("txtSqlBox", wxTextCtrl)

#define CTL_SQLBOX  188

BEGIN_EVENT_TABLE(dlgRule, dlgProperty)
    EVT_TEXT(XRCID("txtName"),                      dlgRule::OnChange)
    EVT_TEXT(XRCID("txtComment"),                   dlgRule::OnChange)
    EVT_TEXT(XRCID("txtCondition"),                 dlgRule::OnChange)
    EVT_CHECKBOX(XRCID("chkSelect"),                dlgRule::OnChange)
    EVT_CHECKBOX(XRCID("chkInsert"),                dlgRule::OnChange)
    EVT_CHECKBOX(XRCID("chkUpdate"),                dlgRule::OnChange)
    EVT_CHECKBOX(XRCID("chkDelete"),                dlgRule::OnChange)
    EVT_CHECKBOX(XRCID("chkDoInstead"),             dlgRule::OnChange)
    EVT_RADIOBOX(XRCID("rbxEvent"),                 dlgRule::OnChange)
    EVT_STC_MODIFIED(CTL_SQLBOX,                    dlgRule::OnChange)
END_EVENT_TABLE();


dlgRule::dlgRule(frmMain *frame, pgRule *node, pgTable *tab)
: dlgProperty(frame, wxT("dlgRule"))
{
    SetIcon(wxIcon(rule_xpm));
    table=tab;
    rule=node;

    txtOID->Disable();

    sqlBox=new ctlSQLBox(pnlDefinition, CTL_SQLBOX, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxSUNKEN_BORDER | wxTE_RICH2);

    wxWindow *placeholder=CTRL("txtSqlBox", wxTextCtrl);
    wxSizer *sizer=placeholder->GetContainingSizer();
    sizer->Add(sqlBox, 1, wxRIGHT|wxGROW, 5);
    sizer->Remove(placeholder);
    delete placeholder;
    sizer->Layout();
}


pgObject *dlgRule::GetObject()
{
    return rule;
}


int dlgRule::Go(bool modal)
{
    if (rule)
    {
        // edit mode

        oldDefinition=rule->GetFormattedDefinition();
        if (!oldDefinition.IsEmpty())
        {
            int doPos=oldDefinition.Find(wxT(" DO INSTEAD "));
            if (doPos > 0)
                oldDefinition = oldDefinition.Mid(doPos + 12).Strip(wxString::both);
            else
            {
                doPos = oldDefinition.Find(wxT(" DO "));
                if (doPos > 0)
                    oldDefinition = oldDefinition.Mid(doPos+4).Strip(wxString::both);
            }
        }
        txtName->SetValue(rule->GetName());
        txtOID->SetValue(NumToStr(rule->GetOid()));
        chkDoInstead->SetValue(rule->GetDoInstead());
        rbxEvent->SetStringSelection(rule->GetEvent());
        txtCondition->SetValue(rule->GetCondition());
        txtComment->SetValue(rule->GetComment());
        sqlBox->SetText(oldDefinition);

        txtName->Disable();
    }
    else
    {
        // create mode
    }

    return dlgProperty::Go(modal);
}


pgObject *dlgRule::CreateObject(pgCollection *collection)
{
    pgObject *obj=pgRule::ReadObjects(collection, 0, 
        wxT("\n   AND rulename=") + qtString(GetName()) +
        wxT("\n   AND rw.ev_class=") + table->GetOidStr());
    return obj;
}


bool dlgRule::didChange()
{
    if (!rule)
        return true;

    if (GetName() != rule->GetName())
        return true;
    if (sqlBox->GetText().Strip(wxString::both) != oldDefinition)
        return true;
    if (chkDoInstead->GetValue() != rule->GetDoInstead())
        return true;
    if (rbxEvent->GetStringSelection() != rule->GetEvent())
        return true;
    if (txtCondition->GetValue() != rule->GetCondition())
        return true;
    if (sqlBox->GetText() != oldDefinition)
        return true;

    return false;
}


void dlgRule::OnChange(wxNotifyEvent &ev)
{
    wxString name=GetName();
    if (rule)
    {
        EnableOK(didChange() || txtComment->GetValue() != rule->GetComment());
    }
    else
    {
        wxString name=GetName();

        bool enable=true;

        CheckValid(enable, !name.IsEmpty(), _("Please specify name."));
        CheckValid(enable, rbxEvent->GetSelection() >= 0,
                    _("Please select at an event."));
        CheckValid(enable, !sqlBox->GetTextLength() || sqlBox->GetTextLength() > 6 , _("Please enter function definition."));

        EnableOK(enable);
    }
}


wxString dlgRule::GetSql()
{
    wxString sql, name=GetName();


    if (!rule || didChange())
    {
        sql += wxT("CREATE OR REPLACE RULE ") + qtIdent(name)
            + wxT(" AS\n   ON ") + rbxEvent->GetStringSelection()
            + wxT(" TO ") + table->GetQuotedFullIdentifier();
        AppendIfFilled(sql, wxT("\n   WHERE ") , txtCondition->GetValue());

        sql += wxT("\n   DO ");

        if (chkDoInstead->GetValue())
            sql += wxT("INSTEAD ");
    
        if (sqlBox->GetTextLength())
        {
            sql += wxT("\n") + sqlBox->GetText().Strip(wxString::both);
            if (sql.Right(1) != wxT(";"))
                sql += wxT(";");
        }
        else
            sql += wxT("NOTHING;");

        sql += wxT("\n");
    }
    AppendComment(sql, wxT("RULE ") + qtIdent(name) 
        + wxT(" ON ") + table->GetQuotedFullIdentifier(), rule);
    return sql;
}

