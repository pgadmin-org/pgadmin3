//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// pgOperatorClass.cpp - OperatorClass class
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "utils/misc.h"
#include "schema/pgOperatorClass.h"
#include "schema/pgFunction.h"


pgOperatorClass::pgOperatorClass(pgSchema *newSchema, const wxString &newName)
	: pgSchemaObject(newSchema, operatorClassFactory, newName)
{
}

pgOperatorClass::~pgOperatorClass()
{
}

wxString pgOperatorClass::GetTranslatedMessage(int kindOfMessage) const
{
	wxString message = wxEmptyString;

	switch (kindOfMessage)
	{
		case RETRIEVINGDETAILS:
			message = _("Retrieving details on operator class");
			message += wxT(" ") + GetName();
			break;
		case REFRESHINGDETAILS:
			message = _("Refreshing operator class");
			message += wxT(" ") + GetName();
			break;
		case DROPINCLUDINGDEPS:
			message = wxString::Format(_("Are you sure you wish to drop operator class \"%s\" including all objects that depend on it?"),
			                           GetFullIdentifier().c_str());
			break;
		case DROPEXCLUDINGDEPS:
			message = wxString::Format(_("Are you sure you wish to drop operator class \"%s\"?"),
			                           GetFullIdentifier().c_str());
			break;
		case DROPCASCADETITLE:
			message = _("Drop operator class cascaded?");
			break;
		case DROPTITLE:
			message = _("Drop operator class?");
			break;
		case PROPERTIESREPORT:
			message = _("Operator class properties report");
			message += wxT(" - ") + GetName();
			break;
		case PROPERTIES:
			message = _("Operator class properties");
			break;
		case DDLREPORT:
			message = _("Operator class DDL report");
			message += wxT(" - ") + GetName();
			break;
		case DDL:
			message = _("Operator class DDL");
			break;
		case DEPENDENCIESREPORT:
			message = _("Operator class dependencies report");
			message += wxT(" - ") + GetName();
			break;
		case DEPENDENCIES:
			message = _("Operator class dependencies");
			break;
		case DEPENDENTSREPORT:
			message = _("Operator class dependents report");
			message += wxT(" - ") + GetName();
			break;
		case DEPENDENTS:
			message = _("Operator class dependents");
			break;
	}

	return message;
}

bool pgOperatorClass::DropObject(wxFrame *frame, ctlTree *browser, bool cascaded)
{
	wxString sql = wxT("DROP OPERATOR CLASS ") + this->GetSchema()->GetQuotedIdentifier() + wxT(".") + this->GetQuotedIdentifier() + wxT(" USING ") + GetAccessMethod();
	if (cascaded)
		sql += wxT(" CASCADE");
	return GetDatabase()->ExecuteVoid(sql);
}

wxString pgOperatorClass::GetSql(ctlTree *browser)
{
	if (sql.IsNull())
	{
		sql = wxT("-- Operator Class: ") + GetName() + wxT("\n\n")
		      + wxT("-- DROP OPERATOR CLASS ") + GetQuotedFullIdentifier() + wxT(" USING ") + GetAccessMethod() + wxT(";")
		      + wxT("\n\nCREATE OPERATOR CLASS ") + GetQuotedFullIdentifier();
		if (GetOpcDefault())
			sql += wxT(" DEFAULT");
		sql += wxT("\n   FOR TYPE ") + GetInType()
		       +  wxT(" USING ") + GetAccessMethod()
		       +  wxT(" AS");
		unsigned int i;
		bool needComma = false;

		for (i = 0 ; i < operators.Count() ; i++)
		{
			if (needComma)
				sql += wxT(",");

			sql += wxT("\n   OPERATOR ") + operators.Item(i);
			needComma = true;
		}
		for (i = 0 ; i < functions.Count() ; i++)
		{
			if (needComma)
				sql += wxT(",");

			sql += wxT("\n   FUNCTION ") + quotedFunctions.Item(i);
			needComma = true;
		}
		AppendIfFilled(sql, wxT("\n   STORAGE "), GetKeyType());
		sql += wxT(";\n");
	}

	return sql;
}


