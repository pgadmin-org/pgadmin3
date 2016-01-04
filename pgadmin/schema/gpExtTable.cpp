//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// gpExtTable.cpp - Greenplum External Table
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "utils/misc.h"
#include "schema/pgColumn.h"
#include "schema/gpExtTable.h"
#include "frm/frmHint.h"


gpExtTable::gpExtTable(pgSchema *newSchema, const wxString &newName)
	: pgSchemaObject(newSchema, extTableFactory, newName)
{
}

gpExtTable::~gpExtTable()
{
}

wxString gpExtTable::GetTranslatedMessage(int kindOfMessage) const
{
	wxString message = wxEmptyString;

	switch (kindOfMessage)
	{
		case RETRIEVINGDETAILS:
			message = _("Retrieving details on external table");
			message += wxT(" ") + GetName();
			break;
		case REFRESHINGDETAILS:
			message = _("Refreshing external table");
			message += wxT(" ") + GetName();
			break;
		case DROPINCLUDINGDEPS:
			message = wxString::Format(_("Are you sure you wish to drop external table \"%s\" including all objects that depend on it?"),
			                           GetFullIdentifier().c_str());
			break;
		case DROPEXCLUDINGDEPS:
			message = wxString::Format(_("Are you sure you wish to drop external table \"%s\"?"),
			                           GetFullIdentifier().c_str());
			break;
		case DROPCASCADETITLE:
			message = _("Drop external table cascaded?");
			break;
		case DROPTITLE:
			message = _("Drop external table?");
			break;
		case PROPERTIESREPORT:
			message = _("External table properties report");
			message += wxT(" - ") + GetName();
			break;
		case PROPERTIES:
			message = _("External table properties");
			break;
		case DDLREPORT:
			message = _("External table DDL report");
			message += wxT(" - ") + GetName();
			break;
		case DDL:
			message = _("External table DDL");
			break;
		case DEPENDENCIESREPORT:
			message = _("External table dependencies report");
			message += wxT(" - ") + GetName();
			break;
		case DEPENDENCIES:
			message = _("External table dependencies");
			break;
		case DEPENDENTSREPORT:
			message = _("External table dependents report");
			message += wxT(" - ") + GetName();
			break;
		case DEPENDENTS:
			message = _("External table dependents");
			break;
	}

	return message;
}

bool gpExtTable::IsUpToDate()
{
	wxString sql = wxT("SELECT xmin FROM pg_class WHERE oid = ") + this->GetOidStr();
	if (!this->GetDatabase()->GetConnection() || this->GetDatabase()->ExecuteScalar(sql) != NumToStr(GetXid()))
		return false;
	else
		return true;
}

wxMenu *gpExtTable::GetNewMenu()
{
	wxMenu *menu = pgObject::GetNewMenu();
	if (schema->GetCreatePrivilege())
		schemaFactory.AppendMenu(menu);

	return menu;
}


bool gpExtTable::DropObject(wxFrame *frame, ctlTree *browser, bool cascaded)
{
	wxString sql = wxT("DROP EXTERNAL TABLE ") + this->GetSchema()->GetQuotedIdentifier() + wxT(".") + this->GetQuotedIdentifier();
	if (cascaded)
		sql += wxT(" CASCADE");
	return GetDatabase()->ExecuteVoid(sql);
}

