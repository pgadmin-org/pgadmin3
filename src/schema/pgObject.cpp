//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2003, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// pgObject.cpp - PostgreSQL object base class
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "misc.h"
#include "menu.h"
#include "pgObject.h"
#include "pgServer.h"
#include "frmMain.h"


// Ordering must match the PG_OBJTYPE enumeration in pgObject.h


pgTypes typesList[] =
{
    {__("None"), -1, 0, 0},
    {__("Servers"), PGICON_SERVER, 0, 0},
    {__("Server"), PGICON_SERVER, __("New Server"), __("Create a new Server connection.") },
    {__("Databases"), PGICON_DATABASE, 0, 0},
    {__("Database"), PGICON_DATABASE, __("New Database"), __("Create a new Database.") },
    {__("Groups"), PGICON_GROUP, 0, 0},
    {__("Group"), PGICON_GROUP, __("New Group"), __("Create a new Group.") },
    {__("Users"), PGICON_USER, 0, 0},
    {__("User"), PGICON_USER, __("New User"),__("Create a new User.") },
    {__("Languages"), PGICON_LANGUAGE, 0, 0},
    {__("Language"), PGICON_LANGUAGE, __("New Language"), __("Create a new Language.") },
    {__("Schemas"), PGICON_SCHEMA, 0, 0},
    {__("Schema"), PGICON_SCHEMA, __("New Schema"), __("Create a new Schema.") },
    {__("Aggregates"), PGICON_AGGREGATE, 0, 0},
    {__("Aggregate"), PGICON_AGGREGATE, __("New Aggregate"), __("Create a new Aggregate.") },
    {__("Casts"), PGICON_CAST, 0, 0},
    {__("Cast"), PGICON_CAST, __("New Cast"), __("Create a new Cast.") },
    {__("Conversions"), PGICON_CONVERSION, 0, 0},
    {__("Conversion"), PGICON_CONVERSION, __("New Conversion"), __("Create a new Conversion.") },
    {__("Domains"), PGICON_DOMAIN, 0, 0},
    {__("Domain"), PGICON_DOMAIN, __("New Domain"), __("Create a new Domain.") },
    {__("Functions"), PGICON_FUNCTION, 0, 0},
    {__("Function"), PGICON_FUNCTION, __("New Function"), __("Create a new Function.") },
    {__("Trigger Functions"), PGICON_TRIGGERFUNCTION, 0, 0},
    {__("Trigger Function"), PGICON_TRIGGERFUNCTION, __("New Trigger Function"), __("Create a new Trigger Function.") },
    {__("Operators"), PGICON_OPERATOR, 0, 0},
    {__("Operator"), PGICON_OPERATOR, __("New Operator"), __("Create a new Operator.") },
    {__("Operator Classes"), PGICON_OPERATORCLASS, 0, 0},
    {__("Operator Class"), PGICON_OPERATORCLASS, __("New Operator Class"), __("Create a new Operator Class.") },
    {__("Sequences"), PGICON_SEQUENCE, 0, 0},
    {__("Sequence"), PGICON_SEQUENCE, __("New Sequence"), __("Create a new Sequence.") },
    {__("Tables"), PGICON_TABLE, 0, 0},
    {__("Table"), PGICON_TABLE, __("New Table"), __("Create a new Table.") },
    {__("Types"), PGICON_TYPE, 0, 0},
    {__("Type"), PGICON_TYPE, __("New Type"), __("Create a new Type.") },
    {__("Views"), PGICON_VIEW, 0, 0},
    {__("View"), PGICON_VIEW, __("New View"), __("Create a new View.") },
    {__("Columns"), PGICON_COLUMN, 0, 0},
    {__("Column"), PGICON_COLUMN, __("New Column"), __("Add a new Column.") },
    {__("Indexes"), PGICON_INDEX, 0, 0},
    {__("Index"), PGICON_INDEX, __("New Index"), __("Add a new Index.") },
    {__("Rules"), PGICON_RULE, 0, 0},
    {__("Rule"), PGICON_RULE, __("New Rule"), __("Create a new Rule.") },
    {__("Triggers"), PGICON_TRIGGER, 0, 0},
    {__("Trigger"), PGICON_TRIGGER, __("New Trigger"), __("Add a new Trigger.") },
    {__("Constraints"), PGICON_CONSTRAINT, 0, 0},      
    {__("Primary Key"), PGICON_PRIMARYKEY, __("New Primary Key"), __("Create a Primary Key.") },
    {__("Unique"), PGICON_UNIQUE, __("New Unique Constraint"), __("Add a new Unique Constraint.") },
    {__("Check"), PGICON_CHECK, __("New Check Constraint"), __("Add a new Check Constraint.") },
    {__("Foreign Key"), PGICON_FOREIGNKEY, __("New Foreign Key"), __("Add a new Foreign Key.") },
    {__("Agent"), PGAICON_AGENT, 0, 0},
    {__("Job"), PGAICON_JOB, __("New Job"), __("Create a new Job") },
    {__("Step"), PGAICON_STEP, __("New Step"), __("Create new Step") }, 
    {__("Schedule"), PGAICON_SCHEDULE, __("New Schedule"), __("Create new Schedule") },
    {__("Unknown"), -1, 0, 0}
};


