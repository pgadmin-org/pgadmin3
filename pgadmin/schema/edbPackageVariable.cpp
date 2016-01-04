//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// edbPackageVariable.cpp - EnterpriseDB Package variable
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "schema/edbPackageVariable.h"


edbPackageVariable::edbPackageVariable(edbPackage *newPackage, const wxString &newName)
	: edbPackageObject(newPackage, packageVariableFactory, newName)
{
}

wxString edbPackageVariable::GetTranslatedMessage(int kindOfMessage) const
{
	wxString message = wxEmptyString;

	switch (kindOfMessage)
	{
		case RETRIEVINGDETAILS:
			message = _("Retrieving details on package variable");
			message += wxT(" ") + GetName();
			break;
		case REFRESHINGDETAILS:
			message = _("Refreshing package variable");
			message += wxT(" ") + GetName();
			break;
		case DROPINCLUDINGDEPS:
			message = wxString::Format(_("Are you sure you wish to drop package variable \"%s\" including all objects that depend on it?"),
			                           GetFullIdentifier().c_str());
			break;
		case DROPEXCLUDINGDEPS:
			message = wxString::Format(_("Are you sure you wish to drop package variable \"%s\"?"),
			                           GetFullIdentifier().c_str());
			break;
		case DROPCASCADETITLE:
			message = _("Drop package variable cascaded?");
			break;
		case DROPTITLE:
			message = _("Drop package variable?");
			break;
		case PROPERTIESREPORT:
			message = _("Package variable properties report");
			message += wxT(" - ") + GetName();
			break;
		case PROPERTIES:
			message = _("Package variable properties");
			break;
		case DDLREPORT:
			message = _("Package variable DDL report");
			message += wxT(" - ") + GetName();
			break;
		case DDL:
			message = _("Package variable DDL");
			break;
	}

	return message;
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
	pgObject *packageVariable = 0;
	pgCollection *coll = browser->GetParentCollection(item);
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

	edbPackageVariable *packageVariable = 0;

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
		edbPackage *package = ((edbPackageObjCollection *)collection)->GetPackage();

		while (!packageVariables->Eof())
		{
			// Do not create edbPackageVariable, if package is wrapped
			if (package->GetBody().Trim(false).StartsWith(wxT("$__EDBwrapped__$")))
			{
				packageVariables->MoveNext();
				continue;
			}
			packageVariable = new edbPackageVariable(package, packageVariables->GetVal(wxT("eltname")));
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

/////////////////////////////

edbPackageVariableCollection::edbPackageVariableCollection(pgaFactory *factory, edbPackage *pkg)
	: edbPackageObjCollection(factory, pkg)
{
}

wxString edbPackageVariableCollection::GetTranslatedMessage(int kindOfMessage) const
{
	wxString message = wxEmptyString;

	switch (kindOfMessage)
	{
		case RETRIEVINGDETAILS:
			message = _("Retrieving details on package variables");
			break;
		case REFRESHINGDETAILS:
			message = _("Refreshing package variables");
			break;
		case GRANTWIZARDTITLE:
			message = _("Privileges for package variables");
			break;
		case OBJECTSLISTREPORT:
			message = _("Package variables list report");
			break;
	}

	return message;
}

/////////////////////////////

#include "images/variable.pngc"
#include "images/variables.pngc"

edbPackageVariableFactory::edbPackageVariableFactory()
	: edbPackageObjFactory(__("Variable"), __("New Variable..."), __("Create a new Variable."), variable_png_img)
{
	metaType = EDB_PACKAGEVARIABLE;
}

pgCollection *edbPackageVariableFactory::CreateCollection(pgObject *obj)
{
	return new edbPackageVariableCollection(GetCollectionFactory(), (edbPackage *)obj);
}

edbPackageVariableFactory packageVariableFactory;
static pgaCollectionFactory cf(&packageVariableFactory, __("Variables"), variables_png_img);
