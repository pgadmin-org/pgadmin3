//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// pgType.cpp - Type class
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "utils/misc.h"
#include "schema/pgType.h"
#include "schema/pgDatatype.h"


pgType::pgType(pgSchema *newSchema, const wxString &newName)
	: pgSchemaObject(newSchema, typeFactory, newName)
{
}

pgType::~pgType()
{
}

wxString pgType::GetTranslatedMessage(int kindOfMessage) const
{
	wxString message = wxEmptyString;

	switch (kindOfMessage)
	{
		case RETRIEVINGDETAILS:
			message = _("Retrieving details on type");
			message += wxT(" ") + GetName();
			break;
		case REFRESHINGDETAILS:
			message = _("Refreshing type");
			message += wxT(" ") + GetName();
			break;
		case DROPINCLUDINGDEPS:
			message = wxString::Format(_("Are you sure you wish to drop type \"%s\" including all objects that depend on it?"),
			                           GetFullIdentifier().c_str());
			break;
		case DROPEXCLUDINGDEPS:
			message = wxString::Format(_("Are you sure you wish to drop type \"%s\"?"),
			                           GetFullIdentifier().c_str());
			break;
		case DROPCASCADETITLE:
			message = _("Drop type cascaded?");
			break;
		case DROPTITLE:
			message = _("Drop type?");
			break;
		case PROPERTIESREPORT:
			message = _("Type properties report");
			message += wxT(" - ") + GetName();
			break;
		case PROPERTIES:
			message = _("Type properties");
			break;
		case DDLREPORT:
			message = _("Type DDL report");
			message += wxT(" - ") + GetName();
			break;
		case DDL:
			message = _("Type DDL");
			break;
		case DEPENDENCIESREPORT:
			message = _("Type dependencies report");
			message += wxT(" - ") + GetName();
			break;
		case DEPENDENCIES:
			message = _("Type dependencies");
			break;
		case DEPENDENTSREPORT:
			message = _("Type dependents report");
			message += wxT(" - ") + GetName();
			break;
		case DEPENDENTS:
			message = _("Type dependents");
			break;
	}

	return message;
}


bool pgType::DropObject(wxFrame *frame, ctlTree *browser, bool cascaded)
{
	wxString sql = wxT("DROP TYPE ") + this->GetSchema()->GetQuotedIdentifier() + wxT(".") + this->GetQuotedIdentifier();
	if (cascaded)
		sql += wxT(" CASCADE");
	return GetDatabase()->ExecuteVoid(sql);
}

wxString pgType::GetSql(ctlTree *browser)
{
	if (sql.IsNull())
	{
		sql = wxT("-- Type: ") + GetQuotedFullIdentifier() + wxT("\n\n")
		      + wxT("-- DROP TYPE ") + GetQuotedFullIdentifier() + wxT(";")
		      + wxT("\n\nCREATE TYPE ") + GetQuotedFullIdentifier();
		if (GetTypeClass() == TYPE_COMPOSITE)
		{
			sql += wxT(" AS\n   (");
			sql += GetQuotedTypesList();
		}
		else if (GetTypeClass() == TYPE_ENUM)
		{
			sql += wxT(" AS ENUM\n   (");
			sql += GetQuotedLabelList();
		}
		else
		{
			sql += wxT("\n   (INPUT=") + qtIdent(GetInputFunction())
			       + wxT(", OUTPUT=") + qtIdent(GetOutputFunction());
			AppendIfFilled(sql, wxT(", DEFAULT="), qtDbString(GetDefault()));
			if (!GetElement().IsNull())
			{
				sql += wxT(",\n       ELEMENT=") + GetElement()
				       + wxT(", DELIMITER='") + GetDelimiter() + wxT("'");
			}
			sql += wxT(",\n       INTERNALLENGTH=") + NumToStr(GetInternalLength())
			       + wxT(", ALIGNMENT=" + GetAlignment()
			             + wxT(", STORAGE=") + GetStorage());
			if (GetConnection()->BackendMinimumVersion(8, 3))
			{
				if (GetTypmodinFunction() != wxEmptyString && GetTypmodoutFunction() != wxEmptyString)
				{
					sql += wxT(",\n       TYPMOD_IN=") + GetTypmodinFunction()
					       + wxT(", TYPMOD_OUT=") + GetTypmodoutFunction();
				}
				else if (GetTypmodinFunction() != wxEmptyString)
					sql += wxT(",\n       TYPMOD_IN=") + GetTypmodinFunction();
				else if (GetTypmodoutFunction() != wxEmptyString)
					sql += wxT(",\n       TYPMOD_OUT=") + GetTypmodoutFunction();
			}
			if (GetConnection()->BackendMinimumVersion(9, 1) && GetCollatable())
			{
				sql += wxT(",\n       COLLATABLE=true");
			}
		}
		sql += wxT(");\n")
		       + GetOwnerSql(8, 0)
		       + GetCommentSql();
	}

	return sql;
}



