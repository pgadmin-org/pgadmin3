//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2005, The pgAdmin Development Team
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
    {__("Tablespaces"), PGICON_TABLESPACE, 0, 0},
    {__("Tablespace"), PGICON_TABLESPACE, __("New Tablespace"), __("Create a new Tablespace.") },
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
    {__("Unknown"), -1, 0, 0},
    {0,0,0,0}
};



int pgObject::GetTypeId(const wxString &typname)
{
    int id;

    for (id=1 ; typesList[id].typName ; id++)
    {
        if (typname.IsSameAs(typesList[id].typName, false))
            return id;
    }

    return -1;
}


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


void pgObject::ShowStatistics(frmMain *form, ctlListView *statistics)
{
    statistics->ClearAll();
    statistics->AddColumn(_("Statistics"), 500);
    statistics->InsertItem(0, _("No statistics are available for the current selection"), PGICON_STATISTICS);
}


void pgObject::ShowDependency(pgDatabase *db, ctlListView *list, const wxString &query, const wxString &clsorder)
{
    list->ClearAll();
    list->AddColumn(_("Type"), 60);
    list->AddColumn(_("Name"), 100);
    list->AddColumn(_("Restriction"), 50);
    pgConn *conn = GetConnection();
    if (conn)
    {
        pgSet *set;
        // currently missing:
        // - pg_cast
        // - pg_operator
        // - pg_opclass
        
        // not being implemented:
        // - pg_attrdef (won't make sense)
        // - pg_index (done by pg_class

        set=conn->ExecuteSet(query + wxT("\n")
            wxT("   AND ") + clsorder + wxT(" IN (\n")
            wxT("   SELECT oid FROM pg_class\n")
            wxT("    WHERE relname IN ('pg_class', 'pg_constraint', 'pg_conversion', 'pg_language', 'pg_proc',\n")
            wxT("                      'pg_rewrite', 'pg_namespace', 'pg_trigger', 'pg_type'))\n")
            wxT(" ORDER BY ") + clsorder + wxT(", cl.relkind"));

        if (set)
        {
            while (!set->Eof())
            {
                wxString refname;
                wxString _refname = set->GetVal(wxT("refname"));

                if (db)
                    refname = db->GetQuotedSchemaPrefix(set->GetVal(wxT("nspname")));
                else
                {
                    refname = qtIdent(set->GetVal(wxT("nspname")));
                    if (!refname.IsEmpty())
                        refname += wxT(".");
                }

                wxString typestr=set->GetVal(wxT("type"));
                int id;

                switch (typestr.c_str()[0])
                {
                    case 'c':
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
                    case 'R':
                    {
                        refname = _refname + wxT(" ON ") + refname + set->GetVal(wxT("ownertable"));
                        _refname=wxEmptyString;
                        id=PG_RULE;
                        break;
                    }
                    case 'C':
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

                refname += _refname;

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


void pgObject::ShowDependsOn(frmMain *form, ctlListView *dependsOn, const wxString &wh)
{
    wxString where;
    if (wh.IsEmpty())
        where = wxT(" WHERE dep.objid=") + GetOidStr();
    else
        where = wh;
    ShowDependency(GetDatabase(), dependsOn,
        wxT("SELECT DISTINCT deptype, refclassid, cl.relkind,\n")
        wxT("       CASE WHEN cl.relkind IS NOT NULL THEN cl.relkind\n")
        wxT("            WHEN tg.oid IS NOT NULL THEN 'T'::text\n")
        wxT("            WHEN ty.oid IS NOT NULL THEN 'y'::text\n")
        wxT("            WHEN ns.oid IS NOT NULL THEN 'n'::text\n")
        wxT("            WHEN pr.oid IS NOT NULL THEN 'p'::text\n")
        wxT("            WHEN la.oid IS NOT NULL THEN 'l'::text\n")
        wxT("            WHEN rw.oid IS NOT NULL THEN 'R'::text\n")
        wxT("            WHEN co.oid IS NOT NULL THEN 'C'::text || contype\n")
        wxT("            ELSE '' END AS type,\n")
        wxT("       COALESCE(coc.relname, clrw.relname) AS ownertable,\n")
        wxT("       COALESCE(cl.relname, conname, proname, tgname, typname, lanname, rulename, ns.nspname) AS refname,\n")
        wxT("       COALESCE(nsc.nspname, nso.nspname, nsp.nspname, nst.nspname, nsrw.nspname) AS nspname\n")
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
        wxT("  LEFT JOIN pg_rewrite rw ON dep.refobjid=rw.oid\n")
        wxT("  LEFT JOIN pg_class clrw ON clrw.oid=rw.ev_class\n")
        wxT("  LEFT JOIN pg_namespace nsrw ON cl.relnamespace=nsrw.oid\n")
        wxT("  LEFT JOIN pg_language la ON dep.refobjid=la.oid\n")
        wxT("  LEFT JOIN pg_namespace ns ON dep.refobjid=ns.oid\n")
        + where, wxT("refclassid"));
}


void pgObject::ShowReferencedBy(frmMain *form, ctlListView *referencedBy, const wxString &wh)
{
    wxString where;
    if (wh.IsEmpty())
        where = wxT(" WHERE dep.refobjid=") + GetOidStr();
    else
        where = wh;

    ShowDependency(GetDatabase(), referencedBy,
        wxT("SELECT DISTINCT deptype, classid, cl.relkind,\n")
        wxT("       CASE WHEN cl.relkind IS NOT NULL THEN cl.relkind\n")
        wxT("            WHEN tg.oid IS NOT NULL THEN 'T'::text\n")
        wxT("            WHEN ty.oid IS NOT NULL THEN 'y'::text\n")
        wxT("            WHEN ns.oid IS NOT NULL THEN 'n'::text\n")
        wxT("            WHEN pr.oid IS NOT NULL THEN 'p'::text\n")
        wxT("            WHEN la.oid IS NOT NULL THEN 'l'::text\n")
        wxT("            WHEN rw.oid IS NOT NULL THEN 'R'::text\n")
        wxT("            WHEN co.oid IS NOT NULL THEN 'C'::text || contype\n")
        wxT("            ELSE '' END AS type,\n")
        wxT("       COALESCE(coc.relname, clrw.relname) AS ownertable,\n")
        wxT("       COALESCE(cl.relname, conname, proname, tgname, typname, lanname, rulename, ns.nspname) AS refname,\n")
        wxT("       COALESCE(nsc.nspname, nso.nspname, nsp.nspname, nst.nspname, nsrw.nspname) AS nspname\n")
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
        wxT("  LEFT JOIN pg_rewrite rw ON dep.objid=rw.oid\n")
        wxT("  LEFT JOIN pg_class clrw ON clrw.oid=rw.ev_class\n")
        wxT("  LEFT JOIN pg_namespace nsrw ON cl.relnamespace=nsrw.oid\n")
        wxT("  LEFT JOIN pg_language la ON dep.refobjid=la.oid\n")
        wxT("  LEFT JOIN pg_namespace ns ON dep.objid=ns.oid\n")
        + where, wxT("classid"));
}


void pgObject::ShowTree(frmMain *form, wxTreeCtrl *browser, ctlListView *properties, ctlSQLBox *sqlPane)
{
    pgConn *conn=GetConnection();
    if (conn)
    {
        int status = conn->GetStatus();
        if (status == PGCONN_BROKEN || status == PGCONN_BAD)
        {
            form->SetStatusText(_(" Connection broken."));
            return;
        }
    }

    wxLogInfo(wxT("Displaying properties for ") + GetTypeName() + wxT(" ")+GetIdentifier());
    if (form)
    {
        form->StartMsg(wxString::Format(_("Retrieving %s details"), GetTranslatedTypeName().c_str()));

        form->SetButtons(this);
        SetContextInfo(form);

        ctlListView *statistics=form->GetStatistics();
        if (statistics)
            ShowStatistics(form, statistics);

        ctlListView *dependsOn=form->GetDependsOn();
        if (dependsOn)
        {
            dependsOn->ClearAll();
            if (!IsCollection())
                ShowDependsOn(form, dependsOn);
        }
        ctlListView *referencedBy=form->GetReferencedBy();
        if (referencedBy)
        {
            referencedBy->ClearAll();
            if (!IsCollection())
                ShowReferencedBy(form, referencedBy);
        }
    }

    ShowTreeDetail(browser, form, properties, sqlPane);
    if (form)
        form->EndMsg();
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


wxString pgObject::GetOwnerSql(int major, int minor, wxString objname)
{
    wxString sql;
    if (GetConnection()->BackendMinimumVersion(major, minor))
    {
//      if (GetConnection()->GetUser() != owner)       // optional?
        {
            if (objname.IsEmpty())
                objname = GetTypeName().Upper() + wxT(" ") + GetQuotedFullIdentifier();
            sql = wxT("ALTER ") + objname + wxT(" OWNER TO ") + qtIdent(owner) + wxT(";\n");
        }
    }
    return sql;
}


void pgObject::AppendRight(wxString &rights, const wxString& acl, wxChar c, wxChar *rightName)
{
    if (acl.Find(c) >= 0)
    {
        if (!rights.IsNull())
            rights.Append(wxT(", "));
        rights.Append(rightName);
    }
}


wxString pgObject::GetPrivilegeGrant(const wxString& allPattern, const wxString& acl, const wxString& grantOnObject, const wxString& user)
{
    wxString rights;

    if (allPattern.Length() > 1 && acl == allPattern)
        rights = wxT("ALL");
    else
    {
        AppendRight(rights, acl, 'r', wxT("SELECT"));
        AppendRight(rights, acl, 'w', wxT("UPDATE"));
        AppendRight(rights, acl, 'a', wxT("INSERT"));
        AppendRight(rights, acl, 'd', wxT("DELETE"));
        AppendRight(rights, acl, 'R', wxT("RULE"));
        AppendRight(rights, acl, 'x', wxT("REFERENCES"));
        AppendRight(rights, acl, 't', wxT("TRIGGER"));
        AppendRight(rights, acl, 'X', wxT("EXECUTE"));
        AppendRight(rights, acl, 'U', wxT("USAGE"));
        AppendRight(rights, acl, 'C', wxT("CREATE"));
        AppendRight(rights, acl, 'T', wxT("TEMPORARY"));
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
        if (allPattern.Find(*p) >= 0)
        {
            if (p[1] == (wxChar)'*')
                aclWithGrant += *p;
            else
                aclWithoutGrant += *p;
        }
        p++;
        if (*p == (wxChar)'*')
            p++;
    }

    wxString grant;
    if (!aclWithoutGrant.IsEmpty() || aclWithGrant.IsEmpty())
        grant += GetPrivilegeGrant(allPattern, aclWithoutGrant, grantOnObject, user) + wxT(";\n");
    if (!aclWithGrant.IsEmpty())
        grant += GetPrivilegeGrant(allPattern, aclWithGrant, grantOnObject, user) + wxT(" WITH GRANT OPTION;\n");

    return grant;
}


wxString pgObject::GetGrant(const wxString& allPattern, const wxString& _grantFor)
{
    wxString grant, str, user, grantFor;
    if (_grantFor.IsNull())
    {
        grantFor = GetTypeName();
        grantFor.MakeUpper();
        grantFor += wxT(" ") + GetQuotedFullIdentifier();
    }
    else
        grantFor = _grantFor;

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

            grant += GetPrivileges(allPattern, str, grantFor, user);
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
        case PG_SERVER:
            conn=((pgServer*)this)->connection();
            break;
        case PG_DATABASES:
        case PG_USER:
        case PG_USERS:
        case PG_GROUP:
        case PG_GROUPS:
        case PG_TABLESPACES:
        case PG_TABLESPACE:
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


void pgServerObject::FillOwned(wxTreeCtrl *browser, ctlListView *referencedBy, const wxArrayString &dblist, const wxString &query)
{
    pgCollection *databases;

    wxCookieType cookie;
    wxTreeItemId item=browser->GetFirstChild(GetServer()->GetId(), cookie);
    while (item)
    {
        databases = (pgCollection*)browser->GetItemData(item);
        if (databases && databases->GetType() == PG_DATABASES)
            break;
        else
            databases=0;

        item=browser->GetNextChild(GetServer()->GetId(), cookie);
    }
    
    size_t i;
    for (i=0 ; i < dblist.GetCount() ; i++)
    {
        wxString dbname=dblist.Item(i);
        pgConn *conn=0;
        pgConn *tmpConn=0;

        if (GetServer()->GetDatabaseName() == dbname)
            conn = GetServer()->GetConnection();
        else
        {
            item=browser->GetFirstChild(databases->GetId(), cookie);
            while (item)
            {
                pgDatabase *db=(pgDatabase*)browser->GetItemData(item);
                if (db->GetType() == PG_DATABASE && db->GetName() == dbname)
                {
                    if (db->GetConnected())
                        conn = db->GetConnection();
                    break;
                }
                item=browser->GetNextChild(databases->GetId(), cookie);
            }
        }
        if (conn && conn->GetStatus() != PGCONN_OK)
            conn=0;

        if (!conn)
        {
		    tmpConn = GetServer()->CreateConn(dbname);
            conn=tmpConn;
        }

        if (conn)
        {
            pgSet *set=conn->ExecuteSet(query);
            
            if (set)
            {
                while (!set->Eof())
                {
                    int id=0;

                    wxString relname = qtIdent(set->GetVal(wxT("nspname")));
                    if (!relname.IsEmpty())
                        relname += wxT(".");
                    relname += qtIdent(set->GetVal(wxT("relname")));

                    switch (set->GetVal(wxT("relkind")).c_str()[0])
                    {
                        case 'r':   id=PG_TABLE;            break;
                        case 'i':   id=PG_INDEX;        
                                    relname = qtIdent(set->GetVal(wxT("indname"))) + wxT(" ON ") + relname;
                                    break;
                        case 'S':   id=PG_SEQUENCE;         break;
                        case 'v':   id=PG_VIEW;             break;
                        case 'c':   // composite type handled in PG_TYPE
                        case 's':   // special
                        case 't':   // toast
                                    break;
                        case 'n':   id=PG_SCHEMA;           break;
                        case 'y':   id=PG_TYPE;             break;
                        case 'd':   id=PG_DOMAIN;           break;
                        case 'C':   id=PG_CONVERSION;       break;
                        case 'p':   id=PG_FUNCTION;         break;
                        case 'T':   id=PG_TRIGGERFUNCTION;  break;
                        case 'o':   id=PG_OPERATOR;
                                    relname = set->GetVal(wxT("relname"));  // unquoted
                                    break;
                    }

                    if (id)
                    {
                        wxString typname = typesList[id].typName;
                        int icon = typesList[id].typeIcon;
                        referencedBy->AppendItem(icon, typname, dbname, relname);
                    }

                    set->MoveNext();
                }
                delete set;
            }
        }

        if (tmpConn)
            delete tmpConn;
    }
}

//////////////////////////////////////////////////////////////

pgServer *pgDatabaseObject::GetServer() const
{
    return database->GetServer();
}

    
bool pgDatabaseObject::CanDrop()
{
    return database->GetCreatePrivilege();
}


bool pgDatabaseObject::CanCreate()
{
    return database->GetCreatePrivilege();
}


wxString pgDatabaseObject::GetSchemaPrefix(const wxString &schemaname) const
{
    return database->GetSchemaPrefix(schemaname);
}


wxString pgDatabaseObject::GetQuotedSchemaPrefix(const wxString &schemaname) const
{
    return database->GetQuotedSchemaPrefix(schemaname);
}


///////////////////////////////////////////////////////////////

void pgSchemaObject::SetSchema(pgSchema *newSchema)
{
    schema = newSchema;
    database = schema->GetDatabase();
}


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
            int col;
            for (col=0 ; col < stats->NumCols() ; col++)
            {
                if (!stats->ColName(col).IsEmpty())
                    statistics->AppendItem(stats->ColName(col), stats->GetVal(col));
            }
            delete stats;
        }
    }
}


wxString pgSchemaObject::GetFullIdentifier() const 
{
    return schema->GetPrefix()+GetName();
}


wxString pgSchemaObject::GetQuotedFullIdentifier() const
{
    return schema->GetQuotedPrefix() + GetQuotedIdentifier();
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

