//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id: edbPackageFunction.cpp 5884 2007-01-23 12:22:27Z dpage $
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// edbPackageFunction.cpp - EnterpriseDB Package member function
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "schema/edbPackageFunction.h"


edbPackageFunction::edbPackageFunction(edbPackage *newPackage, const wxString& newName)
: edbPackageObject(newPackage, packageFunctionFactory, newName)
{
}

edbPackageFunction::edbPackageFunction(edbPackage *newPackage,pgaFactory &factory, const wxString& newName)
: edbPackageObject(newPackage, factory, newName)
{
}

edbPackageProcedure::edbPackageProcedure(edbPackage *newPackage, const wxString& newName)
: edbPackageFunction(newPackage, packageProcedureFactory, newName)
{
}

wxString edbPackageFunction::GetFullName()
{ 
    return GetName() + wxT("(") + GetArgSigList() + wxT(")");
}

wxString edbPackageProcedure::GetFullName()
{ 
    if (GetArgSigList().IsEmpty())
        return GetName();
    else
        return GetName() + wxT("(") + GetArgSigList() + wxT(")");
}

wxString edbPackageFunction::GetArgListWithNames()
{
    wxString args;

    for (unsigned int i=0; i < argTypesArray.Count(); i++)
    {
        if (i > 0)
            args += wxT(", ");

        wxString arg;

        if (GetIsProcedure())
        {
            if (!argNamesArray.Item(i).IsEmpty())
                arg += qtIdent(argNamesArray.Item(i));

            if (!argModesArray.Item(i).IsEmpty())
                if (arg.IsEmpty())
                    arg += argModesArray.Item(i);
                else
                    arg += wxT(" ") + argModesArray.Item(i);
        }
        else
        {
            if (!argModesArray.Item(i).IsEmpty())
                arg += argModesArray.Item(i);

            if (!argNamesArray.Item(i).IsEmpty())
                if (arg.IsEmpty())
                    arg += qtIdent(argNamesArray.Item(i));
                else
                    arg += wxT(" ") + qtIdent(argNamesArray.Item(i));
        }

        if (!arg.IsEmpty())
            arg += wxT(" ") + argTypesArray.Item(i);
        else
            arg += argTypesArray.Item(i);

        args += arg;
    }
    return args;
}

wxString edbPackageFunction::GetArgSigList()
{
    wxString args;

    for (unsigned int i=0; i < argTypesArray.Count(); i++)
    {
        // OUT parameters are not considered part of the signature
        if (argModesArray.Item(i) != wxT("OUT"))
        {
            if (i > 0)
                args += wxT(", ");

            args += argTypesArray.Item(i);
        }
    }
    return args;
}

wxString edbPackageFunction::GetSql(ctlTree *browser)
{
    if (sql.IsNull())
    {
        sql = wxT("-- Package Function: ") + GetName() + wxT("\n\n");
        sql += GetSource() + wxT("\n\n");
    }

    return sql;
}

wxString edbPackageProcedure::GetSql(ctlTree *browser)
{
    if (sql.IsNull())
    {
        sql = wxT("-- Package Procedure: ") + GetName() + wxT("\n\n");
        sql += GetSource() + wxT("\n\n");
    }

    return sql;
}

void edbPackageFunction::ShowTreeDetail(ctlTree *browser, frmMain *form, ctlListView *properties, ctlSQLBox *sqlPane)
{
    if (properties)
    {
        CreateListColumns(properties);

        properties->AppendItem(_("Name"), GetName());
        properties->AppendItem(_("OID"), GetOid());
        properties->AppendItem(_("Argument count"), GetArgCount());
        properties->AppendItem(_("Arguments"), GetArgListWithNames());
        properties->AppendItem(_("Signature arguments"), GetArgSigList());
        if (!GetIsProcedure())
            properties->AppendItem(_("Return type"), GetReturnType());
        properties->AppendItem(_("Visibility"), GetVisibility());
        properties->AppendItem(_("Source"), firstLineOnly(GetSource()));
    }
}



