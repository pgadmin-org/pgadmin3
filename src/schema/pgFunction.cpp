//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id: pgFunction.cpp 4936 2006-01-19 14:13:54Z dpage $
// Copyright (C) 2002 - 2006, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// pgFunction.cpp - function class
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "misc.h"
#include "pgFunction.h"


pgFunction::pgFunction(pgSchema *newSchema, const wxString& newName)
: pgSchemaObject(newSchema, functionFactory, newName)
{
}


pgFunction::pgFunction(pgSchema *newSchema, pgaFactory &factory, const wxString& newName)
: pgSchemaObject(newSchema, factory, newName)
{
}


pgFunction::~pgFunction()
{
}


bool pgFunction::DropObject(wxFrame *frame, ctlTree *browser, bool cascaded)
{
    wxString sql=wxT("DROP FUNCTION ") + GetQuotedFullIdentifier()  + wxT("(") + GetQuotedArgTypes() + wxT(")");
    if (cascaded)
        sql += wxT(" CASCADE");
    return GetDatabase()->ExecuteVoid(sql);
}

pgFunction::pgFunction(pgSchema *newSchema, int newType, const wxString& newName)
: pgSchemaObject(newSchema, newType, newName)
{
}

pgTriggerFunction::pgTriggerFunction(pgSchema *newSchema, const wxString& newName)
: pgFunction(newSchema, triggerFunctionFactory, newName)
{
}

pgProcedure::pgProcedure(pgSchema *newSchema, const wxString& newName)
: pgFunction(newSchema, procedureFactory, newName)
{
}


wxString pgFunction::GetSql(ctlTree *browser)
{
    if (sql.IsNull())
    {
        wxString qtName = GetQuotedFullIdentifier()  + wxT("(") + GetQuotedArgTypeNames() + wxT(")");

        sql = wxT("-- Function: ") + GetQuotedFullIdentifier() + wxT("(") + GetQuotedArgTypeNames() + wxT(")\n\n")
            + wxT("-- DROP FUNCTION ") + qtName + wxT(";")
            + wxT("\n\nCREATE OR REPLACE FUNCTION ") + qtName;

        if (!GetIsProcedure())
        {
            sql += wxT("\n  RETURNS ");
            if (GetReturnAsSet())
                sql += wxT("SETOF ");
            sql += GetQuotedReturnType();
        }
        else if (GetReturnAsSet())
        {
            sql += wxT("\n  RETURNS SETOF ");
            sql += GetQuotedReturnType();
        }

        sql += wxT(" AS\n");
        
        if (GetLanguage().IsSameAs(wxT("C"), false))
        {
            sql += qtString(GetBin()) + wxT(", ") + qtString(GetSource());
        }
        else
        {
            if (GetConnection()->BackendMinimumVersion(7, 5))
                sql += qtStringDollar(GetSource());
            else
                sql += qtString(GetSource());
        }
        sql += wxT("\n  LANGUAGE '") + GetLanguage() + wxT("' ") + GetVolatility();

        if (GetIsStrict())
            sql += wxT(" STRICT");
        if (GetSecureDefiner())
            sql += wxT(" SECURITY DEFINER");
        sql += wxT(";\n")
            +  GetOwnerSql(8, 0, wxT("FUNCTION ") + qtName)
            +  GetGrant(wxT("X"), wxT("FUNCTION ") + qtName);

        if (!GetComment().IsNull())
        {
            sql += wxT("COMMENT ON FUNCTION ") + qtName
                + wxT(" IS ") + qtString(GetComment()) + wxT(";\n");
        }
    }

    return sql;
}


void pgFunction::ShowTreeDetail(ctlTree *browser, frmMain *form, ctlListView *properties, ctlSQLBox *sqlPane)
{
    if (properties)
    {
        CreateListColumns(properties);

        properties->AppendItem(_("Name"), GetName());
        properties->AppendItem(_("OID"), GetOid());
        properties->AppendItem(_("Owner"), GetOwner());
        properties->AppendItem(_("Argument count"), GetArgCount());
        properties->AppendItem(_("Arguments"), GetArgTypeNames());
        if (!GetIsProcedure())
            properties->AppendItem(_("Return type"), GetReturnType());
        properties->AppendItem(_("Language"), GetLanguage());
        properties->AppendItem(_("Returns a set?"), GetReturnAsSet());
        if (GetLanguage().IsSameAs(wxT("C"), false))
        {
            properties->AppendItem(_("Object file"), GetBin());
            properties->AppendItem(_("Link symbol"), GetSource());
        }
        else
            properties->AppendItem(_("Source"), GetSource());

        properties->AppendItem(_("Volatility"), GetVolatility());
        properties->AppendItem(_("Security of definer?"), GetSecureDefiner());
        properties->AppendItem(_("Strict?"), GetIsStrict());
        properties->AppendItem(_("ACL"), GetAcl());
        properties->AppendItem(_("System function?"), GetSystemObject());
        properties->AppendItem(_("Comment"), GetComment());
    }
}