void pgType::ShowTreeDetail(ctlTree *browser, frmMain *form, ctlListView *properties, ctlSQLBox *sqlPane)
{
	wxString query;
	wxString collation;

	if (!expandedKids)
	{
		expandedKids = true;
		if (GetTypeClass() == TYPE_COMPOSITE)
		{
			query = wxT("SELECT attname, format_type(t.oid,NULL) AS typname, attndims, atttypmod, nsp.nspname,\n")
			        wxT("       (SELECT COUNT(1) from pg_type t2 WHERE t2.typname=t.typname) > 1 AS isdup");
			if (GetConnection()->BackendMinimumVersion(9, 1))
				query += wxT(",\n       collname, nspc.nspname as collnspname");
			query += wxT("\n  FROM pg_attribute att\n")
			         wxT("  JOIN pg_type t ON t.oid=atttypid\n")
			         wxT("  JOIN pg_namespace nsp ON t.typnamespace=nsp.oid\n")
			         wxT("  LEFT OUTER JOIN pg_type b ON t.typelem=b.oid\n");
			if (GetConnection()->BackendMinimumVersion(9, 1))
				query += wxT("  LEFT OUTER JOIN pg_collation c ON att.attcollation=c.oid\n")
				         wxT("  LEFT OUTER JOIN pg_namespace nspc ON c.collnamespace=nspc.oid\n");
			query += wxT(" WHERE att.attrelid=") + NumToStr(relOid) + wxT("\n")
			         wxT(" ORDER by attnum");
			pgSet *set = ExecuteSet(query);
			if (set)
			{
				int anzvar = 0;
				while (!set->Eof())
				{
					wxString element;
					if (anzvar++)
					{
						typesList += wxT(", ");
						quotedTypesList += wxT(",\n    ");
					}
					typesList += set->GetVal(wxT("attname")) + wxT(" ");
					typesArray.Add(set->GetVal(wxT("attname")));
					quotedTypesList += qtIdent(set->GetVal(wxT("attname"))) + wxT(" ");

					pgDatatype dt(set->GetVal(wxT("nspname")), set->GetVal(wxT("typname")),
					              set->GetBool(wxT("isdup")), set->GetLong(wxT("attndims")) > 0, set->GetLong(wxT("atttypmod")));

					wxString nspname = set->GetVal(wxT("nspname"));

					typesList += dt.GetSchemaPrefix(GetDatabase()) + dt.FullName();
					typesArray.Add(dt.GetSchemaPrefix(GetDatabase()) + dt.FullName());
					quotedTypesList += dt.GetQuotedSchemaPrefix(GetDatabase()) + dt.QuotedFullName();

					if (GetConnection()->BackendMinimumVersion(9, 1))
					{
						if (set->GetVal(wxT("collname")).IsEmpty() || (set->GetVal(wxT("collname")) == wxT("default") && set->GetVal(wxT("collnspname")) == wxT("pg_catalog")))
							collation = wxEmptyString;
						else
						{
							collation = qtIdent(set->GetVal(wxT("collnspname"))) + wxT(".") + qtIdent(set->GetVal(wxT("collname")));
							quotedTypesList += wxT(" COLLATE ") + collation;
						}
						collationsArray.Add(collation);
					}
					typesArray.Add(collation);

					set->MoveNext();
				}
				delete set;
			}
		}
		else if (GetTypeClass() == TYPE_ENUM)
		{
			query = wxT("SELECT enumlabel\n")
			        wxT("  FROM pg_enum\n")
			        wxT(" WHERE enumtypid=") + GetOidStr() + wxT("\n");
			if (GetConnection()->BackendMinimumVersion(9, 1))
				query += wxT(" ORDER by enumsortorder");
			else
				query += wxT(" ORDER by oid");
			pgSet *set = ExecuteSet(query);
			if (set)
			{
				int anzvar = 0;
				while (!set->Eof())
				{
					wxString element;
					if (anzvar++)
					{
						labelList += wxT(", ");
						quotedLabelList += wxT(",\n    ");
					}
					labelList += set->GetVal(wxT("enumlabel"));
					labelArray.Add(set->GetVal(wxT("enumlabel")));
					quotedLabelList += GetDatabase()->connection()->qtDbString(set->GetVal(wxT("enumlabel")));

					set->MoveNext();
				}
				delete set;
			}
		}
	}

	if (properties)
	{
		CreateListColumns(properties);

		properties->AppendItem(_("Name"), GetName());
		properties->AppendItem(_("OID"), GetOid());
		properties->AppendItem(_("Owner"), GetOwner());
		properties->AppendItem(_("Alias"), GetAlias());
		if (GetTypeClass() == TYPE_COMPOSITE)
		{
			properties->AppendItem(_("Members"), GetTypesList());
		}
		if (GetTypeClass() == TYPE_ENUM)
		{
			properties->AppendItem(_("Labels"), GetLabelList());
		}
		else
		{
			properties->AppendItem(_("Alignment"), GetAlignment());
			properties->AppendItem(_("Internal length"), GetInternalLength());
			properties->AppendItem(_("Default"), GetDefault());
			properties->AppendItem(_("Passed by Value?"), BoolToYesNo(GetPassedByValue()));
			if (!GetElement().IsEmpty())
			{
				properties->AppendItem(_("Element"), GetElement());
				properties->AppendItem(_("Delimiter"), GetDelimiter());
			}
			properties->AppendItem(_("Input function"), GetInputFunction());
			properties->AppendItem(_("Output function"), GetOutputFunction());
			if (GetConnection()->BackendMinimumVersion(7, 4))
			{
				properties->AppendItem(_("Receive function"), GetReceiveFunction());
				properties->AppendItem(_("Send function"), GetSendFunction());
			}
			if (GetConnection()->BackendMinimumVersion(8, 3))
			{
				if (GetTypmodinFunction().Length() > 0)
					properties->AppendItem(_("Typmod in function"), GetTypmodinFunction());
				if (GetTypmodoutFunction().Length() > 0)
					properties->AppendItem(_("Typmod out function"), GetTypmodoutFunction());
			}
			properties->AppendItem(_("Storage"), GetStorage());
			if (GetConnection()->BackendMinimumVersion(9, 1))
				properties->AppendItem(_("Collatable?"), BoolToYesNo(GetCollatable()));
		}
		properties->AppendYesNoItem(_("System type?"), GetSystemObject());
		properties->AppendItem(_("Comment"), firstLineOnly(GetComment()));
	}
}