wxString gpExtTable::GetSql(ctlTree *browser)
{
	wxString colDetails;
	wxString prevComment;
	wxString q;

	if (sql.IsNull())
	{
		sql = wxT("-- External Table: ") + GetQuotedFullIdentifier() + wxT("\n\n")
		      + wxT("-- DROP EXTERNAL TABLE ") + GetQuotedFullIdentifier() + wxT(";\n\n");
		/* Now get required information from pg_exttable */
		if (GetDatabase()->BackendMinimumVersion(8, 2, 5))
		{
			q += wxT(
			         "SELECT x.location, x.fmttype, x.fmtopts, x.command, ")
			     wxT("x.rejectlimit, x.rejectlimittype,")
			     wxT("(SELECT relname ")
			     wxT("FROM pg_class ")
			     wxT("WHERE Oid=x.fmterrtbl) AS errtblname, ")
			     wxT("pg_catalog.pg_encoding_to_char(x.encoding) ")
			     wxT("FROM pg_catalog.pg_exttable x, pg_catalog.pg_class c ")
			     wxT("WHERE x.reloid = c.oid AND c.oid = ") + GetOidStr();
		}
		else
		{
			/* not SREH and encoding colums yet */
			q += wxT(
			         "SELECT x.location, x.fmttype, x.fmtopts, x.command, ")
			     wxT("-1 as rejectlimit, null as rejectlimittype,")
			     wxT("null as errtblname, ")
			     wxT("null as encoding ")
			     wxT("FROM pg_catalog.pg_exttable x, pg_catalog.pg_class c ")
			     wxT("WHERE x.reloid = c.oid AND c.oid = ") + GetOidStr();

		}

		pgSet *extTable = GetDatabase()->ExecuteSet(q);

		wxString locations = extTable->GetVal(0);
		wxString fmttype = extTable->GetVal(1);
		wxString fmtopts = extTable->GetVal(2);
		wxString command = extTable->GetVal(3);
		wxString rejlim = extTable->GetVal(4);
		wxString rejlimtype = extTable->GetVal(5);
		wxString errtblname = extTable->GetVal(6);
		wxString extencoding = extTable->GetVal(7);

		delete extTable;

		if ((command.Length() > 0) ||
		        (locations.Mid(1, 4) ==  wxT("http")))
		{
			sql += wxT("CREATE EXTERNAL WEB TABLE ") +
			       GetQuotedFullIdentifier() + wxT("\n(\n");
		}
		else
		{
			sql += wxT("CREATE EXTERNAL TABLE ") +
			       GetQuotedFullIdentifier() + wxT("\n(\n");
		}

		// Get the columns
		pgCollection *columns = browser->FindCollection(columnFactory, GetId());
		if (columns)
		{
			columns->ShowTreeDetail(browser);
			treeObjectIterator colIt1(browser, columns);
			treeObjectIterator colIt2(browser, columns);


			int lastRealCol = 0;
			int currentCol = 0;
			pgColumn *column;

			// Iterate the columns to find the last 'real' one
			while ((column = (pgColumn *)colIt1.GetNextObject()) != 0)
			{
				currentCol++;

				if (column->GetInheritedCount() == 0)
					lastRealCol = currentCol;
			}

			// Now build the actual column list
			int colCount = 0;
			while ((column = (pgColumn *)colIt2.GetNextObject()) != 0)
			{
				column->ShowTreeDetail(browser);
				if (column->GetColNumber() > 0)
				{
					if (colCount)
					{
						// Only add a comma if this isn't the last 'real' column
						if (colCount != lastRealCol)
							sql += wxT(",");
						if (!prevComment.IsEmpty())
							sql += wxT(" -- ") + firstLineOnly(prevComment);

						sql += wxT("\n");
					}

					/* if (column->GetInheritedCount() > 0)
					 {
					     if (!column->GetIsLocal())
					         sql += wxString::Format(wxT("-- %s "), _("Inherited"))
					             + wxT("from table ") +  column->GetInheritedTableName() + wxT(":");
					 }*/

					sql += wxT("  ") + column->GetQuotedIdentifier() + wxT(" ")
					       + column->GetDefinition();

					prevComment = column->GetComment();

					// Whilst we are looping round the columns, grab their comments as well.
					// Perhaps we should also get storage types here?
					colDetails += column->GetCommentSql();
					if (colDetails.Length() > 0)
						if (colDetails.Last() != '\n') colDetails += wxT("\n");

					colCount++;
				}
			}
		}
		if (!prevComment.IsEmpty())
			sql += wxT(" -- ") + firstLineOnly(prevComment);

		sql += wxT("\n)\n");

		if(command.Length() > 0)
		{

			wxString on_clause = locations;

			/* remove curly braces */
			on_clause = locations.Mid(1, locations.Length() - 2);


			/* add EXECUTE clause */
			sql += wxT(" EXECUTE E'");
			for (size_t i = 0; i < command.Length(); i++)
			{
				if (command[i] == wxT('\\')) sql += wxT('\\');
				if (command[i] == wxT('\'')) sql += wxT('\'');
				sql +=	command[i];
			}
			sql += wxT("' ");


			/* add ON clause */
			wxString temp;

			if(on_clause.StartsWith(wxT("HOST:"), &temp))
			{
				sql += wxT("ON HOST '") + temp + wxT("'");
			}
			else if(on_clause.StartsWith(wxT("PER_HOST"), &temp))
				sql += wxT("ON HOST ");
			else if(on_clause.StartsWith(wxT("MASTER_ONLY"), &temp))
				sql += wxT("ON MASTER ");
			else if(on_clause.StartsWith(wxT("SEGMENT_ID:"), &temp))
				sql += wxT("ON SEGMENT ") + temp + wxT(" ");
			else if(on_clause.StartsWith(wxT("TOTAL_SEGS:"), &temp))
				sql += wxT("ON  ") + temp + wxT(" ");
			else if(on_clause.StartsWith(wxT("ALL_SEGMENTS"), &temp))
				sql += wxT("ON ALL ");
			else
				sql += on_clause;

			sql += wxT("\n ");

		}
		else
		{

			/* add LOCATION clause */

			locations = locations.Mid(1, locations.Length() - 2);
			wxStringTokenizer locs(locations, wxT(","));
			wxString token;
			token = locs.GetNextToken();
			sql += wxT(" LOCATION (\n    '");
			sql += token;
			sql += wxT("'");
			while (locs.HasMoreTokens())
			{
				sql += wxT(",\n    '");
				sql += locs.GetNextToken();
				sql += wxT("'");
			}
			sql += wxT("\n)\n ");

		}

		/* add FORMAT clause */
		sql += wxT("FORMAT '");
		sql +=	fmttype[0] == 't' ? wxT("text") : wxT("csv");
		sql += wxT("'");
		sql += wxT(" (");
		for (size_t i = 0; i < fmtopts.Length(); i++)
		{
			if (fmtopts[i] == wxT('\\')) sql += wxT('\\');
			sql +=	fmtopts[i];
		}
		sql += wxT(")\n");

		if (GetDatabase()->BackendMinimumVersion(8, 2))
		{
			/* add ENCODING clause */
			sql += wxT("ENCODING '");
			sql += extencoding ;
			sql += wxT("'");

			/* add Single Row Error Handling clause (if any) */
			if(rejlim.Length() > 0)
			{
				sql += wxT("\n");

				/*
				* NOTE: error tables get automatically generated if don't exist.
				* therefore we must be sure that this statement will be dumped after
				* the error relation CREATE is dumped, so that we won't try to
				* create it twice. For now we rely on the fact that we pick dumpable
				* objects sorted by OID, and error table oid *should* always be less
				* than its external table oid (could that not be true sometimes?)
				*/
				if(errtblname.Length() > 0)
				{
					sql += wxT("LOG ERRORS INTO ");
					sql += errtblname;
					sql += wxT(" ");
				}

				/* reject limit */
				sql += wxT("SEGMENT REJECT LIMIT ");
				sql += rejlim;

				/* reject limit type */
				if(rejlimtype[0] == 'r')
					sql += wxT(" ROWS");
				else
					sql += wxT(" PERCENT");
			}
		}

		sql += wxT(";\n")
		       + GetOwnerSql(7, 3, wxEmptyString, wxT("TABLE"));


		sql += GetGrant(wxT("r"), wxT("TABLE ") + GetQuotedFullIdentifier());

		sql += GetCommentSql()
		       + wxT("\n");

		// Column/constraint comments
		if (!colDetails.IsEmpty())
			sql += colDetails + wxT("\n");

	}
	return sql;
}


