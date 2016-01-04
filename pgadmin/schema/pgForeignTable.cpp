//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// pgForeignTable.cpp - Foreign Table class
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "utils/misc.h"
#include "schema/pgForeignTable.h"
#include "schema/pgDatatype.h"


pgForeignTable::pgForeignTable(pgSchema *newSchema, const wxString &newName)
	: pgSchemaObject(newSchema, foreignTableFactory, newName)
{
}

pgForeignTable::~pgForeignTable()
{
}

wxString pgForeignTable::GetTranslatedMessage(int kindOfMessage) const
{
	wxString message = wxEmptyString;

	switch (kindOfMessage)
	{
		case RETRIEVINGDETAILS:
			message = _("Retrieving details on foreign table");
			message += wxT(" ") + GetName();
			break;
		case REFRESHINGDETAILS:
			message = _("Refreshing foreign table");
			message += wxT(" ") + GetName();
			break;
		case DROPINCLUDINGDEPS:
			message = wxString::Format(_("Are you sure you wish to drop foreign table \"%s\" including all objects that depend on it?"),
			                           GetFullIdentifier().c_str());
			break;
		case DROPEXCLUDINGDEPS:
			message = wxString::Format(_("Are you sure you wish to drop foreign table \"%s\"?"),
			                           GetFullIdentifier().c_str());
			break;
		case DROPCASCADETITLE:
			message = _("Drop foreign table cascaded?");
			break;
		case DROPTITLE:
			message = _("Drop foreign table?");
			break;
		case PROPERTIESREPORT:
			message = _("Foreign table properties report");
			message += wxT(" - ") + GetName();
			break;
		case PROPERTIES:
			message = _("Foreign table properties");
			break;
		case DDLREPORT:
			message = _("Foreign table DDL report");
			message += wxT(" - ") + GetName();
			break;
		case DDL:
			message = _("Foreign table DDL");
			break;
		case DEPENDENCIESREPORT:
			message = _("Foreign table dependencies report");
			message += wxT(" - ") + GetName();
			break;
		case DEPENDENCIES:
			message = _("Foreign table dependencies");
			break;
		case DEPENDENTSREPORT:
			message = _("Foreign table dependents report");
			message += wxT(" - ") + GetName();
			break;
		case DEPENDENTS:
			message = _("Foreign table dependents");
			break;
	}

	return message;
}


bool pgForeignTable::DropObject(wxFrame *frame, ctlTree *browser, bool cascaded)
{
	wxString sql = wxT("DROP FOREIGN TABLE ") + this->GetSchema()->GetQuotedIdentifier() + wxT(".") + this->GetQuotedIdentifier();
	if (cascaded)
		sql += wxT(" CASCADE");
	return GetDatabase()->ExecuteVoid(sql);
}

wxString pgForeignTable::GetSql(ctlTree *browser)
{
	if (sql.IsNull())
	{
		sql = wxT("-- Foreign Table: ") + GetQuotedFullIdentifier() + wxT("\n\n")
		      + wxT("-- DROP FOREIGN TABLE ") + GetQuotedFullIdentifier() + wxT(";")
		      + wxT("\n\nCREATE FOREIGN TABLE ") + GetQuotedFullIdentifier()
		      + wxT("\n   (") + GetQuotedTypesList()
		      + wxT(")\n   SERVER ") + GetForeignServer();

		if (!GetOptionsList().IsEmpty())
			sql += wxT("\n   OPTIONS (") + GetOptionsList() + wxT(")");

		sql += wxT(";\n")
		       + GetOwnerSql(9, 1)
		       + GetCommentSql();

		if (GetConnection()->BackendMinimumVersion(9, 1))
			sql += GetSeqLabelsSql();
	}

	return sql;
}



