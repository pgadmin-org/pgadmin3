//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2003, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// dlgFunction.cpp - PostgreSQL Function Property
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "misc.h"
#include "pgDefs.h"

#include "ctlSQLBox.h"
#include "dlgFunction.h"
#include "pgFunction.h"
#include "pgCollection.h"
#include "pgDatatype.h"


// Images
#include "images/function.xpm"


// pointer to controls
#define txtArguments        CTRL_TEXT("txtArguments")
#define cbReturntype        CTRL_COMBOBOX("cbReturntype")
#define cbLanguage          CTRL_COMBOBOX("cbLanguage")
#define cbVolatility        CTRL_COMBOBOX("cbVolatility")
#define chkSetof            CTRL_CHECKBOX("chkSetof")
#define chkStrict           CTRL_CHECKBOX("chkStrict")
#define chkSecureDefiner    CTRL_CHECKBOX("chkSecureDefiner")

#define lstArguments        CTRL_LISTBOX("lstArguments")
#define btnAdd              CTRL_BUTTON("btnAdd")
#define btnRemove           CTRL_BUTTON("btnRemove")
#define cbDatatype          CTRL_COMBOBOX("cbDatatype")

#define pnlParameter        CTRL_PANEL("pnlParameter")
#define sbxDefinition       CTRL_STATICBOX("sbxDefinition")
#define stObjectFile        CTRL_STATIC("stObjectFile")
#define txtObjectFile       CTRL_TEXT("txtObjectFile")
#define stLinkSymbol        CTRL_STATIC("stLinkSymbol")
#define txtLinkSymbol       CTRL_TEXT("txtLinkSymbol")
#define txtSqlBox           CTRL_SQLBOX("txtSqlBox")

#define TXTOBJ_LIB  wxT("$libdir/")

BEGIN_EVENT_TABLE(dlgFunction, dlgSecurityProperty)
    EVT_TEXT(XRCID("txtName"),                      dlgFunction::OnChange)
    EVT_TEXT(XRCID("txtComment"),                   dlgFunction::OnChange)
    EVT_TEXT(XRCID("cbVolatility"),                 dlgFunction::OnChange)
    EVT_CHECKBOX(XRCID("chkSetof"),                 dlgFunction::OnChange)
    EVT_CHECKBOX(XRCID("chkStrict"),                dlgFunction::OnChange)
    EVT_CHECKBOX(XRCID("chkSecureDefiner"),         dlgFunction::OnChange)
    EVT_TEXT(XRCID("txtObjectFile"),                dlgFunction::OnChange)
    EVT_TEXT(XRCID("txtLinkSymbol"),                dlgFunction::OnChange)
    EVT_STC_MODIFIED(XRCID("txtSqlBox"),            dlgFunction::OnChange)

    EVT_TEXT(XRCID("cbReturntype"),                 dlgFunction::OnChange)
    EVT_TEXT(XRCID("cbDatatype"),                   dlgFunction::OnSelChangeType)
    EVT_TEXT(XRCID("cbLanguage"),                   dlgFunction::OnSelChangeLanguage)

    EVT_LISTBOX(XRCID("lstArguments"),              dlgFunction::OnSelChangeArg)
    EVT_BUTTON(XRCID("btnAdd"),                     dlgFunction::OnAddArg)
    EVT_BUTTON(XRCID("btnRemove"),                  dlgFunction::OnRemoveArg)
END_EVENT_TABLE();


dlgFunction::dlgFunction(frmMain *frame, pgFunction *node, pgSchema *sch)
: dlgSecurityProperty(frame, node, wxT("dlgFunction"), wxT("EXECUTE"), "X")
{
    SetIcon(wxIcon(function_xpm));
    schema=sch;
    function=node;

    txtOID->Disable();
    txtArguments->Disable();
    btnAdd->Disable();
    btnRemove->Disable();

    libcSizer = stObjectFile->GetContainingSizer();
}


pgObject *dlgFunction::GetObject()
{
    return function;
}