wxString pgProcedure::GetSql(ctlTree *browser)
{
    if (!GetConnection()->EdbMinimumVersion(8, 0))
        return pgFunction::GetSql(browser);

    if (sql.IsNull())
    {
        wxString qtName = GetQuotedFullIdentifier() + wxT("(") + GetQuotedArgTypeNames() + wxT(")");

        sql = wxT("-- Procedure: ") + GetQuotedFullIdentifier() + wxT("\n\n")
            + wxT("-- DROP PROCEDURE") + GetQuotedFullIdentifier() + wxT(";")
            + wxT("\n\nCREATE OR REPLACE ") + qtName;

        if (GetReturnAsSet())
        {
            sql += wxT("\n  RETURNS SETOF ");
            sql +=GetQuotedReturnType();
        }

        sql += wxT(" AS\n")
            + qtStringDollar(GetSource())
            + wxT(";\n");

        if (!GetComment().IsNull())
        {
            sql += wxT("COMMENT ON FUNCTION ") + qtName
                + wxT(" IS ") + qtString(GetComment()) + wxT(";\n");
        }
    }

    return sql;
}


bool pgProcedure::DropObject(wxFrame *frame, ctlTree *browser, bool cascaded)
{
    if (!GetConnection()->EdbMinimumVersion(8, 0))
        return pgFunction::DropObject(frame, browser, cascaded);

    wxString sql=wxT("DROP PROCEDURE ") + GetQuotedFullIdentifier();
    return GetDatabase()->ExecuteVoid(sql);
}




