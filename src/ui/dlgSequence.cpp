//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
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

        txtName->Disable();
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
        txtStart->SetValidator(numericValidator);
        txtMin->SetValidator(numericValidator);
        txtMax->SetValidator(numericValidator);
        txtCache->SetValidator(numericValidator);
    }

    return dlgSecurityProperty::Go(modal);
}


pgObject *dlgSequence::CreateObject(pgCollection *collection)
{
    pgObject *obj=pgSequence::ReadObjects(collection, 0, 
        wxT("   AND relname=") + qtString(txtName->GetValue()) +
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
        wxString name=txtName->GetValue();

        bool enable=true;
        CheckValid(enable, !name.IsEmpty(), wxT("Please specify name."));
        EnableOK(enable);
    }
}


wxString dlgSequence::GetSql()
{
    wxString sql;

    if (sequence)
    {
        // edit mode
        if (txtStart->GetValue() != sequence->GetLastValue().ToString())
            sql += wxT("SELECT setval(") + sequence->GetQuotedFullIdentifier()
                +  wxT(", ") + txtStart->GetValue()
                +  wxT(");\n");
    }
    else
    {
        // create mode
        sql = wxT("CREATE SEQUENCE ") + schema->GetQuotedFullIdentifier()
            + wxT(".") + qtIdent(txtName->GetValue());
        if (chkCycled->GetValue())
            sql += wxT(" CYCLE");
        AppendIfFilled(sql, wxT("\n   INCREMENT "), txtIncrement->GetValue());
        AppendIfFilled(sql, wxT("\n   START "), txtStart->GetValue());
        AppendIfFilled(sql, wxT("\n   MINVALUE "), txtMin->GetValue());
        AppendIfFilled(sql, wxT("\n   MAXVALUE "), txtMax->GetValue());
        AppendIfFilled(sql, wxT("\n   CACHE "), txtMax->GetValue());

        sql += wxT(";\n");
    }

    sql +=  GetGrant(wxT("arwdRxt"), wxT("TABLE ") + schema->GetQuotedFullIdentifier() + wxT(".") + qtIdent(txtName->GetValue()));
    AppendComment(sql, wxT("Sequence"), sequence);

    return sql;
}