pgObject::pgObject(int newType, const wxString& newName)
: wxTreeItemData()
{

    // Set the typename and type
    if (newType >= PG_UNKNOWN)
        newType = PG_UNKNOWN;
    type = newType;

    name = newName;
    expandedKids=false;
    needReread=false;
}


void pgObject::AppendMenu(wxMenu *menu, int type)
{
    if (menu)
    {
        if (type < 0)
        {
            type=GetType();
            if (IsCollection())
                type++;
        }
        menu->Append(MNU_NEW+type, 
            wxGetTranslation(typesList[type].newString),
            wxGetTranslation(typesList[type].newLongString));
    }
}


wxString pgObject::GetHelpPage(bool forCreate) const
{
    wxString page;

    if (!IsCollection())
        page=wxT("sql-create") + GetTypeName().Lower();

    return page;
}


wxMenu *pgObject::GetNewMenu()
{
    wxMenu *menu=new wxMenu();
    if (CanCreate())
        AppendMenu(menu);
    return menu;
}


void pgObject::ShowStatistics(ctlListView *statistics)
{
    statistics->ClearAll();
    statistics->AddColumn(_("Statistics"), 500);
    statistics->InsertItem(0, _("No statistics are available for the current selection"), PGICON_STATISTICS);
}


void pgObject::ShowDependency(ctlListView *list, const wxString &query)
{
    list->AddColumn(_("Type"), 60);
    list->AddColumn(_("Name"), 100);
    list->AddColumn(_("Restriction"), 50);
    pgConn *conn = GetConnection();
    if (conn)
    {
        pgSet *set;
        set=conn->ExecuteSet(query + wxT("\n")
            wxT("   AND classid IN (\n")
            wxT("   SELECT oid FROM pg_class\n")
            wxT("    WHERE relname IN ('pg_class', 'pg_constraint', 'pg_conversion', 'pg_language', 'pg_proc',\n")
            wxT("                      'pg_rewrite', 'pg_trigger', 'pg_type'))\n")
            wxT(" ORDER BY classid, cl.relkind"));

        if (set)
        {
            long pos=0;

            while (!set->Eof())
            {
                wxString nspname=set->GetVal(wxT("nspname"));
                wxString refname;
                if (!nspname.IsEmpty() && nspname != wxT("public") && nspname != wxT("pg_catalog"))
                    refname = nspname + wxT(".");

                wxString typestr=set->GetVal(wxT("type"));
                int id;

                switch (typestr.c_str()[0])
                {
                    case 's':   // we don't know these; internally handled
                    case 't':   set->MoveNext(); continue;

                    case 'r':   id=PG_TABLE;    break;
                    case 'i':   id=PG_INDEX;    break;
                    case 'S':   id=PG_SEQUENCE; break;
                    case 'v':   id=PG_VIEW;     break;
                    case 'p':   id=PG_FUNCTION; break;
                    case 'n':   id=PG_SCHEMA;   break;
                    case 'y':   id=PG_TYPE;     break;
                    case 'T':   id=PG_TRIGGER;  break;
                    case 'l':   id=PG_LANGUAGE; break;
                    case 'c':
                    {
                        switch (typestr.c_str()[1])
                        {
                            case 'c':   id=PG_CHECK;        break;
                            case 'f':   
                                refname += set->GetVal(wxT("ownertable")) + wxT(".");
                                id=PG_FOREIGNKEY;   break;
                            case 'p':   id=PG_PRIMARYKEY;   break;
                            case 'u':   id=PG_UNIQUE;       break;
                            default:    id=PG_UNKNOWN;      break;
                        }
                        break;
                    }
                    default:    id=PG_UNKNOWN;  break;
                }

                refname += set->GetVal(wxT("refname"));

                wxString typname = typesList[id].typName;
                int icon = typesList[id].typeIcon;

                wxString deptype;

                switch (set->GetVal(wxT("deptype")).c_str()[0])
                {
                    case 'n':   deptype=wxT("normal");      break;
                    case 'a':   deptype=wxT("auto");        break;
                    case 'i': 
                    {
                        if (settings->GetShowSystemObjects())
                            deptype=wxT("internal");
                        else
                        {
                            set->MoveNext();
                            continue;
                        }
                        break;
                    }
                    case 'p':   deptype=wxT("pin");  typname=wxEmptyString;        break;
                    default:                                break;
                }

                list->AppendItem(icon, typname, refname, deptype);
                set->MoveNext();
            }
            delete set;
        }
    }
}


