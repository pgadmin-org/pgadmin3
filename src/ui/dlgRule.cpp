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
#define chkSelect       CTRL("chkSelect", wxCheckBox)
#define chkInsert       CTRL("chkInsert", wxCheckBox)
#define chkUpdate       CTRL("chkUpdate", wxCheckBox)
#define chkDelete       CTRL("chkDelete", wxCheckBox)
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
        txtName->SetValue(rule->GetName());
        txtOID->SetValue(NumToStr(rule->GetOid()));
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
    pgObject *obj=0; //pgRule::ReadObjects(collection, 0, wxT(""));
    return obj;
}


void dlgRule::OnChange(wxNotifyEvent &ev)
{
    wxString name=GetName();
    if (rule)
    {
        EnableOK(txtComment->GetValue() != rule->GetComment()
              || sqlBox->GetText() != oldDefinition
              || name != rule->GetName());
    }
    else
    {
        wxString name=GetName();

        bool enable=true;

        CheckValid(enable, !name.IsEmpty(), _("Please specify name."));
        CheckValid(enable, chkSelect->GetValue() || chkInsert->GetValue() || chkUpdate->GetValue() || chkDelete->GetValue(),
                    _("Please select at least one event."));
        CheckValid(enable, sqlBox->GetText().Length() > 14 , _("Please enter function definition."));

        EnableOK(enable);
    }
}


wxString dlgRule::GetSql()
{
    wxString sql, name=GetName();


    if (rule)
    {
        // edit mode

    }

    if (!rule || sqlBox->GetText() != oldDefinition)
    {
        sql += wxT("CREATE OR REPLACE RULE ") + qtIdent(name)
            + wxT(" AS ON ");
        bool needComma=false;


        sql + sqlBox->GetText()
            + wxT(";\n");
    }

    AppendComment(sql, wxT("RULE ") + qtIdent(name) 
        + wxT(" ON ") + table->GetQuotedFullIdentifier(), rule);
    return sql;
}

