//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002 - 2003, The pgAdmin Development Team
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
    return GetDatabase()->ExecuteVoid(wxT("DROP FUNCTION ") + GetQuotedFullIdentifier()  + wxT("(") + GetArgTypes() + wxT(");"));
}

pgFunction::pgFunction(pgSchema *newSchema, int newType, const wxString& newName)
: pgSchemaObject(newSchema, newType, newName)
{
}

pgTriggerFunction::pgTriggerFunction(pgSchema *newSchema, const wxString& newName)
: pgFunction(newSchema, PG_TRIGGERFUNCTION, newName)
{
}


bool pgFunction::CanReload()
{
    return GetLanguage().IsSameAs(wxT("C"), false);
}


bool pgFunction::ReloadLibrary()
{
    return ExecuteVoid(wxT("LOAD ") + qtString(GetBin()));
}


wxString pgFunction::GetSql(wxTreeCtrl *browser)
{
    if (sql.IsNull())
    {
        sql = wxT("-- Function: ") + GetQuotedFullIdentifier() + wxT("(") + GetArgTypes() + wxT(")\n\n")
            + wxT("-- DROP FUNCTION ") + GetQuotedFullIdentifier()  + wxT("(") + GetArgTypes() + wxT(");")
            + wxT("\n\nCREATE OR REPLACE FUNCTION ") + GetQuotedFullIdentifier() + wxT("(") + GetArgTypes()
            + wxT(")\n  RETURNS ");
        if (GetReturnAsSet())
            sql += wxT("SETOF ");
        sql +=GetReturnType() 
            + wxT(" AS\n");
        
        if (GetLanguage().IsSameAs(wxT("C"), false))
            sql += qtString(GetBin()) + wxT(", ");

        sql += qtString(GetSource())
            + wxT("\n  LANGUAGE '") + GetLanguage() + wxT("' ") + GetVolatility();

        if (GetIsStrict())
            sql += wxT(" STRICT");
        if (GetSecureDefiner())
            sql += wxT(" SECURITY DEFINER");
        sql += wxT(";\n");

    if (!GetComment().IsNull())
    {
        sql += wxT("COMMENT ON FUNCTION ") + GetQuotedFullIdentifier() + wxT("(") + GetArgTypes() + wxT(")")
            + wxT(" IS ") + qtString(GetComment()) + wxT(";\n");
    }
}

    return sql;
}


void pgFunction::ShowTreeDetail(wxTreeCtrl *browser, frmMain *form, wxListCtrl *properties, wxListCtrl *statistics, ctlSQLBox *sqlPane)
{
    if (properties)
    {
        CreateListColumns(properties);
        int pos=0;

        InsertListItem(properties, pos++, _("Name"), GetName());
        InsertListItem(properties, pos++, _("OID"), GetOid());
        InsertListItem(properties, pos++, _("Owner"), GetOwner());
        InsertListItem(properties, pos++, _("Argument Count"), GetArgCount());
        InsertListItem(properties, pos++, _("Arguments"), GetArgTypes());
        InsertListItem(properties, pos++, _("Returns"), GetReturnType());
        InsertListItem(properties, pos++, _("Language"), GetLanguage());
        InsertListItem(properties, pos++, _("Returns a Set?"), GetReturnAsSet());
        if (GetLanguage().IsSameAs(wxT("C"), false))
        {
            InsertListItem(properties, pos++, _("Object File"), GetBin());
            InsertListItem(properties, pos++, _("Link Symbol"), GetSource());
        }
        else
            InsertListItem(properties, pos++, _("Source"), GetSource());

        InsertListItem(properties, pos++, _("Volatility"), GetVolatility());
        InsertListItem(properties, pos++, _("Security Definer?"), GetSecureDefiner());
        InsertListItem(properties, pos++, _("Strict?"), GetIsStrict());
        InsertListItem(properties, pos++, _("ACL"), GetAcl());
        InsertListItem(properties, pos++, _("System Function?"), GetSystemObject());
        InsertListItem(properties, pos++, _("Comment"), GetComment());
    }
}




pgFunction *pgFunction::AppendFunctions(pgObject *obj, pgSchema *schema, wxTreeCtrl *browser, const wxString &restriction)
{
    pgFunction *function=0;

    pgSet *functions = obj->GetDatabase()->ExecuteSet(
            wxT("SELECT pr.oid, pr.*, TYP.typname, lanname, pg_get_userbyid(proowner) as funcowner, description\n")
            wxT("  FROM pg_proc pr\n")
            wxT("  JOIN pg_type TYP ON TYP.oid=prorettype\n")
            wxT("  JOIN pg_language LNG ON LNG.oid=prolang\n")
            wxT("  LEFT OUTER JOIN pg_description des ON des.objoid=pr.oid\n")
            + restriction +
            wxT(" ORDER BY proname"));

    pgSet *types = obj->GetDatabase()->ExecuteSet(wxT(
                    "SELECT oid, typname FROM pg_type"));

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
            function->iSetReturnType(functions->GetVal(wxT("typname")));
            function->iSetComment(functions->GetVal(wxT("description")));
            wxString oids=functions->GetVal(wxT("proargtypes"));
            function->iSetArgTypeOids(oids);

            wxString str, argTypes;
            wxStringTokenizer args(oids);
            while (args.HasMoreTokens())
            {
                str = args.GetNextToken();

                if (types)
                {
                    types->MoveFirst();
                    while (types->GetVal(0) != str)
                        types->MoveNext();

                    if (!argTypes.IsNull())
                        argTypes += wxT(", ");

                    argTypes += types->GetVal(1);

                }
            }

            function->iSetArgTypes(argTypes);

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