int dlgFunction::Go(bool modal)
{
    AddGroups();
    AddUsers();

    pgSet *lang=connection->ExecuteSet(wxT("SELECT lanname FROM pg_language"));
    if (lang)
    {
        while (!lang->Eof())
        {
            wxString language=lang->GetVal(0);
            if (objectType != PG_TRIGGERFUNCTION || !language.IsSameAs(wxT("SQL"), false))
                cbLanguage->Append(language);
            lang->MoveNext();
        }
        delete lang;
    }
    if (function)
    {
        // edit mode
        txtName->SetValue(function->GetName());
        txtOID->SetValue(NumToStr((long)function->GetOid()));
        txtComment->SetValue(function->GetComment());
        txtArguments->SetValue(function->GetArgTypes());
        cbReturntype->Append(function->GetReturnType());
        cbReturntype->SetSelection(0);

        cbLanguage->SetValue(function->GetLanguage());
        cbVolatility->SetValue(function->GetVolatility());

        chkSetof->SetValue(function->GetReturnAsSet());
        chkStrict->SetValue(function->GetIsStrict());
        chkSecureDefiner->SetValue(function->GetSecureDefiner());

        if (function->GetLanguage().IsSameAs(wxT("C"), false))
        {
            txtObjectFile->SetValue(function->GetBin());
            txtLinkSymbol->SetValue(function->GetSource());
        }
        else
            txtSqlBox->SetText(function->GetSource());

        if (!connection->BackendMinimumVersion(7, 4))
            txtName->Disable();
        cbReturntype->Disable();
        cbDatatype->Disable();
        lstArguments->Disable();
    }
    else
    {
        // create mode
        DatatypeReader tr(connection, wxString(wxT("(typtype IN ('b', 'd') OR typname IN ('void', 'cstring'))")));
        while (tr.HasMore())
        {
            pgDatatype dt=tr.GetDatatype();

            typOids.Add(tr.GetOidStr());
            types.Add(tr.GetSchemaPrefix() + dt.QuotedFullName());

            cbDatatype->Append(tr.GetSchemaPrefix() + dt.FullName());
            if (objectType != PG_TRIGGERFUNCTION)
                cbReturntype->Append(tr.GetSchemaPrefix() + dt.FullName());
            tr.MoveNext();
        }

        long sel;
        if (objectType == PG_TRIGGERFUNCTION)
        {
            cbReturntype->Append(wxT("trigger"));
            cbReturntype->SetSelection(0);
            cbReturntype->Disable();
            lstArguments->Disable();
            cbDatatype->Disable();
            btnAdd->Disable();
            btnRemove->Disable();
            sel=cbLanguage->FindString(wxT("c"));
        }
        else
            sel=cbLanguage->FindString(wxT("sql"));

        if (sel >= 0)
            cbLanguage->SetSelection(sel);
        txtObjectFile->SetValue(TXTOBJ_LIB);
    }

    wxNotifyEvent event;
    OnSelChangeLanguage(event);

    return dlgSecurityProperty::Go(modal);
}


pgObject *dlgFunction::CreateObject(pgCollection *collection)
{
    wxString sql=wxT(" WHERE proname=") + qtString(GetName()) +
        wxT("\n   AND pronamespace=") + schema->GetOidStr();

    long argCount;
    for (argCount=0 ; argCount < (int)argOids.GetCount() ; argCount++)
        sql += wxT("\n   AND proargtypes[") + NumToStr(argCount) + wxT("] = ") + argOids.Item(argCount);

    sql += wxT("\n   AND proargtypes[") + NumToStr(argCount) + wxT("] = 0\n");

    pgObject *obj=pgFunction::AppendFunctions(collection, collection->GetSchema(), 0, sql);
    return obj;
}


void dlgFunction::OnChange(wxNotifyEvent &ev)
{
    bool isC=cbLanguage->GetValue().IsSameAs(wxT("C"), false);
    if (function)
    {

        EnableOK(txtComment->GetValue() != function->GetComment()
              || txtName->GetValue() != function->GetName()
              || cbVolatility->GetValue() != function->GetVolatility()
              || chkSecureDefiner->GetValue() != function->GetSecureDefiner()
              || chkSetof->GetValue() != function->GetReturnAsSet()
              || chkStrict->GetValue() != function->GetIsStrict()
              || cbLanguage->GetValue() != function->GetLanguage()
              || (isC && (txtObjectFile->GetValue() != function->GetBin() || txtLinkSymbol->GetValue() != function->GetSource()))
              || (!isC && txtSqlBox->GetText() != function->GetSource()));
    }
    else
    {
        wxString name=GetName();

        bool enable=true;

        CheckValid(enable, !name.IsEmpty(), _("Please specify name."));
        CheckValid(enable, cbReturntype->GetSelection() >= 0, _("Please select return type."));
        CheckValid(enable, cbLanguage->GetSelection() >= 0, _("Please select language."));
        if (isC)
        {
            wxString objfile=txtObjectFile->GetValue();
            CheckValid(enable, !objfile.IsEmpty() && objfile != TXTOBJ_LIB, _("Please specify object library."));
        }
        else
        {
            CheckValid(enable, !txtSqlBox->GetText().IsEmpty(), _("Please enter function source code."));
        }

        EnableOK(enable);
    }
}


