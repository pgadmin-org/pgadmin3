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


void pgFunction::ShowTreeDetail(wxTreeCtrl *browser, frmMain *form, wxListCtrl *properties, wxListCtrl *statistics, ctlSQLBox *sqlPane)
{
    SetButtons(form);

    properties->ClearAll();
    properties->InsertColumn(0, wxT("Property"), wxLIST_FORMAT_LEFT, 150);
    properties->InsertColumn(1, wxT("Value"), wxLIST_FORMAT_LEFT, 200);
  

    int pos=0;

    InsertListItem(properties, pos++, wxT("Name"), GetName());
    InsertListItem(properties, pos++, wxT("OID"), NumToStr(GetOid()));
    InsertListItem(properties, pos++, wxT("Owner"), GetOwner());
    InsertListItem(properties, pos++, wxT("Argument Count"), NumToStr(GetArgCount()));
    InsertListItem(properties, pos++, wxT("Arguments"), GetArgTypes());
    InsertListItem(properties, pos++, wxT("Returns"), GetReturnType());
    InsertListItem(properties, pos++, wxT("Language"), GetLanguage());
    // Source
    InsertListItem(properties, pos++, wxT("Return a Set?"), BoolToYesNo(GetReturnAsSet()));
    InsertListItem(properties, pos++, wxT("Volatility"), GetVolatility());
    InsertListItem(properties, pos++, wxT("Secure Definer?"), BoolToYesNo(GetSecureDefiner()));
    InsertListItem(properties, pos++, wxT("Strict"), BoolToYesNo(GetIsStrict()));
    InsertListItem(properties, pos++, wxT("System Function"), BoolToYesNo(GetSystemObject()));
    InsertListItem(properties, pos++, wxT("Comment"), GetComment());
}



void pgFunction::ShowTreeCollection(pgCollection *collection, frmMain *form, wxTreeCtrl *browser, wxListCtrl *properties, wxListCtrl *statistics, ctlSQLBox *sqlPane)
{
    pgFunction *function;

    if (browser->GetChildrenCount(collection->GetId(), FALSE) == 0)
    {
        // Log
        wxLogInfo(wxT("Adding Functions to schema ") + collection->GetSchema()->GetIdentifier());

        // Get the Functions
        pgSet *functions= collection->GetDatabase()->ExecuteSet(wxT(
            "SELECT pr.oid, pr.*, TYP.typname, lanname, pg_get_userbyid(proowner) as funcowner\n"
            "  FROM pg_proc pr\n"
            "  JOIN pg_type TYP ON TYP.oid=prorettype\n"
            "  JOIN pg_language LNG ON LNG.oid=prolang\n"
            " WHERE proisagg = FALSE AND pronamespace = ") + NumToStr(collection->GetSchema()->GetOid()) + wxT("::oid\n"
            " ORDER BY proname"));

        if (functions)
        {
            while (!functions->Eof())
            {
                function = new pgFunction(collection->GetSchema(), functions->GetVal(wxT("proname")));

                function->iSetOid(StrToDouble(functions->GetVal(wxT("oid"))));
                function->iSetOwner(functions->GetVal(wxT("funcowner")));
                function->iSetArgCount(StrToLong(functions->GetVal(wxT("pronargs"))));
                function->iSetReturnType(functions->GetVal(wxT("typname")));
                wxString oids=functions->GetVal(wxT("proargtypes"));
                function->iSetArgTypeOids(oids);

                wxString str, argTypes;
                wxStringTokenizer args(oids);
                while (args.HasMoreTokens())
                {
                    str = args.GetNextToken();
                    pgSet *set=collection->GetDatabase()->ExecuteSet(wxT(
                        "SELECT typname FROM pg_type where oid=") + str);
                    if (set)
                    {
                        if (!argTypes.IsNull())
                            argTypes += wxT(", ");
                        argTypes += set->GetVal(0);
                        delete set;
                    }
                }

                function->iSetArgTypes(argTypes);

                function->iSetLanguage(functions->GetVal(wxT("lanname")));
                function->iSetSecureDefiner(StrToBool(functions->GetVal(wxT("prosecdef"))));
                function->iSetReturnAsSet(StrToBool(functions->GetVal(wxT("proretset"))));
                function->iSetIsStrict(StrToBool(functions->GetVal(wxT("proisstrict"))));
                wxString vol=functions->GetVal(wxT("provolatile"));
                function->iSetVolatility(
                    vol.IsSameAs("i") ? wxT("immutable") : 
                    vol.IsSameAs("s") ? wxT("stable") :
                    vol.IsSameAs("v") ? wxT("volatile") : wxT("unknown"));

                browser->AppendItem(collection->GetId(), function->GetFullName(), PGICON_FUNCTION, -1, function);
	    
			    functions->MoveNext();
            }

		    delete functions;
        }
    }
}

