//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// pgOperatorFamily.cpp - OperatorFamily class
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "utils/misc.h"
#include "schema/pgOperatorFamily.h"
#include "schema/pgFunction.h"


pgOperatorFamily::pgOperatorFamily(pgSchema *newSchema, const wxString &newName)
	: pgSchemaObject(newSchema, operatorFamilyFactory, newName)
{
}

pgOperatorFamily::~pgOperatorFamily()
{
}

wxString pgOperatorFamily::GetTranslatedMessage(int kindOfMessage) const
{
	wxString message = wxEmptyString;

	switch (kindOfMessage)
	{
		case RETRIEVINGDETAILS:
			message = _("Retrieving details on operator family");
			message += wxT(" ") + GetName();
			break;
		case REFRESHINGDETAILS:
			message = _("Refreshing operator family");
			message += wxT(" ") + GetName();
			break;
		case DROPINCLUDINGDEPS:
			message = wxString::Format(_("Are you sure you wish to drop operator family \"%s\" including all objects that depend on it?"),
			                           GetFullIdentifier().c_str());
			break;
		case DROPEXCLUDINGDEPS:
			message = wxString::Format(_("Are you sure you wish to drop operator family \"%s\"?"),
			                           GetFullIdentifier().c_str());
			break;
		case DROPCASCADETITLE:
			message = _("Drop operator family cascaded?");
			break;
		case DROPTITLE:
			message = _("Drop operator family?");
			break;
		case PROPERTIESREPORT:
			message = _("Operator family properties report");
			message += wxT(" - ") + GetName();
			break;
		case PROPERTIES:
			message = _("Operator family properties");
			break;
		case DDLREPORT:
			message = _("Operator family DDL report");
			message += wxT(" - ") + GetName();
			break;
		case DDL:
			message = _("Operator family DDL");
			break;
		case DEPENDENCIESREPORT:
			message = _("Operator family dependencies report");
			message += wxT(" - ") + GetName();
			break;
		case DEPENDENCIES:
			message = _("Operator family dependencies");
			break;
		case DEPENDENTSREPORT:
			message = _("Operator family dependents report");
			message += wxT(" - ") + GetName();
			break;
		case DEPENDENTS:
			message = _("Operator family dependents");
			break;
	}

	return message;
}

bool pgOperatorFamily::DropObject(wxFrame *frame, ctlTree *browser, bool cascaded)
{
	wxString sql = wxT("DROP OPERATOR FAMILY ") + this->GetSchema()->GetQuotedIdentifier() + wxT(".") + this->GetQuotedIdentifier() + wxT(" USING ") + GetAccessMethod();
	if (cascaded)
		sql += wxT(" CASCADE");
	return GetDatabase()->ExecuteVoid(sql);
}

wxString pgOperatorFamily::GetSql(ctlTree *browser)
{
	if (sql.IsNull())
	{
		sql = wxT("-- Operator Family: ") + GetName() + wxT("\n\n")
		      + wxT("-- DROP OPERATOR FAMILY ") + GetQuotedFullIdentifier() + wxT(" USING ") + GetAccessMethod() + wxT(";")
		      + wxT("\n\nCREATE OPERATOR FAMILY ") + GetQuotedFullIdentifier()
		      + wxT(" USING ") + GetAccessMethod()
		      + wxT(";");
	}

	return sql;
}


void pgOperatorFamily::ShowTreeDetail(ctlTree *browser, frmMain *form, ctlListView *properties, ctlSQLBox *sqlPane)
{
	if (properties)
	{
		CreateListColumns(properties);

		properties->AppendItem(_("Name"), GetName());
		properties->AppendItem(_("OID"), GetOid());
		properties->AppendItem(_("Owner"), GetOwner());
		properties->AppendItem(_("Access method"), GetAccessMethod());
		properties->AppendYesNoItem(_("System operator family?"), GetSystemObject());
		properties->AppendItem(_("Comment"), firstLineOnly(GetComment()));
	}
}



pgObject *pgOperatorFamily::Refresh(ctlTree *browser, const wxTreeItemId item)
{
	pgObject *operatorFamily = 0;
	pgCollection *coll = browser->GetParentCollection(item);
	if (coll)
		operatorFamily = operatorFamilyFactory.CreateObjects(coll, 0, wxT("\n   AND opf.oid=") + GetOidStr());

	return operatorFamily;
}


///////////////////////////////////////////////////


pgOperatorFamilyCollection::pgOperatorFamilyCollection(pgaFactory *factory, pgSchema *sch)
	: pgSchemaObjCollection(factory, sch)
{
}


wxString pgOperatorFamilyCollection::GetTranslatedMessage(int kindOfMessage) const
{
	wxString message = wxEmptyString;

	switch (kindOfMessage)
	{
		case RETRIEVINGDETAILS:
			message = _("Retrieving details on operator families");
			break;
		case REFRESHINGDETAILS:
			message = _("Refreshing operator families");
			break;
		case OBJECTSLISTREPORT:
			message = _("Operator families list report");
			break;
	}

	return message;
}


///////////////////////////////////////////////////


pgObject *pgOperatorFamilyFactory::CreateObjects(pgCollection *collection, ctlTree *browser, const wxString &restriction)
{
	pgOperatorFamily *operatorFamily = 0;

	pgSet *operatorFamilies;

	operatorFamilies = collection->GetDatabase()->ExecuteSet(
	                       wxT("SELECT opf.oid, opf.*, pg_get_userbyid(opf.opfowner) as opowner, amname\n")
	                       wxT("  FROM pg_opfamily opf\n")
	                       wxT("  JOIN pg_am am ON am.oid=opf.opfmethod\n")
	                       wxT(" WHERE opfnamespace = ") + collection->GetSchema()->GetOidStr()
	                       + restriction + wxT("\n")
	                       wxT(" ORDER BY opfname"));


	if (operatorFamilies)
	{
		while (!operatorFamilies->Eof())
		{
			operatorFamily = new pgOperatorFamily(
			    collection->GetSchema(), operatorFamilies->GetVal(wxT("opfname")));

			operatorFamily->iSetOid(operatorFamilies->GetOid(wxT("oid")));
			operatorFamily->iSetOwner(operatorFamilies->GetVal(wxT("opowner")));
			operatorFamily->iSetAccessMethod(operatorFamilies->GetVal(wxT("amname")));

			if (browser)
			{
				browser->AppendObject(collection, operatorFamily);
				operatorFamilies->MoveNext();
			}
			else
				break;
		}

		delete operatorFamilies;
	}
	return operatorFamily;
}


#include "images/operatorfamily.pngc"
#include "images/operatorfamilies.pngc"

pgOperatorFamilyFactory::pgOperatorFamilyFactory()
	: pgSchemaObjFactory(__("Operator Family"), __("New Operator Family..."), __("Create a new Operator Family."), operatorfamily_png_img)
{
	metaType = PGM_OPFAMILY;
}

dlgProperty *pgOperatorFamilyFactory::CreateDialog(frmMain *frame, pgObject *node, pgObject *parent)
{
	return 0; // not implemented
}

pgCollection *pgOperatorFamilyFactory::CreateCollection(pgObject *obj)
{
	return new pgOperatorFamilyCollection(GetCollectionFactory(), (pgSchema *)obj);
}

pgOperatorFamilyFactory operatorFamilyFactory;
static pgaCollectionFactory cf(&operatorFamilyFactory, __("Operator Families"), operatorfamilies_png_img);
