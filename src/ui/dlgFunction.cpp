//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2004, The pgAdmin Development Team
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
#define cbReturntype        CTRL_COMBOBOX2("cbReturntype")
#define cbLanguage          CTRL_COMBOBOX("cbLanguage")
#define chkSetof            CTRL_CHECKBOX("chkSetof")
#define cbVolatility        CTRL_COMBOBOX("cbVolatility")
#define chkStrict           CTRL_CHECKBOX("chkStrict")
#define chkSecureDefiner    CTRL_CHECKBOX("chkSecureDefiner")

#define lstArguments        CTRL_LISTVIEW("lstArguments")
#define txtArgName          CTRL_TEXT("txtArgName")
#define btnAdd              CTRL_BUTTON("btnAdd")
#define btnChange           CTRL_BUTTON("btnChange")
#define btnRemove           CTRL_BUTTON("btnRemove")

#define pnlParameter        CTRL_PANEL("pnlParameter")
#define sbxDefinition       CTRL_STATICBOX("sbxDefinition")
#define stObjectFile        CTRL_STATIC("stObjectFile")
#define txtObjectFile       CTRL_TEXT("txtObjectFile")
#define stLinkSymbol        CTRL_STATIC("stLinkSymbol")
#define txtLinkSymbol       CTRL_TEXT("txtLinkSymbol")
#define txtSqlBox           CTRL_SQLBOX("txtSqlBox")

#define TXTOBJ_LIB  wxT("$libdir/")

BEGIN_EVENT_TABLE(dlgFunction, dlgSecurityProperty)
    EVT_TEXT(XRCID("cbVolatility"),                 dlgProperty::OnChange)
    EVT_CHECKBOX(XRCID("chkStrict"),                dlgProperty::OnChange)
    EVT_CHECKBOX(XRCID("chkSecureDefiner"),         dlgProperty::OnChange)
    EVT_TEXT(XRCID("txtObjectFile"),                dlgProperty::OnChange)
    EVT_TEXT(XRCID("txtLinkSymbol"),                dlgProperty::OnChange)
    EVT_STC_MODIFIED(XRCID("txtSqlBox"),            dlgProperty::OnChangeStc)

    EVT_TEXT(XRCID("cbReturntype"),                 dlgFunction::OnChangeReturn)
    EVT_TEXT(XRCID("cbDatatype"),                   dlgFunction::OnSelChangeType)
    EVT_TEXT(XRCID("cbLanguage"),                   dlgFunction::OnSelChangeLanguage)

    EVT_LIST_ITEM_SELECTED(XRCID("lstArguments"),   dlgFunction::OnSelChangeArg)
    EVT_TEXT(XRCID("txtArgName"),                   dlgFunction::OnChangeArgName)
    EVT_BUTTON(XRCID("btnAdd"),                     dlgFunction::OnAddArg)
    EVT_BUTTON(XRCID("btnChange"),                  dlgFunction::OnChangeArg)
    EVT_BUTTON(XRCID("btnRemove"),                  dlgFunction::OnRemoveArg)
END_EVENT_TABLE();


dlgFunction::dlgFunction(frmMain *frame, pgFunction *node, pgSchema *sch)
: dlgSecurityProperty(frame, node, wxT("dlgFunction"), wxT("EXECUTE"), "X")
{
    SetIcon(wxIcon(function_xpm));
    schema=sch;
    function=node;

    txtArguments->Disable();
    btnAdd->Disable();
    btnRemove->Disable();

    txtSqlBox->SetMarginType(1, wxSTC_MARGIN_NUMBER);
    txtSqlBox->SetMarginWidth(1, ConvertDialogToPixels(wxPoint(16, 0)).x);

    libcSizer = stObjectFile->GetContainingSizer();

    btnAdd->Disable();
    btnRemove->Disable();
    btnChange->Disable();
}


pgObject *dlgFunction::GetObject()
{
    return function;
}


