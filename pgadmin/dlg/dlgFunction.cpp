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
#define txtCost             CTRL_TEXT("txtCost")
#define txtRows             CTRL_TEXT("txtRows")

#define lstArguments        CTRL_LISTVIEW("lstArguments")
#define rdbIn               CTRL_RADIOBUTTON("rdbIn")
#define rdbOut              CTRL_RADIOBUTTON("rdbOut")
#define rdbInOut            CTRL_RADIOBUTTON("rdbInOut")

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
    EVT_TEXT(XRCID("txtCost"),                      dlgProperty::OnChange)
    EVT_TEXT(XRCID("txtRows"),                      dlgProperty::OnChange)
    EVT_STC_MODIFIED(XRCID("txtSqlBox"),            dlgProperty::OnChangeStc)

    EVT_CHECKBOX(XRCID("chkSetof"),                 dlgFunction::OnChangeSetof)
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
        rdbIn->Disable();
        rdbOut->Disable();
        rdbInOut->Disable();
        isProcedure = function->GetIsProcedure();
    }
    else
        cbOwner->Append(wxEmptyString);

    AddGroups();
    AddUsers(cbOwner);

    lstArguments->AddColumn(_("Type"), 80);
    lstArguments->AddColumn(_("Direction"), 40);
    lstArguments->AddColumn(_("Name"), 80);

    if (!connection->BackendMinimumVersion(8, 0))
        cbOwner->Disable();

    if (!connection->BackendMinimumVersion(8, 3))
        txtCost->Disable();

    txtRows->Disable();

    if (!connection->BackendMinimumVersion(8, 0))
        txtArgName->Disable();

    if (isProcedure)
    {
        if (function)
            txtName->Disable();
        cbOwner->Disable();
        txtComment->Disable();
        stLanguage->Disable();
        cbLanguage->Disable();
        chkStrict->Disable();
        chkSecureDefiner->Disable();
        chkSetof->Disable();
        stVolatility->Disable();
        cbVolatility->Disable();
        stReturntype->Disable();
        cbReturntype->Disable();
        txtCost->Disable();
        txtRows->Disable();
    }
    else
    {
        if (!connection->BackendMinimumVersion(8, 1))
        {
            rdbIn->SetValue(true);
            rdbIn->Disable();
            rdbOut->Disable();
            rdbInOut->Disable();
        }
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
                lstArguments->AppendItem(-1, argTypes.Item(i), argModes[i], argNames[i]);
            }
        }

        txtArguments->SetValue(function->GetArgListWithNames());
        cbReturntype->Append(function->GetReturnType());
        cbReturntype->SetValue(function->GetReturnType());

        cbLanguage->SetValue(function->GetLanguage());
        cbVolatility->SetValue(function->GetVolatility());

        chkSetof->SetValue(function->GetReturnAsSet());
        chkStrict->SetValue(function->GetIsStrict());
        chkSecureDefiner->SetValue(function->GetSecureDefiner());

        if (connection->BackendMinimumVersion(8, 3))
        {
            txtCost->SetValue(NumToStr(function->GetCost()));
            if (function->GetReturnAsSet())
            {
                txtRows->SetValue(NumToStr(function->GetRows()));
                txtRows->Enable();
            }
            else
                txtRows->Disable();
        }

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
    bool enable=true, didChange=true;

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

    if (function)
    {
        didChange = (txtComment->GetValue() != function->GetComment()
              || name != function->GetName()
              || cbVolatility->GetValue() != function->GetVolatility()
              || chkSecureDefiner->GetValue() != function->GetSecureDefiner()
              || chkStrict->GetValue() != function->GetIsStrict()
              || cbLanguage->GetValue() != function->GetLanguage()
              || cbOwner->GetValue() != function->GetOwner()
              || GetArgs() != function->GetArgListWithNames()
              || (isC && (txtObjectFile->GetValue() != function->GetBin() || txtLinkSymbol->GetValue() != function->GetSource()))
              || (!isC && txtSqlBox->GetText() != function->GetSource()));

        if (connection->BackendMinimumVersion(8, 3))
        {
            didChange = (didChange ||
                txtCost->GetValue() != NumToStr(function->GetCost()) ||
                (chkSetof->GetValue() && txtRows->GetValue() != NumToStr(function->GetRows())));
        }
    }

    EnableOK(enable && didChange);
}

bool dlgFunction::IsUpToDate()
{
	if (function && !function->IsUpToDate())
		return false;
	else
		return true;
}
  
void dlgFunction::OnSelChangeLanguage(wxCommandEvent &ev)
{
    bool isC=(cbLanguage->GetValue().IsSameAs(wxT("C"), false));

    stObjectFile->Show(isC);
    txtObjectFile->Show(isC);
    stLinkSymbol->Show(isC);
    txtLinkSymbol->Show(isC);
    txtSqlBox->Show(!isC);

    txtSqlBox->GetContainingSizer()->Layout();

    CheckChange();
}


