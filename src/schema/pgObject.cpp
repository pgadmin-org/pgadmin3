//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
//
// pgObject.cpp - PostgreSQL object base class
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>
#include <wx/treectrl.h>

// App headers
#include "pgAdmin3.h"
#include "misc.h"
#include "pgObject.h"
#include "pgServer.h"
#include "pgLanguage.h"
#include "frmMain.h"


// Must match the PG_OBJTYPE enumeration in pgObject.h
char *typeNameList[] =
{
    "None",
    "Servers",          "Server",       "Add Server",
    "Databases",        "Databases",    "Add Database",
    "Groups",           "Groups",       "Add Group",
    "Users",            "Users",        "Add User",
    "Languages",        "Language",     "Add Language",
    "Schemas",          "Schema",       "Add Schema",
    "Aggregates",       "Aggregate",
    "Casts",            "Cast",
    "Conversions",      "Conversion",
    "Domains",          "Domain",
    "Functions",        "Function",
    "Trigger Functions","Trigger Function",
    "Operators",        "Operator",
    "Operator Classes", "Operator Class",
    "Sequences",        "Sequence",
    "Tables",           "Table",
    "Types",            "Type",
    "Views",            "View",
    "Checks",           "Check",
    "Columns",          "Column",
    "Foreign Keys",     "Foreign Key",
    "Indexes",          "Index",
    "Rules",            "Rule",
    "Triggers",         "Trigger",
    "Unknown"
};



pgObject::pgObject(int newType, const wxString& newName)
: wxTreeItemData()
{
    wxLogInfo(wxT("Creating a pgObject object"));

    // Set the typename and type
    if (newType >= PG_UNKNOWN)
        newType = PG_UNKNOWN;
    type = newType;
    typeName = typeNameList[type];

    name = newName;
    expandedKids=false;
    needReread=false;
}


void pgObject::ShowTree(frmMain *form, wxTreeCtrl *browser, wxListCtrl *properties, wxListCtrl *statistics, ctlSQLBox *sqlPane)
{
    if (form)
    {
        bool canSql;
        switch (GetType())
        {
            case PG_SERVERS:
            case PG_SERVER:
            case PG_DATABASES:
            case PG_GROUPS:
            case PG_GROUP:
            case PG_USERS:
            case PG_USER:
                canSql=false;
                break;
            default:
                canSql=true;
                break;
        }
        form->SetButtons(TRUE, CanCreate(), CanDrop(), CanEdit(), canSql, CanView(), CanVacuum());
        SetContextInfo(form);
    }

    wxLogInfo(wxT("Displaying properties for ") + GetTypeName() + wxT(" ")+GetIdentifier());
    StartMsg(wxT("Retrieving ") + typeName + wxT(" details"));
    ShowTreeDetail(browser, form, properties, statistics, sqlPane);
    EndMsg();
}


void pgObject::CreateListColumns(wxListCtrl *properties, const wxString &left, const wxString &right)
{
    properties->ClearAll();
    properties->InsertColumn(0, wxT("Property"), wxLIST_FORMAT_LEFT, 150);
    properties->InsertColumn(1, wxT("Value"), wxLIST_FORMAT_LEFT, 700);
}



void pgObject::InsertListItem(wxListCtrl *list, const int pos, const wxString& str1, const wxString& str2)
{
    list->InsertItem(pos, str1, 0);
    if (str2 != wxT(""))
        list->SetItem(pos, 1, str2);
}


wxString pgObject::GetCommentSql()
{
    wxString cmt;
    if (!comment.IsNull())
    {
        cmt = wxT("COMMENT ON ") + typeName + wxT(" ") + GetQuotedFullIdentifier() 
            + wxT(" IS ") + qtString(comment) + wxT("\n");
    }
    return cmt;
}