void pgObject::CreateListColumns(ctlListView *list, const wxString &left, const wxString &right)
{
    list->ClearAll();
    list->AddColumn(left, 90);
    list->AddColumn(right, 450);
}


void pgObject::ShowDependsOn(ctlListView *dependsOn)
{
    ShowDependency(dependsOn,
        wxT("SELECT CASE WHEN cl.relkind IS NOT NULL THEN cl.relkind\n")
        wxT("            WHEN tg.oid IS NOT NULL THEN 'T'::text\n")
        wxT("            WHEN ty.oid IS NOT NULL THEN 'y'::text\n")
        wxT("            WHEN ns.oid IS NOT NULL THEN 'n'::text\n")
        wxT("            WHEN pr.oid IS NOT NULL THEN 'p'::text\n")
        wxT("            WHEN la.oid IS NOT NULL THEN 'l'::text\n")
        wxT("            WHEN co.oid IS NOT NULL THEN 'c'::text || contype\n")
        wxT("            ELSE '' END AS type,\n")
        wxT("       COALESCE(cl.relname, conname, proname, tgname, typname, lanname, ns.nspname) AS refname,\n")
        wxT("       COALESCE(nsc.nspname, nso.nspname, nsp.nspname, nst.nspname) AS nspname,\n")
        wxT("       deptype, coc.relname AS ownertable\n") 
        wxT("  FROM pg_depend dep\n")
        wxT("  LEFT JOIN pg_class cl ON dep.refobjid=cl.oid\n")
        wxT("  LEFT JOIN pg_namespace nsc ON cl.relnamespace=nsc.oid\n")
        wxT("  LEFT JOIN pg_proc pr on dep.refobjid=pr.oid\n")
        wxT("  LEFT JOIN pg_namespace nsp ON pronamespace=nsp.oid\n")
        wxT("  LEFT JOIN pg_trigger tg ON dep.refobjid=tg.oid\n")
        wxT("  LEFT JOIN pg_type ty on dep.refobjid=ty.oid\n")
        wxT("  LEFT JOIN pg_namespace nst ON typnamespace=nst.oid\n")
        wxT("  LEFT JOIN pg_constraint co ON dep.refobjid=co.oid\n")
        wxT("  LEFT JOIN pg_class coc ON conrelid=coc.oid\n")
        wxT("  LEFT JOIN pg_namespace nso ON connamespace=nso.oid\n")
        wxT("  LEFT JOIN pg_language la ON dep.refobjid=la.oid\n")
        wxT("  LEFT JOIN pg_namespace ns ON dep.refobjid=ns.oid\n")
        wxT(" WHERE dep.objid=") + GetOidStr());
}


