//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2010, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// pgSchema.cpp - schema class
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "frm/menu.h"
#include "utils/misc.h"
#include "schema/pgSchema.h"
#include "frm/frmMain.h"
#include "schema/pgCatalogObject.h"
#include "schema/edbPackage.h"
#include "schema/pgDomain.h"
#include "schema/pgAggregate.h"
#include "schema/pgConversion.h"
#include "schema/pgFunction.h"
#include "schema/pgOperator.h"
#include "schema/pgOperatorClass.h"
#include "schema/pgOperatorFamily.h"
#include "schema/pgSequence.h"
#include "schema/pgTable.h"
#include "schema/gpExtTable.h"
#include "schema/pgTextSearchConfiguration.h"
#include "schema/pgTextSearchDictionary.h"
#include "schema/pgTextSearchParser.h"
#include "schema/pgTextSearchTemplate.h"
#include "schema/pgType.h"
#include "schema/pgView.h"
#include "schema/gpPartition.h"
#include "schema/edbPrivateSynonym.h"
#include "frm/frmReport.h"

#include "wx/regex.h"

pgSchema::pgSchema(const wxString &newName)
	: pgSchemaBase(schemaFactory, newName)
{
}

pgCatalog::pgCatalog(const wxString &newName)
	: pgSchemaBase(catalogFactory, newName)
{
}

pgSchemaBase::pgSchemaBase(pgaFactory &factory, const wxString &newName)
	: pgDatabaseObject(factory, newName)
{
}

wxString pgCatalog::GetDisplayName()
{
	if (GetFullName() == wxT("pg_catalog"))
		return wxT("PostgreSQL (pg_catalog)");
	else if (GetFullName() == wxT("pgagent"))
		return wxT("pgAgent (pgagent)");
	else if (GetFullName() == wxT("information_schema"))
		return wxT("ANSI (information_schema)");
	else if (GetFullName().StartsWith(wxT("_")))
		return wxT("Slony cluster (") + GetFullName().AfterFirst('_') + wxT(")");
	else if (GetFullName() == wxT("dbo"))
		return wxT("Redmond (dbo)");
	else if (GetFullName() == wxT("sys"))
		return wxT("Redwood (sys)");
	else
		return GetFullName();
}


wxMenu *pgSchemaBase::GetNewMenu()
{
	wxMenu *menu = pgObject::GetNewMenu();

	if (GetCreatePrivilege())
	{
		if (settings->GetDisplayOption(_("Aggregates")))
			aggregateFactory.AppendMenu(menu);
		if (settings->GetDisplayOption(_("Conversions")))
			conversionFactory.AppendMenu(menu);
		if (settings->GetDisplayOption(_("Domains")))
			domainFactory.AppendMenu(menu);
		if (settings->GetDisplayOption(_("Functions")))
			functionFactory.AppendMenu(menu);
		if (settings->GetDisplayOption(_("Packages")))
		{
			if (GetConnection()->EdbMinimumVersion(8, 0))
				packageFactory.AppendMenu(menu);
		}
		if (settings->GetDisplayOption(_("Procedures")))
		{
			if (GetConnection()->EdbMinimumVersion(8, 0))
				procedureFactory.AppendMenu(menu);
		}
		if (settings->GetDisplayOption(_("Operators")))
			operatorFactory.AppendMenu(menu);
		if (settings->GetDisplayOption(_("Sequences")))
			sequenceFactory.AppendMenu(menu);
		if (settings->GetDisplayOption(_("Tables")))
			tableFactory.AppendMenu(menu);
		if (settings->GetDisplayOption(_("FTS Configurations")))
		{
			if (GetConnection()->BackendMinimumVersion(8, 3))
				textSearchConfigurationFactory.AppendMenu(menu);
		}
		if (settings->GetDisplayOption(_("FTS Dictionaries")))
		{
			if (GetConnection()->BackendMinimumVersion(8, 3))
				textSearchConfigurationFactory.AppendMenu(menu);
		}
		if (settings->GetDisplayOption(_("FTS Parsers")))
		{
			if (GetConnection()->BackendMinimumVersion(8, 3))
				textSearchConfigurationFactory.AppendMenu(menu);
		}
		if (settings->GetDisplayOption(_("FTS Templates")))
		{
			if (GetConnection()->BackendMinimumVersion(8, 3))
				textSearchConfigurationFactory.AppendMenu(menu);
		}
		if (settings->GetDisplayOption(_("Trigger Functions")))
			triggerFunctionFactory.AppendMenu(menu);
		if (settings->GetDisplayOption(_("Types")))
			typeFactory.AppendMenu(menu);
		if (settings->GetDisplayOption(_("Views")))
			viewFactory.AppendMenu(menu);
		if (settings->GetDisplayOption(_("External Tables")))
		{
			if (GetConnection() != 0 && GetConnection()->GetIsGreenplum())
				extTableFactory.AppendMenu(menu);
		}
		if (settings->GetDisplayOption(_("Synonyms")) && GetConnection()->EdbMinimumVersion(8, 4))
			edbPrivFactory.AppendMenu(menu);
	}
	return menu;
}

