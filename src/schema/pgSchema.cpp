//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002 - 2003, The pgAdmin Development Team
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


pgSchema::pgSchema(const wxString& newName)
: pgDatabaseObject(PG_SCHEMA, newName)
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
        AppendMenu(menu, PG_AGGREGATE);
        AppendMenu(menu, PG_CONVERSION);
        AppendMenu(menu, PG_DOMAIN);
        AppendMenu(menu, PG_FUNCTION);
        AppendMenu(menu, PG_TRIGGERFUNCTION);
        AppendMenu(menu, PG_OPERATOR);
//        AppendMenu(menu, PG_OPERATORCLASS);
        AppendMenu(menu, PG_SEQUENCE);
        AppendMenu(menu, PG_TABLE);
        AppendMenu(menu, PG_TYPE);
        AppendMenu(menu, PG_VIEW);
    }
    return menu;
}


bool pgSchema::DropObject(wxFrame *frame, wxTreeCtrl *browser)
{
    return GetDatabase()->ExecuteVoid(wxT("DROP SCHEMA ") + GetQuotedFullIdentifier() + wxT(";"));
}

wxString pgSchema::GetSql(wxTreeCtrl *browser)
{
    if (sql.IsNull())
    {
        sql = wxT("-- Schema: \"") + GetName() + wxT("\"\n\n")
            + wxT("-- DROP SCHEMA ") + GetQuotedFullIdentifier() + wxT(";")
            + wxT("\n\nCREATE SCHEMA ") + qtIdent(GetName()) 
            + wxT("\n  AUTHORIZATION ") + qtIdent(GetOwner()) + wxT(";\n")
            + GetGrant(wxT("UC"), GetTypeName(), true)
            + GetCommentSql();
    }
    return sql;
}


void pgSchema::ShowTreeDetail(wxTreeCtrl *browser, frmMain *form, wxListCtrl *properties, wxListCtrl *statistics, ctlSQLBox *sqlPane)
{
    if (form)
        form->SetDatabase(GetDatabase());

    GetDatabase()->GetServer()->iSetLastDatabase(GetDatabase()->GetName());
    GetDatabase()->GetServer()->iSetLastSchema(GetName());

    if (!expandedKids)
    {
        expandedKids=true;

        // Log
        wxLogInfo(wxT("Adding child object to schema ") + GetIdentifier());

        pgCollection *collection;

        // Aggregates
        collection = new pgCollection(PG_AGGREGATES, this);
        AppendBrowserItem(browser, collection);

        // Conversions
        collection = new pgCollection(PG_CONVERSIONS, this);
        AppendBrowserItem(browser, collection);

        // Domains
        collection = new pgCollection(PG_DOMAINS, this);
        AppendBrowserItem(browser, collection);

        // Functions
        collection = new pgCollection(PG_FUNCTIONS, this);
        AppendBrowserItem(browser, collection);

        collection = new pgCollection(PG_TRIGGERFUNCTIONS, this);
        AppendBrowserItem(browser, collection);

        // Operators
        collection = new pgCollection(PG_OPERATORS, this);
        AppendBrowserItem(browser, collection);

        // Operator Classes
        collection = new pgCollection(PG_OPERATORCLASSES, this);
        AppendBrowserItem(browser, collection);

        // Sequences
        collection = new pgCollection(PG_SEQUENCES, this);
        AppendBrowserItem(browser, collection);

        // Tables
        collection = new pgCollection(PG_TABLES, this);
        AppendBrowserItem(browser, collection);

        // Types
        collection = new pgCollection(PG_TYPES, this);
        AppendBrowserItem(browser, collection);

        // Views
        collection = new pgCollection(PG_VIEWS, this);
        AppendBrowserItem(browser, collection);
    }


    if (properties)
    {
        wxLogInfo(wxT("Displaying properties for schema ") + GetIdentifier());

        CreateListColumns(properties);
        int pos=0;

        InsertListItem(properties, pos++, _("Name"), GetName());
        InsertListItem(properties, pos++, _("OID"), GetOid());
        InsertListItem(properties, pos++, _("Owner"), GetOwner());
        InsertListItem(properties, pos++, _("ACL"), GetAcl());
        InsertListItem(properties, pos++, _("System schema?"), GetSystemObject());
        InsertListItem(properties, pos++, _("Comment"), GetComment());
    }
}



pgObject *pgSchema::Refresh(wxTreeCtrl *browser, const wxTreeItemId item)
{
    pgObject *schema=0;
    wxTreeItemId parentItem=browser->GetItemParent(item);
    if (parentItem)
    {
        pgObject *obj=(pgObject*)browser->GetItemData(parentItem);
        if (obj->GetType() == PG_SCHEMAS)
            schema = ReadObjects((pgCollection*)obj, 0, wxT(" WHERE nsp.oid=") + GetOidStr() + wxT("\n"));
    }
    return schema;
}



pgObject *pgSchema::ReadObjects(pgCollection *collection, wxTreeCtrl *browser, const wxString &restriction)
{
    pgSchema *schema=0;

    pgSet *schemas= collection->GetDatabase()->ExecuteSet(
       wxT("SELECT CASE WHEN nsp.oid<") + NumToStr(collection->GetServer()->GetLastSystemOID()) 
                + wxT(" THEN 0 WHEN nspname LIKE 'pg\\_temp\\_%%' THEN 1 ELSE 2 END AS nsptyp,\n")
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

            schema = new pgSchema(schemas->GetVal(wxT("nspname")));
            schema->iSetDatabase(collection->GetDatabase());
            schema->iSetComment(schemas->GetVal(wxT("description")));
            schema->iSetOid(schemas->GetOid(wxT("oid")));
            schema->iSetOwner(schemas->GetVal(wxT("namespaceowner")));
            schema->iSetAcl(schemas->GetVal(wxT("nspacl")));
            schema->iSetSchemaTyp(schemas->GetLong(wxT("nsptyp")));
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
        systemRestriction = 
            wxT(" WHERE (nsp.oid = 2200 OR nsp.oid > ") + NumToStr(collection->GetConnection()->GetLastSystemOID()) + wxT(")\n")
            wxT("   AND nsp.nspname NOT LIKE 'pg\\_temp\\_%'\n");

    // Get the schemas
    return ReadObjects(collection, browser, systemRestriction);
}