void pgObject::ShowReferencedBy(ctlListView *referencedBy)
{
    ShowDependency(referencedBy,
        wxT("SELECT CASE WHEN cl.relkind IS NOT NULL THEN cl.relkind\n")
        wxT("            WHEN tg.oid IS NOT NULL THEN 'T'::text\n")
        wxT("            WHEN ty.oid IS NOT NULL THEN 'y'::text\n")
        wxT("            WHEN ns.oid IS NOT NULL THEN 'n'::text\n")
        wxT("            WHEN pr.oid IS NOT NULL THEN 'p'::text\n")
        wxT("            WHEN la.oid IS NOT NULL THEN 'l'::text\n")
        wxT("            WHEN co.oid IS NOT NULL THEN 'c'::text || contype\n")
        wxT("            ELSE '' END AS type,\n")
        wxT("       COALESCE(cl.relname, conname, proname, tgname, typname, lanname, ns.nspname) AS refname,\n")
        wxT("       COALESCE(nsc.nspname, nso.nspname, nsp.nspname, nst.nspname) AS nspname,\n")
        wxT("       deptype, coc.relname AS ownertable\n") 
        wxT("  FROM pg_depend dep\n")
        wxT("  LEFT JOIN pg_class cl ON dep.objid=cl.oid\n")
        wxT("  LEFT JOIN pg_namespace nsc ON cl.relnamespace=nsc.oid\n")
        wxT("  LEFT JOIN pg_proc pr on dep.objid=pr.oid\n")
        wxT("  LEFT JOIN pg_namespace nsp ON pronamespace=nsp.oid\n")
        wxT("  LEFT JOIN pg_trigger tg ON dep.objid=tg.oid\n")
        wxT("  LEFT JOIN pg_type ty on dep.objid=ty.oid\n")
        wxT("  LEFT JOIN pg_namespace nst ON typnamespace=nst.oid\n")
        wxT("  LEFT JOIN pg_constraint co on dep.objid=co.oid\n")
        wxT("  LEFT JOIN pg_class coc ON conrelid=coc.oid\n")
        wxT("  LEFT JOIN pg_namespace nso ON connamespace=nso.oid\n")
        wxT("  LEFT JOIN pg_language la ON dep.refobjid=la.oid\n")
        wxT("  LEFT JOIN pg_namespace ns ON dep.objid=ns.oid\n")
        wxT(" WHERE dep.refobjid=") + GetOidStr());
}


void pgObject::ShowTree(frmMain *form, wxTreeCtrl *browser, ctlListView *properties, ctlSQLBox *sqlPane)
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
        form->SetButtons(TRUE, CanCreate(), CanDrop(), CanEdit(), canSql, CanView(), CanMaintenance());
        SetContextInfo(form);

        ctlListView *statistics=form->GetStatistics();
        if (statistics)
            ShowStatistics(statistics);

        ctlListView *dependsOn=form->GetDependsOn();
        if (dependsOn)
        {
            dependsOn->ClearAll();
            if (!IsCollection())
                ShowDependsOn(dependsOn);
        }
        ctlListView *referencedBy=form->GetReferencedBy();
        if (referencedBy)
        {
            referencedBy->ClearAll();
            if (!IsCollection())
                ShowReferencedBy(referencedBy);
        }
    }

    wxLogInfo(wxT("Displaying properties for ") + GetTypeName() + wxT(" ")+GetIdentifier());
    StartMsg(wxString::Format(_("Retrieving %s details"), wxGetTranslation(GetTypeName())));
    ShowTreeDetail(browser, form, properties, sqlPane);
    EndMsg();
}


void pgObject::RemoveDummyChild(wxTreeCtrl *browser)
{
    wxCookieType cookie;
    wxTreeItemId childItem=browser->GetFirstChild(GetId(), cookie);
    if (childItem && !browser->GetItemData(childItem))
    {
        // The child was a dummy item, which will be replaced by the following ShowTreeDetail by true items
        browser->Delete(childItem);
    }
}

void pgObject::AppendBrowserItem(wxTreeCtrl *browser, pgObject *object)
{
    wxString label;
    if (object->IsCollection())
        label = object->GetTypeName();
    else
        label = object->GetFullName();
    browser->AppendItem(GetId(), label, object->GetIcon(), -1, object);
    if (object->IsCollection())
        object->ShowTreeDetail(browser);
    else if (object->WantDummyChild())
        browser->AppendItem(object->GetId(), wxT("Dummy"));
}


wxString pgObject::GetCommentSql()
{
    wxString cmt;
    if (!comment.IsNull())
    {
        cmt = wxT("COMMENT ON ") + GetTypeName().Upper() + wxT(" ") + GetQuotedFullIdentifier() 
            + wxT(" IS ") + qtString(comment) + wxT(";\n");
    }
    return cmt;
}