bool pgSchemaBase::DropObject(wxFrame *frame, ctlTree *browser, bool cascaded)
{
	wxString sql = wxT("DROP SCHEMA ") + GetQuotedFullIdentifier();
	if (cascaded)
		sql += wxT(" CASCADE");
	return GetDatabase()->ExecuteVoid(sql);
}


wxString pgSchemaBase::GetSql(ctlTree *browser)
{
	if (sql.IsNull())
	{
		wxString strName = qtIdent(GetName());
		if (GetMetaType() == PGM_CATALOG)
			sql = wxT("-- Catalog: ") + GetName() + wxT("\n\n");
		else
			sql = wxT("-- Schema: ") + GetName() + wxT("\n\n");

		sql += wxT("-- DROP SCHEMA ") + GetQuotedFullIdentifier() + wxT(";")
		       + wxT("\n\nCREATE SCHEMA ") + strName
		       + wxT("\n  AUTHORIZATION ") + qtIdent(GetOwner());
		sql += wxT(";\n\n");

		sql += GetGrant(wxT("UC"), wxT("SCHEMA ") + GetQuotedFullIdentifier())
		       + GetCommentSql();

		sql += wxT("\n") + pgDatabase::GetDefaultPrivileges('r', m_defPrivsOnTables, strName);
		sql += pgDatabase::GetDefaultPrivileges('S', m_defPrivsOnSeqs, strName);
		sql += pgDatabase::GetDefaultPrivileges('f', m_defPrivsOnFuncs, strName);
	}
	return sql;
}