pgFunction *pgFunctionFactory::AppendFunctions(pgObject *obj, pgSchema *schema, ctlTree *browser, const wxString &restriction)
{
    pgFunction *function=0;
    wxString argNamesCol;
    if (obj->GetConnection()->BackendMinimumVersion(7, 5))
        argNamesCol = wxT("proargnames, ");

    pgSet *functions = obj->GetDatabase()->ExecuteSet(
            wxT("SELECT pr.oid, pr.*, TYP.typname, TYPNS.nspname AS typnsp, lanname, ") + argNamesCol + 
                        wxT("pg_get_userbyid(proowner) as funcowner, description\n")
            wxT("  FROM pg_proc pr\n")
            wxT("  JOIN pg_type TYP ON TYP.oid=prorettype\n")
            wxT("  JOIN pg_namespace TYPNS ON TYPNS.oid=TYP.typnamespace\n")
            wxT("  JOIN pg_language LNG ON LNG.oid=prolang\n")
            wxT("  LEFT OUTER JOIN pg_description des ON des.objoid=pr.oid\n")
            + restriction +
            wxT(" ORDER BY proname"));

    pgSet *types = obj->GetDatabase()->ExecuteSet(wxT(
                    "SELECT t.oid, t.typname, n.nspname FROM pg_type t, pg_namespace n WHERE t.typnamespace = n.oid"));

    if (functions)
    {
        while (!functions->Eof())
        {
            bool isProcedure=false;
            wxString lanname=functions->GetVal(wxT("lanname"));
            if (obj->GetConnection()->EdbMinimumVersion(8, 0) && lanname == wxT("edbspl"))
                isProcedure=true;

            wxString oids=functions->GetVal(wxT("proargtypes"));
            wxStringTokenizer args(oids);
            wxStringTokenizer modes(wxEmptyString);

            wxString argNames;

            if (obj->GetConnection()->BackendMinimumVersion(7, 5))
                argNames = functions->GetVal(wxT("proargnames"));

            if (obj->GetConnection()->EdbMinimumVersion(8, 0) && isProcedure)
            {
                wxString argDirs=functions->GetVal(wxT("proargdirs"));
                modes.SetString(argDirs);
            }
            if (obj->GetConnection()->BackendMinimumVersion(8, 1))
            {
                wxString allArgTypes=functions->GetVal(wxT("proallargtypes"));
                if (!allArgTypes.IsEmpty())
                    args.SetString(allArgTypes.Mid(1, allArgTypes.Length()-2), wxT(","));

                wxString argModes= functions->GetVal(wxT("proargmodes"));
                if (!argModes.IsEmpty())
                {
                    modes.SetString(argModes.Mid(1, argModes.Length()-2), wxT(","));
                    isProcedure=true;
                }
            }

            if (isProcedure)
                function = new pgProcedure(schema, functions->GetVal(wxT("proname")));
            else if (functions->GetVal(wxT("typname")).IsSameAs(wxT("trigger")))
                function = new pgTriggerFunction(schema, functions->GetVal(wxT("proname")));
            else
                function = new pgFunction(schema, functions->GetVal(wxT("proname")));


            wxString type, name, argTypes, quotedArgTypes, argTypeNames, quotedArgTypeNames, mode;
            wxStringTokenizer names(argNames.Mid(1, argNames.Length()-2), wxT(","));

            
            while (args.HasMoreTokens())
            {
                type = args.GetNextToken();

                if (types)
                {
                    types->MoveFirst();
                    while (types->GetVal(wxT("oid")) != type)
                    {
                        if (types->Eof())
                        {
                            wxLogError(wxT("Internal Error while checking function args"));
                            return 0;
                        }
                        types->MoveNext();
                    }
                    if (!argTypes.IsNull())
                    {
                        argTypes += wxT(", ");
                        quotedArgTypes += wxT(", ");
                        argTypeNames += wxT(", ");
                        quotedArgTypeNames += wxT(", ");
                    }
                    name = names.GetNextToken();
                    if (name[0] == '"')
                        name = name.Mid(1, name.Length()-2);

                    mode = modes.GetNextToken();
                    
                    if (!mode.IsNull())
                    {
                        if (mode == wxT('o') || mode == wxT("2"))
                            mode = wxT("OUT");
                        else if (mode == wxT("b"))
                            mode = wxT("INOUT");
                        else if (mode == wxT("3"))
                            mode = wxT("IN OUT");
                        else
                            mode = wxT("IN");

                        function->iAddArgMode(mode);
                        argTypeNames += mode + wxT(" ");
                        quotedArgTypeNames += mode + wxT(" ");
                        quotedArgTypes += mode + wxT(" ");
                    }
                    if (!name.IsNull())
                    {
                        function->iAddArgName(name);
                        argTypeNames += name + wxT(" ");
                        quotedArgTypeNames += qtIdent(name) + wxT(" ");
                    }

    				argTypeNames += obj->GetDatabase()->GetSchemaPrefix(types->GetVal(wxT("nspname"))) + types->GetVal(wxT("typname"));
					quotedArgTypeNames += obj->GetDatabase()->GetQuotedSchemaPrefix(types->GetVal(wxT("nspname"))) + qtIdent(types->GetVal(wxT("typname")));
    				argTypes += obj->GetDatabase()->GetSchemaPrefix(types->GetVal(wxT("nspname"))) + types->GetVal(wxT("typname"));
					quotedArgTypes += obj->GetDatabase()->GetQuotedSchemaPrefix(types->GetVal(wxT("nspname"))) + qtIdent(types->GetVal(wxT("typname")));
                }
            }

            function->iSetOid(functions->GetOid(wxT("oid")));
            function->UpdateSchema(browser, functions->GetOid(wxT("pronamespace")));
            function->iSetOwner(functions->GetVal(wxT("funcowner")));
            function->iSetAcl(functions->GetVal(wxT("proacl")));
            function->iSetArgCount(functions->GetLong(wxT("pronargs")));
            function->iSetReturnType(obj->GetDatabase()->GetSchemaPrefix(functions->GetVal(wxT("typnsp"))) + functions->GetVal(wxT("typname")));
			function->iSetQuotedReturnType(obj->GetDatabase()->GetQuotedSchemaPrefix(functions->GetVal(wxT("typnsp"))) + qtIdent(functions->GetVal(wxT("typname"))));
            function->iSetComment(functions->GetVal(wxT("description")));
            function->iSetArgTypeOids(oids);

            function->iSetArgTypes(argTypes);
            function->iSetQuotedArgTypes(quotedArgTypes);
            function->iSetArgTypeNames(argTypeNames);
            function->iSetQuotedArgTypeNames(quotedArgTypeNames);

            function->iSetLanguage(lanname);
            function->iSetSecureDefiner(functions->GetBool(wxT("prosecdef")));
            function->iSetReturnAsSet(functions->GetBool(wxT("proretset")));
            function->iSetIsStrict(functions->GetBool(wxT("proisstrict")));
            function->iSetSource(functions->GetVal(wxT("prosrc")));
            function->iSetBin(functions->GetVal(wxT("probin")));

            wxString vol=functions->GetVal(wxT("provolatile"));
            function->iSetVolatility(
                vol.IsSameAs(wxT("i")) ? wxT("IMMUTABLE") : 
                vol.IsSameAs(wxT("s")) ? wxT("STABLE") :
                vol.IsSameAs(wxT("v")) ? wxT("VOLATILE") : wxT("unknown"));


            if (browser)
            {
                browser->AppendObject(obj, function);
			    functions->MoveNext();
            }
            else
                break;
        }

		delete functions;
        delete types;
    }
    return function;
}



