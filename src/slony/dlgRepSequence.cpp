//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2005, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// dlgRepSequence.cpp - PostgreSQL Slony-I Sequence Property
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"

#include "dlgRepSequence.h"
#include "slCluster.h"
#include "slSet.h"
#include "slSequence.h"
#include "pgDatatype.h"

// Images
#include "images/sequence.xpm"


// pointer to controls
#define txtID               CTRL_TEXT("txtID")
#define cbSequence          CTRL_COMBOBOX2("cbSequence")



BEGIN_EVENT_TABLE(dlgRepSequence, dlgProperty)
    EVT_TEXT(XRCID("txtID"),                dlgRepSequence::OnChange)
    EVT_TEXT(XRCID("cbSequence"),           dlgRepSequence::OnChange)
    EVT_COMBOBOX(XRCID("cbSequence"),       dlgRepSequence::OnChangeSel)
    EVT_TEXT(XRCID("cbSequence"),           dlgRepSequence::OnChange)
END_EVENT_TABLE();


dlgRepSequence::dlgRepSequence(frmMain *frame, slSequence *node, slSet *s)
: dlgRepProperty(frame, s->GetCluster(), wxT("dlgRepSequence"))
{
    SetIcon(wxIcon(sequence_xpm));
    sequence=node;
    set=s;
}


pgObject *dlgRepSequence::GetObject()
{
    return sequence;
}


int dlgRepSequence::Go(bool modal)
{
    txtID->SetValidator(numericValidator);

    if (sequence)
    {
        // edit mode
        cbSequence->Append(sequence->GetName());
        cbSequence->SetSelection(0);

        txtID->SetValue(NumToStr(sequence->GetSlId()));

        cbSequence->Disable();
        txtComment->Disable();
        txtID->Disable();
    }
    else
    {
        // create mode

        wxString restriction;
        if (!settings->GetShowSystemObjects())
            restriction = wxT("\n   AND ") + connection->SystemNamespaceRestriction(wxT("nspname"));

        pgSet *tabs=connection->ExecuteSet(
            wxT("SELECT DISTINCT cl.oid, nspname, relname\n")
            wxT("  FROM pg_class cl\n")
            wxT("  JOIN pg_namespace nsp ON relnamespace=nsp.oid\n")
            wxT("  LEFT JOIN ") + cluster->GetSchemaPrefix() + wxT("sl_sequence s ON s.seq_reloid=cl.oid\n")
            wxT("  WHERE s.seq_id IS NULL AND cl.relkind = 'S'") + restriction + wxT("\n")
            wxT(" ORDER BY nspname, relname")
            );

        if (tabs)
        {
            while (!tabs->Eof())
            {
                cbSequence->Append(tabs->GetVal(wxT("nspname")) + wxT(".") + tabs->GetVal(wxT("relname")),
                    (void*)tabs->GetOid(wxT("oid")));
                tabs->MoveNext();
            }
            delete tabs;
        }
    }

    return dlgProperty::Go(modal);
}


pgObject *dlgRepSequence::CreateObject(pgCollection *collection)
{
    pgObject *obj=slSequence::ReadObjects((slSetCollection*)collection, 0,
         wxT(" WHERE deq_reloid = ") + NumToStr((OID)cbSequence->GetClientData(cbSequence->GetGuessedSelection())));

    return obj;
}


void dlgRepSequence::OnChangeSel(wxCommandEvent &ev)
{
    cbSequence->GuessSelection(ev);
    OnChange(ev);
}


void dlgRepSequence::CheckChange()
{
    if (sequence)
    {
        EnableOK(txtComment->GetValue() != sequence->GetComment());
    }
    else
    {
        bool enable=true;
        CheckValid(enable, cbSequence->GetGuessedSelection() >= 0, _("Please select sequence to replicate."));

        EnableOK(enable);
    }
}



wxString dlgRepSequence::GetSql()
{
    wxString sql;

    if (sequence)
    {
        // edit mode
    }
    else
    {
        // create mode

        sql = wxT("SELECT ") + cluster->GetSchemaPrefix() + wxT("setaddsequence(")
            + NumToStr(set->GetSlId()) + wxT(", ");

        if (StrToLong(txtID->GetValue()) > 0)
            sql += txtID->GetValue();
        else
            sql += wxT("(SELECT COALESCE(MAX(seq_id), 0) + 1 FROM ") + cluster->GetSchemaPrefix() + wxT("sl_sequence)");

        sql += wxT(", ") + qtString(cbSequence->GetGuessedStringSelection())
            +  wxT(", ") + qtString(txtComment->GetValue())
            + wxT(")\n");
    }

    return sql;
}