pgObject *pgType::Refresh(ctlTree *browser, const wxTreeItemId item)
{
	pgObject *type = 0;
	pgCollection *coll = browser->GetParentCollection(item);
	if (coll)
		type = typeFactory.CreateObjects(coll, 0, wxT("\n   AND t.oid=") + GetOidStr());

	return type;
}


wxString pgTypeCollection::GetTranslatedMessage(int kindOfMessage) const
{
	wxString message = wxEmptyString;

	switch (kindOfMessage)
	{
		case RETRIEVINGDETAILS:
			message = _("Retrieving details on types");
			break;
		case REFRESHINGDETAILS:
			message = _("Refreshing types");
			break;
		case OBJECTSLISTREPORT:
			message = _("Types list report");
			break;
	}

	return message;
}


/////////////////////////////////////////////////////////


pgObject *pgTypeFactory::CreateObjects(pgCollection *collection, ctlTree *browser, const wxString &restriction)
{
	pgType *type = 0;
	wxString systemRestriction;
	if (!settings->GetShowSystemObjects())
		systemRestriction = wxT("   AND ct.oid IS NULL\n");

	wxString sql =	wxT("SELECT t.oid, t.*, format_type(t.oid, null) AS alias, pg_get_userbyid(t.typowner) as typeowner, e.typname as element, description, ct.oid AS taboid\n")
	                wxT("  FROM pg_type t\n")
	                wxT("  LEFT OUTER JOIN pg_type e ON e.oid=t.typelem\n")
	                wxT("  LEFT OUTER JOIN pg_class ct ON ct.oid=t.typrelid AND ct.relkind <> 'c'\n")
	                wxT("  LEFT OUTER JOIN pg_description des ON des.objoid=t.oid\n");

	if (collection->GetDatabase()->BackendMinimumVersion(8, 1))
		sql += wxT(" WHERE t.typtype != 'd' AND t.typname NOT LIKE E'\\\\_%' AND t.typnamespace = ") + collection->GetSchema()->GetOidStr() + wxT("\n");
	else
		sql += wxT(" WHERE t.typtype != 'd' AND t.typname NOT LIKE '\\\\_%' AND t.typnamespace = ") + collection->GetSchema()->GetOidStr() + wxT("\n");

	sql += restriction + systemRestriction +
	       wxT(" ORDER BY t.typname");

	pgSet *types = collection->GetDatabase()->ExecuteSet(sql);

	if (types)
	{
		while (!types->Eof())
		{
			type = new pgType(collection->GetSchema(), types->GetVal(wxT("typname")));

			type->iSetOid(types->GetOid(wxT("oid")));
			type->iSetOwner(types->GetVal(wxT("typeowner")));
			type->iSetAlias(types->GetVal(wxT("alias")));
			type->iSetComment(types->GetVal(wxT("description")));
			type->iSetPassedByValue(types->GetBool(wxT("typbyval")));

			if (types->GetVal(wxT("typtype")) == wxT("c"))
				type->iSetTypeClass(TYPE_COMPOSITE);
			else if (types->GetVal(wxT("typtype")) == wxT("e"))
				type->iSetTypeClass(TYPE_ENUM);
			else
				type->iSetTypeClass(TYPE_EXTERNAL);

			type->iSetRelOid(types->GetOid(wxT("typrelid")));
			type->iSetIsRecordType(types->GetOid(wxT("taboid")) != 0);
			type->iSetInternalLength(types->GetLong(wxT("typlen")));
			type->iSetDelimiter(types->GetVal(wxT("typdelim")));
			type->iSetElement(types->GetVal(wxT("element")));
			type->iSetInputFunction(types->GetVal(wxT("typinput")));
			type->iSetOutputFunction(types->GetVal(wxT("typoutput")));
			if (collection->GetConnection()->BackendMinimumVersion(7, 4))
			{
				type->iSetReceiveFunction(types->GetVal(wxT("typreceive")));
				type->iSetSendFunction(types->GetVal(wxT("typsend")));
			}
			if (collection->GetConnection()->BackendMinimumVersion(8, 3))
			{
				if (types->GetVal(wxT("typmodin")) != wxT("-"))
					type->iSetTypmodinFunction(types->GetVal(wxT("typmodin")));
				if (types->GetVal(wxT("typmodout")) != wxT("-"))
					type->iSetTypmodoutFunction(types->GetVal(wxT("typmodout")));
			}
			wxString align = types->GetVal(wxT("typalign"));
			type->iSetAlignment(
			    align == wxT("c") ? wxT("char") :
			    align == wxT("s") ? wxT("int2") :
			    align == wxT("i") ? wxT("int4") :
			    align == wxT("d") ? wxT("double") : wxT("unknown"));
			type->iSetDefault(types->GetVal(wxT("typdefault")));
			wxString storage = types->GetVal(wxT("typstorage"));
			type->iSetStorage(
			    storage == wxT("p") ? wxT("PLAIN") :
			    storage == wxT("e") ? wxT("EXTERNAL") :
			    storage == wxT("m") ? wxT("MAIN") :
			    storage == wxT("x") ? wxT("EXTENDED") : wxT("unknown"));
			if (collection->GetConnection()->BackendMinimumVersion(9, 1))
				type->iSetCollatable(types->GetLong(wxT("typcollation")) == 100);

			if (browser)
			{
				browser->AppendObject(collection, type);
				types->MoveNext();
			}
			else
				break;
		}

		delete types;
	}
	return type;
}


#include "images/type.pngc"
#include "images/types.pngc"

pgTypeFactory::pgTypeFactory()
	: pgSchemaObjFactory(__("Type"), __("New Type..."), __("Create a new Type."), type_png_img)
{
}


pgTypeFactory typeFactory;
static pgaCollectionFactory cf(&typeFactory, __("Types"), types_png_img);
