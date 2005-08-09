//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2005, The pgAdmin Development Team
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

#include "ctl/ctlSQLBox.h"
#include "dlgView.h"
#include "pgView.h"
#include "pgSchema.h"



// pointer to controls
#define pnlDefinition   CTRL_PANEL("pnlDefinition")
#define txtSqlBox       CTRL_SQLBOX("txtSqlBox")



BEGIN_EVENT_TABLE(dlgView, dlgSecurityProperty)
    EVT_STC_MODIFIED(XRCID("txtSqlBox"),            dlgProperty::OnChangeStc)
END_EVENT_TABLE();


dlgProperty *pgaViewFactory::CreateDialog(frmMain *frame, pgObject *node, pgObject *parent)
{
    return new dlgView(frame, (pgView*)node, (pgSchema*)parent);
}

dlgView::dlgView(frmMain *frame, pgView *node, pgSchema *sch)
: dlgSecurityProperty(frame, node, wxT("dlgView"), wxT("INSERT,SELECT,UPDATE,DELETE,RULE,REFERENCE,TRIGGER"), "arwdRxt")
{
    schema=sch;
    view=node;
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
    pgObject *obj=viewFactory.CreateObjects(collection, 0, 
        wxT("\n   AND c.relname=") + qtString(txtName->GetValue()) +
        wxT("\n   AND c.relnamespace=") + schema->GetOidStr());
    return obj;
}


void dlgView::CheckChange()
{
    wxString name=GetName();
    if (view)
    {
        EnableOK(txtComment->GetValue() != view->GetComment()
              || txtSqlBox->GetText() != oldDefinition
              || cbOwner->GetValue() != view->GetOwner()
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
                +  wxT(" RENAME TO ") + qtIdent(name) + wxT(";\n");
        }
    }

    if (!view || txtSqlBox->GetText() != oldDefinition)
    {
        sql += wxT("CREATE OR REPLACE VIEW ") + schema->GetQuotedPrefix() + qtIdent(name) + wxT(" AS\n")
            + txtSqlBox->GetText()
            + wxT(";\n");
    }

	if (view)
		AppendOwnerChange(sql, wxT("TABLE ") + schema->GetQuotedPrefix() + qtIdent(name));
	else
		AppendOwnerNew(sql, wxT("TABLE ") + schema->GetQuotedPrefix() + qtIdent(name));


    sql +=  GetGrant(wxT("arwdRxt"), wxT("TABLE ") + schema->GetQuotedPrefix() + qtIdent(name));

    AppendComment(sql, wxT("VIEW"), schema, view);
    return sql;
}

