//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2005, The pgAdmin Development Team
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
#include "pgObject.h"
#include "pgFunction.h"
#include "pgCollection.h"


pgFunction::pgFunction(pgSchema *newSchema, const wxString& newName)
: pgSchemaObject(newSchema, PG_FUNCTION, newName)
{
}

pgFunction::~pgFunction()
{
}


bool pgFunction::DropObject(wxFrame *frame, wxTreeCtrl *browser)
{
    return GetDatabase()->ExecuteVoid(wxT("DROP FUNCTION ") + GetQuotedFullIdentifier()  + wxT("(") + GetQuotedArgTypes() + wxT(");"));
}

pgFunction::pgFunction(pgSchema *newSchema, int newType, const wxString& newName)
: pgSchemaObject(newSchema, newType, newName)
{
}

pgTriggerFunction::pgTriggerFunction(pgSchema *newSchema, const wxString& newName)
: pgFunction(newSchema, PG_TRIGGERFUNCTION, newName)
{
}


wxString pgFunction::GetSql(wxTreeCtrl *browser)
{
    if (sql.IsNull())
    {
        wxString qtName = wxT("FUNCTION ") + GetQuotedFullIdentifier()  + wxT("(") + GetQuotedArgTypes() + wxT(")");

        sql = wxT("-- Function: ") + GetFullIdentifier() + wxT("(") + GetArgTypeNames() + wxT(")\n\n")
            + wxT("-- DROP ") + qtName + wxT(";")
            + wxT("\n\nCREATE OR REPLACE ") + qtName
            + wxT("\n  RETURNS ");
        if (GetReturnAsSet())
            sql += wxT("SETOF ");
        sql +=GetQuotedReturnType() 
            + wxT(" AS\n");
        
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
            +  GetOwnerSql(8, 0, qtName)
            +  GetGrant(wxT("X"), qtName);

        if (!GetComment().IsNull())
        {
            sql += wxT("COMMENT ON ") + qtName
                + wxT(" IS ") + qtString(GetComment()) + wxT(";\n");
        }
    }

    return sql;
}


void pgFunction::ShowTreeDetail(wxTreeCtrl *browser, frmMain *form, ctlListView *properties, ctlSQLBox *sqlPane)
{
    if (properties)
    {
        CreateListColumns(properties);

        properties->AppendItem(_("Name"), GetName());
        properties->AppendItem(_("OID"), GetOid());
        properties->AppendItem(_("Owner"), GetOwner());
        properties->AppendItem(_("Argument count"), GetArgCount());
        properties->AppendItem(_("Arguments"), GetArgTypeNames());
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




pgFunction *pgFunction::AppendFunctions(pgObject *obj, pgSchema *schema, wxTreeCtrl *browser, const wxString &restriction)
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
            if (functions->GetVal(wxT("typname")).IsSameAs(wxT("trigger")))
                function = new pgTriggerFunction(schema, functions->GetVal(wxT("proname")));
            else
                function = new pgFunction(schema, functions->GetVal(wxT("proname")));

            function->iSetOid(functions->GetOid(wxT("oid")));
            function->iSetOwner(functions->GetVal(wxT("funcowner")));
            function->iSetAcl(functions->GetVal(wxT("proacl")));
            function->iSetArgCount(functions->GetLong(wxT("pronargs")));
            function->iSetReturnType(obj->GetDatabase()->GetSchemaPrefix(functions->GetVal(wxT("typnsp"))) + functions->GetVal(wxT("typname")));
			function->iSetQuotedReturnType(obj->GetDatabase()->GetQuotedSchemaPrefix(functions->GetVal(wxT("typnsp"))) + qtIdent(functions->GetVal(wxT("typname"))));
            function->iSetComment(functions->GetVal(wxT("description")));
            wxString oids=functions->GetVal(wxT("proargtypes"));
            wxString argNames;
            if (obj->GetConnection()->BackendMinimumVersion(7, 5))
                argNames = functions->GetVal(wxT("proargnames"));
            function->iSetArgTypeOids(oids);

            wxString str, argTypes, quotedArgTypes, argTypeNames, quotedArgTypeNames;
            wxStringTokenizer args(oids);
            wxStringTokenizer names(argNames.Mid(1, argNames.Length()-2), wxT(","));
            while (args.HasMoreTokens())
            {
                str = args.GetNextToken();

                if (types)
                {
                    types->MoveFirst();
                    while (types->GetVal(wxT("oid")) != str)
                        types->MoveNext();

                    if (!argTypes.IsNull())
                    {
                        argTypes += wxT(", ");
                        quotedArgTypes += wxT(", ");
                        argTypeNames += wxT(", ");
                        quotedArgTypeNames += wxT(", ");
                    }
                    str = names.GetNextToken();
                    if (!str.IsNull())
                    {
                        function->iAddArgName(str);
                        argTypeNames += str + wxT(" ");
                        quotedArgTypeNames += qtIdent(str) + wxT(" ");
                    }

    				argTypeNames += obj->GetDatabase()->GetSchemaPrefix(types->GetVal(wxT("nspname"))) + types->GetVal(wxT("typname"));
					quotedArgTypeNames += obj->GetDatabase()->GetQuotedSchemaPrefix(types->GetVal(wxT("nspname"))) + qtIdent(types->GetVal(wxT("typname")));
    				argTypes += obj->GetDatabase()->GetSchemaPrefix(types->GetVal(wxT("nspname"))) + types->GetVal(wxT("typname"));
					quotedArgTypes += obj->GetDatabase()->GetQuotedSchemaPrefix(types->GetVal(wxT("nspname"))) + qtIdent(types->GetVal(wxT("typname")));
                }
            }

            function->iSetArgTypes(argTypes);
            function->iSetQuotedArgTypes(quotedArgTypes);
            function->iSetArgTypeNames(argTypeNames);
            function->iSetQuotedArgTypeNames(quotedArgTypeNames);

            function->iSetLanguage(functions->GetVal(wxT("lanname")));
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
                obj->AppendBrowserItem(browser, function);
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



pgObject *pgFunction::Refresh(wxTreeCtrl *browser, const wxTreeItemId item)
{
    pgObject *function=0;
    wxTreeItemId parentItem=browser->GetItemParent(item);
    if (parentItem)
    {
        pgObject *obj=(pgObject*)browser->GetItemData(parentItem);
        if (obj->GetType() == PG_FUNCTIONS || obj->GetType() == PG_TRIGGERFUNCTIONS)
            function = AppendFunctions((pgCollection*)obj, GetSchema(), 0, wxT(" WHERE pr.oid=") + GetOidStr() + wxT("\n"));
    }
    return function;
}



pgObject *pgFunction::ReadObjects(pgCollection *collection, wxTreeCtrl *browser)
{
    wxString funcRestriction=wxT(
        " WHERE proisagg = FALSE AND pronamespace = ") + NumToStr(collection->GetSchema()->GetOid()) 
        + wxT("::oid\n   AND typname <> 'trigger'\n");

    // Get the Functions
    return AppendFunctions(collection, collection->GetSchema(), browser, funcRestriction);
}


pgObject *pgTriggerFunction::ReadObjects(pgCollection *collection, wxTreeCtrl *browser)
{
    wxString funcRestriction=wxT(
        " WHERE proisagg = FALSE AND pronamespace = ") + NumToStr(collection->GetSchema()->GetOid()) 
        + wxT("::oid\n   AND typname = 'trigger'\n");

    // Get the Functions
    return AppendFunctions(collection, collection->GetSchema(), browser, funcRestriction);
}