void pgOperatorClass::ShowTreeDetail(ctlTree *browser, frmMain *form, ctlListView *properties, ctlSQLBox *sqlPane)
{
	if (!expandedKids)
	{
		expandedKids = true;

		pgSet *set;

		if (!GetConnection()->BackendMinimumVersion(8, 3))
		{
			set = ExecuteSet(
			          wxT("SELECT amopstrategy, amopreqcheck, oprname, lt.typname as lefttype, rt.typname as righttype\n")
			          wxT("  FROM pg_amop am\n")
			          wxT("  JOIN pg_operator op ON amopopr=op.oid\n")
			          wxT("  LEFT OUTER JOIN pg_type lt ON lt.oid=oprleft\n")
			          wxT("  LEFT OUTER JOIN pg_type rt ON rt.oid=oprright\n")
			          wxT(" WHERE amopclaid=") + GetOidStr() + wxT("\n")
			          wxT(" ORDER BY amopstrategy"));
		}
		else if (!GetConnection()->BackendMinimumVersion(8, 4))
		{
			set = ExecuteSet(
			          wxT("SELECT amopstrategy, amopreqcheck, oprname, lt.typname as lefttype, rt.typname as righttype\n")
			          wxT("  FROM pg_amop am\n")
			          wxT("  JOIN pg_operator op ON amopopr=op.oid\n")
			          wxT("  JOIN pg_opfamily opf ON amopfamily = opf.oid\n")
			          wxT("  JOIN pg_opclass opc ON opf.oid = opcfamily\n")
			          wxT("  LEFT OUTER JOIN pg_type lt ON lt.oid=oprleft\n")
			          wxT("  LEFT OUTER JOIN pg_type rt ON rt.oid=oprright\n")
			          wxT(" WHERE opc.oid=") + GetOidStr() + wxT("\n")
			          wxT(" AND amopmethod = opf.opfmethod\n")
			          wxT(" AND amoplefttype = op.oprleft AND amoprighttype = op.oprright\n")
			          wxT(" ORDER BY amopstrategy"));
		}
		else
		{
			set = ExecuteSet(
			          wxT("SELECT amopstrategy, oprname, lt.typname as lefttype, rt.typname as righttype\n")
			          wxT("  FROM pg_amop am\n")
			          wxT("  JOIN pg_operator op ON amopopr=op.oid\n")
			          wxT("  JOIN pg_opfamily opf ON amopfamily = opf.oid\n")
			          wxT("  JOIN pg_opclass opc ON opf.oid = opcfamily\n")
			          wxT("  LEFT OUTER JOIN pg_type lt ON lt.oid=oprleft\n")
			          wxT("  LEFT OUTER JOIN pg_type rt ON rt.oid=oprright\n")
			          wxT(" WHERE opc.oid=") + GetOidStr() + wxT("\n")
			          wxT(" AND amopmethod = opf.opfmethod\n")
			          wxT(" AND amoplefttype = op.oprleft AND amoprighttype = op.oprright\n")
			          wxT(" ORDER BY amopstrategy"));
		}

		if (set)
		{
			while (!set->Eof())
			{
				wxString str = set->GetVal(wxT("amopstrategy")) + wxT("  ") + set->GetVal(wxT("oprname"));
				wxString lt = set->GetVal(wxT("lefttype"));
				wxString rt = set->GetVal(wxT("righttype"));
				if (lt == GetInType() && (rt.IsEmpty() || rt == GetInType()))
					lt = wxEmptyString;
				if (rt == GetInType() && lt.IsEmpty())
					rt = wxEmptyString;

				if (!lt.IsEmpty() || !rt.IsEmpty())
				{
					str += wxT("(");
					if (!lt.IsEmpty())
					{
						str += lt;
						if (!rt.IsEmpty())
							str += wxT(", ");
					}
					if (!rt.IsEmpty())
						str += rt;
					str += wxT(")");
				}

				if (!GetConnection()->BackendMinimumVersion(8, 4))
				{
					if (set->GetBool(wxT("amopreqcheck")))
						str += wxT(" RECHECK");
				}

				operators.Add(str);
				set->MoveNext();
			}
			delete set;
		}

		if (!GetConnection()->BackendMinimumVersion(8, 3))
		{
			set = ExecuteSet(
			          wxT("SELECT amprocnum, amproc::oid\n")
			          wxT("  FROM pg_amproc am\n")
			          wxT(" WHERE amopclaid=") + GetOidStr() + wxT("\n")
			          wxT(" ORDER BY amprocnum"));
		}
		else
		{
			set = ExecuteSet(
			          wxT("SELECT amprocnum, amproc::oid\n")
			          wxT("  FROM pg_amproc am\n")
			          wxT("  JOIN pg_opfamily opf ON amprocfamily = opf.oid\n")
			          wxT("  JOIN pg_opclass opc ON opf.oid = opcfamily\n")
			          wxT(" WHERE opc.oid=") + GetOidStr() + wxT("\n")
			          wxT(" AND amproclefttype = opc.opcintype AND amprocrighttype = opc.opcintype\n")
			          wxT(" ORDER BY amprocnum"));
		}

		if (set)
		{
			while (!set->Eof())
			{
				wxString amproc = set->GetVal(wxT("amproc"));
				functionOids.Add(amproc);

				// We won't build a PG_FUNCTIONS collection under OperatorClass, so we create
				// temporary function items
				pgFunction *function = functionFactory.AppendFunctions(this, GetSchema(), 0, wxT(" WHERE pr.oid=") + amproc);
				if (function)
				{
					functions.Add(set->GetVal(wxT("amprocnum")) + wxT("  ") + function->GetFullName());
					quotedFunctions.Add(set->GetVal(wxT("amprocnum")) + wxT("  ")
					                    + function->GetQuotedFullIdentifier() + wxT("(") + function->GetArgSigList() + wxT(")"));
					delete function;
				}

				set->MoveNext();
			}
			delete set;
		}
	}
	if (properties)
	{
		CreateListColumns(properties);

		properties->AppendItem(_("Name"), GetName());
		properties->AppendItem(_("OID"), GetOid());
		properties->AppendItem(_("Owner"), GetOwner());
		properties->AppendYesNoItem(_("Default?"), GetOpcDefault());
		properties->AppendItem(_("For type"), GetInType());
		properties->AppendItem(_("Access method"), GetAccessMethod());
		if (GetConnection()->BackendMinimumVersion(8, 3))
			properties->AppendItem(_("Family"), GetFamily());

		if (!GetKeyType().IsEmpty())
			properties->AppendItem(_("Storage"), GetKeyType());
		unsigned int i;
		for (i = 0 ; i < operators.Count() ; i++)
			properties->AppendItem(wxT("OPERATOR"), operators.Item(i));
		for (i = 0 ; i < functions.Count() ; i++)
			properties->AppendItem(wxT("FUNCTION"), functions.Item(i));
		properties->AppendYesNoItem(_("System operator class?"), GetSystemObject());
		if (GetConnection()->BackendMinimumVersion(7, 5))
			properties->AppendItem(_("Comment"), firstLineOnly(GetComment()));
	}
}



