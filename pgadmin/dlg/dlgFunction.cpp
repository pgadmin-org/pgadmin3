//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// dlgFunction.cpp - PostgreSQL Function Property
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "utils/misc.h"
#include "utils/pgDefs.h"

#include "ctl/ctlSQLBox.h"
#include "dlg/dlgFunction.h"
#include "schema/pgFunction.h"
#include "schema/pgSchema.h"
#include "schema/pgDatatype.h"



// pointer to controls
#define txtArguments        CTRL_TEXT("txtArguments")
#define stReturntype        CTRL_STATIC("stReturntype")
#define cbReturntype        CTRL_COMBOBOX2("cbReturntype")
#define stLanguage          CTRL_STATIC("stLanguage")
#define cbLanguage          CTRL_COMBOBOX("cbLanguage")
#define chkSetof            CTRL_CHECKBOX("chkSetof")
#define stVolatility        CTRL_STATIC("stVolatility")
#define cbVolatility        CTRL_COMBOBOX("cbVolatility")
#define chkStrict           CTRL_CHECKBOX("chkStrict")
#define chkSecureDefiner    CTRL_CHECKBOX("chkSecureDefiner")

#define lstArguments        CTRL_LISTVIEW("lstArguments")
#define rdbDirection        CTRL_RADIOBOX("rdbDirection")
#define txtArgName          CTRL_TEXT("txtArgName")
#define btnAdd              CTRL_BUTTON("wxID_ADD")
#define btnChange           CTRL_BUTTON("wxID_CHANGE")
#define btnRemove           CTRL_BUTTON("wxID_REMOVE")

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
    EVT_COMBOBOX(XRCID("cbReturntype"),             dlgFunction::OnChangeReturn)
    EVT_TEXT(XRCID("cbDatatype"),                   dlgFunction::OnSelChangeType)
    EVT_COMBOBOX(XRCID("cbDatatype"),               dlgFunction::OnSelChangeType)
    EVT_TEXT(XRCID("cbLanguage"),                   dlgFunction::OnSelChangeLanguage)
    EVT_COMBOBOX(XRCID("cbLanguage"),               dlgFunction::OnSelChangeLanguage)

    EVT_LIST_ITEM_SELECTED(XRCID("lstArguments"),   dlgFunction::OnSelChangeArg)
    EVT_TEXT(XRCID("txtArgName"),                   dlgFunction::OnChangeArgName)
    EVT_BUTTON(wxID_ADD,                            dlgFunction::OnAddArg)
    EVT_BUTTON(XRCID("wxID_CHANGE"),                dlgFunction::OnChangeArg)
    EVT_BUTTON(wxID_REMOVE,                         dlgFunction::OnRemoveArg)
END_EVENT_TABLE();


dlgProperty *pgFunctionFactory::CreateDialog(frmMain *frame, pgObject *node, pgObject *parent)
{
    pgSchema *sch;

    if (parent->GetMetaType() == PGM_TRIGGER)
        sch = parent->GetSchema();
    else
        sch = (pgSchema*)parent;

    return new dlgFunction(this, frame, (pgFunction*)node, sch);
}