wxString pgObject::GetPrivilegeGrant(const wxString& allPattern, const wxString& str, const wxString& grantOnObject, const wxString& user)
{
    wxString rights;

    if (str == allPattern)
        rights = wxT("ALL");
    else
    {
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
        if (str.Find('a') >= 0)
        {
            if (!rights.IsNull()) rights.Append(wxT(", "));
            rights.Append(wxT("INSERT"));
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
        if (str.Find('X') >= 0)
        {
            if (!rights.IsNull()) rights.Append(wxT(", "));
            rights.Append(wxT("EXECUTE"));
        }
        if (str.Find('U') >= 0)
        {
            if (!rights.IsNull()) rights.Append(wxT(", "));
            rights.Append(wxT("USAGE"));
        }
        if (str.Find('C') >= 0)
        {
            if (!rights.IsNull()) rights.Append(wxT(", "));
            rights.Append(wxT("CREATE"));
        }
        if (str.Find('T') >= 0)
        {
            if (!rights.IsNull()) rights.Append(wxT(", "));
            rights.Append(wxT("TEMPORARY"));
        }
    }
    wxString grant;
    if (rights.IsNull())    grant += wxT("REVOKE ALL");
    else                    grant += wxT("GRANT ") + rights;
    
    grant += wxT(" ON ") + grantOnObject;

    if (rights.IsNull())    grant += wxT(" FROM ");
    else                    grant += wxT(" TO "); 
              
    grant += user;

    return grant;
}


wxString pgObject::GetPrivileges(const wxString& allPattern, const wxString& str, const wxString& grantOnObject, const wxString& user)
{
    wxString aclWithGrant, aclWithoutGrant;

    const wxChar *p=str.c_str();
    while (*p)
    {
        if (p[1] == (wxChar)'*')
        {
            aclWithGrant += *p;
            p += 2;
        }
        else
        {
            aclWithoutGrant += *p;
            p++;
        }
    }

    wxString grant;
    if (!aclWithoutGrant.IsEmpty() || aclWithGrant.IsEmpty())
        grant += GetPrivilegeGrant(allPattern, aclWithoutGrant, grantOnObject, user) + wxT(";\n");
    if (!aclWithGrant.IsEmpty())
        grant += GetPrivilegeGrant(allPattern, aclWithGrant, grantOnObject, user) + wxT(" WITH GRANT OPTION;\n");

    return grant;
}


wxString pgObject::GetGrant(const wxString& allPattern, const wxString& _grantOnType, bool noOwner)
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
            str=str.AfterFirst('=').BeforeFirst('/');
            if (user == wxT(""))
                user=wxT("public");
            else
            {
                if (user.Left(6) == wxT("group "))
                    user = wxT("GROUP ") + qtIdent(user.Mid(6));
                else
                    user = qtIdent(user);
            }

            grant += GetPrivileges(allPattern, str, grantOnType + wxT(" ") + GetQuotedFullIdentifier(), user);
        }
    }
    return grant;
}



pgConn *pgObject::GetConnection() const
{
    pgConn *conn=0;
    pgDatabase *db;
    switch (type)
    {
        case PG_DATABASES:
        case PG_USER:
        case PG_USERS:
        case PG_GROUP:
        case PG_GROUPS:
            conn=((pgServerObject*)this)->GetServer()->connection();
            break;
        default:
            db=GetDatabase();
            if (db)
                conn=db->connection();
            break;
    }
    return conn;    
}

pgDatabase *pgObject::GetDatabase() const
{
    pgDatabase *db=0;
    switch (GetType())
    {
        case PG_USERS:
        case PG_GROUPS:
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
        case PG_COLUMNS:
        case PG_INDEXES:
        case PG_RULES:
        case PG_TRIGGERS:
        case PG_CONSTRAINTS:
            db=((pgCollection*)this)->GetDatabase();
            break;
        case PG_DATABASE:
            db=(pgDatabase*)this;
            break;
        case PG_LANGUAGE:
        case PG_SCHEMA:
        case PG_CAST:
        case PGA_AGENT:
        case PGA_JOB:
        case PGA_STEP:
        case PGA_SCHEDULE:
            db=((pgDatabaseObject*)this)->GetDatabase();
            break;
        case PG_AGGREGATE:
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
        case PG_UNIQUE:
        case PG_PRIMARYKEY:
        case PG_FOREIGNKEY:
        case PG_INDEX:
        case PG_RULE:
        case PG_TRIGGER:
            db=((pgSchemaObject*)this)->GetSchema()->GetDatabase();
            break;
    }
    return db;
}


