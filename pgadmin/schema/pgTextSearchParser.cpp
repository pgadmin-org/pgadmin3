//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// pgTextSearchParser.cpp - Text Search Parser class
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "utils/misc.h"
#include "schema/pgTextSearchParser.h"


pgTextSearchParser::pgTextSearchParser(pgSchema *newSchema, const wxString &newName)
	: pgSchemaObject(newSchema, textSearchParserFactory, newName)
{
}

pgTextSearchParser::~pgTextSearchParser()
{
}

wxString pgTextSearchParser::GetTranslatedMessage(int kindOfMessage) const
{
	wxString message = wxEmptyString;

	switch (kindOfMessage)
	{
		case RETRIEVINGDETAILS:
			message = _("Retrieving details on FTS parser");
			message += wxT(" ") + GetName();
			break;
		case REFRESHINGDETAILS:
			message = _("Refreshing FTS parser");
			message += wxT(" ") + GetName();
			break;
		case DROPINCLUDINGDEPS:
			message = wxString::Format(_("Are you sure you wish to drop FTS parser \"%s\" including all objects that depend on it?"),
			                           GetFullIdentifier().c_str());
			break;
		case DROPEXCLUDINGDEPS:
			message = wxString::Format(_("Are you sure you wish to drop FTS parser \"%s\"?"),
			                           GetFullIdentifier().c_str());
			break;
		case DROPCASCADETITLE:
			message = _("Drop FTS parser cascaded?");
			break;
		case DROPTITLE:
			message = _("Drop FTS parser?");
			break;
		case PROPERTIESREPORT:
			message = _("FTS parser properties report");
			message += wxT(" - ") + GetName();
			break;
		case PROPERTIES:
			message = _("FTS parser properties");
			break;
		case DDLREPORT:
			message = _("FTS parser DDL report");
			message += wxT(" - ") + GetName();
			break;
		case DDL:
			message = _("FTS parser DDL");
			break;
		case DEPENDENCIESREPORT:
			message = _("FTS parser dependencies report");
			message += wxT(" - ") + GetName();
			break;
		case DEPENDENCIES:
			message = _("FTS parser dependencies");
			break;
		case DEPENDENTSREPORT:
			message = _("FTS parser dependents report");
			message += wxT(" - ") + GetName();
			break;
		case DEPENDENTS:
			message = _("FTS parser dependents");
			break;
	}

	return message;
}

bool pgTextSearchParser::DropObject(wxFrame *frame, ctlTree *browser, bool cascaded)
{
	wxString sql = wxT("DROP TEXT SEARCH PARSER ") + this->GetSchema()->GetQuotedIdentifier() + wxT(".") + qtIdent(this->GetIdentifier());

	if (cascaded)
		sql += wxT(" CASCADE");

	return GetDatabase()->ExecuteVoid(sql);
}


wxString pgTextSearchParser::GetSql(ctlTree *browser)
{
	if (sql.IsNull())
	{
		sql = wxT("-- Text Search Parser: ") + GetFullIdentifier() + wxT("\n\n")
		      + wxT("-- DROP TEXT SEARCH PARSER ") + GetFullIdentifier() + wxT("\n\n")
		      + wxT("CREATE TEXT SEARCH PARSER ") + GetFullIdentifier() + wxT(" (")
		      + wxT("\n  START = ") + qtTypeIdent(GetStart())
		      + wxT(",\n  GETTOKEN = ") + qtTypeIdent(GetGettoken())
		      + wxT(",\n  END = ") + qtTypeIdent(GetEnd())
		      + wxT(",\n  LEXTYPES = ") + qtTypeIdent(GetLextypes());

		AppendIfFilled(sql, wxT(",\n  HEADLINE = "), GetHeadline());

		sql += wxT("\n);\n");

		if (!GetComment().IsNull())
			sql += wxT("COMMENT ON TEXT SEARCH PARSER ") + GetFullIdentifier()
			       + wxT(" IS ") + qtDbString(GetComment()) + wxT(";\n");
	}

	return sql;
}