void pgForeignTable::ShowTreeDetail(ctlTree *browser, frmMain *form, ctlListView *properties, ctlSQLBox *sqlPane)
{
	wxString constraint;

	if (!expandedKids)
	{
		expandedKids = true;
		pgSet *set = ExecuteSet(
		                 wxT("SELECT attname, format_type(t.oid,NULL) AS typname, attndims, atttypmod, nspname, attnotnull,\n")
		                 wxT("       (SELECT COUNT(1) from pg_type t2 WHERE t2.typname=t.typname) > 1 AS isdup\n")
		                 wxT("  FROM pg_attribute att\n")
		                 wxT("  JOIN pg_type t ON t.oid=atttypid\n")
		                 wxT("  JOIN pg_namespace nsp ON t.typnamespace=nsp.oid\n")
		                 wxT("  LEFT OUTER JOIN pg_type b ON t.typelem=b.oid\n")
		                 wxT(" WHERE att.attrelid=") + GetOidStr() + wxT("\n")
		                 wxT(" AND attnum>0\n")
		                 wxT(" ORDER by attnum"));
		if (set)
		{
			int anzvar = 0;
			while (!set->Eof())
			{
				pgDatatype dt(set->GetVal(wxT("nspname")), set->GetVal(wxT("typname")),
				              set->GetBool(wxT("isdup")), set->GetLong(wxT("attndims")) > 0, set->GetLong(wxT("atttypmod")));
				constraint = set->GetBool(wxT("attnotnull")) ? wxT("NOT NULL") : wxT("");

				if (anzvar++)
				{
					typesList += wxT(", ");
					quotedTypesList += wxT(",\n    ");
				}

				typesList += set->GetVal(wxT("attname")) + wxT(" ")
				             + dt.GetSchemaPrefix(GetDatabase()) + dt.FullName() + wxT(" ")
				             + constraint;

				quotedTypesList += qtIdent(set->GetVal(wxT("attname"))) + wxT(" ")
				                   + dt.GetQuotedSchemaPrefix(GetDatabase()) + dt.QuotedFullName() + wxT(" ")
				                   + constraint;

				typesArray.Add(set->GetVal(wxT("attname")));
				typesArray.Add(dt.GetSchemaPrefix(GetDatabase()) + dt.FullName());
				typesArray.Add(constraint);

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
		properties->AppendItem(_("Server"), GetForeignServer());
		properties->AppendItem(_("Columns"), GetQuotedTypesList());
		properties->AppendItem(_("Options"), GetOptionsList());
		properties->AppendItem(_("Comment"), firstLineOnly(GetComment()));

		if (!GetLabels().IsEmpty())
		{
			wxArrayString seclabels = GetProviderLabelArray();
			if (seclabels.GetCount() > 0)
			{
				for (unsigned int index = 0 ; index < seclabels.GetCount() - 1 ; index += 2)
				{
					properties->AppendItem(seclabels.Item(index), seclabels.Item(index + 1));
				}
			}
		}
	}
}


wxString pgForeignTable::GetSelectSql(ctlTree *browser)
{

	wxString columns = wxEmptyString;
	wxArrayString elements = GetTypesArray();
	size_t i;
	for (i = 0 ; i < elements.GetCount() ; i += 3)
	{
		if (!columns.IsEmpty())
			columns += wxT(", ");
		columns += qtIdent(elements.Item(i));
	}

	wxString sql =
	    wxT("SELECT ") + columns + wxT("\n")
	    wxT("  FROM ") + GetQuotedFullIdentifier() + wxT(";\n");
	return sql;
}


pgObject *pgForeignTable::Refresh(ctlTree *browser, const wxTreeItemId item)
{
	pgObject *ft = 0;
	pgCollection *coll = browser->GetParentCollection(item);
	if (coll)
		ft = foreignTableFactory.CreateObjects(coll, 0, wxT("\n   AND c.oid=") + GetOidStr());

	return ft;
}


void pgForeignTable::iSetOptions(const wxString &tmpoptions)
{
	wxString tmp;
	wxString option;
	wxString value;
	wxString currentChar;
	bool wrappedInQuotes = false, antislash = false;

	options = wxEmptyString;

	if (tmpoptions == wxEmptyString)
		return;

	// parse the options string
	// we start at 1 and stop at length-1 to get rid of the { and } of the array
	for (unsigned int index = 1 ; index < tmpoptions.Length() - 1 ; index++)
	{
		// get current char
		currentChar = tmpoptions.Mid(index, 1);

		// if there is a double quote at the beginning of an option,
		// the whole option=value will be wrapped in quotes
		if (currentChar == wxT("\"") && tmp.IsEmpty())
			wrappedInQuotes = true;
		else if (currentChar == wxT("\\") && wrappedInQuotes)
			antislash = true;
		else
		{
			if ((currentChar == wxT(",") && !wrappedInQuotes && !tmp.IsEmpty())
			        || (currentChar == wxT("\"") && wrappedInQuotes && !antislash && !tmp.IsEmpty()))
			{
				// new options

				if (currentChar == wxT("\"") && wrappedInQuotes && !antislash && !tmp.IsEmpty())
				{
					// In this specific case, the next character is the comma,
					// but we don't want to start the next option with the comma
					// so we skip it right now
					index++;
				}

				// we need to grab option and value from tmp string
				option = tmp.BeforeFirst('=');
				value = tmp.AfterFirst('=');

				// put them in the array
				optionsArray.Add(option);
				optionsArray.Add(value);

				// build the options string
				if (options.Length() > 0)
					options += wxT(", ");
				options += option + wxT(" '") + value + wxT("'");

				// reinit tmp
				tmp = wxEmptyString;
				wrappedInQuotes = false;
			}
			else
				tmp += currentChar;
			antislash = false;
		}
	}

	// last options

	if (!tmp.IsEmpty())
	{
		// we need to grab option and value from tmp string
		option = tmp.BeforeFirst('=');
		value = tmp.AfterFirst('=');

		// put them in the array
		optionsArray.Add(option);
		optionsArray.Add(value);

		// build the options string
		if (options.Length() > 0)
			options += wxT(", ");
		options += option + wxT(" '") + value + wxT("'");
	}
}


/////////////////////////////////////////////////////////


pgForeignTableCollection::pgForeignTableCollection(pgaFactory *factory, pgSchema *sch)
	: pgSchemaObjCollection(factory, sch)
{
}


wxString pgForeignTableCollection::GetTranslatedMessage(int kindOfMessage) const
{
	wxString message = wxEmptyString;

	switch (kindOfMessage)
	{
		case RETRIEVINGDETAILS:
			message = _("Retrieving details on foreign tables");
			break;
		case REFRESHINGDETAILS:
			message = _("Refreshing foreign tables");
			break;
		case OBJECTSLISTREPORT:
			message = _("Foreign tables list report");
			break;
	}

	return message;
}


/////////////////////////////////////////////////////////


pgObject *pgForeignTableFactory::CreateObjects(pgCollection *collection, ctlTree *browser, const wxString &restriction)
{
	pgForeignTable *foreigntable = 0;

	wxString sql =	wxT("SELECT c.oid AS ftoid, c.relname AS ftrelname, pg_get_userbyid(relowner) AS ftowner,\n")
	                wxT("  ftoptions, srvname AS ftsrvname, description,\n")
	                wxT("  (SELECT array_agg(label) FROM pg_seclabels sl1 WHERE sl1.objoid=c.oid) AS labels,\n")
	                wxT("  (SELECT array_agg(provider) FROM pg_seclabels sl2 WHERE sl2.objoid=c.oid) AS providers\n")
	                wxT("  FROM pg_class c\n")
	                wxT("  JOIN pg_foreign_table ft ON c.oid=ft.ftrelid\n")
	                wxT("  LEFT OUTER JOIN pg_foreign_server fs ON ft.ftserver=fs.oid\n")
	                wxT("  LEFT OUTER JOIN pg_description des ON (des.objoid=c.oid AND des.classoid='pg_class'::regclass)\n")
	                wxT(" WHERE c.relnamespace = ") + collection->GetSchema()->GetOidStr() + wxT("\n")
	                + restriction +
	                wxT(" ORDER BY c.relname");

	pgSet *foreigntables = collection->GetDatabase()->ExecuteSet(sql);

	if (foreigntables)
	{
		while (!foreigntables->Eof())
		{
			foreigntable = new pgForeignTable(collection->GetSchema(), foreigntables->GetVal(wxT("ftrelname")));

			foreigntable->iSetOid(foreigntables->GetOid(wxT("ftoid")));
			foreigntable->iSetOwner(foreigntables->GetVal(wxT("ftowner")));
			foreigntable->iSetForeignServer(foreigntables->GetVal(wxT("ftsrvname")));
			foreigntable->iSetOptions(foreigntables->GetVal(wxT("ftoptions")));
			foreigntable->iSetComment(foreigntables->GetVal(wxT("description")));
			foreigntable->iSetProviders(foreigntables->GetVal(wxT("providers")));
			foreigntable->iSetLabels(foreigntables->GetVal(wxT("labels")));

			if (browser)
			{
				browser->AppendObject(collection, foreigntable);
				foreigntables->MoveNext();
			}
			else
				break;
		}

		delete foreigntables;
	}
	return foreigntable;
}


#include "images/foreigntable.pngc"
#include "images/foreigntables.pngc"

pgForeignTableFactory::pgForeignTableFactory()
	: pgSchemaObjFactory(__("Foreign Table"), __("New Foreign Table..."), __("Create a new Foreign Table."), foreigntable_png_img)
{
	metaType = PGM_FOREIGNTABLE;
}


pgCollection *pgForeignTableFactory::CreateCollection(pgObject *obj)
{
	return new pgForeignTableCollection(GetCollectionFactory(), (pgSchema *)obj);
}

pgForeignTableFactory foreignTableFactory;
static pgaCollectionFactory cf(&foreignTableFactory, __("Foreign Tables"), foreigntables_png_img);
