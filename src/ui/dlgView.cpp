//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2004, The pgAdmin Development Team
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
#define pnlDefinition   CTRL_PANEL("pnlDefinition")
#define txtSqlBox       CTRL_SQLBOX("txtSqlBox")



BEGIN_EVENT_TABLE(dlgView, dlgSecurityProperty)
    EVT_TEXT(XRCID("txtName"),                      dlgView::OnChange)
    EVT_TEXT(XRCID("txtComment"),                   dlgView::OnChange)
    EVT_STC_MODIFIED(XRCID("txtSqlBox"),            dlgView::OnChangeStc)
END_EVENT_TABLE();


dlgView::dlgView(frmMain *frame, pgView *node, pgSchema *sch)
: dlgSecurityProperty(frame, node, wxT("dlgView"), wxT("INSERT,SELECT,UPDATE,DELETE,RULE,REFERENCE,TRIGGER"), "arwdRxt")
{
    SetIcon(wxIcon(view_xpm));
    schema=sch;
    view=node;

    txtOID->Disable();
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
        txtSqlBox->SetText(oldDefinition);
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


void dlgView::OnChangeStc(wxStyledTextEvent &ev)
{
    OnChange(*(wxCommandEvent*)&ev);
}


void dlgView::OnChange(wxCommandEvent &ev)
{
    wxString name=GetName();
    if (view)
    {
        EnableOK(txtComment->GetValue() != view->GetComment()
              || txtSqlBox->GetText() != oldDefinition
              || name != view->GetName());
    }
    else
    {
        bool enable=true;

        CheckValid(enable, !name.IsEmpty(), _("Please specify name."));
        CheckValid(enable, txtSqlBox->GetText().Length() > 14 , _("Please enter function definition."));

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

    if (!view || txtSqlBox->GetText() != oldDefinition)
    {
        sql += wxT("CREATE OR REPLACE VIEW ") + schema->GetQuotedPrefix() + qtIdent(name) + wxT(" AS\n")
            + txtSqlBox->GetText()
            + wxT(";\n");
    }

    sql +=  GetGrant(wxT("arwdRxt"), wxT("TABLE ") + schema->GetQuotedPrefix() + qtIdent(name));

    AppendComment(sql, wxT("VIEW"), schema, view);
    return sql;
}