pgObject *edbPackageFunction::Refresh(ctlTree *browser, const wxTreeItemId item)
{
    pgObject *packageFunction=0;
    pgCollection *coll=browser->GetParentCollection(item);
    if (coll)
    {
        if (coll->GetConnection()->EdbMinimumVersion(8, 2))
            packageFunction = packageFunctionFactory.CreateObjects(coll, 0, wxT("\n   AND pronamespace=") + GetPackage()->GetOidStr() + wxT(" AND proname='") + GetName() + wxT("'"));
        else
            packageFunction = packageFunctionFactory.CreateObjects(coll, 0, wxT("\n   AND packageoid=") + GetPackage()->GetOidStr() + wxT(" AND eltname='") + GetName() + wxT("'"));
    }

    return packageFunction;
}


///////////////////////////////////////////////////

edbPackageFunction *edbPackageFunctionFactory::AppendFunctions(pgObject *obj, edbPackage *package, ctlTree *browser, const wxString &restriction)
{
    edbPackageFunction *packageFunction=0;

	pgSet *packageFunctions;

	

    wxString sql;

    if (obj->GetConnection()->EdbMinimumVersion(8, 2))
    {
		sql = wxT("SELECT pg_proc.oid, proname AS eltname, prorettype AS eltdatatype, pronargs AS nargs, proaccess AS visibility,\n")
              wxT("       proallargtypes AS allargtypes, proargtypes AS argtypes, proargnames AS argnames, proargmodes AS argmodes,\n")
              wxT("       CASE WHEN format_type(prorettype, NULL) = 'void' THEN 'P' ELSE 'F' END AS eltclass\n")
              wxT("  FROM pg_proc, pg_namespace\n")
              + restriction + wxT("\n")
              wxT("  AND pg_proc.pronamespace = pg_namespace.oid\n")
 		      wxT("  ORDER BY eltname");
    }
    else
    {
		sql = wxT("SELECT oid, eltname, eltdatatype, eltclass, nargs, visibility,\n")
              wxT("       allargtypes, argtypes, argnames, argmodes\n")
              wxT("  FROM edb_pkgelements\n")
		      + restriction + wxT("\n")
		      wxT("  ORDER BY eltname");
    }

    packageFunctions = obj->GetDatabase()->ExecuteSet(sql);

    pgSet *types = obj->GetDatabase()->ExecuteSet(wxT(
                    "SELECT oid, format_type(oid, typtypmod) AS typname FROM pg_type"));

    typeMap map;

    while(!types->Eof())
    {
        map[types->GetVal(wxT("oid"))] = types->GetVal(wxT("typname"));
        types->MoveNext();
    }

    if (packageFunctions)
    {
        while (!packageFunctions->Eof())
        {
            if (packageFunctions->GetVal(wxT("eltclass")) == wxT("F"))
                packageFunction = new edbPackageFunction(package, packageFunctions->GetVal(wxT("eltname")));
            else
                packageFunction = new edbPackageProcedure(package, packageFunctions->GetVal(wxT("eltname")));

            // Tokenize the arguments
            wxStringTokenizer argNamesTkz(wxEmptyString), argTypesTkz(wxEmptyString), argModesTkz(wxEmptyString);
            wxString tmp;

            // Types
            tmp = packageFunctions->GetVal(wxT("allargtypes"));
            if (!tmp.IsEmpty())
                argTypesTkz.SetString(tmp.Mid(1, tmp.Length()-2), wxT(","));
            else
            {
                tmp = packageFunctions->GetVal(wxT("argtypes"));
                if (!tmp.IsEmpty())
                    argTypesTkz.SetString(tmp);
            }

            // Names
            tmp = packageFunctions->GetVal(wxT("argnames"));
            if (!tmp.IsEmpty())
                argNamesTkz.SetString(tmp.Mid(1, tmp.Length()-2), wxT(","));

            // Modes
            tmp = packageFunctions->GetVal(wxT("argmodes"));
            if (!tmp.IsEmpty())
                argModesTkz.SetString(tmp.Mid(1, tmp.Length()-2), wxT(","));

            // Now iterate the arguments and build the arrays
            wxString type, name, mode;
            
            while (argTypesTkz.HasMoreTokens())
            {
                // Add the arg type. This is a type oid, so 
                // look it up in the hashmap
                type = argTypesTkz.GetNextToken();
                packageFunction->iAddArgType(map[type]);

                // Now add the name, stripping the quotes if
                // necessary. 
                name = argNamesTkz.GetNextToken();
                if (!name.IsEmpty())
                {
                    if (name[0] == '"')
                        name = name.Mid(1, name.Length()-2);
                    packageFunction->iAddArgName(name);
                }
                else
                    packageFunction->iAddArgName(wxEmptyString);

                // Now the mode
                mode = argModesTkz.GetNextToken();
                if (!mode.IsEmpty())
                {
                    if (mode == wxT('o') || mode == wxT("2"))
                        mode = wxT("OUT");
                    else if (mode == wxT("b"))
                        if (packageFunctions->GetVal(wxT("eltclass")) == wxT("F"))
                            mode = wxT("IN OUT");
                        else
                            mode = wxT("INOUT");
                    else if (mode == wxT("3"))
                        mode = wxT("IN OUT");
                    else
                        mode = wxT("IN");

                    packageFunction->iAddArgMode(mode);
                }
                else
                    packageFunction->iAddArgMode(wxEmptyString);
            }

            packageFunction->iSetOid(packageFunctions->GetOid(wxT("oid")));
            packageFunction->iSetArgCount(packageFunctions->GetOid(wxT("nargs")));
            packageFunction->iSetReturnType(map[packageFunctions->GetVal(wxT("eltdatatype"))]);

            if (packageFunctions->GetVal(wxT("visibility")) == wxT("+"))
                packageFunction->iSetVisibility(_("Public"));
            else if (packageFunctions->GetVal(wxT("visibility")) == wxT("-"))
                packageFunction->iSetVisibility(_("Private"));
            else
                packageFunction->iSetVisibility(_("Unknown"));

            packageFunction->iSetSource(package->GetBodyInner());

            if (browser)
            {
                browser->AppendObject(obj, packageFunction);
				packageFunctions->MoveNext();
            }
            else
                break;
        }

		delete packageFunctions;
        delete types;
    }
    return packageFunction;
}

