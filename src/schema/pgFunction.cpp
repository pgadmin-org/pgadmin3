//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
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
    return GetDatabase()->ExecuteVoid(wxT("DROP FUNCTION ") + GetQuotedFullIdentifier()  + wxT("(") + GetArgTypes() + wxT(")"));
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
        sql = wxT("-- Function: ") + GetQuotedFullIdentifier() + wxT("(") + GetArgTypes() + wxT(")\n")
            + wxT("CREATE OR REPLACE FUNCTION ") + GetQuotedFullIdentifier() + wxT("(") + GetArgTypes()
            + wxT(")\n  RETURNS ") + GetReturnType() 
            + wxT(" AS '")
            + GetSource()
            + wxT("'\n  LANGUAGE '") + GetLanguage() + wxT("' ") + GetVolatility();

        if (GetIsStrict())
            sql += wxT(" STRICT");
        if (GetSecureDefiner())
            sql += wxT(" SECURE DEFINER");
        sql += wxT(";\n")
            + GetCommentSql();
    }

    return sql;
}


void pgFunction::ShowTreeDetail(wxTreeCtrl *browser, frmMain *form, wxListCtrl *properties, wxListCtrl *statistics, ctlSQLBox *sqlPane)
{
    if (properties)
    {
        CreateListColumns(properties);
        int pos=0;

        InsertListItem(properties, pos++, wxT("Name"), GetName());
        InsertListItem(properties, pos++, wxT("OID"), GetOid());
        InsertListItem(properties, pos++, wxT("Owner"), GetOwner());
        InsertListItem(properties, pos++, wxT("Argument Count"), GetArgCount());
        InsertListItem(properties, pos++, wxT("Arguments"), GetArgTypes());
        InsertListItem(properties, pos++, wxT("Returns"), GetReturnType());
        InsertListItem(properties, pos++, wxT("Language"), GetLanguage());
        InsertListItem(properties, pos++, wxT("Returns a Set?"), GetReturnAsSet());
        InsertListItem(properties, pos++, wxT("Source"), GetSource());
        InsertListItem(properties, pos++, wxT("Volatility"), GetVolatility());
        InsertListItem(properties, pos++, wxT("Secure Definer?"), GetSecureDefiner());
        InsertListItem(properties, pos++, wxT("Strict?"), GetIsStrict());
        InsertListItem(properties, pos++, wxT("System Function?"), GetSystemObject());
        InsertListItem(properties, pos++, wxT("Comment"), GetComment());
    }
}




pgFunction *pgFunction::AppendFunctions(pgObject *obj, pgSchema *schema, wxTreeCtrl *browser, const wxString &restriction)
{
    pgFunction *function=0;

    pgSet *functions = obj->GetDatabase()->ExecuteSet(wxT(
            "SELECT pr.oid, pr.*, TYP.typname, lanname, pg_get_userbyid(proowner) as funcowner, description\n"
            "  FROM pg_proc pr\n"
            "  JOIN pg_type TYP ON TYP.oid=prorettype\n"
            "  JOIN pg_language LNG ON LNG.oid=prolang\n"
            "  LEFT OUTER JOIN pg_description des ON des.objoid=pr.oid\n"
            + restriction +
            " ORDER BY proname"));

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
            wxString vol=functions->GetVal(wxT("provolatile"));
            function->iSetVolatility(
                vol.IsSameAs("i") ? wxT("IMMUTABLE") : 
                vol.IsSameAs("s") ? wxT("STABLE") :
                vol.IsSameAs("v") ? wxT("VOLATILE") : wxT("unknown"));


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
            function = AppendFunctions((pgCollection*)obj, GetSchema(), 0, wxT("   AND pr.oid=") + GetOidStr() + wxT("\n"));
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