pgObject *pgOperatorClass::Refresh(ctlTree *browser, const wxTreeItemId item)
{
	pgObject *operatorClass = 0;
	pgCollection *coll = browser->GetParentCollection(item);
	if (coll)
		operatorClass = operatorClassFactory.CreateObjects(coll, 0, wxT("\n   AND op.oid=") + GetOidStr());

	return operatorClass;
}


///////////////////////////////////////////////////


pgOperatorClassCollection::pgOperatorClassCollection(pgaFactory *factory, pgSchema *sch)
	: pgSchemaObjCollection(factory, sch)
{
}


wxString pgOperatorClassCollection::GetTranslatedMessage(int kindOfMessage) const
{
	wxString message = wxEmptyString;

	switch (kindOfMessage)
	{
		case RETRIEVINGDETAILS:
			message = _("Retrieving details on operator classes");
			break;
		case REFRESHINGDETAILS:
			message = _("Refreshing operator classes");
			break;
		case OBJECTSLISTREPORT:
			message = _("Operator classes list report");
			break;
	}

	return message;
}


///////////////////////////////////////////////////


pgObject *pgOperatorClassFactory::CreateObjects(pgCollection *collection, ctlTree *browser, const wxString &restriction)
{
	pgOperatorClass *operatorClass = 0;

	pgSet *operatorClasses;
	if (collection->GetDatabase()->BackendMinimumVersion(8, 3))
	{
		operatorClasses = collection->GetDatabase()->ExecuteSet(
		                      wxT("SELECT op.oid, op.*, pg_get_userbyid(op.opcowner) as opowner, it.typname as intypename, dt.typname as keytypename, amname, opfname\n")
		                      wxT("  FROM pg_opclass op\n")
		                      wxT("  JOIN pg_opfamily opf ON op.opcfamily=opf.oid\n")
		                      wxT("  JOIN pg_am am ON am.oid=opf.opfmethod\n")
		                      wxT("  JOIN pg_type it ON it.oid=opcintype\n")
		                      wxT("  LEFT OUTER JOIN pg_type dt ON dt.oid=opckeytype\n")
		                      wxT(" WHERE opcnamespace = ") + collection->GetSchema()->GetOidStr()
		                      + restriction + wxT("\n")
		                      wxT(" ORDER BY opcname"));
	}
	else
	{
		operatorClasses = collection->GetDatabase()->ExecuteSet(
		                      wxT("SELECT op.oid, op.*, pg_get_userbyid(op.opcowner) as opowner, it.typname as intypename, dt.typname as keytypename, amname\n")
		                      wxT("  FROM pg_opclass op\n")
		                      wxT("  JOIN pg_am am ON am.oid=opcamid\n")
		                      wxT("  JOIN pg_type it ON it.oid=opcintype\n")
		                      wxT("  LEFT OUTER JOIN pg_type dt ON dt.oid=opckeytype\n")
		                      wxT(" WHERE opcnamespace = ") + collection->GetSchema()->GetOidStr()
		                      + restriction + wxT("\n")
		                      wxT(" ORDER BY opcname"));
	}


	if (operatorClasses)
	{
		while (!operatorClasses->Eof())
		{
			operatorClass = new pgOperatorClass(
			    collection->GetSchema(), operatorClasses->GetVal(wxT("opcname")));

			operatorClass->iSetOid(operatorClasses->GetOid(wxT("oid")));
			operatorClass->iSetOwner(operatorClasses->GetVal(wxT("opowner")));
			operatorClass->iSetAccessMethod(operatorClasses->GetVal(wxT("amname")));
			operatorClass->iSetInType(operatorClasses->GetVal(wxT("intypename")));
			operatorClass->iSetKeyType(operatorClasses->GetVal(wxT("keytypename")));
			operatorClass->iSetOpcDefault(operatorClasses->GetBool(wxT("opcdefault")));

			if (collection->GetDatabase()->BackendMinimumVersion(8, 3))
				operatorClass->iSetFamily(operatorClasses->GetVal(wxT("opfname")));

			if (browser)
			{
				browser->AppendObject(collection, operatorClass);
				operatorClasses->MoveNext();
			}
			else
				break;
		}

		delete operatorClasses;
	}
	return operatorClass;
}


#include "images/operatorclass.pngc"
#include "images/operatorclasses.pngc"

pgOperatorClassFactory::pgOperatorClassFactory()
	: pgSchemaObjFactory(__("Operator Class"), __("New Operator Class..."), __("Create a new Operator Class."), operatorclass_png_img)
{
	metaType = PGM_OPCLASS;
}

dlgProperty *pgOperatorClassFactory::CreateDialog(frmMain *frame, pgObject *node, pgObject *parent)
{
	return 0; // not implemented
}

pgCollection *pgOperatorClassFactory::CreateCollection(pgObject *obj)
{
	return new pgOperatorClassCollection(GetCollectionFactory(), (pgSchema *)obj);
}

pgOperatorClassFactory operatorClassFactory;
static pgaCollectionFactory cf(&operatorClassFactory, __("Operator Classes"), operatorclasses_png_img);
