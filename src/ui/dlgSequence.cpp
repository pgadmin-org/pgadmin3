//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
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
#define cbOwner             CTRL("cbOwner",         wxComboBox)
#define txtMin              CTRL("txtMin",          wxTextCtrl)
#define txtMax              CTRL("txtMax",          wxTextCtrl)
#define txtStart            CTRL("txtStart",        wxTextCtrl)
#define txtCache            CTRL("txtCache",        wxTextCtrl)
#define chkCycled           CTRL("chkCycled",       wxCheckBox)
#define stStart             CTRL("stStart",         wxStaticText)

// pointer to controls

BEGIN_EVENT_TABLE(dlgSequence, dlgSecurityProperty)
    EVT_TEXT(XRCID("txtName"),                      dlgSequence::OnChange)
    EVT_TEXT(XRCID("cbOwner"),                      dlgSequence::OnChange)
    EVT_TEXT(XRCID("txtStart"),                     dlgSequence::OnChange)
    EVT_TEXT(XRCID("txtMin"),                       dlgSequence::OnChange)
    EVT_TEXT(XRCID("txtMax"),                       dlgSequence::OnChange)
    EVT_TEXT(XRCID("txtCache"),                     dlgSequence::OnChange)
    EVT_TEXT(XRCID("txtIncrement"),                 dlgSequence::OnChange)
    EVT_CHECKBOX(XRCID("chkCycled"),                dlgSequence::OnChange)

    EVT_TEXT(XRCID("txtComment"),                   dlgSequence::OnChange)
END_EVENT_TABLE();


dlgSequence::dlgSequence(frmMain *frame, pgSequence *node, pgSchema *sch)
: dlgSecurityProperty(frame, node, wxT("dlgSequence"), wxT("INSERT,SELECT,UPDATE,DELETE,RULE,REFERENCES,TRIGGER"), "arwdRxt")
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
    if (!sequence)
        cbOwner->Append(wxEmptyString);
    AddGroups();
    AddUsers(cbOwner);

    if (sequence)
    {
        // edit mode
        txtName->SetValue(sequence->GetName());
        txtOID->SetValue(NumToStr((long)sequence->GetOid()));
        cbOwner->SetValue(sequence->GetOwner());
        txtComment->SetValue(sequence->GetComment());
        txtIncrement->SetValue(sequence->GetIncrement().ToString());
        txtStart->SetValue(sequence->GetLastValue().ToString());
        txtMin->SetValue(sequence->GetMinValue().ToString());
        txtMax->SetValue(sequence->GetMaxValue().ToString());
        txtCache->SetValue(sequence->GetCacheValue().ToString());
        chkCycled->SetValue(sequence->GetCycled());

        stStart->SetLabel(_("Current value"));

        if (!connection->BackendMinimumVersion(7, 4))
        {
            txtIncrement->Disable();
            txtMin->Disable();
            txtMax->Disable();
            txtCache->Disable();
            chkCycled->Disable();
        }
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
    wxString name=GetName();
    if (sequence)
    {
        EnableOK(name != sequence->GetName() 
               || txtComment->GetValue() != sequence->GetComment()
               || cbOwner->GetValue() != sequence->GetOwner()
               || txtStart->GetValue() != sequence->GetLastValue().ToString()
               || txtMin->GetValue() != sequence->GetMinValue().ToString()
               || txtMax->GetValue() != sequence->GetMaxValue().ToString()
               || txtCache->GetValue() != sequence->GetCacheValue().ToString()
               || txtIncrement->GetValue() != sequence->GetIncrement().ToString()
               || chkCycled->GetValue() != sequence->GetCycled());
    }
    else
    {
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

        if (connection->BackendMinimumVersion(7, 4))
        {
            wxString tmp;

            if (txtIncrement->GetValue() != sequence->GetIncrement().ToString())
                tmp += wxT("\n   INCREMENT ") + txtIncrement->GetValue();

            if (txtMin->GetValue() != sequence->GetMinValue().ToString())
            {
                if (txtMin->GetValue().IsEmpty())
                    tmp += wxT("\n   NO MINVALUE");
                else
                    tmp += wxT("\n   MINVALUE ") + txtMin->GetValue();
            }

            if (txtMax->GetValue() != sequence->GetMaxValue().ToString())
            {
                if (txtMax->GetValue().IsEmpty())
                    tmp += wxT("\n   NO MAXVALUE");
                else
                    tmp += wxT("\n   MAXVALUE ") + txtMax->GetValue();
            }
            if (txtStart->GetValue() != sequence->GetLastValue().ToString())
                tmp += wxT("\n   RESTART WITH ") + txtStart->GetValue();

            if (txtCache->GetValue() != sequence->GetCacheValue().ToString())
                tmp += wxT("\n   CACHE ") + txtCache->GetValue();

            if (chkCycled->GetValue() != sequence->GetCycled())
                if (chkCycled->GetValue())
                    tmp += wxT("\n   CYCLE");
                else
                    tmp += wxT("\n   NO CYCLE");

            if (!tmp.IsEmpty())
            {
                sql += wxT("ALTER SEQUENCE ") + sequence->GetQuotedFullIdentifier()
                    +  tmp + wxT(";\n");
            }
 
        }
        else
        {
            if (txtStart->GetValue() != sequence->GetLastValue().ToString())
                sql += wxT("SELECT setval('") + sequence->GetQuotedFullIdentifier()
                    +  wxT("', ") + txtStart->GetValue()
                    +  wxT(");\n");
        }

        if (cbOwner->GetValue() != sequence->GetOwner())
        {
            sql += wxT("ALTER TABLE ") + sequence->GetQuotedFullIdentifier()
                +  wxT(" OWNER TO ") + qtIdent(cbOwner->GetValue()) + wxT(";\n");
        }
        if (GetName() != sequence->GetName())
        {
            sql += wxT("ALTER TABLE ") + sequence->GetQuotedFullIdentifier()
                +  wxT(" RENAME TO ") + GetName() + wxT(";\n");
        }
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
        AppendIfFilled(sql, wxT("\n   CACHE "), txtCache->GetValue());

        sql += wxT(";\n");
        if (cbOwner->GetSelection() > 0)
        {
            sql += wxT("ALTER TABLE ")  + schema->GetQuotedFullIdentifier()
                + wxT(".") + qtIdent(GetName())
                +  wxT(" OWNER TO ") + qtIdent(cbOwner->GetValue()) + wxT(";\n");
        }
    }

    sql +=  GetGrant(wxT("arwdRxt"), wxT("TABLE ") + schema->GetQuotedFullIdentifier() + wxT(".") + qtIdent(GetName()));
    AppendComment(sql, wxT("SEQUENCE"), schema, sequence);

    return sql;
}