void pgSchemaBase::ShowTreeDetail(ctlTree *browser, frmMain *form, ctlListView *properties, ctlSQLBox *sqlPane)
{

	GetDatabase()->GetServer()->iSetLastDatabase(GetDatabase()->GetName());
	GetDatabase()->GetServer()->iSetLastSchema(GetName());

	if (!expandedKids)
	{
		expandedKids = true;

		browser->RemoveDummyChild(this);

		// Log
		wxLogInfo(wxT("Adding child object to schema %s"), GetIdentifier().c_str());

		if (!(GetMetaType() == PGM_CATALOG && (GetFullName() == wxT("dbo") || GetFullName() == wxT("sys") || GetFullName() == wxT("information_schema"))))
		{
			if (settings->GetDisplayOption(_("Aggregates")))
				browser->AppendCollection(this, aggregateFactory);
			if (settings->GetDisplayOption(_("Conversions")))
				browser->AppendCollection(this, conversionFactory);
			if (settings->GetDisplayOption(_("Domains")))
				browser->AppendCollection(this, domainFactory);
			if (settings->GetDisplayOption(_("FTS Configurations")))
			{
				if (GetConnection()->BackendMinimumVersion(8, 3))
					browser->AppendCollection(this, textSearchConfigurationFactory);
			}
			if (settings->GetDisplayOption(_("FTS Dictionaries")))
			{
				if (GetConnection()->BackendMinimumVersion(8, 3))
					browser->AppendCollection(this, textSearchDictionaryFactory);
			}
			if (settings->GetDisplayOption(_("FTS Parsers")))
			{
				if (GetConnection()->BackendMinimumVersion(8, 3))
					browser->AppendCollection(this, textSearchParserFactory);
			}
			if (settings->GetDisplayOption(_("FTS Templates")))
			{
				if (GetConnection()->BackendMinimumVersion(8, 3))
					browser->AppendCollection(this, textSearchTemplateFactory);
			}
			if (settings->GetDisplayOption(_("Functions")))
				browser->AppendCollection(this, functionFactory);

			if (settings->GetDisplayOption(_("Operators")))
				browser->AppendCollection(this, operatorFactory);
			if (settings->GetDisplayOption(_("Operator Classes")))
				browser->AppendCollection(this, operatorClassFactory);

			if (settings->GetDisplayOption(_("Operator Families")))
			{
				if (GetConnection()->BackendMinimumVersion(8, 3))
					browser->AppendCollection(this, operatorFamilyFactory);
			}

			if (settings->GetDisplayOption(_("Packages")) && GetConnection()->EdbMinimumVersion(8, 1))
				browser->AppendCollection(this, packageFactory);

			if (settings->GetDisplayOption(_("Procedures")))
			{
				if (GetConnection()->EdbMinimumVersion(8, 0))
					browser->AppendCollection(this, procedureFactory);
			}

			if (settings->GetDisplayOption(_("Sequences")))
				browser->AppendCollection(this, sequenceFactory);
			if (settings->GetDisplayOption(_("Tables")))
				browser->AppendCollection(this, tableFactory);
			if (settings->GetDisplayOption(_("External Tables")))
			{
				if (GetConnection() != 0 && GetConnection()->GetIsGreenplum())
					browser->AppendCollection(this, extTableFactory);
			}
			if (settings->GetDisplayOption(_("Trigger Functions")))
				browser->AppendCollection(this, triggerFunctionFactory);
			if (settings->GetDisplayOption(_("Types")))
				browser->AppendCollection(this, typeFactory);
			if (settings->GetDisplayOption(_("Views")))
				browser->AppendCollection(this, viewFactory);
			if (settings->GetDisplayOption(_("Synonyms")) && GetConnection()->EdbMinimumVersion(8, 4))
				browser->AppendCollection(this, edbPrivFactory);
		}
		else
			browser->AppendCollection(this, catalogObjectFactory);
	}


	if (properties)
	{
		CreateListColumns(properties);

		properties->AppendItem(_("Name"), GetName());
		properties->AppendItem(_("OID"), GetOid());
		properties->AppendItem(_("Owner"), GetOwner());
		properties->AppendItem(_("ACL"), GetAcl());
		properties->AppendItem(_("Default table ACL"), m_defPrivsOnTables);
		properties->AppendItem(_("Default sequence ACL"), m_defPrivsOnSeqs);
		properties->AppendItem(_("Default function ACL"), m_defPrivsOnFuncs);

		if (GetMetaType() != PGM_CATALOG)
			properties->AppendItem(_("System schema?"), GetSystemObject());

		properties->AppendItem(_("Comment"), firstLineOnly(GetComment()));
	}
}



pgObject *pgSchemaBase::Refresh(ctlTree *browser, const wxTreeItemId item)
{
	pgObject *schema = 0;
	pgCollection *coll = browser->GetParentCollection(item);
	if (coll)
	{
		if (coll->IsCollectionForType(PGM_CATALOG))
			schema = catalogFactory.CreateObjects(coll, 0, wxT(" WHERE nsp.oid=") + GetOidStr() + wxT("\n"));
		else
			schema = schemaFactory.CreateObjects(coll, 0, wxT(" WHERE nsp.oid=") + GetOidStr() + wxT("\n"));
	}

	return schema;
}