dlgFunction::dlgFunction(pgaFactory *f, frmMain *frame, pgFunction *node, pgSchema *sch)
: dlgSecurityProperty(f, frame, node, wxT("dlgFunction"), wxT("EXECUTE"), "X")
{
    schema=sch;
    function=node;
    isProcedure = false;

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



dlgProperty *pgProcedureFactory::CreateDialog(frmMain *frame, pgObject *node, pgObject *parent)
{
    return new dlgProcedure(this, frame, (pgFunction*)node, (pgSchema*)parent);
}

dlgProcedure::dlgProcedure(pgaFactory *f, frmMain *frame, pgFunction *node, pgSchema *sch)
: dlgFunction(f, frame, node, sch)
{
    isProcedure = true;
}

pgObject *dlgFunction::GetObject()
{
    return function;
}


int dlgFunction::Go(bool modal)
{
    if (function)
    {
        rdbDirection->Disable();
        isProcedure = function->GetIsProcedure();
    }
    else
        cbOwner->Append(wxEmptyString);

    AddGroups();
    AddUsers(cbOwner);

    if (!connection->BackendMinimumVersion(8, 0))
        cbOwner->Disable();

    // the listview's column that contains the type name
    typeColNo = (connection->BackendMinimumVersion(8, 0) ? 1 : 0);

    if (factory != &triggerFunctionFactory)
    {
        if (typeColNo)
            lstArguments->CreateColumns(0, _("Name"), _("Type"));
        else
            lstArguments->CreateColumns(0, _("Type"), wxEmptyString, 0);
    }
    if (!typeColNo)
        txtArgName->Disable();

    if (isProcedure)
    {
        rdbDirection->SetString(2, wxT("IN OUT"));
        stLanguage->Disable();
        cbLanguage->Disable();
        chkStrict->Disable();
        chkSecureDefiner->Disable();
        chkSetof->Disable();
        stVolatility->Disable();
        cbVolatility->Disable();
        stReturntype->Disable();
        cbReturntype->Disable();
    }

    pgSet *lang=connection->ExecuteSet(wxT("SELECT lanname FROM pg_language"));
    if (lang)
    {
        while (!lang->Eof())
        {
            wxString language=lang->GetVal(0);
            if (factory != &triggerFunctionFactory || !language.IsSameAs(wxT("SQL"), false))
                cbLanguage->Append(language);
            lang->MoveNext();
        }
        delete lang;
    }

    if (function)
    {
        // edit mode

        if (factory != &triggerFunctionFactory)
        {
			wxArrayString argTypes = function->GetArgTypesArray();
            wxArrayString argNames = function->GetArgNamesArray();
            wxArrayString argModes = function->GetArgModesArray();

            for (unsigned int i=0; i<argTypes.Count(); i++)
            {
                if (typeColNo)
                {
                    wxString arg;

                    if (isProcedure)
                    {
                        if (!argNames.Item(i).IsEmpty())
                            arg += argNames.Item(i);

                        if (!argModes.Item(i).IsEmpty())
                            if (arg.IsEmpty())
                                arg += argModes.Item(i);
                            else
                                arg += wxT(" ") + argModes.Item(i);
                    }
                    else
                    {
                        if (!argModes.Item(i).IsEmpty())
                            arg += argModes.Item(i);

                        if (!argNames.Item(i).IsEmpty())
                            if (arg.IsEmpty())
                                arg += argNames.Item(i);
                            else
                                arg += wxT(" ") + argNames.Item(i);
                    }

                    lstArguments->AppendItem(-1, arg, argTypes.Item(i));
                }
                else
                    lstArguments->AppendItem(-1, argTypes.Item(i));
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
            types.Add(dt.QuotedFullName());

            cbDatatype->Append(dt.FullName());
            if (factory != &triggerFunctionFactory)
                cbReturntype->Append(dt.FullName());
            tr.MoveNext();
        }

        long sel;
        if (factory == &triggerFunctionFactory)
        {
            cbReturntype->Append(wxT("trigger"));
            cbReturntype->SetSelection(0);
            cbReturntype->Disable();
            lstArguments->Disable();
            cbDatatype->Disable();
            txtArgName->Disable();
            sel=cbLanguage->FindString(wxT("c"));
        }
        else if (isProcedure)
            sel=cbLanguage->FindString(wxT("edbspl"));
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
    wxString sql=wxT(" WHERE proname=") + qtDbString(GetName()) +
        wxT("\n   AND pronamespace=") + schema->GetOidStr();

    long argCount;
    for (argCount=0 ; argCount < (int)argOids.GetCount() ; argCount++)
        sql += wxT("\n   AND proargtypes[") + NumToStr(argCount) + wxT("] = ") + argOids.Item(argCount);

    sql += wxT("\n   AND proargtypes[") + NumToStr(argCount) + wxT("] = 0\n");

    pgObject *obj=functionFactory.AppendFunctions(collection, collection->GetSchema(), 0, sql);
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
              || GetArgs() != function->GetArgTypeNames()
              || (isC && (txtObjectFile->GetValue() != function->GetBin() || txtLinkSymbol->GetValue() != function->GetSource()))
              || (!isC && txtSqlBox->GetText() != function->GetSource()));
    }
    else
    {

        bool enable=true;

        CheckValid(enable, !name.IsEmpty(), _("Please specify name."));
        if (!isProcedure)
            CheckValid(enable, cbReturntype->GetGuessedSelection() >= 0, _("Please select return type."));

        if (!isProcedure || !connection->EdbMinimumVersion(8, 0))
            CheckValid(enable, cbLanguage->GetCurrentSelection() >= 0, _("Please select language."));

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

bool dlgFunction::IsUpToDate()
{
	if (function && !function->IsUpToDate())
		return false;
	else
		return true;
}


void dlgFunction::ReplaceSizer(wxWindow *w, bool isC, int border)
{
    if (isC && !w->GetContainingSizer())
        libcSizer->Add(w, 0, wxLEFT|wxTOP, border);
    else
        libcSizer->Detach(w);
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
            wxString colName=lstArguments->GetText(row, 0);
            int i=GetDirection(colName);
            rdbDirection->SetSelection(i);

            if (colName != rdbDirection->GetString(i))
            {
                if (isProcedure)
                    colName = colName.Left(colName.Length() - (rdbDirection->GetString(i).Length() + 1));
                else
                    colName = colName.Mid(rdbDirection->GetString(i).Length() +1);
                    
                txtArgName->SetValue(colName);
            }
            else
                txtArgName->SetValue(wxEmptyString);

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
    cbReturntype->GuessSelection(ev);
    CheckChange();
}


void dlgFunction::OnSelChangeType(wxCommandEvent &ev)
{
    cbDatatype->GuessSelection(ev);
    OnChangeArgName(ev);
}


void dlgFunction::OnChangeArgName(wxCommandEvent &ev)
{
    int argNameRow=-1;
    if (!txtArgName->GetValue().IsEmpty())
        argNameRow = lstArguments->FindItem(-1, txtArgName->GetValue());

    int pos=lstArguments->GetSelection();

    bool typeValid = (function != 0 || cbDatatype->GetGuessedSelection() >= 0);

    btnChange->Enable(pos >= 0 && typeValid);
    if (!function)
    {
        btnAdd->Enable(argNameRow < 0 && typeValid);
        btnRemove->Enable(pos >= 0);
    }
}


void dlgFunction::OnChangeArg(wxCommandEvent &ev)
{
    int row=lstArguments->GetSelection();

    if (row >= 0)
    {
        if (typeColNo)
        {
            wxString colName;

            if (txtArgName->GetValue().IsEmpty())
                colName = rdbDirection->GetStringSelection(); 
            else
            {
                if (isProcedure)
                    colName = txtArgName->GetValue() + wxT(" ") + rdbDirection->GetStringSelection(); 
                else
                    colName = rdbDirection->GetStringSelection() + wxT(" ") + txtArgName->GetValue();
            }

            lstArguments->SetItem(row, 0, colName);
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
    {
        wxString colName;

        if (txtArgName->GetValue().IsEmpty())
            colName = rdbDirection->GetStringSelection(); 
        else
        {
            if (isProcedure)
                colName = txtArgName->GetValue() + wxT(" ") + rdbDirection->GetStringSelection(); 
            else
                colName = rdbDirection->GetStringSelection() + wxT(" ") + txtArgName->GetValue();
        }

        lstArguments->AppendItem(-1, colName, cbDatatype->GetValue());
    }
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


int dlgFunction::GetDirection(const wxString &colName)
{
    unsigned int i;
    wxString sel;
    for (i=rdbDirection->GetCount() ; i>0  ; i--)
    {
        sel = rdbDirection->GetString(i-1);
        if (sel == colName)
            return i-1;
        if (isProcedure)
        {
            if (colName.Right(sel.Length()+1) == wxT(" ") + sel)
                return i-1;
        }
        else
        {
            if (colName.Left(sel.Length()+1) == sel + wxT(" "))
                return i-1;
        }
    }
    return -1;
}


wxString dlgFunction::GetArgs(const bool withNames)
{
    wxString args;
 
    for (int i=0; i < lstArguments->GetItemCount(); i++)
    {
        if (i)
            args += wxT(", ");

        if (typeColNo)
        {
            if (withNames)
            {
                wxString colName = lstArguments->GetText(i);

                int i=GetDirection(colName);

                if (i >= 0)
                {
                    wxString dir=rdbDirection->GetString(i);
                    if (isProcedure)
                    {
                        if (colName != dir)
                        {
                            colName = colName.Left(colName.Length() - (dir.Length() + 1));
                            args += qtIdent(colName) + wxT(" ") + dir + wxT(" ");
                        }
                        else
                            args += dir + wxT(" ");
                    }
                    else
                    {
                        if (colName != dir)
                        {
                            colName = colName.Mid(dir.Length()+1);
                            args += dir + wxT(" ") + qtIdent(colName) + wxT(" ");
                        }
                        else
                            args += dir + wxT(" ");
                    }
                }
                else
                    if (!colName.IsEmpty())
                        args += qtIdent(colName) + wxT(" ");  
            }
        }

        args += lstArguments->GetText(i, typeColNo);
    }

    return args;
}


wxString dlgFunction::GetSql()
{
    wxString sql;
    wxString name=GetName();
    wxString objType;
    if (isProcedure)
        objType = wxT("PROCEDURE ");
    else
        objType = wxT("FUNCTION ");

    bool isC=cbLanguage->GetValue().IsSameAs(wxT("C"), false);
    bool didChange = !function 
        || cbVolatility->GetValue() != function->GetVolatility()
        || chkSecureDefiner->GetValue() != function->GetSecureDefiner()
        || chkStrict->GetValue() != function->GetIsStrict()
        || cbOwner->GetValue() != function->GetOwner()
        || GetArgs() != function->GetArgTypeNames()
        || (isC && (txtObjectFile->GetValue() != function->GetBin() || txtLinkSymbol->GetValue() != function->GetSource()))
        || (!isC && txtSqlBox->GetText() != function->GetSource());

    if (function)
    {
        // edit mode
        if (name != function->GetName())
        {
            sql = wxT("ALTER FUNCTION ") + function->GetQuotedFullIdentifier() 
                                         + wxT("(") + function->GetArgTypes() + wxT(")")
                + wxT(" RENAME TO ") + qtIdent(name) + wxT(";\n");
        }
 
        if (didChange)
            sql += wxT("CREATE OR REPLACE ") + objType;
    }
    else
    {
        // create mode
        sql = wxT("CREATE " ) + objType;
    }


    name = schema->GetQuotedPrefix() + qtIdent(name) 
         + wxT("(") + GetArgs(false) + wxT(")");


    if (didChange)
    {
        sql  += schema->GetQuotedPrefix() + qtIdent(GetName()) 
             + wxT("(") + GetArgs() + wxT(")");

        if (!isProcedure)
        {
            sql += wxT(" RETURNS ");
            if (chkSetof->GetValue())
                sql += wxT("SETOF ");

            sql += cbReturntype->GetValue();
        }

        sql += wxT(" AS\n");

        if (isProcedure)
        {
            sql += txtSqlBox->GetText();
            sql += wxT("\n");
        }
        else
        {
            if (cbLanguage->GetValue().IsSameAs(wxT("C"), false))
            {
                sql += qtDbString(txtObjectFile->GetValue());
                if (!txtLinkSymbol->GetValue().IsEmpty())
                    sql += wxT(", ") + qtDbString(txtLinkSymbol->GetValue());
            }
            else
            {
                if (connection->BackendMinimumVersion(7, 5))
                    sql += qtDbStringDollar(txtSqlBox->GetText());
                else
                    sql += qtDbString(txtSqlBox->GetText());
            }

            sql += wxT("\nLANGUAGE ") + qtDbString(cbLanguage->GetValue())
                +  wxT(" ") + cbVolatility->GetValue();
            if (chkStrict->GetValue())
                sql += wxT(" STRICT");
            if (chkSecureDefiner->GetValue())
                sql += wxT(" SECURITY DEFINER");

            sql += wxT(";\n");
        }
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


