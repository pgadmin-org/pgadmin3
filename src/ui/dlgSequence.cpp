//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2004, The pgAdmin Development Team
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


#define txtIncrement        CTRL_TEXT("txtIncrement")
#define cbOwner             CTRL_COMBOBOX2("cbOwner")
#define cbTablespace        CTRL_COMBOBOX("cbTablespace")
#define txtMin              CTRL_TEXT("txtMin")
#define txtMax              CTRL_TEXT("txtMax")
#define txtStart            CTRL_TEXT("txtStart")
#define txtCache            CTRL_TEXT("txtCache")
#define chkCycled           CTRL_CHECKBOX("chkCycled")
#define stStart             CTRL_STATIC("stStart")

// pointer to controls

BEGIN_EVENT_TABLE(dlgSequence, dlgSecurityProperty)
    EVT_TEXT(XRCID("cbTablespace"),                 dlgProperty::OnChange)
    EVT_TEXT(XRCID("txtStart"),                     dlgProperty::OnChange)
    EVT_TEXT(XRCID("txtMin"),                       dlgProperty::OnChange)
    EVT_TEXT(XRCID("txtMax"),                       dlgProperty::OnChange)
    EVT_TEXT(XRCID("txtCache"),                     dlgProperty::OnChange)
    EVT_TEXT(XRCID("txtIncrement"),                 dlgProperty::OnChange)
    EVT_CHECKBOX(XRCID("chkCycled"),                dlgProperty::OnChange)
END_EVENT_TABLE();


dlgSequence::dlgSequence(frmMain *frame, pgSequence *node, pgSchema *sch)
: dlgSecurityProperty(frame, node, wxT("dlgSequence"), wxT("INSERT,SELECT,UPDATE,DELETE,RULE,REFERENCES,TRIGGER"), "arwdRxt")
{
    SetIcon(wxIcon(sequence_xpm));
    schema=sch;
    sequence=node;
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
        PrepareTablespace(cbTablespace, schema->GetTablespace());
    }
    else
    {
        // create mode
        PrepareTablespace(cbTablespace);
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


void dlgSequence::CheckChange()
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
               || chkCycled->GetValue() != sequence->GetCycled()
               || cbTablespace->GetValue() != sequence->GetTablespace());
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

    wxString name=GetName();

    if (sequence)
    {
        // edit mode

        if (GetName() != sequence->GetName())
        {
            sql += wxT("ALTER TABLE ") + sequence->GetQuotedFullIdentifier()
                +  wxT(" RENAME TO ") + qtIdent(name) + wxT(";\n");
        }
        AppendOwnerChange(sql);

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
                sql += wxT("ALTER SEQUENCE ") + qtIdent(name)
                    +  tmp + wxT(";\n");
            }
 
        }
        else
        {
            if (txtStart->GetValue() != sequence->GetLastValue().ToString())
                sql += wxT("SELECT setval('") + qtString(name)
                    +  wxT("', ") + txtStart->GetValue()
                    +  wxT(");\n");
        }
        if (cbTablespace->GetValue() != sequence->GetTablespace())
            sql += wxT("ALTER TABLE ") + qtIdent(name)
                +  wxT(" SET TABLESPACE ") + qtIdent(cbTablespace->GetValue())
                +  wxT(";\n");
    }
    else
    {
        // create mode
        sql = wxT("CREATE SEQUENCE ") + schema->GetQuotedPrefix() + qtIdent(GetName());
        if (chkCycled->GetValue())
            sql += wxT(" CYCLE");
        AppendIfFilled(sql, wxT("\n   INCREMENT "), txtIncrement->GetValue());
        AppendIfFilled(sql, wxT("\n   START "), txtStart->GetValue());
        AppendIfFilled(sql, wxT("\n   MINVALUE "), txtMin->GetValue());
        AppendIfFilled(sql, wxT("\n   MAXVALUE "), txtMax->GetValue());
        AppendIfFilled(sql, wxT("\n   CACHE "), txtCache->GetValue());
        AppendIfFilled(sql, wxT("\n   TABLESPACE "), qtIdent(cbTablespace->GetValue()));

        sql += wxT(";\n");
        if (cbOwner->GetGuessedSelection() > 0)
        {
            sql += wxT("ALTER TABLE ")  + schema->GetQuotedPrefix() + qtIdent(GetName())
                +  wxT(" OWNER TO ") + qtIdent(cbOwner->GetValue()) + wxT(";\n");
        }
    }

    sql +=  GetGrant(wxT("arwdRxt"), wxT("TABLE ") + schema->GetQuotedPrefix() + qtIdent(GetName()));
    AppendComment(sql, wxT("SEQUENCE"), schema, sequence);

    return sql;
}