pgObject *pgSchemaBaseFactory::CreateObjects(pgCollection *collection, ctlTree *browser, const wxString &restriction)
{
	pgSchema *schema = 0;
	pgCatalog *catalog = 0;

	wxString restr = restriction;

	if (restr.IsEmpty())
		restr += wxT(" WHERE ");
	else
		restr += wxT("   AND ");

	if (GetMetaType() != PGM_CATALOG)
	{
		restr += wxT("NOT ");
	}

	restr += wxT("((nspname = 'pg_catalog' AND EXISTS (SELECT 1 FROM pg_class WHERE relname = 'pg_class' AND relnamespace = nsp.oid LIMIT 1)) OR\n");
	restr += wxT("(nspname = 'pgagent' AND EXISTS (SELECT 1 FROM pg_class WHERE relname = 'pga_job' AND relnamespace = nsp.oid LIMIT 1)) OR\n");
	restr += wxT("(nspname = 'information_schema' AND EXISTS (SELECT 1 FROM pg_class WHERE relname = 'tables' AND relnamespace = nsp.oid LIMIT 1)) OR\n");
	restr += wxT("(nspname LIKE '_%' AND EXISTS (SELECT 1 FROM pg_proc WHERE proname='slonyversion' AND pronamespace = nsp.oid LIMIT 1)) OR\n");
	restr += wxT("(nspname = 'dbo' AND EXISTS (SELECT 1 FROM pg_class WHERE relname = 'systables' AND relnamespace = nsp.oid LIMIT 1)) OR\n");
	restr += wxT("(nspname = 'sys' AND EXISTS (SELECT 1 FROM pg_class WHERE relname = 'all_tables' AND relnamespace = nsp.oid LIMIT 1)))\n");

	if (collection->GetConnection()->EdbMinimumVersion(8, 2))
	{
		restr += wxT("  AND nsp.nspparent = 0\n");
		// Do not show dbms_job_procedure in schemas
		if (!settings->GetShowSystemObjects())
			restr += wxT("AND NOT (nspname = 'dbms_job_procedure' AND EXISTS(SELECT 1 FROM pg_proc WHERE pronamespace = nsp.oid and proname = 'run_job' LIMIT 1))\n");
	}

	if (!collection->GetDatabase()->GetSchemaRestriction().IsEmpty())
		restr += wxT("  AND nspname IN (") + collection->GetDatabase()->GetSchemaRestriction() + wxT(")");

	// Don't fetch temp schemas if not actually required, as Greenplum seems to
	// generate thousands in some circumstances.
	if (!settings->GetShowSystemObjects())
	{
		if (collection->GetDatabase()->BackendMinimumVersion(8, 1))
			restr += wxT("  AND nspname NOT LIKE E'pg\\\\_temp\\\\_%'AND nspname NOT LIKE E'pg\\\\_toast_temp\\\\_%'");
		else
			restr += wxT("  AND nspname NOT LIKE 'pg\\\\_temp\\\\_%'AND nspname NOT LIKE 'pg\\\\_toast_temp\\\\_%'");
	}

	wxString sql;

	if (GetMetaType() == PGM_CATALOG)
	{
		sql = wxT("SELECT 2 AS nsptyp,\n")
		      wxT("       nsp.nspname, nsp.oid, pg_get_userbyid(nspowner) AS namespaceowner, nspacl, description,")
		      wxT("       FALSE as cancreate\n")
		      wxT("  FROM pg_namespace nsp\n")
		      wxT("  LEFT OUTER JOIN pg_description des ON des.objoid=nsp.oid\n")
		      + restr +
		      wxT(" ORDER BY 1, nspname");
	}
	else
	{
		if (collection->GetDatabase()->BackendMinimumVersion(8, 1))
		{
			sql = wxT("SELECT CASE WHEN nspname LIKE E'pg\\\\_temp\\\\_%' THEN 1\n")
			      wxT("            WHEN (nspname LIKE E'pg\\\\_%') THEN 0\n");
		}
		else
		{
			sql = wxT("SELECT CASE WHEN nspname LIKE 'pg\\\\_temp\\\\_%' THEN 1\n")
			      wxT("            WHEN (nspname LIKE 'pg\\\\_%') THEN 0\n");
		}
		sql += wxT("            ELSE 3 END AS nsptyp,\n")
		       wxT("       nsp.nspname, nsp.oid, pg_get_userbyid(nspowner) AS namespaceowner, nspacl, description,")
		       wxT("       has_schema_privilege(nsp.oid, 'CREATE') as cancreate\n")
		       wxT("  FROM pg_namespace nsp\n")
		       wxT("  LEFT OUTER JOIN pg_description des ON des.objoid=nsp.oid\n")
		       + restr +
		       wxT(" ORDER BY 1, nspname");
	}

	pgSet *schemas = collection->GetDatabase()->ExecuteSet(sql);

	if (schemas)
	{
		while (!schemas->Eof())
		{
			wxString name = schemas->GetVal(wxT("nspname"));
			long nsptyp = schemas->GetLong(wxT("nsptyp"));

			wxStringTokenizer tokens(settings->GetSystemSchemas(), wxT(","));
			while (tokens.HasMoreTokens())
			{
				wxRegEx regex(tokens.GetNextToken());
				if (regex.Matches(name))
				{
					nsptyp = SCHEMATYP_USERSYS;
					break;
				}
			}

			if (nsptyp <= SCHEMATYP_USERSYS && this->GetMetaType() != PGM_CATALOG && !settings->GetShowSystemObjects())
			{
				schemas->MoveNext();
				continue;
			}

			if (GetMetaType() == PGM_CATALOG)
			{
				catalog = new pgCatalog(name);

				catalog->iSetSchemaTyp(nsptyp);
				catalog->iSetDatabase(collection->GetDatabase());
				catalog->iSetComment(schemas->GetVal(wxT("description")));
				catalog->iSetOid(schemas->GetOid(wxT("oid")));
				catalog->iSetOwner(schemas->GetVal(wxT("namespaceowner")));
				catalog->iSetAcl(schemas->GetVal(wxT("nspacl")));
				catalog->iSetCreatePrivilege(false);

				if (collection->GetDatabase()->BackendMinimumVersion(9, 0))
				{
					catalog->iSetDefPrivsOnTables(collection->GetConnection()->ExecuteScalar(wxT("SELECT defaclacl FROM pg_catalog.pg_default_acl dacl WHERE dacl.defaclnamespace = " + catalog->GetOidStr() + wxT(" AND defaclobjtype='r'"))));
					catalog->iSetDefPrivsOnSeqs(collection->GetConnection()->ExecuteScalar(wxT("SELECT defaclacl FROM pg_catalog.pg_default_acl dacl WHERE dacl.defaclnamespace = " + catalog->GetOidStr() + wxT(" AND defaclobjtype='S'"))));
					catalog->iSetDefPrivsOnFuncs(collection->GetConnection()->ExecuteScalar(wxT("SELECT defaclacl FROM pg_catalog.pg_default_acl dacl WHERE dacl.defaclnamespace = " + catalog->GetOidStr() + wxT(" AND defaclobjtype='f'"))));
				}

				if (browser)
				{
					browser->AppendObject(collection, catalog);
					schemas->MoveNext();
				}
				else
					break;
			}
			else
			{
				schema = new pgSchema(name);

				schema->iSetSchemaTyp(nsptyp);
				schema->iSetDatabase(collection->GetDatabase());
				schema->iSetComment(schemas->GetVal(wxT("description")));
				schema->iSetOid(schemas->GetOid(wxT("oid")));
				schema->iSetOwner(schemas->GetVal(wxT("namespaceowner")));
				schema->iSetAcl(schemas->GetVal(wxT("nspacl")));
				schema->iSetCreatePrivilege(schemas->GetBool(wxT("cancreate")));

				if (collection->GetDatabase()->BackendMinimumVersion(9, 0))
				{
					schema->iSetDefPrivsOnTables(collection->GetConnection()->ExecuteScalar(wxT("SELECT defaclacl FROM pg_catalog.pg_default_acl dacl WHERE dacl.defaclnamespace = " + schema->GetOidStr() + wxT(" AND defaclobjtype='r'"))));
					schema->iSetDefPrivsOnSeqs(collection->GetConnection()->ExecuteScalar(wxT("SELECT defaclacl FROM pg_catalog.pg_default_acl dacl WHERE dacl.defaclnamespace = " + schema->GetOidStr() + wxT(" AND defaclobjtype='S'"))));
					schema->iSetDefPrivsOnFuncs(collection->GetConnection()->ExecuteScalar(wxT("SELECT defaclacl FROM pg_catalog.pg_default_acl dacl WHERE dacl.defaclnamespace = " + schema->GetOidStr() + wxT(" AND defaclobjtype='f'"))));
				}

				if (browser)
				{
					browser->AppendObject(collection, schema);
					schemas->MoveNext();
				}
				else
					break;
			}
		}

		delete schemas;
	}

	if (GetMetaType() == PGM_CATALOG)
		return catalog;
	else
		return schema;
}


