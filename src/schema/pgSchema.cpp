//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
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

bool pgSchema::DropObject(wxFrame *frame, wxTreeCtrl *browser)
{
    return GetDatabase()->ExecuteVoid(wxT("DROP SCHEMA ") + GetQuotedFullIdentifier());
}

wxString pgSchema::GetSql(wxTreeCtrl *browser)
{
    if (sql.IsNull())
    {
        sql = wxT("-- Schema: \"") + GetName() + wxT("\"\n")
            + wxT("CREATE SCHEMA ") + qtIdent(GetName()) 
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
        collection = new pgCollection(PG_AGGREGATES);
        collection->SetInfo(GetDatabase()->GetServer(), GetDatabase(), this);
        AppendBrowserItem(browser, collection);

        // Conversions
        collection = new pgCollection(PG_CONVERSIONS);
        collection->SetInfo(GetDatabase()->GetServer(), GetDatabase(), this);
        AppendBrowserItem(browser, collection);

        // Domains
        collection = new pgCollection(PG_DOMAINS);
        collection->SetInfo(GetDatabase()->GetServer(), GetDatabase(), this);
        AppendBrowserItem(browser, collection);

        // Functions
        collection = new pgCollection(PG_FUNCTIONS);
        collection->SetInfo(GetDatabase()->GetServer(), GetDatabase(), this);
        AppendBrowserItem(browser, collection);

        collection = new pgCollection(PG_TRIGGERFUNCTIONS);
        collection->SetInfo(GetDatabase()->GetServer(), GetDatabase(), this);
        AppendBrowserItem(browser, collection);

        // Operators
        collection = new pgCollection(PG_OPERATORS);
        collection->SetInfo(GetDatabase()->GetServer(), GetDatabase(), this);
        AppendBrowserItem(browser, collection);

        // Operator Classes
        collection = new pgCollection(PG_OPERATORCLASSES);
        collection->SetInfo(GetDatabase()->GetServer(), GetDatabase(), this);
        AppendBrowserItem(browser, collection);

        // Sequences
        collection = new pgCollection(PG_SEQUENCES);
        collection->SetInfo(GetDatabase()->GetServer(), GetDatabase(), this);
        AppendBrowserItem(browser, collection);

        // Tables
        collection = new pgCollection(PG_TABLES);
        collection->SetInfo(GetDatabase()->GetServer(), GetDatabase(), this);
        AppendBrowserItem(browser, collection);

        // Types
        collection = new pgCollection(PG_TYPES);
        collection->SetInfo(GetDatabase()->GetServer(), GetDatabase(), this);
        AppendBrowserItem(browser, collection);

        // Views
        collection = new pgCollection(PG_VIEWS);
        collection->SetInfo(GetDatabase()->GetServer(), GetDatabase(), this);
        AppendBrowserItem(browser, collection);
    }


    if (properties)
    {
        wxLogInfo(wxT("Displaying properties for schema ") + GetIdentifier());

        CreateListColumns(properties);
        int pos=0;

        InsertListItem(properties, pos++, wxT("Name"), GetName());
        InsertListItem(properties, pos++, wxT("OID"), GetOid());
        InsertListItem(properties, pos++, wxT("Owner"), GetOwner());
        InsertListItem(properties, pos++, wxT("ACL"), GetAcl());
        InsertListItem(properties, pos++, wxT("System Object?"), GetSystemObject());
        InsertListItem(properties, pos++, wxT("Comment"), GetComment());
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

    pgSet *schemas= collection->GetDatabase()->ExecuteSet(wxT(
       "SELECT CASE WHEN nsp.oid<100 THEN 0 WHEN nspname LIKE 'pg\\_temp\\_%%' THEN 1 ELSE 2 END AS nsptyp,\n"
       "       nsp.nspname, nsp.oid, pg_get_userbyid(nspowner) AS namespaceowner, nspacl, description\n"
       "  FROM pg_namespace nsp\n"
       "  LEFT OUTER JOIN pg_description des ON des.objoid=nsp.oid\n"
        + restriction +
       " ORDER BY 1, nspname"));

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

            if (browser)
            {
                browser->AppendItem(collection->GetId(), schema->GetIdentifier(), PGICON_SCHEMA, -1, schema);
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
        systemRestriction = wxT(
            " WHERE nsp.oid >= 100\n"
            "   AND nsp.nspname NOT LIKE 'pg\\_temp\\_%'\n");

    // Get the schemas
    return ReadObjects(collection, browser, systemRestriction);
}

