//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// pgTextSearchConfiguration.cpp - Text Search Configuration class
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "utils/misc.h"
#include "schema/pgTextSearchConfiguration.h"


pgTextSearchConfiguration::pgTextSearchConfiguration(pgSchema *newSchema, const wxString &newName)
	: pgSchemaObject(newSchema, textSearchConfigurationFactory, newName)
{
}

pgTextSearchConfiguration::~pgTextSearchConfiguration()
{
}

wxString pgTextSearchConfiguration::GetTranslatedMessage(int kindOfMessage) const
{
	wxString message = wxEmptyString;

	switch (kindOfMessage)
	{
		case RETRIEVINGDETAILS:
			message = _("Retrieving details on FTS configuration");
			message += wxT(" ") + GetName();
			break;
		case REFRESHINGDETAILS:
			message = _("Refreshing FTS configuration");
			message += wxT(" ") + GetName();
			break;
		case DROPINCLUDINGDEPS:
			message = wxString::Format(_("Are you sure you wish to drop FTS configuration \"%s\" including all objects that depend on it?"),
			                           GetFullIdentifier().c_str());
			break;
		case DROPEXCLUDINGDEPS:
			message = wxString::Format(_("Are you sure you wish to drop FTS configuration \"%s\"?"),
			                           GetFullIdentifier().c_str());
			break;
		case DROPCASCADETITLE:
			message = _("Drop FTS configuration cascaded?");
			break;
		case DROPTITLE:
			message = _("Drop FTS configuration?");
			break;
		case PROPERTIESREPORT:
			message = _("FTS configuration properties report");
			message += wxT(" - ") + GetName();
			break;
		case PROPERTIES:
			message = _("FTS configuration properties");
			break;
		case DDLREPORT:
			message = _("FTS configuration DDL report");
			message += wxT(" - ") + GetName();
			break;
		case DDL:
			message = _("FTS configuration DDL");
			break;
		case DEPENDENCIESREPORT:
			message = _("FTS configuration dependencies report");
			message += wxT(" - ") + GetName();
			break;
		case DEPENDENCIES:
			message = _("FTS configuration dependencies");
			break;
		case DEPENDENTSREPORT:
			message = _("FTS configuration dependents report");
			message += wxT(" - ") + GetName();
			break;
		case DEPENDENTS:
			message = _("FTS configuration dependents");
			break;
	}

	return message;
}

bool pgTextSearchConfiguration::DropObject(wxFrame *frame, ctlTree *browser, bool cascaded)
{
	wxString sql = wxT("DROP TEXT SEARCH CONFIGURATION ") + this->GetSchema()->GetQuotedIdentifier() + wxT(".") + qtIdent(this->GetIdentifier());

	if (cascaded)
		sql += wxT(" CASCADE");

	return GetDatabase()->ExecuteVoid(sql);
}


wxString pgTextSearchConfiguration::GetSql(ctlTree *browser)
{
	if (sql.IsNull())
	{
		sql = wxT("-- Text Search Configuration: ") + GetFullIdentifier() + wxT("\n\n")
		      + wxT("-- DROP TEXT SEARCH CONFIGURATION ") + GetFullIdentifier() + wxT("\n\n")
		      + wxT("CREATE TEXT SEARCH CONFIGURATION ") + GetFullIdentifier() + wxT(" (")
		      + wxT("\n  PARSER = ") + qtTypeIdent(GetParser())
		      + wxT("\n);\n");

		for (size_t i = 0 ; i < tokens.GetCount() ; i++)
			sql += wxT("ALTER TEXT SEARCH CONFIGURATION ") + GetQuotedFullIdentifier()
			       +  wxT(" ADD MAPPING FOR ") + tokens.Item(i).BeforeFirst('/')
			       +  wxT(" WITH ") + tokens.Item(i).AfterFirst('/')
			       +  wxT(";\n");

		if (!GetComment().IsNull())
			sql += wxT("COMMENT ON TEXT SEARCH CONFIGURATION ") + GetFullIdentifier()
			       + wxT(" IS ") + qtDbString(GetComment()) + wxT(";\n");
	}

	return sql;
}


void pgTextSearchConfiguration::ShowTreeDetail(ctlTree *browser, frmMain *form, ctlListView *properties, ctlSQLBox *sqlPane)
{
	if (properties)
	{
		CreateListColumns(properties);

		properties->AppendItem(_("Name"), GetName());
		properties->AppendItem(_("OID"), GetOid());
		properties->AppendItem(_("Owner"), GetOwner());
		properties->AppendItem(_("Parser"), GetParser());
		properties->AppendItem(_("Comment"), firstLineOnly(GetComment()));
	}
}



pgObject *pgTextSearchConfiguration::Refresh(ctlTree *browser, const wxTreeItemId item)
{
	pgObject *config = 0;
	pgCollection *coll = browser->GetParentCollection(item);
	if (coll)
		config = textSearchConfigurationFactory.CreateObjects(coll, 0, wxT("\n   AND cfg.oid=") + GetOidStr());

	return config;
}


