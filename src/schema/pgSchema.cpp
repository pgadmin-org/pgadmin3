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
: pgObject(PG_SCHEMA, newName)
{
    wxLogInfo(wxT("Creating a pgSchema object"));
}

pgSchema::~pgSchema()
{
    wxLogInfo(wxT("Destroying a pgSchema object"));
}


wxString pgSchema::GetSql(wxTreeCtrl *browser)
{
    if (sql.IsNull())
    {
        sql = wxT("CREATE SCHEMA ") + qtIdent(GetName()) + wxT(" AUTHORIZATION ") + qtIdent(GetOwner()) + wxT(";\n")
            + GetGrant(GetTypeName(), true)
            + GetCommentSql();
    }
    return sql;
}


void pgSchema::ShowTreeDetail(wxTreeCtrl *browser, frmMain *form, wxListCtrl *properties, wxListCtrl *statistics, ctlSQLBox *sqlPane)
{
    form->SetDatabase(GetDatabase());
    form->SetButtons(true, true, true, true, true, false, true);
    GetDatabase()->GetServer()->SetLastSchema(GetName());

    if (!expandedKids)
    {
        expandedKids=true;

        // Log
        wxLogInfo(wxT("Adding child object to schema ") + GetIdentifier());

        pgCollection *collection;

        // Aggregates
        collection = new pgCollection(PG_AGGREGATES, wxString("Aggregates"));
        collection->SetInfo(GetDatabase()->GetServer(), GetDatabase(), this);
        browser->AppendItem(GetId(), collection->GetTypeName(), PGICON_AGGREGATE, -1, collection);

        // Domains
        collection = new pgCollection(PG_DOMAINS, wxString("Domains"));
        collection->SetInfo(GetDatabase()->GetServer(), GetDatabase(), this);
        browser->AppendItem(GetId(), collection->GetTypeName(), PGICON_DOMAIN, -1, collection);

        // Functions
        collection = new pgCollection(PG_FUNCTIONS, wxString("Functions"));
        collection->SetInfo(GetDatabase()->GetServer(), GetDatabase(), this);
        browser->AppendItem(GetId(), collection->GetTypeName(), PGICON_FUNCTION, -1, collection);

        // Operators
        collection = new pgCollection(PG_OPERATORS, wxString("Operators"));
        collection->SetInfo(GetDatabase()->GetServer(), GetDatabase(), this);
        browser->AppendItem(GetId(), collection->GetTypeName(), PGICON_OPERATOR, -1, collection);

        // Sequences
        collection = new pgCollection(PG_SEQUENCES, wxString("Sequences"));
        collection->SetInfo(GetDatabase()->GetServer(), GetDatabase(), this);
        browser->AppendItem(GetId(), collection->GetTypeName(), PGICON_SEQUENCE, -1, collection);

        // Tables
        collection = new pgCollection(PG_TABLES, wxString("Tables"));
        collection->SetInfo(GetDatabase()->GetServer(), GetDatabase(), this);
        browser->AppendItem(GetId(), collection->GetTypeName(), PGICON_TABLE, -1, collection);

        // Types
        collection = new pgCollection(PG_TYPES, wxString("Types"));
        collection->SetInfo(GetDatabase()->GetServer(), GetDatabase(), this);
        browser->AppendItem(GetId(), collection->GetTypeName(), PGICON_TYPE, -1, collection);

        // Views
        collection = new pgCollection(PG_VIEWS, wxString("Views"));
        collection->SetInfo(GetDatabase()->GetServer(), GetDatabase(), this);
        browser->AppendItem(GetId(), collection->GetTypeName(), PGICON_VIEW, -1, collection);
    }


    if (properties)
    {
        wxLogInfo(wxT("Displaying properties for schema ") + GetIdentifier());

        properties->ClearAll();
        properties->InsertColumn(0, wxT("Property"), wxLIST_FORMAT_LEFT, 150);
        properties->InsertColumn(1, wxT("Value"), wxLIST_FORMAT_LEFT, 200);
  

        int pos=0;

        InsertListItem(properties, pos++, wxT("Name"), GetName());
        InsertListItem(properties, pos++, wxT("OID"), NumToStr(GetOid()));
        InsertListItem(properties, pos++, wxT("Owner"), GetOwner());
        InsertListItem(properties, pos++, wxT("ACL"), GetAcl());
        InsertListItem(properties, pos++, wxT("System Object?"), BoolToYesNo(GetSystemObject()));
        InsertListItem(properties, pos++, wxT("Comment"), GetComment());
    }
}



void pgSchema::ShowTreeCollection(pgCollection *collection, frmMain *form, wxTreeCtrl *browser, wxListCtrl *properties, wxListCtrl *statistics, ctlSQLBox *sqlPane)
{
    wxString msg;
    pgSchema *schema;

    if (browser->GetChildrenCount(collection->GetId(), FALSE) == 0)
    {

        // Log
        msg.Printf(wxT("Adding schemas to database %s"), collection->GetDatabase()->GetIdentifier().c_str());
        wxLogInfo(msg);

        extern sysSettings *settings;
        wxString systemRestriction;
        if (!settings->GetShowSystemObjects())
            systemRestriction = "   AND nspname NOT LIKE 'pg\\_temp\\_%%'\n";

        // Get the schemas
        pgSet *schemas= collection->GetDatabase()->ExecuteSet(wxT(
           "SELECT oid, nspname, pg_get_userbyid(nspowner) AS namespaceowner, nspacl\n"
           "  FROM pg_namespace\n"
           " WHERE oid >= 100\n" + systemRestriction +
           " ORDER BY nspname"));

        if (schemas)
        {
            while (!schemas->Eof())
            {

                schema = new pgSchema(schemas->GetVal(wxT("nspname")));
                schema->SetDatabase(collection->GetDatabase());
                schema->iSetOid(StrToDouble(schemas->GetVal(wxT("oid"))));
                schema->iSetOwner(schemas->GetVal(wxT("namespaceowner")));
                schema->iSetAcl(schemas->GetVal(wxT("nspacl")));

                browser->AppendItem(collection->GetId(), schema->GetIdentifier(), PGICON_SCHEMA, -1, schema);
	    
			    schemas->MoveNext();
            }

		    delete schemas;
        }
    }
}

