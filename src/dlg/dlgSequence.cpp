//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2006, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// dlgSequence.cpp - PostgreSQL Sequence Property
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "utils/misc.h"
#include "dlgSequence.h"

#include "pgSchema.h"
#include "pgSequence.h"


#define txtIncrement        CTRL_TEXT("txtIncrement")
#define cbOwner             CTRL_COMBOBOX2("cbOwner")
#define txtMin              CTRL_TEXT("txtMin")
#define txtMax              CTRL_TEXT("txtMax")
#define txtStart            CTRL_TEXT("txtStart")
#define txtCache            CTRL_TEXT("txtCache")
#define chkCycled           CTRL_CHECKBOX("chkCycled")
#define stStart             CTRL_STATIC("stStart")

// pointer to controls

BEGIN_EVENT_TABLE(dlgSequence, dlgSecurityProperty)
    EVT_TEXT(XRCID("txtStart"),                     dlgProperty::OnChange)
    EVT_TEXT(XRCID("txtMin"),                       dlgProperty::OnChange)
    EVT_TEXT(XRCID("txtMax"),                       dlgProperty::OnChange)
    EVT_TEXT(XRCID("txtCache"),                     dlgProperty::OnChange)
    EVT_TEXT(XRCID("txtIncrement"),                 dlgProperty::OnChange)
    EVT_CHECKBOX(XRCID("chkCycled"),                dlgProperty::OnChange)
END_EVENT_TABLE();


dlgProperty *pgSequenceFactory::CreateDialog(frmMain *frame, pgObject *node, pgObject *parent)
{
    return new dlgSequence(this, frame, (pgSequence*)node, (pgSchema*)parent);
}


dlgSequence::dlgSequence(pgaFactory *f, frmMain *frame, pgSequence *node, pgSchema *sch)
: dlgSecurityProperty(f, frame, node, wxT("dlgSequence"), wxT("INSERT,SELECT,UPDATE,DELETE,RULE,REFERENCES,TRIGGER,USAGE"), "arwdRxtU")
{
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

    // Find, and disable the USAGE ACL option if we're on pre 8.2
    // 8.2+ only supports SELECT, UPDATE and USAGE
    if (!connection->BackendMinimumVersion(8, 2))
    {
        // Disable the checkbox
        if (!DisablePrivilege(wxT("USAGE")))
            wxLogError(_("Failed to disable the USAGE privilege checkbox!"));
    }
    else
    {
        if (!DisablePrivilege(wxT("INSERT")))
            wxLogError(_("Failed to disable the INSERT privilege checkbox!"));
        if (!DisablePrivilege(wxT("DELETE")))
            wxLogError(_("Failed to disable the DELETE privilege checkbox!"));
        if (!DisablePrivilege(wxT("RULE")))
            wxLogError(_("Failed to disable the RULE privilege checkbox!"));
        if (!DisablePrivilege(wxT("REFERENCES")))
            wxLogError(_("Failed to disable the REFERENCES privilege checkbox!"));
        if (!DisablePrivilege(wxT("TRIGGER")))
            wxLogError(_("Failed to disable the TRIGGER privilege checkbox!"));
    }

    return dlgSecurityProperty::Go(modal);
}


pgObject *dlgSequence::CreateObject(pgCollection *collection)
{
    pgObject *obj=sequenceFactory.CreateObjects(collection, 0, 
        wxT("   AND relname=") + qtDbString(GetName()) +
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

    wxString name=GetName();

    if (sequence)
    {
        // edit mode

        if (GetName() != sequence->GetName())
        {
            sql += wxT("ALTER TABLE ") + sequence->GetQuotedFullIdentifier()
                +  wxT(" RENAME TO ") + qtIdent(name) + wxT(";\n");
        }
        if (sequence->GetOwner() != cbOwner->GetValue())
        {
            sql += wxT("ALTER TABLE ") + schema->GetQuotedPrefix() + qtIdent(name)
                +  wxT(" OWNER TO ") + qtIdent(cbOwner->GetValue()) + wxT(";\n");
        }

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
                sql += wxT("ALTER SEQUENCE ") + schema->GetQuotedPrefix() + qtIdent(name)
                    +  tmp + wxT(";\n");
            }
        }
        else
        {
            if (txtStart->GetValue() != sequence->GetLastValue().ToString())
                sql += wxT("SELECT setval('") + qtIdent(schema->GetName()) + wxT(".") + qtIdent(name)
                    +  wxT("', ") + txtStart->GetValue()
                    +  wxT(", false);\n");
        }
    }
    else
    {
        // create mode
        sql = wxT("CREATE SEQUENCE ") + schema->GetQuotedPrefix() + qtIdent(name);
        if (chkCycled->GetValue())
            sql += wxT(" CYCLE");
        AppendIfFilled(sql, wxT("\n   INCREMENT "), txtIncrement->GetValue());
        AppendIfFilled(sql, wxT("\n   START "), txtStart->GetValue());
        AppendIfFilled(sql, wxT("\n   MINVALUE "), txtMin->GetValue());
        AppendIfFilled(sql, wxT("\n   MAXVALUE "), txtMax->GetValue());
        AppendIfFilled(sql, wxT("\n   CACHE "), txtCache->GetValue());

        sql += wxT(";\n");
        if (cbOwner->GetGuessedSelection() > 0)
        {
            sql += wxT("ALTER TABLE ")  + schema->GetQuotedPrefix() + qtIdent(name)
                +  wxT(" OWNER TO ") + qtIdent(cbOwner->GetValue()) + wxT(";\n");
        }
    }

    if (!connection->BackendMinimumVersion(8, 2))
        sql +=  GetGrant(wxT("arwdRxt"), wxT("TABLE ") + schema->GetQuotedPrefix() + qtIdent(name));
    else
        sql +=  GetGrant(wxT("rwU"), wxT("TABLE ") + schema->GetQuotedPrefix() + qtIdent(name));

    AppendComment(sql, wxT("SEQUENCE"), schema, sequence);

    return sql;
}
