//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
// This software is released under the Artistic Licence
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
#include "schema/pgDomain.h"
#include "schema/pgAggregate.h"
#include "schema/pgConversion.h"
#include "schema/pgFunction.h"
#include "schema/pgType.h"
#include "schema/pgSequence.h"
#include "schema/pgTable.h"
#include "schema/pgOperator.h"
#include "schema/pgOperatorClass.h"
#include "schema/pgOperatorFamily.h"
#include "schema/pgView.h"
#include "frm/frmReport.h"

#include "wx/regex.h"

pgSchema::pgSchema(const wxString& newName)
: pgDatabaseObject(schemaFactory, newName)
{
    wxLogInfo(wxT("Creating a pgSchema object"));
}

pgSchema::~pgSchema()
{
    wxLogInfo(wxT("Destroying a pgSchema object"));
}


wxMenu *pgSchema::GetNewMenu()
{
    wxMenu *menu=pgObject::GetNewMenu();

    if (GetCreatePrivilege())
    {
        aggregateFactory.AppendMenu(menu);
        conversionFactory.AppendMenu(menu);
        domainFactory.AppendMenu(menu);
        functionFactory.AppendMenu(menu);
        triggerFunctionFactory.AppendMenu(menu);
        if (GetConnection()->BackendMinimumVersion(8, 1) || GetConnection()->EdbMinimumVersion(8, 0))
            procedureFactory.AppendMenu(menu);
        operatorFactory.AppendMenu(menu);
//      opclass
//		opfamily
        sequenceFactory.AppendMenu(menu);
        tableFactory.AppendMenu(menu);
        typeFactory.AppendMenu(menu);
        viewFactory.AppendMenu(menu);
    }
    return menu;
}

bool pgSchema::DropObject(wxFrame *frame, ctlTree *browser, bool cascaded)
{
    wxString sql = wxT("DROP SCHEMA ") + GetQuotedFullIdentifier();
    if (cascaded)
        sql += wxT(" CASCADE");
    return GetDatabase()->ExecuteVoid(sql);
}


wxString pgSchema::GetSql(ctlTree *browser)
{
    if (sql.IsNull())
    {
        sql = wxT("-- Schema: \"") + GetName() + wxT("\"\n\n")
            + wxT("-- DROP SCHEMA ") + GetQuotedFullIdentifier() + wxT(";")
            + wxT("\n\nCREATE SCHEMA ") + qtIdent(GetName()) 
            + wxT("\n  AUTHORIZATION ") + qtIdent(GetOwner());

        sql += wxT(";\n")
            + GetGrant(wxT("UC"), wxT("SCHEMA ") + GetQuotedFullIdentifier())
            + GetCommentSql();
    }
    return sql;
}


void pgSchema::ShowTreeDetail(ctlTree *browser, frmMain *form, ctlListView *properties, ctlSQLBox *sqlPane)
{
//    if (form)
//        form->SetDatabase(GetDatabase());

    GetDatabase()->GetServer()->iSetLastDatabase(GetDatabase()->GetName());
    GetDatabase()->GetServer()->iSetLastSchema(GetName());

    if (!expandedKids)
    {
        expandedKids=true;

        browser->RemoveDummyChild(this);

        // Log
        wxLogInfo(wxT("Adding child object to schema ") + GetIdentifier());

        if (settings->GetDisplayOption(wxT("Aggregates")))
			browser->AppendCollection(this, aggregateFactory);
		if (settings->GetDisplayOption(wxT("Conversions")))
			browser->AppendCollection(this, conversionFactory);
		if (settings->GetDisplayOption(wxT("Domains")))
			browser->AppendCollection(this, domainFactory);
		if (settings->GetDisplayOption(wxT("Functions")))
			browser->AppendCollection(this, functionFactory);
		if (settings->GetDisplayOption(wxT("Trigger functions")))
			browser->AppendCollection(this, triggerFunctionFactory);

		if (settings->GetDisplayOption(wxT("Procedures")))
		{
			if (GetConnection()->BackendMinimumVersion(8, 1) || GetConnection()->EdbMinimumVersion(8, 0))
				browser->AppendCollection(this, procedureFactory);
		}

		if (settings->GetDisplayOption(wxT("Operators")))
			browser->AppendCollection(this, operatorFactory);
		if (settings->GetDisplayOption(wxT("Operator classes")))
			browser->AppendCollection(this, operatorClassFactory);

		if (settings->GetDisplayOption(wxT("Operator families")))
		{
			if (GetConnection()->BackendMinimumVersion(8, 3))
				browser->AppendCollection(this, operatorFamilyFactory);
		}

		if (settings->GetDisplayOption(wxT("Sequences")))
			browser->AppendCollection(this, sequenceFactory);
		if (settings->GetDisplayOption(wxT("Tables")))
			browser->AppendCollection(this, tableFactory);
		if (settings->GetDisplayOption(wxT("Types")))
			browser->AppendCollection(this, typeFactory);
		if (settings->GetDisplayOption(wxT("Views")))
			browser->AppendCollection(this, viewFactory);
    }


    if (properties)
    {
        wxLogInfo(wxT("Displaying properties for schema ") + GetIdentifier());

        CreateListColumns(properties);

        properties->AppendItem(_("Name"), GetName());
        properties->AppendItem(_("OID"), GetOid());
        properties->AppendItem(_("Owner"), GetOwner());
        properties->AppendItem(_("ACL"), GetAcl());
        properties->AppendItem(_("System schema?"), GetSystemObject());
        properties->AppendItem(_("Comment"), firstLineOnly(GetComment()));
    }
}