wxString gpExtTable::GetCols(ctlTree *browser, size_t indent, wxString &QMs, bool withQM)
{
	wxString sql;
	wxString line;

	int colcount = 0;
	pgSetIterator set(GetConnection(),
	                  wxT("SELECT attname\n")
	                  wxT("  FROM pg_attribute\n")
	                  wxT(" WHERE attrelid=") + GetOidStr() + wxT(" AND attnum>0\n")
	                  wxT(" ORDER BY attnum"));


	while (set.RowsLeft())
	{
		if (colcount++)
		{
			line += wxT(", ");
			QMs += wxT(", ");
		}
		if (line.Length() > 60)
		{
			if (!sql.IsEmpty())
			{
				sql += wxT("\n") + wxString(' ', indent);
			}
			sql += line;
			line = wxEmptyString;
			QMs += wxT("\n") + wxString(' ', indent);
		}

		line += qtIdent(set.GetVal(0));
		if (withQM)
			line += wxT("=?");
		QMs += wxT("?");
	}

	if (!line.IsEmpty())
	{
		if (!sql.IsEmpty())
			sql += wxT("\n") + wxString(' ', indent);
		sql += line;
	}
	return sql;
}


wxString gpExtTable::GetSelectSql(ctlTree *browser)
{
	wxString qms;
	wxString sql =
	    wxT("SELECT ") + GetCols(browser, 7, qms, false) + wxT("\n")
	    wxT("  FROM ") + GetQuotedFullIdentifier() + wxT(";\n");
	return sql;
}


