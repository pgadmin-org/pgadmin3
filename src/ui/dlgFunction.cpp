//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
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
#define txtArguments        CTRL("txtArguments", wxTextCtrl)
#define cbReturntype        CTRL("cbReturntype", wxComboBox)
#define cbLanguage          CTRL("cbLanguage", wxComboBox)
#define cbVolatility        CTRL("cbVolatility", wxComboBox)
#define chkSetof            CTRL("chkSetof", wxCheckBox)
#define chkStrict           CTRL("chkStrict", wxCheckBox)
#define chkSecureDefiner    CTRL("chkSecureDefiner", wxCheckBox)

#define lstArguments        CTRL("lstArguments", wxListBox)
#define btnAdd              CTRL("btnAdd", wxButton)
#define btnRemove           CTRL("btnRemove", wxButton)
#define cbDatatype          CTRL("cbDatatype", wxComboBox)

#define pnlParameter        CTRL("pnlParameter", wxPanel)
#define sbxDefinition       CTRL("sbxDefinition", wxStaticBox)
#define stObjectFile        CTRL("stObjectFile", wxStaticText)
#define txtObjectFile       CTRL("txtObjectFile", wxTextCtrl)
#define stLinkSymbol        CTRL("stLinkSymbol", wxStaticText)
#define txtLinkSymbol       CTRL("txtLinkSymbol", wxTextCtrl)


#define TXTOBJ_LIB  wxT("$libdir/")
#define CTL_SQLBOX  188

BEGIN_EVENT_TABLE(dlgFunction, dlgSecurityProperty)
    EVT_TEXT(XRCID("txtName"),                      dlgFunction::OnChange)
    EVT_TEXT(XRCID("txtComment"),                   dlgFunction::OnChange)
    EVT_COMBOBOX(XRCID("cbVolatility"),             dlgFunction::OnChange)
    EVT_CHECKBOX(XRCID("chkSetof"),                 dlgFunction::OnChange)
    EVT_CHECKBOX(XRCID("chkStrict"),                dlgFunction::OnChange)
    EVT_CHECKBOX(XRCID("chkSecureDefiner"),         dlgFunction::OnChange)
    EVT_TEXT(XRCID("txtObjectFile"),                dlgFunction::OnChange)
    EVT_TEXT(XRCID("txtLinkSymbol"),                dlgFunction::OnChange)
    EVT_STC_MODIFIED(CTL_SQLBOX,                    dlgFunction::OnChange)

    EVT_COMBOBOX(XRCID("cbReturntype"),             dlgFunction::OnChange)
    EVT_COMBOBOX(XRCID("cbDatatype"),               dlgFunction::OnSelChangeType)
    EVT_COMBOBOX(XRCID("cbLanguage"),               dlgFunction::OnSelChangeLanguage)

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

    sqlBox=new ctlSQLBox(pnlParameter, CTL_SQLBOX, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxSUNKEN_BORDER | wxTE_RICH2);

    wxWindow *placeholder=CTRL("txtSqlBox", wxTextCtrl);
    wxSizer *sizer=placeholder->GetContainingSizer();
    sizer->Add(sqlBox, 1, wxRIGHT|wxGROW, 5);
    sizer->Remove(placeholder);
    delete placeholder;
    sizer->Layout();

    libSizer = stObjectFile->GetContainingSizer();
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
            sqlBox->SetText(function->GetSource());

        txtName->Disable();
        cbReturntype->Disable();
    }
    else
    {
        // create mode
        DatatypeReader tr(connection, wxString("(typtype IN ('b', 'd') OR typname IN ('void', 'cstring'))"));
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
              || cbVolatility->GetValue() != function->GetVolatility()
              || chkSecureDefiner->GetValue() != function->GetSecureDefiner()
              || chkStrict->GetValue() != function->GetIsStrict()
              || cbLanguage->GetValue() != function->GetLanguage()
              || (isC && (txtObjectFile->GetValue() != function->GetBin() || txtLinkSymbol->GetValue() != function->GetSource()))
              || (!isC && sqlBox->GetText() != function->GetSource()));
    }
    else
    {
        wxString name=GetName();

        bool enable=true;

        CheckValid(enable, !name.IsEmpty(), wxT("Please specify name."));
        CheckValid(enable, cbReturntype->GetSelection() >= 0, wxT("Please select return type."));
        CheckValid(enable, cbLanguage->GetSelection() >= 0, wxT("Please select language."));
        if (isC)
        {
            wxString objfile=txtObjectFile->GetValue();
            CheckValid(enable, !objfile.IsEmpty() && objfile != TXTOBJ_LIB, wxT("Please specify object library."));
        }
        else
        {
            CheckValid(enable, !sqlBox->GetText().IsEmpty(), wxT("Please enter function source code."));
        }

        EnableOK(enable);
    }
}


void dlgFunction::ReplaceSizer(wxWindow *w, bool isC, int border)
{
    if (isC && !w->GetContainingSizer())
        libSizer->Add(w, 0, wxLEFT|wxTOP, border);
    else
        libSizer->Remove(w);
}

        
void dlgFunction::OnSelChangeLanguage(wxNotifyEvent &ev)
{
    bool isC=(cbLanguage->GetValue().IsSameAs(wxT("C"), false));

    stObjectFile->Show(isC);
    txtObjectFile->Show(isC);
    stLinkSymbol->Show(isC);
    txtLinkSymbol->Show(isC);
    sqlBox->Show(!isC);

    ReplaceSizer(stObjectFile, isC, 3);
    ReplaceSizer(txtObjectFile, isC, 0);
    ReplaceSizer(stLinkSymbol, isC, 3);
    ReplaceSizer(txtLinkSymbol, isC, 0);

    libSizer->Layout();
    sqlBox->GetContainingSizer()->Layout();

    OnChange(ev);
}


void dlgFunction::OnSelChangeArg(wxNotifyEvent &ev)
{
    btnRemove->Enable();
}


void dlgFunction::OnSelChangeType(wxNotifyEvent &ev)
{
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
    wxString sql, name;


    if (function)
    {
        // edit mode
        name = function->GetQuotedFullIdentifier();
        
        sql = wxT("CREATE OR REPLACE FUNCTION ") + name;
    }
    else
    {
        // create mode
        name = schema->GetQuotedFullIdentifier() + wxT(".") 
             + qtIdent(GetName()) 
             + wxT("(") + GetArgs(true) + wxT(")");

        sql = wxT("CREATE FUNCTION ") + name;
    }
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
        sql += qtString(sqlBox->GetText());
    sql += wxT("\nLANGUAGE ") + qtString(cbLanguage->GetValue())
        +  wxT(" ") + cbVolatility->GetValue();
    if (chkStrict->GetValue())
        sql += wxT(" STRICT");
    if (chkSecureDefiner->GetValue())
        sql += wxT(" SECURE DEFINER");

    sql += wxT(";\n");

    wxString comment=txtComment->GetValue();
    if ((function && function->GetComment() != comment) 
        || (!function && !comment.IsEmpty()))
    {
        sql += wxT("COMMENT ON FUNCTION ") + name 
            + wxT(" IS ") + qtString(comment)
            + wxT(";\n");
    }
    sql += GetGrant(wxT("-"), wxT("FUNCTION ") + name);

    return sql;
}