pgObject *pgSchemaBase::ReadObjects(pgCollection *collection, ctlTree *browser)
{
	wxString systemRestriction;
	if (!settings->GetShowSystemObjects())
		systemRestriction = wxT("WHERE ") + collection->GetConnection()->SystemNamespaceRestriction(wxT("nsp.nspname"));

	// Get the schemas
	return schemaFactory.CreateObjects(collection, browser, systemRestriction);
}


/////////////////////////////////////////////////////

pgSchemaObjCollection::pgSchemaObjCollection(pgaFactory *factory, pgSchema *sch)
	: pgCollection(factory)
{
	schema = sch;
	database = schema->GetDatabase();
	server = database->GetServer();
	iSetOid(sch->GetOid());
}


bool pgSchemaObjCollection::CanCreate()
{
	if(IsCollectionForType(PGM_OPCLASS) || IsCollectionForType(PGM_OPFAMILY))
		return false;

	// TODO
	// OK, this is a hack. Rules and Views are both derived from pgRuleObject, which
	// is derived from pgSchemaObject. In order that they attach to the treeview
	// under the relevant node however, the Schema object is actually the table or
	// View (yeah, I know - I didn't write it :-p ). This works fine *except* for
	// Get CreatePrivilege() which doesn't exist in these classes so must be fixed
	// up at this level. This needs a major rethink in the longer term
	if (GetSchema()->GetMetaType() == PGM_TABLE || GetSchema()->GetMetaType() == PGM_VIEW || GetSchema()->GetMetaType() == GP_EXTTABLE || GetSchema()->GetMetaType() == GP_PARTITION)
		return GetSchema()->GetSchema()->GetCreatePrivilege();
	else
		return GetSchema()->GetCreatePrivilege();
}


