//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id: edbPackageVariable.cpp 5884 2007-01-23 12:22:27Z dpage $
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// edbPackageVariable.cpp - EnterpriseDB Package variable
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "schema/edbPackageVariable.h"


edbPackageVariable::edbPackageVariable(edbPackage *newPackage, const wxString& newName)
: edbPackageObject(newPackage, packageVariableFactory, newName)
{
}

wxString edbPackageVariable::GetSql(ctlTree *browser)
{
    if (sql.IsNull())
    {
        sql = wxT("-- Package Variable: ") + GetName() + wxT("\n\n");
        sql += GetName() + wxT(" ") + GetDataType() + wxT(";\n\n");
    }

    return sql;
}

void edbPackageVariable::ShowTreeDetail(ctlTree *browser, frmMain *form, ctlListView *properties, ctlSQLBox *sqlPane)
{
    if (properties)
    {
        CreateListColumns(properties);

        properties->AppendItem(_("Name"), GetName());
        properties->AppendItem(_("OID"), GetOid());
        properties->AppendItem(_("Data type"), GetDataType());
        properties->AppendItem(_("Visibility"), GetVisibility());
    }
}



pgObject *edbPackageVariable::Refresh(ctlTree *browser, const wxTreeItemId item)
{
    pgObject *packageVariable=0;
    pgCollection *coll=browser->GetParentCollection(item);
    if (coll)
    {
        if (coll->GetConnection()->EdbMinimumVersion(8, 2))
            packageVariable = packageVariableFactory.CreateObjects(coll, 0, wxT("\n   AND varname='") + GetName() + wxT("'"));
        else
            packageVariable = packageVariableFactory.CreateObjects(coll, 0, wxT("\n   AND eltname='") + GetName() + wxT("'"));
    }

    return packageVariable;
}


///////////////////////////////////////////////////

pgObject *edbPackageVariableFactory::CreateObjects(pgCollection *collection, ctlTree *browser, const wxString &restriction)
{

    edbPackageVariable *packageVariable=0;

	pgSet *packageVariables;

	wxString sql;

    if (collection->GetConnection()->EdbMinimumVersion(8, 2))
    {
        sql = wxT("SELECT oid, varname AS eltname, varaccess AS visibility, format_type(vartype, NULL) as datatype FROM edb_variable\n")
              wxT(" WHERE varpackage = ") + ((edbPackageObjCollection *)collection)->GetPackage()->GetOidStr() + wxT("\n")
		      + restriction + wxT("\n")
		      wxT(" ORDER BY varname");
    }
    else
    {
        sql = wxT("SELECT oid, eltname, visibility, format_type(eltdatatype, NULL) as datatype FROM edb_pkgelements\n")
              wxT(" WHERE eltclass = 'V'\n")
              wxT(" AND packageoid = ") + ((edbPackageObjCollection *)collection)->GetPackage()->GetOidStr() + wxT("\n")
		      + restriction + wxT("\n")
		      wxT(" ORDER BY eltname");
    }

    packageVariables = collection->GetDatabase()->ExecuteSet(sql);

    if (packageVariables)
    {
        while (!packageVariables->Eof())
        {
            packageVariable = new edbPackageVariable(((edbPackageObjCollection *)collection)->GetPackage(), packageVariables->GetVal(wxT("eltname")));
            packageVariable->iSetOid(packageVariables->GetOid(wxT("oid")));
            packageVariable->iSetDataType(packageVariables->GetVal(wxT("datatype")));
            if (packageVariables->GetVal(wxT("visibility")) == wxT("+"))
                packageVariable->iSetVisibility(_("Public"));
            else if (packageVariables->GetVal(wxT("visibility")) == wxT("-"))
                packageVariable->iSetVisibility(_("Private"));
            else
                packageVariable->iSetVisibility(_("Unknown"));

            if (browser)
            {
                browser->AppendObject(collection, packageVariable);
				packageVariables->MoveNext();
            }
            else
                break;
        }

		delete packageVariables;
    }
    return packageVariable;
}

#include "images/variable.xpm"
#include "images/variables.xpm"

edbPackageVariableFactory::edbPackageVariableFactory() 
: edbPackageObjFactory(__("Variable"), __("New Variable..."), __("Create a new Variable."), variable_xpm)
{
    metaType = EDB_PACKAGEVARIABLE;
}

edbPackageVariableFactory packageVariableFactory;
static pgaCollectionFactory cf(&packageVariableFactory, __("Variables"), variables_xpm);