void pgTextSearchParser::ShowTreeDetail(ctlTree *browser, frmMain *form, ctlListView *properties, ctlSQLBox *sqlPane)
{
	if (properties)
	{
		CreateListColumns(properties);

		properties->AppendItem(_("Name"), GetName());
		properties->AppendItem(_("OID"), GetOid());
		properties->AppendItem(_("Start"), GetStart());
		properties->AppendItem(_("Gettoken"), GetGettoken());
		properties->AppendItem(_("End"), GetEnd());
		properties->AppendItem(_("Lextypes"), GetLextypes());
		properties->AppendItem(_("Headline"), GetHeadline());
		properties->AppendItem(_("Comment"), firstLineOnly(GetComment()));
	}
}



pgObject *pgTextSearchParser::Refresh(ctlTree *browser, const wxTreeItemId item)
{
	pgObject *parser = 0;
	pgCollection *coll = browser->GetParentCollection(item);
	if (coll)
		parser = textSearchParserFactory.CreateObjects(coll, 0, wxT("\n   AND prs.oid=") + GetOidStr());

	return parser;
}


///////////////////////////////////////////////////


pgTextSearchParserCollection::pgTextSearchParserCollection(pgaFactory *factory, pgSchema *sch)
	: pgSchemaObjCollection(factory, sch)
{
}


wxString pgTextSearchParserCollection::GetTranslatedMessage(int kindOfMessage) const
{
	wxString message = wxEmptyString;

	switch (kindOfMessage)
	{
		case RETRIEVINGDETAILS:
			message = _("Retrieving details on FTS parsers");
			break;
		case REFRESHINGDETAILS:
			message = _("Refreshing FTS parsers");
			break;
		case OBJECTSLISTREPORT:
			message = _("FTS parsers list report");
			break;
	}

	return message;
}


//////////////////////////////////////////////////////


pgObject *pgTextSearchParserFactory::CreateObjects(pgCollection *collection, ctlTree *browser, const wxString &restriction)
{
	pgTextSearchParser *parser = 0;

	pgSet *parsers;
	parsers = collection->GetDatabase()->ExecuteSet(
	              wxT("SELECT prs.oid, prs.prsname, prs.prsstart, prs.prstoken, prs.prsend, prs.prslextype, prs.prsheadline, description\n")
	              wxT("  FROM pg_ts_parser prs\n")
	              wxT("  LEFT OUTER JOIN pg_description des ON (des.objoid=prs.oid AND des.classoid='pg_ts_parser'::regclass)\n")
	              wxT(" WHERE prs.prsnamespace = ") + collection->GetSchema()->GetOidStr()
	              + restriction + wxT("\n")
	              wxT(" ORDER BY prs.prsname"));

	if (parsers)
	{
		while (!parsers->Eof())
		{
			parser = new pgTextSearchParser(collection->GetSchema(), parsers->GetVal(wxT("prsname")));
			parser->iSetOid(parsers->GetOid(wxT("oid")));
			parser->iSetComment(parsers->GetVal(wxT("description")));
			parser->iSetStart(parsers->GetVal(wxT("prsstart")));
			parser->iSetGettoken(parsers->GetVal(wxT("prstoken")));
			parser->iSetEnd(parsers->GetVal(wxT("prsend")));
			parser->iSetLextypes(parsers->GetVal(wxT("prslextype")));
			if (parsers->GetVal(wxT("prsheadline")).Cmp(wxT("-")) != 0)
			{
				parser->iSetHeadline(parsers->GetVal(wxT("prsheadline")));
			}
			else
			{
				parser->iSetHeadline(wxT(""));
			}

			if (browser)
			{
				browser->AppendObject(collection, parser);
				parsers->MoveNext();
			}
			else
				break;
		}

		delete parsers;
	}
	return parser;
}


#include "images/parser.pngc"
#include "images/parsers.pngc"

pgTextSearchParserFactory::pgTextSearchParserFactory()
	: pgSchemaObjFactory(__("FTS Parser"), __("New FTS Parser..."), __("Create a new FTS Parser."), parser_png_img)
{
}


pgCollection *pgTextSearchParserFactory::CreateCollection(pgObject *obj)
{
	return new pgTextSearchParserCollection(GetCollectionFactory(), (pgSchema *)obj);
}

pgTextSearchParserFactory textSearchParserFactory;
static pgaCollectionFactory cf(&textSearchParserFactory, __("FTS Parsers"), parsers_png_img);
