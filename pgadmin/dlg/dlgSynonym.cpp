//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2008, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// dlgSynonym.cpp - EnterpriseDB Synonym Property
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "utils/misc.h"
#include "utils/pgDefs.h"

#include "dlg/dlgSynonym.h"
#include "schema/edbSynonym.h"

// pointer to controls
#define txtName             CTRL_TEXT("txtName")
#define cbTargetType        CTRL_COMBOBOX2("cbTargetType")
#define cbTargetSchema      CTRL_COMBOBOX2("cbTargetSchema")
#define cbTargetObject      CTRL_COMBOBOX("cbTargetObject")
#define stComment           CTRL_STATIC("stComment")

dlgProperty *edbSynonymFactory::CreateDialog(frmMain *frame, pgObject *node, pgObject *parent)
{
    return new dlgSynonym(this, frame, (edbSynonym*)node);
}


BEGIN_EVENT_TABLE(dlgSynonym, dlgProperty)
    EVT_TEXT(XRCID("cbTargetType"),                 dlgSynonym::OnChangeTargetType)
    EVT_COMBOBOX(XRCID("cbTargetType"),             dlgProperty::OnChange)
    EVT_TEXT(XRCID("cbTargetSchema"),               dlgSynonym::OnChangeTargetSchema)
    EVT_COMBOBOX(XRCID("cbTargetSchema"),           dlgProperty::OnChange)
    EVT_COMBOBOX(XRCID("cbTargetObject"),           dlgProperty::OnChange)
END_EVENT_TABLE();


dlgSynonym::dlgSynonym(pgaFactory *f, frmMain *frame, edbSynonym *node)
: dlgProperty(f, frame, wxT("dlgSynonym"))
{
    synonym=node;
    cbOwner->Disable();

    cbTargetType->Append(_("Sequence"));
    cbTargetType->Append(_("Public synonym"));
    cbTargetType->Append(_("Table"));
    cbTargetType->Append(_("View"));
}


pgObject *dlgSynonym::GetObject()
{
    return synonym;
}


int dlgSynonym::Go(bool modal)
{
    if (synonym)
    {
        // edit mode
        txtName->Disable();

        cbTargetType->SetSelection(cbTargetType->FindString(synonym->GetTargetType()));
        ProcessTypeChange();

        if (cbTargetType->GetValue() != _("Public synonym"))
        {
            cbTargetSchema->SetSelection(cbTargetSchema->FindString(synonym->GetTargetSchema()));
            ProcessSchemaChange();
        }

        cbTargetObject->SetSelection(cbTargetObject->FindString(synonym->GetTargetObject()));
    }
    else
    {
        // create mode
        cbTargetType->Enable();
        cbTargetSchema->Disable();
        cbTargetObject->Disable();
    }

    txtComment->Disable();

    return dlgProperty::Go(modal);
}


pgObject *dlgSynonym::CreateObject(pgCollection *collection)
{
    pgObject *obj=synonymFactory.CreateObjects(collection, 0,
         wxT(" WHERE synname = ") + qtDbString(GetName()));

    return obj;
}


void dlgSynonym::CheckChange()
{
    bool enable=true;
    CheckValid(enable, !txtName->GetValue().IsEmpty(), _("Please specify name."));
    CheckValid(enable, !cbTargetType->GetValue().IsEmpty(), _("Please select target type."));
    if (cbTargetType->GetValue() != _("Public synonym"))
        CheckValid(enable, !cbTargetSchema->GetValue().IsEmpty(), _("Please select target schema."));
    CheckValid(enable, !cbTargetObject->GetValue().IsEmpty(), _("Please select target object."));

    if (!enable)
    {
        EnableOK(enable);
        return;
    }

    if (synonym)
        EnableOK(synonym->GetTargetObject() != cbTargetObject->GetValue());
    else
        EnableOK(txtName->GetValue() != wxEmptyString && cbTargetObject->GetValue() != wxEmptyString);
}

void dlgSynonym::ProcessTypeChange()
{
    cbTargetSchema->Clear();
    cbTargetObject->Clear();
    if (cbTargetType->GetValue() != _("Public synonym"))
    {
        pgSet *schemas;
        if (connection->BackendMinimumVersion(8, 2))
            schemas = connection->ExecuteSet(wxT("SELECT nspname FROM pg_namespace WHERE nspparent = 0 AND nspname NOT LIKE E'pg\\_%' AND nspname NOT IN ('pg_catalog', 'sys', 'dbo', 'pgagent', 'information_schema') ORDER BY nspname;"));
        else if (connection->BackendMinimumVersion(8, 1))
            schemas = connection->ExecuteSet(wxT("SELECT nspname FROM pg_namespace WHERE nspname NOT LIKE E'pg\\_%' ORDER BY nspname;"));
        else
            schemas = connection->ExecuteSet(wxT("SELECT nspname FROM pg_namespace WHERE nspname NOT LIKE 'pg\\_%' ORDER BY nspname;"));

        for (int x = 0; x < schemas->NumRows(); x++)
        {
            cbTargetSchema->Append(schemas->GetVal(0));
            schemas->MoveNext();
        }
        delete schemas;

        cbTargetSchema->Enable();
    }
    else
    {
        pgSet *synonyms = connection->ExecuteSet(wxT("SELECT synname FROM pg_synonym ORDER BY synname;"));
        for (int x = 0; x < synonyms->NumRows(); x++)
        {
            cbTargetObject->Append(synonyms->GetVal(0));
            synonyms->MoveNext();
        }
        delete synonyms;

        cbTargetSchema->Disable();
        cbTargetObject->Enable();
    }
}

void dlgSynonym::ProcessSchemaChange()
{
    cbTargetObject->Clear();

    wxString restriction;
    if (cbTargetType->GetValue() == _("Sequence"))
        restriction = wxT("S");
    else if (cbTargetType->GetValue() == _("Table"))
        restriction = wxT("r");
    else if (cbTargetType->GetValue() == _("View"))
        restriction = wxT("v");

    wxString sql = wxT("SELECT relname FROM pg_class c, pg_namespace n\n")
        wxT("  WHERE c.relnamespace = n.oid AND\n")
        wxT("        n.nspname = ") + qtDbString(cbTargetSchema->GetValue()) + wxT(" AND\n")
        wxT("        c.relkind = '") + restriction + wxT("' ORDER BY relname;");

    pgSet *objects = connection->ExecuteSet(sql);
    for (int x = 0; x < objects->NumRows(); x++)
    {
        cbTargetObject->Append(objects->GetVal(0));
        objects->MoveNext();
    }
    delete objects;

    cbTargetObject->Enable();
}


wxString dlgSynonym::GetSql()
{
    wxString sql;

    sql = wxT("CREATE OR REPLACE PUBLIC SYNONYM ") + qtIdent(txtName->GetValue()) + wxT("\n FOR ");

    if (cbTargetSchema->GetValue() != wxEmptyString)
        sql += qtIdent(cbTargetSchema->GetValue()) + wxT(".");

    sql += qtIdent(cbTargetObject->GetValue()) + wxT(";\n");

    AppendComment(sql, wxT("PUBLIC SYNONYM ") + qtIdent(txtName->GetValue()), synonym);

    return sql;
}
