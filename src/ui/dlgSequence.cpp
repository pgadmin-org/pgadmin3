//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002 - 2003, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// dlgSequence.cpp - PostgreSQL Sequence Property
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "misc.h"
#include "dlgSequence.h"

#include "pgSchema.h"
#include "pgSequence.h"

// Images
#include "images/sequence.xpm"


#define txtIncrement        CTRL("txtIncrement",    wxTextCtrl)
#define txtMin              CTRL("txtMin",          wxTextCtrl)
#define txtMax              CTRL("txtMax",          wxTextCtrl)
#define txtStart            CTRL("txtStart",        wxTextCtrl)
#define txtCache            CTRL("txtCache",        wxTextCtrl)
#define chkCycled           CTRL("chkCycled",       wxCheckBox)
#define stStart             CTRL("stStart",         wxStaticText)

// pointer to controls

BEGIN_EVENT_TABLE(dlgSequence, dlgSecurityProperty)
    EVT_TEXT(XRCID("txtName"),                      dlgSequence::OnChange)
    EVT_TEXT(XRCID("txtStart"),                     dlgSequence::OnChange)
    EVT_TEXT(XRCID("txtComment"),                   dlgSequence::OnChange)
END_EVENT_TABLE();


dlgSequence::dlgSequence(frmMain *frame, pgSequence *node, pgSchema *sch)
: dlgSecurityProperty(frame, node, wxT("dlgSequence"), wxT("INSERT,SELECT,UPDATE,DELETE,RULE,REFERENCE,TRIGGER"), "arwdRxt")
{
    SetIcon(wxIcon(sequence_xpm));
    schema=sch;
    sequence=node;

    txtOID->Disable();
}


pgObject *dlgSequence::GetObject()
{
    return sequence;
}


int dlgSequence::Go(bool modal)
{
    AddGroups();
    AddUsers();

    if (sequence)
    {
        // edit mode
        txtName->SetValue(sequence->GetName());
        txtOID->SetValue(NumToStr((long)sequence->GetOid()));
        txtComment->SetValue(sequence->GetComment());
        txtIncrement->SetValue(sequence->GetIncrement().ToString());
        txtStart->SetValue(sequence->GetLastValue().ToString());
        txtMin->SetValue(sequence->GetMinValue().ToString());
        txtMax->SetValue(sequence->GetMaxValue().ToString());
        txtCache->SetValue(sequence->GetCacheValue().ToString());
        chkCycled->SetValue(sequence->GetCycled());

        stStart->SetLabel(wxT("Current value"));

        txtIncrement->Disable();
        txtMin->Disable();
        txtMax->Disable();
        txtCache->Disable();
        chkCycled->Disable();
    }
    else
    {
        // create mode
        txtIncrement->SetValidator(numericValidator);
        txtMin->SetValidator(numericValidator);
        txtMax->SetValidator(numericValidator);
        txtCache->SetValidator(numericValidator);
    }

    txtStart->SetValidator(numericValidator);

    return dlgSecurityProperty::Go(modal);
}


pgObject *dlgSequence::CreateObject(pgCollection *collection)
{
    pgObject *obj=pgSequence::ReadObjects(collection, 0, 
        wxT("   AND relname=") + qtString(GetName()) +
        wxT("\n   AND relnamespace=") + schema->GetOidStr());
         
    return obj;
}


void dlgSequence::OnChange(wxNotifyEvent &ev)
{
    if (sequence)
    {
        EnableOK(txtComment->GetValue() != sequence->GetComment()
               || txtStart->GetValue() != sequence->GetLastValue().ToString());
    }
    else
    {
        wxString name=GetName();

        bool enable=true;
        CheckValid(enable, !name.IsEmpty(), _("Please specify name."));
        EnableOK(enable);
    }
}


wxString dlgSequence::GetSql()
{
    wxString sql;

    if (sequence)
    {
        // edit mode
        if (GetName() != sequence->GetName())
        {
            sql += wxT("ALTER TABLE ") + sequence->GetQuotedFullIdentifier()
                +  wxT(" RENAME TO ") + GetName() + wxT(";\n");
        }

        if (txtStart->GetValue() != sequence->GetLastValue().ToString())
            sql += wxT("SELECT setval(") + sequence->GetQuotedFullIdentifier()
                +  wxT(", ") + txtStart->GetValue()
                +  wxT(");\n");
    }
    else
    {
        // create mode
        sql = wxT("CREATE SEQUENCE ") + schema->GetQuotedFullIdentifier()
            + wxT(".") + qtIdent(GetName());
        if (chkCycled->GetValue())
            sql += wxT(" CYCLE");
        AppendIfFilled(sql, wxT("\n   INCREMENT "), txtIncrement->GetValue());
        AppendIfFilled(sql, wxT("\n   START "), txtStart->GetValue());
        AppendIfFilled(sql, wxT("\n   MINVALUE "), txtMin->GetValue());
        AppendIfFilled(sql, wxT("\n   MAXVALUE "), txtMax->GetValue());
        AppendIfFilled(sql, wxT("\n   CACHE "), txtMax->GetValue());

        sql += wxT(";\n");
    }

    sql +=  GetGrant(wxT("arwdRxt"), wxT("TABLE ") + schema->GetQuotedFullIdentifier() + wxT(".") + qtIdent(GetName()));
    AppendComment(sql, wxT("Sequence"), schema, sequence);

    return sql;
}