int dlgFunction::Go(bool modal)
{
    if (!function)
        cbOwner->Append(wxEmptyString);
    AddGroups();
    AddUsers(cbOwner);

    // the listview's column that contains the type name
    typeColNo = (connection->BackendMinimumVersion(7, 5) ? 1 : 0);

    if (objectType != PG_TRIGGERFUNCTION)
    {
        if (typeColNo)
            lstArguments->CreateColumns((wxImageList*)0, _("Name"), _("Type"));
        else
            lstArguments->CreateColumns((wxImageList*)0, _("Type"), wxEmptyString, 0);
    }
    if (!typeColNo)
        txtArgName->Disable();

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

        if (objectType != PG_TRIGGERFUNCTION)
        {
            wxStringTokenizer argtypes(function->GetArgTypes(), wxT(", "));
            size_t cnt=0;

            while (argtypes.HasMoreTokens())
            {
                wxString str=argtypes.GetNextToken();
                if (str.IsEmpty())
                    continue;
                if (typeColNo)
                {
                    if (cnt < function->GetArgNames().GetCount())
                        lstArguments->AppendItem(-1, function->GetArgNames().Item(cnt++), str);
                    else
                        lstArguments->AppendItem(-1, wxEmptyString, str);
                }
                else
                    lstArguments->AppendItem(-1, str);
            }
        }

        txtArguments->SetValue(function->GetArgTypeNames());
        cbReturntype->Append(function->GetReturnType());
        cbReturntype->SetValue(function->GetReturnType());

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
        chkSetof->Disable();
        cbDatatype->Disable();
    }
    else
    {
		wxString restrict;
        // create mode
    	restrict = wxT("(typtype IN ('b', 'c', 'd', 'p') AND typname NOT IN ('any', 'trigger', 'language_handler'))");
		if (!settings->GetShowSystemObjects()) 
			restrict += wxT(" AND nspname NOT LIKE 'pg_toast%' AND nspname NOT LIKE 'pg_temp%'");

		DatatypeReader tr(database, restrict);
        while (tr.HasMore())
        {
            pgDatatype dt=tr.GetDatatype();

            typOids.Add(tr.GetOidStr());
            types.Add(tr.GetQuotedSchemaPrefix() + dt.QuotedFullName());

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
            txtArgName->Disable();
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


void dlgFunction::CheckChange()
{
    wxString name=GetName();
    bool isC=cbLanguage->GetValue().IsSameAs(wxT("C"), false);
    if (function)
    {

        EnableOK(txtComment->GetValue() != function->GetComment()
              || name != function->GetName()
              || cbVolatility->GetValue() != function->GetVolatility()
              || chkSecureDefiner->GetValue() != function->GetSecureDefiner()
              || chkStrict->GetValue() != function->GetIsStrict()
              || cbLanguage->GetValue() != function->GetLanguage()
              || cbOwner->GetValue() != function->GetOwner()
              || GetArgs(true, true) != function->GetQuotedArgTypeNames()
              || (isC && (txtObjectFile->GetValue() != function->GetBin() || txtLinkSymbol->GetValue() != function->GetSource()))
              || (!isC && txtSqlBox->GetText() != function->GetSource()));
    }
    else
    {

        bool enable=true;

        CheckValid(enable, !name.IsEmpty(), _("Please specify name."));
        CheckValid(enable, cbReturntype->GetGuessedSelection() >= 0, _("Please select return type."));
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

        
void dlgFunction::OnSelChangeLanguage(wxCommandEvent &ev)
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

    CheckChange();
}


void dlgFunction::OnSelChangeArg(wxListEvent &ev)
{
    int row=lstArguments->GetSelection();
    if (row >= 0)
    {
        if (typeColNo)
        {
            txtArgName->SetValue(lstArguments->GetText(row, 0));
            cbDatatype->SetValue(lstArguments->GetText(row, typeColNo));
        }
        else
            cbDatatype->SetValue(lstArguments->GetText(row, 0));

        wxCommandEvent ev;
        OnChangeArgName(ev);
    }
}


void dlgFunction::OnChangeReturn(wxCommandEvent &ev)
{
    cbReturntype->GuessSelection();
    CheckChange();
}


void dlgFunction::OnSelChangeType(wxCommandEvent &ev)
{
    cbDatatype->GuessSelection();
    OnChangeArgName(ev);
}


void dlgFunction::OnChangeArgName(wxCommandEvent &ev)
{
    int argNameRow=-1;
    if (!txtArgName->GetValue().IsEmpty())
        argNameRow = lstArguments->FindItem(-1, txtArgName->GetValue());

    int pos=lstArguments->GetSelection();

    bool typeValid = (function != 0 || cbDatatype->GetGuessedSelection() >= 0);

    btnAdd->Enable(argNameRow < 0 && typeValid);
    btnChange->Enable(pos >= 0 && typeValid);
    btnRemove->Enable(pos >= 0);
}


void dlgFunction::OnChangeArg(wxCommandEvent &ev)
{
    int row=lstArguments->GetSelection();

    if (row >= 0)
    {
        if (typeColNo)
        {
            lstArguments->SetItem(row, 0, txtArgName->GetValue());
            lstArguments->SetItem(row, typeColNo, cbDatatype->GetValue());
        }
        else
            lstArguments->SetItem(row, 0, cbDatatype->GetValue());

        if (!function)
            argOids.Item(row) = typOids.Item(cbDatatype->GetGuessedSelection());
        txtArguments->SetValue(GetArgs());
    }
    OnChangeArgName(ev);
    CheckChange();
}


void dlgFunction::OnAddArg(wxCommandEvent &ev)
{
    if (typeColNo)
        lstArguments->AppendItem(-1, txtArgName->GetValue(), cbDatatype->GetValue());
    else
        lstArguments->AppendItem(-1, cbDatatype->GetValue());

    if (!function)
        argOids.Add(typOids.Item(cbDatatype->GetGuessedSelection()));

    txtArguments->SetValue(GetArgs());
    OnChangeArgName(ev);
}


void dlgFunction::OnRemoveArg(wxCommandEvent &ev)
{
    int sel=lstArguments->GetSelection();
    argOids.RemoveAt(sel);
    lstArguments->DeleteItem(sel);
    btnRemove->Disable();
    txtArguments->SetValue(GetArgs());
    OnChangeArgName(ev);
}


wxString dlgFunction::GetArgs(bool withNames, bool quoted)
{
    wxString args;
    int i;
    for (i=0 ; i < lstArguments->GetItemCount() ; i++)
    {
        if (i)
            args += wxT(", ");

        if (typeColNo)
        {
            if (withNames)
            {
                if (quoted)
                    args += qtIdent(lstArguments->GetText(i)) + wxT(" ");
                else
                    args += lstArguments->GetText(i) + wxT(" ");
            }
        }
        if (quoted)
            AppendQuoted(args, lstArguments->GetText(i, typeColNo));
        else
            args += lstArguments->GetText(i, typeColNo);
    }

    return args;
}


wxString dlgFunction::GetSql()
{
    wxString sql;
    wxString name=GetName();


    bool isC=cbLanguage->GetValue().IsSameAs(wxT("C"), false);
    bool didChange = !function 
        || cbVolatility->GetValue() != function->GetVolatility()
        || chkSecureDefiner->GetValue() != function->GetSecureDefiner()
        || chkStrict->GetValue() != function->GetIsStrict()
        || cbOwner->GetValue() != function->GetOwner()
        || GetArgs(true, true) != function->GetQuotedArgTypeNames()
        || (isC && (txtObjectFile->GetValue() != function->GetBin() || txtLinkSymbol->GetValue() != function->GetSource()))
        || (!isC && txtSqlBox->GetText() != function->GetSource());

    if (function)
    {
        // edit mode
        if (name != function->GetName())
        {
            sql = wxT("ALTER FUNCTION ") + function->GetQuotedFullIdentifier() 
                                         + wxT("(") + function->GetQuotedArgTypes() + wxT(")")
                + wxT(" RENAME TO ") + qtIdent(name) + wxT(";\n");
        }
 
        if (didChange)
            sql += wxT("CREATE OR REPLACE FUNCTION ");
    }
    else
    {
        // create mode
        sql = wxT("CREATE FUNCTION ");
    }

    name = schema->GetQuotedPrefix() + qtIdent(name) 
         + wxT("(") + GetArgs(false, true) + wxT(")");


    if (didChange)
    {
        sql  += schema->GetQuotedPrefix() + qtIdent(GetName()) 
             + wxT("(") + GetArgs(true, true) + wxT(")")
               wxT(" RETURNS ");
        if (chkSetof->GetValue())
            sql += wxT("SETOF ");

        AppendQuoted(sql, cbReturntype->GetValue());
        sql += wxT(" AS\n");

        if (cbLanguage->GetValue().IsSameAs(wxT("C"), false))
        {
            sql += qtString(txtObjectFile->GetValue());
            if (!txtLinkSymbol->GetValue().IsEmpty())
                sql += wxT(", ") + qtString(txtLinkSymbol->GetValue());
        }
        else
        {
            if (connection->BackendMinimumVersion(7, 5))
                sql += qtStringDollar(txtSqlBox->GetText());
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

    if (function)
    {
        if (cbOwner->GetValue() != function->GetOwner())
            sql += wxT("ALTER FUNCTION ") + name
                +  wxT(" OWNER TO ") + qtIdent(cbOwner->GetValue())
                + wxT(";\n");
    }
    else
    {
        if (cbOwner->GetGuessedSelection() > 0)
            AppendOwnerNew(sql,wxT("FUNCTION ") + name);
    }

    sql += GetGrant(wxT("X"), wxT("FUNCTION ") + name);
    AppendComment(sql, wxT("FUNCTION ") + name, function);

    return sql;
}