//////////////////////////////////////////////////////////////

bool pgServerObject::CanDrop()
{
    if (GetType() == PG_DATABASE)
        return server->GetCreatePrivilege();
    else
        return server->GetSuperUser();
}


bool pgServerObject::CanCreate()
{
    if (GetType() == PG_DATABASE)
        return server->GetCreatePrivilege();
    else
        return server->GetSuperUser();
}


//////////////////////////////////////////////////////////////

bool pgDatabaseObject::CanDrop()
{
    return database->GetCreatePrivilege();
}


bool pgDatabaseObject::CanCreate()
{
    return database->GetCreatePrivilege();
}

///////////////////////////////////////////////////////////////

bool pgSchemaObject::GetSystemObject() const
{
    if (!schema)
        return false;
    return GetOid() < GetConnection()->GetLastSystemOID();
}


bool pgSchemaObject::CanDrop()
{
    return schema->GetCreatePrivilege();
}


bool pgSchemaObject::CanCreate()
{
    return schema->GetCreatePrivilege();
}


void pgSchemaObject::SetContextInfo(frmMain *form)
{
    form->SetDatabase(schema->GetDatabase());
}

pgSet *pgSchemaObject::ExecuteSet(const wxString& sql)
{
    return schema->GetDatabase()->ExecuteSet(sql);
}

wxString pgSchemaObject::ExecuteScalar(const wxString& sql)
{
    return schema->GetDatabase()->ExecuteScalar(sql);
}

bool pgSchemaObject::ExecuteVoid(const wxString& sql)
{
    return schema->GetDatabase()->ExecuteVoid(sql);
}

