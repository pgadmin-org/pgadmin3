//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002 - 2003, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// dlgView.cpp - PostgreSQL View Property
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "misc.h"
#include "pgDefs.h"

#include "ctlSQLBox.h"
#include "dlgView.h"
#include "pgView.h"
#include "pgCollection.h"


// Images
#include "images/view.xpm"


// pointer to controls
#define pnlDefinition CTRL("pnlDefinition", wxPanel)
#define txtSqlBox CTRL("txtSqlBox", wxTextCtrl)


#define CTL_SQLBOX  188

BEGIN_EVENT_TABLE(dlgView, dlgSecurityProperty)
    EVT_TEXT(XRCID("txtName"),                      dlgView::OnChange)
    EVT_TEXT(XRCID("txtComment"),                   dlgView::OnChange)
    EVT_STC_MODIFIED(CTL_SQLBOX,                    dlgView::OnChange)
END_EVENT_TABLE();


dlgView::dlgView(frmMain *frame, pgView *node, pgSchema *sch)
: dlgSecurityProperty(frame, node, wxT("dlgView"), wxT("INSERT,SELECT,UPDATE,DELETE,RULE,REFERENCE,TRIGGER"), "arwdRxt")
{
    SetIcon(wxIcon(view_xpm));
    schema=sch;
    view=node;

    txtOID->Disable();

    sqlBox=new ctlSQLBox(pnlDefinition, CTL_SQLBOX, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxSUNKEN_BORDER | wxTE_RICH2);

    wxWindow *placeholder=CTRL("txtSqlBox", wxTextCtrl);
    wxSizer *sizer=placeholder->GetContainingSizer();
    sizer->Add(sqlBox, 1, wxRIGHT|wxGROW, 5);
    sizer->Remove(placeholder);
    delete placeholder;
    sizer->Layout();
}


pgObject *dlgView::GetObject()
{
    return view;
}


int dlgView::Go(bool modal)
{
    AddGroups();
    AddUsers();

    if (view)
    {
        // edit mode

        oldDefinition=view->GetFormattedDefinition();
        txtName->SetValue(view->GetName());
        txtOID->SetValue(NumToStr(view->GetOid()));
        txtComment->SetValue(view->GetComment());
        sqlBox->SetText(oldDefinition);
    }
    else
    {
        // create mode
    }

    return dlgSecurityProperty::Go(modal);
}


pgObject *dlgView::CreateObject(pgCollection *collection)
{
    pgObject *obj=pgView::ReadObjects(collection, 0, 
        wxT("\n   AND c.relname=") + qtString(txtName->GetValue()) +
        wxT("\n   AND c.relnamespace=") + schema->GetOidStr());
    return obj;
}


void dlgView::OnChange(wxNotifyEvent &ev)
{
    wxString name=GetName();
    if (view)
    {
        EnableOK(txtComment->GetValue() != view->GetComment()
              || sqlBox->GetText() != oldDefinition
              || name != view->GetName());
    }
    else
    {
        bool enable=true;

        CheckValid(enable, !name.IsEmpty(), _("Please specify name."));
        CheckValid(enable, sqlBox->GetText().Length() > 14 , _("Please enter function definition."));

        EnableOK(enable);
    }
}


wxString dlgView::GetSql()
{
    wxString sql, name=GetName();


    if (view)
    {
        // edit mode

        if (name != view->GetName())
        {
            sql += wxT("ALTER TABLE ") + view->GetQuotedFullIdentifier()
                +  wxT(" RENAME TO ") + name + wxT(";\n");
        }
    }

    if (!view || sqlBox->GetText() != oldDefinition)
    {
        sql += wxT("CREATE OR REPLACE VIEW ") + schema->GetQuotedFullIdentifier()
            + wxT(".") + qtIdent(name) + wxT(" AS\n")
            + sqlBox->GetText()
            + wxT(";\n");
    }

    sql +=  GetGrant(wxT("arwdRxt"), wxT("TABLE ") + schema->GetQuotedFullIdentifier() + wxT(".") + qtIdent(name));

    AppendComment(sql, wxT("VIEW"), schema, view);
    return sql;
}