void dlgFunction::ReplaceSizer(wxWindow *w, bool isC, int border)
{
    if (isC && !w->GetContainingSizer())
        libcSizer->Add(w, 0, wxLEFT|wxTOP, border);
    else
        libcSizer->Remove(w);
}

        
void dlgFunction::OnSelChangeLanguage(wxNotifyEvent &ev)
{
    bool isC=(cbLanguage->GetValue().IsSameAs(wxT("C"), false));

    stObjectFile->Show(isC);
    txtObjectFile->Show(isC);
    stLinkSymbol->Show(isC);
    txtLinkSymbol->Show(isC);
    txtSqlBox->Show(!isC);

    ReplaceSizer(stObjectFile, isC, 3);
    ReplaceSizer(txtObjectFile, isC, 0);
    ReplaceSizer(stLinkSymbol, isC, 3);
    ReplaceSizer(txtLinkSymbol, isC, 0);

    libcSizer->Layout();
    txtSqlBox->GetContainingSizer()->Layout();

    OnChange(ev);
}


void dlgFunction::OnSelChangeArg(wxNotifyEvent &ev)
{
    if (objectType != PG_TRIGGERFUNCTION)
        btnRemove->Enable();
}


void dlgFunction::OnSelChangeType(wxNotifyEvent &ev)
{
    if (objectType != PG_TRIGGERFUNCTION)
        btnAdd->Enable();
}


void dlgFunction::OnAddArg(wxNotifyEvent &ev)
{
    lstArguments->Append(cbDatatype->GetValue());
    argOids.Add(typOids.Item(cbDatatype->GetSelection()));
    txtArguments->SetValue(GetArgs());
}


void dlgFunction::OnRemoveArg(wxNotifyEvent &ev)
{
    int sel=lstArguments->GetSelection();
    argOids.RemoveAt(sel);
    lstArguments->Delete(sel);
    btnRemove->Disable();
    txtArguments->SetValue(GetArgs());
}


wxString dlgFunction::GetArgs(bool quoted)
{
    wxString args;
    int i;
    for (i=0 ; i < lstArguments->GetCount() ; i++)
    {
        if (i)
            args += wxT(", ");
        if (quoted)
            AppendQuoted(args, lstArguments->GetString(i));
        else
            args += lstArguments->GetString(i);
    }

    return args;
}


wxString dlgFunction::GetSql()
{
    wxString sql;
    wxString name;


    bool isC=cbLanguage->GetValue().IsSameAs(wxT("C"), false);
    bool didChange = !function 
        || (isC && (txtObjectFile->GetValue() != function->GetBin() || txtLinkSymbol->GetValue() != function->GetSource()))
        || (!isC && txtSqlBox->GetText() != function->GetSource());

    if (function)
    {
        // edit mode

        name = function->GetQuotedFullIdentifier() 
                            + wxT("(") + function->GetQuotedArgTypes() + wxT(")");

        if (didChange)
            sql += wxT("CREATE OR REPLACE FUNCTION ") + name;
    }
    else
    {
        // create mode
        name = schema->GetQuotedFullIdentifier() + wxT(".") 
             + qtIdent(GetName()) 
             + wxT("(") + GetArgs(true) + wxT(")");

        sql = wxT("CREATE FUNCTION ") + name;
    }

    if (didChange)
    {
        sql += wxT(" RETURNS ");
        if (chkSetof->GetValue())
            sql += wxT("SETOF ");
        sql += cbReturntype->GetValue()
            + wxT(" AS\n");

        if (cbLanguage->GetValue().IsSameAs(wxT("C"), false))
        {
            sql += qtString(txtObjectFile->GetValue());
            if (!txtLinkSymbol->GetValue().IsEmpty())
                sql += wxT(", ") + qtString(txtLinkSymbol->GetValue());
        }
        else
        {
            if (connection->BackendMinimumVersion(7, 5))
                sql += qtDocumentHere(txtSqlBox->GetText());
            else
                sql += qtString(txtSqlBox->GetText());
        }
        sql += wxT("\nLANGUAGE ") + qtString(cbLanguage->GetValue())
            +  wxT(" ") + cbVolatility->GetValue();
        if (chkStrict->GetValue())
            sql += wxT(" STRICT");
        if (chkSecureDefiner->GetValue())
            sql += wxT(" SECURITY DEFINER");

        sql += wxT(";\n");
    }

    wxString comment=txtComment->GetValue();
    if ((function && function->GetComment() != comment) 
        || (!function && !comment.IsEmpty()))
    {
        sql += wxT("COMMENT ON FUNCTION ") + name 
            + wxT(" IS ") + qtString(comment)
            + wxT(";\n");
    }
    sql += GetGrant(wxT("-"), wxT("FUNCTION ") + name);

    if (function && GetName() != function->GetName())
        sql += wxT("ALTER FUNCTION ") + name 
             + wxT(" RENAME TO ") + qtIdent(GetName()) + wxT(";\n");

    return sql;
}