pgObject *pgFunction::Refresh(ctlTree *browser, const wxTreeItemId item)
{
    pgObject *function=0;
    pgCollection *coll=browser->GetParentCollection(item);
    if (coll)
        function = functionFactory.AppendFunctions(coll, GetSchema(), 0, wxT(" WHERE pr.oid=") + GetOidStr() + wxT("\n"));

    return function;
}



pgObject *pgFunctionFactory::CreateObjects(pgCollection *collection, ctlTree *browser, const wxString &restr)
{
    wxString funcRestriction=wxT(
        " WHERE proisagg = FALSE AND pronamespace = ") + NumToStr(collection->GetSchema()->GetOid()) 
        + wxT("::oid\n   AND typname <> 'trigger'\n");

    if (collection->GetConnection()->BackendMinimumVersion(8, 1))
        funcRestriction += wxT("   AND proargmodes IS NULL");
    else if (collection->GetConnection()->EdbMinimumVersion(8, 0))
        funcRestriction += wxT("   AND lanname <>'edbspl' AND typname <> 'void'\n");

    // Get the Functions
    return AppendFunctions(collection, collection->GetSchema(), browser, funcRestriction);
}


pgObject *pgTriggerFunctionFactory::CreateObjects(pgCollection *collection, ctlTree *browser, const wxString &restr)
{
    wxString funcRestriction=wxT(
        " WHERE proisagg = FALSE AND pronamespace = ") + NumToStr(collection->GetSchema()->GetOid()) 
        + wxT("::oid\n   AND typname = 'trigger'\n");

    // Get the Functions
    return AppendFunctions(collection, collection->GetSchema(), browser, funcRestriction);
}


pgObject *pgProcedureFactory::CreateObjects(pgCollection *collection, ctlTree *browser, const wxString &restr)
{
    wxString funcRestriction=wxT(
        " WHERE proisagg = FALSE AND pronamespace = ") + NumToStr(collection->GetSchema()->GetOid()) 
        + wxT("::oid\n");

    if (collection->GetConnection()->BackendMinimumVersion(8, 1))
        funcRestriction += wxT("   AND proargmodes IS NOT NULL");
    else if (collection->GetConnection()->EdbMinimumVersion(8, 0))
        funcRestriction += wxT("   AND lanname = 'edbspl' AND typname = 'void'\n");

    // Get the Functions
    return AppendFunctions(collection, collection->GetSchema(), browser, funcRestriction);
}


#include "images/function.xpm"
#include "images/functions.xpm"

pgFunctionFactory::pgFunctionFactory(const wxChar *tn, const wxChar *ns, const wxChar *nls, char **img) 
: pgSchemaObjFactory(tn, ns, nls, img)
{
    metaType = PGM_FUNCTION;
}

pgFunctionFactory functionFactory(__("Function"), __("New Function..."), __("Create a new Function."), function_xpm);
static pgaCollectionFactory cf(&functionFactory, __("Functions"), functions_xpm);


#include "images/triggerfunction.xpm"
#include "images/triggerfunctions.xpm"

pgTriggerFunctionFactory::pgTriggerFunctionFactory() 
: pgFunctionFactory(__("Trigger Function"), __("New Trigger Function..."), __("Create a new Trigger Function."), triggerfunction_xpm)
{
}

pgTriggerFunctionFactory triggerFunctionFactory;
static pgaCollectionFactory cft(&triggerFunctionFactory, __("Trigger Functions"), triggerfunctions_xpm);

pgProcedureFactory::pgProcedureFactory() 
: pgFunctionFactory(__("Procedure"), __("New Procedure"), __("Create a new Procedure."), function_xpm)
{
}


pgProcedureFactory procedureFactory;
static pgaCollectionFactory cfp(&procedureFactory, __("Procedures"), functions_xpm);