wxString pgObject::GetGrant(const wxString& _grantOnType, bool noOwner)
{
    wxString grant, str, user, grantOnType;
    if (_grantOnType.IsNull())
        grantOnType=GetTypeName();
    else
        grantOnType = _grantOnType;

    grantOnType.MakeUpper();

    if (!acl.IsNull())
    {
        queryTokenizer acls(acl.Mid(1, acl.Length()-2), ',');
        while (acls.HasMoreTokens())
        {
            str=acls.GetNextToken();
            if (str.Left(1) == '"')
                str = str.Mid(1, str.Length()-2);
            user=str.BeforeFirst('=');
            str=str.Mid(user.Length()+1);
            if (user == wxT(""))
                user=wxT("public");
            else
            {
                if (user.Left(6) == wxT("group "))
                    user = wxT("GROUP ") + qtIdent(user.Mid(6));
                else
                    user = qtIdent(user);
            }
            wxString rights;
            if (str == wxT("arwdRxt"))
                rights = wxT("ALL");
            else
            {
                if (str.Find('a') >= 0)
                {
                    if (!rights.IsNull()) rights.Append(wxT(", "));
                    rights.Append(wxT("INSERT"));
                }
                if (str.Find('r') >= 0) 
                {
                    if (!rights.IsNull()) rights.Append(wxT(", "));
                    rights.Append(wxT("SELECT"));
                }
                if (str.Find('w') >= 0)
                {
                    if (!rights.IsNull()) rights.Append(wxT(", "));
                    rights.Append(wxT("UPDATE"));
                }
                if (str.Find('d') >= 0)
                {
                    if (!rights.IsNull()) rights.Append(wxT(", "));
                    rights.Append(wxT("DELETE"));
                }
                if (str.Find('R') >= 0)
                {
                    if (!rights.IsNull()) rights.Append(wxT(", "));
                    rights.Append(wxT("RULE"));
                }
                if (str.Find('x') >= 0)
                {
                    if (!rights.IsNull()) rights.Append(wxT(", "));
                    rights.Append(wxT("REFERENCES"));
                }
                if (str.Find('t') >= 0)
                {
                    if (!rights.IsNull()) rights.Append(wxT(", "));
                    rights.Append(wxT("TRIGGER"));
                }
                if (str.Find('U') >= 0)
                {
                    if (!rights.IsNull()) rights.Append(wxT(", "));
                    rights.Append(wxT("USAGE"));
                }
            }
            if (rights.IsNull())    grant += wxT("REVOKE ALL");
            else                    grant += wxT("GRANT ") + rights;
            
            grant += wxT(" ON ") + grantOnType + wxT(" ");

            if (noOwner)            grant += GetQuotedIdentifier();
            else                    grant += GetQuotedFullIdentifier();

            if (rights.IsNull())    grant += wxT(" FROM ");
            else                    grant += wxT(" TO "); 
                      
            grant += user + wxT(";\n");
        }
    }
    return grant;
}


pgDatabase *pgObject::GetDatabase()
{
    pgDatabase *db=0;
    switch (GetType())
    {
        case PG_DATABASE:
            db=(pgDatabase*)this;
            break;
        case PG_LANGUAGES:
        case PG_SCHEMAS:
        case PG_AGGREGATES:
        case PG_CASTS:
        case PG_CONVERSIONS:
        case PG_DOMAINS:
        case PG_FUNCTIONS:
        case PG_TRIGGERFUNCTIONS:
        case PG_OPERATORS:
        case PG_OPERATORCLASSES:
        case PG_SEQUENCES:
        case PG_TABLES:
        case PG_TYPES:
        case PG_VIEWS:
        case PG_CHECKS:
        case PG_COLUMNS:
        case PG_FOREIGNKEYS:
        case PG_INDEXES:
        case PG_RULES:
        case PG_TRIGGERS:
            db=((pgCollection*)this)->GetDatabase();
            break;
        case PG_LANGUAGE:
            db=((pgLanguage*)this)->GetDatabase();
            break;
        case PG_SCHEMA:
            db=((pgSchema*)this)->GetDatabase();
            break;
        case PG_AGGREGATE:
        case PG_CAST:
        case PG_CONVERSION:
        case PG_DOMAIN:
        case PG_FUNCTION:
        case PG_TRIGGERFUNCTION:
        case PG_OPERATOR:
        case PG_OPERATORCLASS:
        case PG_SEQUENCE:
        case PG_TABLE:
        case PG_TYPE:
        case PG_VIEW:
        case PG_CHECK:
        case PG_COLUMN:
        case PG_FOREIGNKEY:
        case PG_INDEX:
        case PG_RULE:
        case PG_TRIGGER:
            db=((pgSchemaObject*)this)->GetSchema()->GetDatabase();
            break;
    }
    return db;
}



void pgSchemaObject::SetContextInfo(frmMain *form)
{
    form->SetDatabase(schema->GetDatabase());
}



pgSet *pgSchemaObject::ExecuteSet(const wxString& sql)
{
    return schema->GetDatabase()->ExecuteSet(sql);
}



void pgSchemaObject::DisplayStatistics(wxListCtrl *statistics, const wxString& query)
{
    if (statistics)
    {
        wxLogInfo(wxT("Displaying statistics for %s on %s"), GetTypeName().c_str(), GetSchema()->GetIdentifier().c_str());

        // Add the statistics view columns
        CreateListColumns(statistics, wxT("Statistic"), wxT("Value"));

        pgSet *stats = ExecuteSet(query);
    
        if (stats)
        {
            int col=0;
            while (col < stats->NumCols())
                InsertListItem(statistics, col++, stats->ColName(col), stats->GetVal(col));

            delete stats;
        }
    }
}


wxString pgSchemaObject::GetFullIdentifier() const 
{
    return schema->GetName() + wxT(".")+GetName();
}


wxString pgSchemaObject::GetQuotedFullIdentifier() const
{
    return qtIdent(schema->GetName()) + wxT(".")+qtIdent(GetName());
}