void gpExtTable::ShowTreeDetail(ctlTree *browser, frmMain *form, ctlListView *properties, ctlSQLBox *sqlPane)
{
	if (!expandedKids)
	{
		expandedKids = true;
		browser->RemoveDummyChild(this);

		browser->AppendCollection(this, columnFactory);
	}
	if (properties)
	{
		CreateListColumns(properties);

		properties->AppendItem(_("Name"), GetName());
		properties->AppendItem(_("OID"), GetOid());
		properties->AppendItem(_("Owner"), GetOwner());
		properties->AppendItem(_("ACL"), GetAcl());
		properties->AppendYesNoItem(_("System Table?"), GetSystemObject());
		properties->AppendItem(_("Comment"), firstLineOnly(GetComment()));
	}
}



pgObject *gpExtTable::Refresh(ctlTree *browser, const wxTreeItemId item)
{
	pgObject *extTable = 0;
	pgCollection *coll = browser->GetParentCollection(item);
	if (coll)
	{
		extTable = extTableFactory.CreateObjects(coll, 0, wxT("\n   AND c.oid=") + GetOidStr());
	}

	return extTable;
}

void gpExtTable::ShowHint(frmMain *form, bool force)
{
	wxArrayString hints;
	hints.Add(HINT_OBJECT_EDITING);
	frmHint::ShowHint((wxWindow *)form, hints, GetFullIdentifier(), force);
}

///////////////////////////////////////////////////////


pgObject *gpExtTableFactory::CreateObjects(pgCollection *collection, ctlTree *browser, const wxString &restriction)
{
	gpExtTable *extTable = 0;


	pgSet *extTables = collection->GetDatabase()->ExecuteSet(
	                       wxT("SELECT c.oid, c.xmin, c.relname, pg_get_userbyid(c.relowner) AS exttableowner, c.relacl AS relacl, description \n")
	                       wxT("  FROM pg_class c\n")
	                       wxT("  LEFT OUTER JOIN pg_description des ON (des.objoid=c.oid AND des.objsubid=0 AND des.classoid='pg_class'::regclass)\n")
	                       wxT(" WHERE  (c.relkind = 'x' OR (c.relkind = 'r' AND c.relstorage = 'x'))\n")
	                       wxT("   AND relnamespace = ") + collection->GetSchema()->GetOidStr() + wxT("\n")
	                       + restriction
	                       + wxT(" ORDER BY relname"));

	if (extTables)
	{
		while (!extTables->Eof())
		{
			extTable = new gpExtTable(collection->GetSchema(), extTables->GetVal(wxT("relname")));

			extTable->iSetOid(extTables->GetOid(wxT("oid")));
			extTable->iSetXid(extTables->GetOid(wxT("xmin")));
			extTable->iSetOwner(extTables->GetVal(wxT("exttableowner")));
			extTable->iSetComment(extTables->GetVal(wxT("description")));
			extTable->iSetAcl(extTables->GetVal(wxT("relacl")));
			//extTable->iSetDefinition(extTables->GetVal(wxT("definition")));

			if (browser)
			{
				collection->AppendBrowserItem(browser, extTable);
				extTables->MoveNext();
			}
			else
				break;
		}

		delete extTables;
	}
	return extTable;
}

/////////////////////////////

gpExtTableCollection::gpExtTableCollection(pgaFactory *factory, pgSchema *sch)
	: pgSchemaObjCollection(factory, sch)
{
}

wxString gpExtTableCollection::GetTranslatedMessage(int kindOfMessage) const
{
	wxString message = wxEmptyString;

	switch (kindOfMessage)
	{
		case RETRIEVINGDETAILS:
			message = _("Retrieving details on external tables");
			break;
		case REFRESHINGDETAILS:
			message = _("Refreshing external tables");
			break;
		case GRANTWIZARDTITLE:
			message = _("Privileges for external tables");
			break;
		case OBJECTSLISTREPORT:
			message = _("External tables list report");
			break;
	}

	return message;
}

/////////////////////////////

#include "images/exttable.pngc"
#include "images/exttable-sm.pngc"
#include "images/exttables.pngc"

gpExtTableFactory::gpExtTableFactory()
	: pgSchemaObjFactory(__("External Table"), __("New External Table..."), __("Create a new External Table."), exttable_png_img, exttable_sm_png_img)
{
	metaType = GP_EXTTABLE;
}


pgCollection *gpExtTableFactory::CreateCollection(pgObject *obj)
{
	return new gpExtTableCollection(GetCollectionFactory(), (pgSchema *)obj);
}

gpExtTableFactory extTableFactory;
static pgaCollectionFactory cf(&extTableFactory, __("External Tables"), exttables_png_img);