///////////////////////////////////////////////////


pgTextSearchConfigurationCollection::pgTextSearchConfigurationCollection(pgaFactory *factory, pgSchema *sch)
	: pgSchemaObjCollection(factory, sch)
{
}


wxString pgTextSearchConfigurationCollection::GetTranslatedMessage(int kindOfMessage) const
{
	wxString message = wxEmptyString;

	switch (kindOfMessage)
	{
		case RETRIEVINGDETAILS:
			message = _("Retrieving details on FTS configurations");
			break;
		case REFRESHINGDETAILS:
			message = _("Refreshing FTS configurations");
			break;
		case OBJECTSLISTREPORT:
			message = _("FTS configurations list report");
			break;
	}

	return message;
}


//////////////////////////////////////////////////////


pgObject *pgTextSearchConfigurationFactory::CreateObjects(pgCollection *collection, ctlTree *browser, const wxString &restriction)
{
	pgTextSearchConfiguration *config = 0;

	pgSet *configurations;
	configurations = collection->GetDatabase()->ExecuteSet(
	                     wxT("SELECT cfg.oid, cfg.cfgname, pg_get_userbyid(cfg.cfgowner) as cfgowner, cfg.cfgparser, parser.prsname as parsername, description\n")
	                     wxT("  FROM pg_ts_config cfg\n")
	                     wxT("  LEFT OUTER JOIN pg_ts_parser parser ON parser.oid=cfg.cfgparser\n")
	                     wxT("  LEFT OUTER JOIN pg_description des ON (des.objoid=cfg.oid AND des.classoid='pg_ts_config'::regclass)\n")
	                     wxT(" WHERE cfg.cfgnamespace = ") + collection->GetSchema()->GetOidStr()
	                     + restriction + wxT("\n")
	                     wxT(" ORDER BY cfg.cfgname"));

	if (configurations)
	{
		while (!configurations->Eof())
		{
			config = new pgTextSearchConfiguration(collection->GetSchema(), configurations->GetVal(wxT("cfgname")));
			config->iSetOid(configurations->GetOid(wxT("oid")));
			config->iSetOwner(configurations->GetVal(wxT("cfgowner")));
			config->iSetComment(configurations->GetVal(wxT("description")));
			config->iSetParser(configurations->GetVal(wxT("parsername")));
			config->iSetParserOid(configurations->GetOid(wxT("cfgparser")));

			pgSet *maps;
			maps = collection->GetDatabase()->ExecuteSet(
			           wxT("SELECT\n")
			           wxT("  (SELECT t.alias FROM pg_catalog.ts_token_type(cfgparser) AS t")
			           wxT("    WHERE t.tokid = maptokentype) AS tokenalias,\n")
			           wxT("  dictname\n")
			           wxT("FROM pg_ts_config_map\n")
			           wxT("  LEFT OUTER JOIN pg_ts_config ON mapcfg=pg_ts_config.oid\n")
			           wxT("  LEFT OUTER JOIN pg_ts_dict ON mapdict=pg_ts_dict.oid\n")
			           wxT("WHERE mapcfg=") + config->GetOidStr() + wxT("\n")
			           wxT("ORDER BY 1, mapseqno"));

			if (maps)
			{
				wxString tokenToAdd;
				while (!maps->Eof())
				{
					if (tokenToAdd.Length() > 0 &&
					        !tokenToAdd.BeforeFirst('/').IsSameAs(maps->GetVal(wxT("tokenalias")), false))
					{
						config->GetTokens().Add(tokenToAdd);
						tokenToAdd = wxT("");
					}

					if (tokenToAdd.Length() == 0)
						tokenToAdd = maps->GetVal(wxT("tokenalias")) + wxT("/") + maps->GetVal(wxT("dictname"));
					else
						tokenToAdd += wxT(",") + maps->GetVal(wxT("dictname"));

					maps->MoveNext();
				}

				delete maps;
			}

			if (browser)
			{
				browser->AppendObject(collection, config);
				configurations->MoveNext();
			}
			else
				break;
		}

		delete configurations;
	}
	return config;
}


#include "images/configuration.pngc"
#include "images/configurations.pngc"

pgTextSearchConfigurationFactory::pgTextSearchConfigurationFactory()
	: pgSchemaObjFactory(__("FTS Configuration"), __("New FTS Configuration..."), __("Create a new FTS Configuration."), configuration_png_img)
{
}


pgCollection *pgTextSearchConfigurationFactory::CreateCollection(pgObject *obj)
{
	return new pgTextSearchConfigurationCollection(GetCollectionFactory(), (pgSchema *)obj);
}

pgTextSearchConfigurationFactory textSearchConfigurationFactory;
static pgaCollectionFactory cf(&textSearchConfigurationFactory, __("FTS Configurations"), configurations_png_img);