#include "images/namespace.xpm"
#include "images/namespace-sm.xpm"
#include "images/namespaces.xpm"
#include "images/catalog.xpm"
#include "images/catalog-sm.xpm"
#include "images/catalogs.xpm"

pgSchemaBaseFactory::pgSchemaBaseFactory(const wxChar *tn, const wxChar *ns, const wxChar *nls, const char **img, const char **imgSm)
	: pgDatabaseObjFactory(tn, ns, nls, img, imgSm)
{

}

pgSchemaFactory::pgSchemaFactory()
	: pgSchemaBaseFactory(__("Schema"), __("New Schema..."), __("Create a new Schema."), namespace_xpm, namespace_sm_xpm)
{
	metaType = PGM_SCHEMA;
}

pgCatalogFactory::pgCatalogFactory()
	: pgSchemaBaseFactory(__("Catalog"), __("New Catalog..."), __("Create a new Catalog."), catalog_xpm, catalog_sm_xpm)
{
	metaType = PGM_CATALOG;
}

pgCollection *pgSchemaObjFactory::CreateCollection(pgObject *obj)
{
	return new pgSchemaObjCollection(GetCollectionFactory(), (pgSchema *)obj);
}


pgSchemaFactory schemaFactory;
static pgaCollectionFactory scf(&schemaFactory, __("Schemas"), namespaces_xpm);

pgCatalogFactory catalogFactory;
static pgaCollectionFactory ccf(&catalogFactory, __("Catalogs"), catalogs_xpm);