pgObject *pgSchema::Refresh(ctlTree *browser, const wxTreeItemId item)
{
    pgObject *schema=0;
    pgCollection *coll=browser->GetParentCollection(item);
    if (coll)
        schema = schemaFactory.CreateObjects(coll, 0, wxT(" WHERE nsp.oid=") + GetOidStr() + wxT("\n"));

    return schema;
}



pgObject *pgSchemaFactory::CreateObjects(pgCollection *collection, ctlTree *browser, const wxString &restriction)
{
    pgSchema *schema=0;

    wxString restr=restriction;

    if (!collection->GetDatabase()->GetSchemaRestriction().IsEmpty())
    {
        if (restr.IsEmpty())
            restr += wxT(" WHERE (");
        else
            restr += wxT("   AND (");
        restr += collection->GetDatabase()->GetSchemaRestriction() + wxT(")");
    }

	wxString sql;
	if (collection->GetDatabase()->BackendMinimumVersion(8, 1))
	{
		sql =   wxT("SELECT CASE WHEN nspname LIKE E'pg\\_temp\\_%%' THEN 1\n")
				wxT("            WHEN (nspname LIKE E'pg\\_%' OR nspname = 'information_schema') THEN 0\n");
	}
	else
	{
		sql =   wxT("SELECT CASE WHEN nspname LIKE 'pg\\_temp\\_%%' THEN 1\n")
				wxT("            WHEN (nspname LIKE 'pg\\_%' OR nspname = 'information_schema') THEN 0\n");
	}
	sql +=	wxT("            ELSE 3 END AS nsptyp,\n")
			wxT("       nsp.nspname, nsp.oid, pg_get_userbyid(nspowner) AS namespaceowner, nspacl, description,")
			wxT("       has_schema_privilege(nsp.oid, 'CREATE') as cancreate\n")
			wxT("  FROM pg_namespace nsp\n")
			wxT("  LEFT OUTER JOIN pg_description des ON des.objoid=nsp.oid\n")
			+ restr +
			wxT(" ORDER BY 1, nspname");

    pgSet *schemas = collection->GetDatabase()->ExecuteSet(sql);

    if (schemas)
    {
        while (!schemas->Eof())
        {
            wxString name=schemas->GetVal(wxT("nspname"));
            long nsptyp=schemas->GetLong(wxT("nsptyp"));

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

            if (nsptyp <= SCHEMATYP_USERSYS && !settings->GetShowSystemObjects())
            {
                schemas->MoveNext();
                continue;
            }

            schema = new pgSchema(name);
            schema->iSetSchemaTyp(nsptyp);
            schema->iSetDatabase(collection->GetDatabase());
            schema->iSetComment(schemas->GetVal(wxT("description")));
            schema->iSetOid(schemas->GetOid(wxT("oid")));
            schema->iSetOwner(schemas->GetVal(wxT("namespaceowner")));
            schema->iSetAcl(schemas->GetVal(wxT("nspacl")));
            schema->iSetCreatePrivilege(schemas->GetBool(wxT("cancreate")));

            if (browser)
            {
                browser->AppendObject(collection, schema);
				schemas->MoveNext();
            }
            else
                break;
        }

		delete schemas;
    }
    return schema;
}


pgObject *pgSchema::ReadObjects(pgCollection *collection, ctlTree *browser)
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
    server= database->GetServer();
    iSetOid(sch->GetOid());
}


bool pgSchemaObjCollection::CanCreate()
{
	if(IsCollectionForType(PGM_OPCLASS) || IsCollectionForType(PGM_OPFAMILY))
		return false;

    return GetSchema()->GetCreatePrivilege();
}


#include "images/namespace.xpm"
#include "images/namespace-sm.xpm"
#include "images/namespaces.xpm"

pgSchemaFactory::pgSchemaFactory() 
: pgDatabaseObjFactory(__("Schema"), __("New Schema..."), __("Create a new Schema."), namespace_xpm, namespace_sm_xpm)
{
    metaType = PGM_SCHEMA;
}

pgCollection *pgSchemaObjFactory::CreateCollection(pgObject *obj)
{
    return new pgSchemaObjCollection(GetCollectionFactory(), (pgSchema*)obj);
}


pgSchemaFactory schemaFactory;
static pgaCollectionFactory cf(&schemaFactory, __("Schemas"), namespaces_xpm);