void dlgFunction::OnSelChangeArg(wxListEvent &ev)
{
    int row=lstArguments->GetSelection();
    if (row >= 0)
    {
        cbDatatype->SetValue(lstArguments->GetText(row, 0));
        wxString mode = lstArguments->GetText(row, 1);
        if (mode == wxT("IN"))
            rdbIn->SetValue(true);
        else if (mode == wxT("OUT"))
            rdbOut->SetValue(true);
        else if (mode == wxT("IN OUT") || mode == wxT("INOUT"))
            rdbInOut->SetValue(true);
        txtArgName->SetValue(lstArguments->GetText(row, 2));

        wxCommandEvent ev;
        OnChangeArgName(ev);
    }
}


void dlgFunction::OnChangeReturn(wxCommandEvent &ev)
{
    cbReturntype->GuessSelection(ev);
    CheckChange();
}


void dlgFunction::OnChangeSetof(wxCommandEvent &ev)
{
    if (chkSetof->GetValue() && connection->BackendMinimumVersion(8, 3) && !isProcedure)
        txtRows->Enable();
    else
        txtRows->Disable();

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
        lstArguments->SetItem(row, 0, cbDatatype->GetValue());
        lstArguments->SetItem(row, 1, GetSelectedDirection());
        lstArguments->SetItem(row, 2, txtArgName->GetValue());

        if (!function)
            argOids.Item(row) = typOids.Item(cbDatatype->GetGuessedSelection());
        txtArguments->SetValue(GetArgs());
    }
    OnChangeArgName(ev);
    CheckChange();
}


void dlgFunction::OnAddArg(wxCommandEvent &ev)
{
    lstArguments->AppendItem(-1, cbDatatype->GetValue(), GetSelectedDirection(), txtArgName->GetValue());

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

wxString dlgFunction::GetSelectedDirection()
{
    if (rdbIn->GetValue())
        return wxT("IN");
    else if (rdbOut->GetValue())
        return wxT("OUT");
    else if (rdbInOut->GetValue())
    {
        if (isProcedure)
            return wxT("IN OUT");
        else
            return wxT("INOUT");
    }
    else
        return wxEmptyString;
}


wxString dlgFunction::GetArgs(const bool withNames, const bool inOnly)
{
    wxString args;
 
    for (int i=0; i < lstArguments->GetItemCount(); i++)
    {
        if (inOnly && lstArguments->GetText(i, 1) == wxT("OUT"))
            continue;

        if (i && !args.EndsWith(wxT(", ")))
            args += wxT(", ");

        if (isProcedure)
        {
            if (withNames && lstArguments->GetText(i, 2) != wxEmptyString)
                args += qtIdent(lstArguments->GetText(i, 2)) + wxT(" ");

            if (lstArguments->GetText(i, 1) != wxEmptyString)
                args += lstArguments->GetText(i, 1) + wxT(" ") + 
                
            args += lstArguments->GetText(i, 0);
        }
        else
        {
            if (connection->BackendMinimumVersion(8, 1) && lstArguments->GetText(i, 1) != wxEmptyString)
                args += lstArguments->GetText(i, 1) + wxT(" ");

            if (connection->BackendMinimumVersion(8, 0) && withNames && lstArguments->GetText(i, 2) != wxEmptyString)
                args += qtIdent(lstArguments->GetText(i, 2)) + wxT(" ");

            args += lstArguments->GetText(i, 0);
        }
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
        || GetArgs() != function->GetArgListWithNames()
        || (isC && (txtObjectFile->GetValue() != function->GetBin() || txtLinkSymbol->GetValue() != function->GetSource()))
        || (!isC && txtSqlBox->GetText() != function->GetSource());

    if (connection->BackendMinimumVersion(8, 3))
    {
        didChange = (didChange ||
            txtCost->GetValue() != NumToStr(function->GetCost()) ||
            (chkSetof->GetValue() && txtRows->GetValue() != NumToStr(function->GetRows())));
    }

    if (function)
    {
        // edit mode
        if (name != function->GetName())
        {
            sql = wxT("ALTER FUNCTION ") + function->GetQuotedFullIdentifier() 
                                         + wxT("(") + function->GetArgSigList() + wxT(")")
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

    if (didChange)
    {
        if (isProcedure && GetArgs().IsEmpty())
        {
            sql += schema->GetQuotedPrefix() + qtIdent(GetName());
        }
        else
        {
            sql += schema->GetQuotedPrefix() + qtIdent(GetName()) 
                + wxT("(") + GetArgs() + wxT(")");
        }

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

            // PostgreSQL 8.3+ cost/row estimations
            if (connection->BackendMinimumVersion(8, 3))
            {
                sql += wxT("\nCOST ") + txtCost->GetValue();

                if (chkSetof->GetValue())
                    sql += wxT("\nROWS ") + txtRows->GetValue();
            }

            sql += wxT(";\n");
        }
    }

    name = schema->GetQuotedPrefix() + qtIdent(name) 
         + wxT("(") + GetArgs(false, true) + wxT(")");

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

    if (isProcedure)
        sql += GetGrant(wxT("X"), wxT("PROCEDURE ") + name);
    else
        sql += GetGrant(wxT("X"), wxT("FUNCTION ") + name);

    AppendComment(sql, wxT("FUNCTION ") + name, function);

    return sql;
}