void pgSchemaObject::DisplayStatistics(ctlListView *statistics, const wxString& query)
{
    if (statistics)
    {
        wxLogInfo(wxT("Displaying statistics for %s on %s"), GetTypeName().c_str(), GetSchema()->GetIdentifier().c_str());

        // Add the statistics view columns
        CreateListColumns(statistics, _("Statistic"), _("Value"));

        pgSet *stats = ExecuteSet(query);
    
        if (stats)
        {
            int col=0;
            while (col++ < stats->NumCols())
                statistics->AppendItem(stats->ColName(col), stats->GetVal(col));

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
    return qtIdent(schema->GetName()) + wxT(".") + GetQuotedIdentifier();
}




enum tokentype
{
    SQLTK_NORMAL=0,
    SQLTK_JOINMOD,
    SQLTK_JOIN,
    SQLTK_ON,
    SQLTK_UNION

};

typedef struct __tokenaction
{
    wxChar *keyword, *replaceKeyword;
    int actionBefore, actionAfter;
    tokentype special;
    bool doBreak;
} tokenAction;


tokenAction sqlTokens[] =
{
    { wxT("WHERE")},     // initializing fails, so we're doing it in the code
    { wxT("SELECT"), wxT(" SELECT"),   0, 8,      SQLTK_NORMAL,   true},
    { wxT("FROM"),   wxT("   FROM"),  -8, 8,      SQLTK_NORMAL,   true},
    { wxT("LEFT"),   wxT("   LEFT"),  -8, 13,     SQLTK_JOINMOD,  true},
    { wxT("RIGHT"),  wxT("   RIGHT"), -8, 13,     SQLTK_JOINMOD,  true},
    { wxT("NATURAL"),wxT("   NATURAL"), -8, 13,   SQLTK_JOINMOD,  true},
    { wxT("FULL"),   wxT("   FULL"),  -8, 13,     SQLTK_JOINMOD,  true},
    { wxT("CROSS"),  wxT("   CROSS"), -8, 13,     SQLTK_JOINMOD,  true},
    { wxT("UNION"),  wxT("   UNION"), -8, 13,     SQLTK_UNION,    true},
    { wxT("JOIN"),   wxT("   JOIN"),  -8, 13,     SQLTK_JOIN,     true},
    { wxT("ON"),     wxT("ON"),        0, -5,     SQLTK_ON,       false},
    { wxT("ORDER"),  wxT("  ORDER"),  -8, 8,      SQLTK_NORMAL,   true},
    { wxT("GROUP"),  wxT("  GROUP"),  -8, 8,      SQLTK_NORMAL,   true},
    { wxT("HAVING"), wxT(" HAVING"),  -8, 8,      SQLTK_NORMAL,   true},
    { wxT("LIMIT"),  wxT("  LIMIT"),  -8, 8,      SQLTK_NORMAL,   true},
    { wxT("CASE"),   wxT("CASE"),      0, 4,      SQLTK_NORMAL,   true},
    { wxT("WHEN"),   wxT("WHEN"),      0, 0,      SQLTK_NORMAL,   true},
    { wxT("ELSE"),   wxT("ELSE"),      0, 0,      SQLTK_NORMAL,   true},
    { wxT("END"),    wxT("END "),     -4, 0,      SQLTK_NORMAL,   true},
    {0, 0, 0, 0, SQLTK_NORMAL, false}
};

tokenAction secondOnToken= 
    { wxT("ON"),     wxT("ON"),       -5, 0,      SQLTK_ON,       true};



wxString pgRuleObject::GetFormattedDefinition()
{
    // pgsql 7.4 does formatting itself
    if (!GetDatabase()->GetPrettyOption().IsEmpty())
        return GetDefinition();

    ////////////////////////////////
    // ok, this code looks weird. It's necessary, because somebody (NOT the running code)
    // will screw up that entry. It's broken in pgAdmin3::OnInit() already.
    // maybe your compiler does better (VC6SP5, but an older c2xx to avoid other bugs)

    sqlTokens[0].replaceKeyword=wxT("  WHERE");
    sqlTokens[0].actionBefore = -8;
    sqlTokens[0].actionAfter = 8;
    sqlTokens[0].special = SQLTK_NORMAL;
    sqlTokens[0].doBreak = true;

    wxString fc, token;
    queryTokenizer tokenizer(GetDefinition());
    int indent=0;
    int position=0;  // col position. updated, but not used at the moment.
    bool wasOn=false;

    while (tokenizer.HasMoreTokens())
    {
        token=tokenizer.GetNextToken();

gotToken:
        wxString trailingChars;

        // token may contain brackets
        int bracketPos;
        bracketPos=token.Find('(', true);
        while (bracketPos >= 0)
        {
            fc += token.Left(bracketPos+1);
            token = token.Mid(bracketPos+1);
            bracketPos=token.Find('(', true);
        }

        bracketPos=token.Find(')', true);
        while (bracketPos >= 0)
        {
            trailingChars = token.Mid(bracketPos) + trailingChars;
            token = token.Left(bracketPos);
            bracketPos=token.Find(')', true);
        }
        // identify token
        tokenAction *tp=sqlTokens;
        while (tp->keyword)
        {
            if (!token.CmpNoCase(tp->keyword))
            {
                if (tp->special == SQLTK_ON && wasOn)
                    tp=&secondOnToken;
                else
                    wasOn = (tp->special == SQLTK_ON);
                break;
            }
            tp++;
        }
        
        if (tp && tp->keyword)
        {
            // we found a keyword.
            if (tp->special == SQLTK_UNION || tp->special == SQLTK_JOINMOD)
            {
                token=tokenizer.GetNextToken();
                if (tp->special == SQLTK_UNION && token.CmpNoCase(wxT("JOIN")))
                {
                    fc += wxT("\nUNION\n");
                    indent=0;
                    goto gotToken;
                }
                else
                {
                    trailingChars = token + wxT(" ") + trailingChars;
                    indent += tp->actionBefore;
                    if (indent<0)   indent=0;
                }
            }
            else
            {
                indent += tp->actionBefore;
                if (indent<0)   indent=0;
            }
            if (tp->doBreak)
            {
                fc += wxT("\n") + wxString(' ', (size_t)indent);
                position = indent;
            }
            else
            {
                fc += wxT(" ");
                position += 1;
            }
            fc += tp->replaceKeyword;
            position += wxString(tp->replaceKeyword).Length();

            indent += tp->actionAfter;
            if (indent<0)   indent=0;
        }
        else
        {
            fc += token;
            position += token.Length();
        }
        fc += wxT(" ");
        position++;
        if (!trailingChars.IsNull())
        {
            fc += trailingChars + wxT(" ");;
            position += trailingChars.Length() + 1;
        }
    }
    return fc;
}

