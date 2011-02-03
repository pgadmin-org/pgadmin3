//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2010, The pgAdmin Development Team
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

bool pgTextSearchConfiguration::DropObject(wxFrame *frame, ctlTree *browser, bool cascaded)
{
	wxString sql = wxT("DROP TEXT SEARCH CONFIGURATION ") + this->GetSchema()->GetQuotedIdentifier() + wxT(".") + this->GetIdentifier();

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



//////////////////////////////////////////////////////


pgObject *pgTextSearchConfigurationFactory::CreateObjects(pgCollection *collection, ctlTree *browser, const wxString &restriction)
{
	pgTextSearchConfiguration *config = 0;

	pgSet *configurations;
	configurations = collection->GetDatabase()->ExecuteSet(
	                     wxT("SELECT cfg.oid, cfg.cfgname, pg_get_userbyid(cfg.cfgowner) as cfgowner, cfg.cfgparser, parser.prsname as parsername, description\n")
	                     wxT("  FROM pg_ts_config cfg\n")
	                     wxT("  LEFT OUTER JOIN pg_ts_parser parser ON parser.oid=cfg.cfgparser\n")
	                     wxT("  LEFT OUTER JOIN pg_description des ON des.objoid=cfg.oid\n")
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

				if (tokenToAdd.Length() > 0 &&
				        !tokenToAdd.BeforeFirst('/').IsSameAs(maps->GetVal(wxT("tokenalias")), false))
				{
					config->GetTokens().Add(tokenToAdd);
					tokenToAdd = wxT("");
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


#include "images/configuration.xpm"
#include "images/configurations.xpm"

pgTextSearchConfigurationFactory::pgTextSearchConfigurationFactory()
	: pgSchemaObjFactory(__("FTS Configuration"), __("New FTS Configuration..."), __("Create a new FTS Configuration."), configuration_xpm)
{
}


pgTextSearchConfigurationFactory textSearchConfigurationFactory;
static pgaCollectionFactory cf(&textSearchConfigurationFactory, __("FTS Configurations"), configurations_xpm);
