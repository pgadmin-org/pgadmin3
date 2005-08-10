//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2005, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// pgSchema.cpp - schema class
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "misc.h"
#include "pgObject.h"
#include "pgSchema.h"
#include "pgCollection.h"
#include "frmMain.h"
#include "pgDomain.h"
#include "pgAggregate.h"
#include "pgConversion.h"
#include "pgFunction.h"
#include "pgType.h"
#include "pgSequence.h"
#include "pgTable.h"
#include "pgOperator.h"
#include "pgOperatorClass.h"
#include "pgView.h"

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
        sequenceFactory.AppendMenu(menu);
        tableFactory.AppendMenu(menu);
        typeFactory.AppendMenu(menu);
        viewFactory.AppendMenu(menu);
    }
    return menu;
}


bool pgSchema::DropObject(wxFrame *frame, wxTreeCtrl *browser, bool cascaded)
{
    wxString sql = wxT("DROP SCHEMA ") + GetQuotedFullIdentifier();
    if (cascaded)
        sql += wxT(" CASCADE");
    return GetDatabase()->ExecuteVoid(sql);
}


wxString pgSchema::GetSql(wxTreeCtrl *browser)
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


void pgSchema::ShowTreeDetail(wxTreeCtrl *browser, frmMain *form, ctlListView *properties, ctlSQLBox *sqlPane)
{
    if (form)
        form->SetDatabase(GetDatabase());

    GetDatabase()->GetServer()->iSetLastDatabase(GetDatabase()->GetName());
    GetDatabase()->GetServer()->iSetLastSchema(GetName());

    if (!expandedKids)
    {
        expandedKids=true;

        RemoveDummyChild(browser);
        // Log
        wxLogInfo(wxT("Adding child object to schema ") + GetIdentifier());

        pgCollection *collection;

        // Aggregates
        collection = new pgSchemaObjCollection(*aggregateFactory.GetCollectionFactory(), this);
        AppendBrowserItem(browser, collection);

        // Conversions
        collection = new pgSchemaObjCollection(*conversionFactory.GetCollectionFactory(), this);
        AppendBrowserItem(browser, collection);

        // Domains
        collection = new pgSchemaObjCollection(*domainFactory.GetCollectionFactory(), this);
        AppendBrowserItem(browser, collection);

        // Functions
        collection = new pgSchemaObjCollection(*functionFactory.GetCollectionFactory(), this);
        AppendBrowserItem(browser, collection);

        collection = new pgSchemaObjCollection(*triggerFunctionFactory.GetCollectionFactory(), this);
        AppendBrowserItem(browser, collection);

        if (GetConnection()->BackendMinimumVersion(8, 1) || GetConnection()->EdbMinimumVersion(8, 0))
        {
            collection = new pgSchemaObjCollection(*procedureFactory.GetCollectionFactory(), this);
            AppendBrowserItem(browser, collection);
        }

        // Operators
        collection = new pgSchemaObjCollection(*operatorFactory.GetCollectionFactory(), this);
        AppendBrowserItem(browser, collection);

        // Operator Classes
        collection = new pgOperatorClassCollection(*operatorClassFactory.GetCollectionFactory(), this);
        AppendBrowserItem(browser, collection);

        // Sequences
        collection = new pgSchemaObjCollection(*sequenceFactory.GetCollectionFactory(), this);
        AppendBrowserItem(browser, collection);

        // Tables
        collection = new pgTableCollection(*tableFactory.GetCollectionFactory(), this);
        AppendBrowserItem(browser, collection);

        // Types
        collection = new pgSchemaObjCollection(*typeFactory.GetCollectionFactory(), this);
        AppendBrowserItem(browser, collection);

        // Views
        collection = new pgSchemaObjCollection(*viewFactory.GetCollectionFactory(), this);
        AppendBrowserItem(browser, collection);
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
        properties->AppendItem(_("Comment"), GetComment());
    }
}



pgObject *pgSchema::Refresh(wxTreeCtrl *browser, const wxTreeItemId item)
{
    pgObject *schema=0;
    wxTreeItemId parentItem=browser->GetItemParent(item);
    if (parentItem)
    {
        pgObject *obj=(pgObject*)browser->GetItemData(parentItem);
        if (obj->IsCollection())
            schema = schemaFactory.CreateObjects((pgCollection*)obj, 0, wxT(" WHERE nsp.oid=") + GetOidStr() + wxT("\n"));
    }
    return schema;
}



pgObject *pgaSchemaFactory::CreateObjects(pgCollection *collection, wxTreeCtrl *browser, const wxString &restriction)
{
    pgSchema *schema=0;

    pgSet *schemas = collection->GetDatabase()->ExecuteSet(
        wxT("SELECT CASE WHEN nspname LIKE 'pg\\_temp\\_%%' THEN 1\n")
        wxT("            WHEN nsp.oid<") + NumToStr(collection->GetServer()->GetLastSystemOID()) +
                         wxT(" OR nspname like 'pg\\_%' THEN 0\n")
        wxT("            ELSE 3 END AS nsptyp,\n")
        wxT("       nsp.nspname, nsp.oid, pg_get_userbyid(nspowner) AS namespaceowner, nspacl, description,")
        wxT("       has_schema_privilege(nsp.oid, 'CREATE') as cancreate\n")
        wxT("  FROM pg_namespace nsp\n")
        wxT("  LEFT OUTER JOIN pg_description des ON des.objoid=nsp.oid\n")
         + restriction +
        wxT(" ORDER BY 1, nspname"));

    if (schemas)
    {
        while (!schemas->Eof())
        {
            wxString name=schemas->GetVal(wxT("nspname"));
            long nsptyp=schemas->GetLong(wxT("nsptyp"));
            if (nsptyp == SCHEMATYP_NORMAL)
            {
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
                if (nsptyp == SCHEMATYP_USERSYS && !settings->GetShowSystemObjects())
                {
                    schemas->MoveNext();
                    continue;
                }
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
                collection->AppendBrowserItem(browser, schema);
				schemas->MoveNext();
            }
            else
                break;
        }

		delete schemas;
    }
    return schema;
}


pgObject *pgSchema::ReadObjects(pgCollection *collection, wxTreeCtrl *browser)
{
    wxString systemRestriction;
    if (!settings->GetShowSystemObjects())
        systemRestriction = wxT("WHERE ") + collection->GetConnection()->SystemNamespaceRestriction(wxT("nsp.nspname"));

    // Get the schemas
    return schemaFactory.CreateObjects(collection, browser, systemRestriction);
}


/////////////////////////////////////////////////////

pgSchemaObjCollection::pgSchemaObjCollection(pgaFactory &factory, pgSchema *sch)
: pgCollection(factory)
{ 
    schema = sch;
    database = schema->GetDatabase();
    server= database->GetServer();
}


bool pgSchemaObjCollection::CanCreate()
{
    return GetSchema()->GetCreatePrivilege();
}


#include "images/namespace.xpm"

pgaSchemaFactory::pgaSchemaFactory() 
: pgaFactory(__("Schema"), __("New Schema"), __("Create a new Schema."), namespace_xpm)
{
    metaType = PGM_SCHEMA;
}


pgaSchemaFactory schemaFactory;
static pgaCollectionFactory cf(&schemaFactory, __("Schemas"));