pgObject *edbPackageFunctionFactory::CreateObjects(pgCollection *collection, ctlTree *browser, const wxString &restriction)
{
    wxString restr;
    
    if (collection->GetDatabase()->GetConnection()->EdbMinimumVersion(8, 2))
        restr = wxT(" WHERE format_type(prorettype, NULL) != 'void' AND pronamespace = ");
    else
        restr = wxT(" WHERE eltclass = 'F' AND packageoid = ");

    restr += ((edbPackageObjCollection *)collection)->GetPackage()->GetOidStr();

    return AppendFunctions(collection, ((edbPackageObjCollection *)collection)->GetPackage(), browser, restr);
}

pgObject *edbPackageProcedureFactory::CreateObjects(pgCollection *collection, ctlTree *browser, const wxString &restriction)
{
    wxString restr;
    
    if (collection->GetDatabase()->GetConnection()->EdbMinimumVersion(8, 2))
        restr = wxT(" WHERE format_type(prorettype, NULL) = 'void' AND pronamespace = ");
    else
        restr = wxT(" WHERE eltclass = 'P' AND packageoid = ");

    restr += ((edbPackageObjCollection *)collection)->GetPackage()->GetOidStr();

    return AppendFunctions(collection, ((edbPackageObjCollection *)collection)->GetPackage(), browser, restr);
}

#include "images/function.xpm"
#include "images/functions.xpm"

edbPackageFunctionFactory::edbPackageFunctionFactory(const wxChar *tn, const wxChar *ns, const wxChar *nls, char **img) 
: edbPackageObjFactory(tn, ns, nls, img)
{
    metaType = EDB_PACKAGEFUNCTION;
}

edbPackageFunctionFactory packageFunctionFactory(__("Function"), __("New Function..."), __("Create a new Function."), function_xpm);
static pgaCollectionFactory cff(&packageFunctionFactory, __("Functions"), functions_xpm);

pgCollection *edbPackageObjFactory::CreateCollection(pgObject *obj)
{
    return new edbPackageObjCollection(GetCollectionFactory(), (edbPackage*)obj);
}

#include "images/procedure.xpm"
#include "images/procedures.xpm"

edbPackageProcedureFactory::edbPackageProcedureFactory() 
: edbPackageFunctionFactory(__("Procedure"), __("New Procedure..."), __("Create a new Procedure."), procedure_xpm)
{
}

edbPackageProcedureFactory packageProcedureFactory;
static pgaCollectionFactory cfp(&packageProcedureFactory, __("Procedures"), procedures_xpm);