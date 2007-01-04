//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
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
#include "pgObject.h"
#include "pgServer.h"
#include "frmMain.h"
#include "frmReport.h"
#include "pgDomain.h"

#include "pgSequence.h"
#include "pgFunction.h"
#include "pgType.h"
#include "pgDatabase.h"
#include "pgTable.h"
#include "pgColumn.h"
#include "pgView.h"
#include "pgType.h"
#include "pgOperator.h"
#include "pgLanguage.h"
#include "pgConversion.h"
#include "pgTablespace.h"
#include "pgGroup.h"
#include "pgUser.h"
#include "pgIndex.h"
#include "pgTrigger.h"
#include "pgCheck.h"
#include "pgIndexConstraint.h"
#include "pgForeignKey.h"
#include "pgRule.h"
#include "pgRole.h"
#include "pgDefs.h"
#include "pgaJob.h"


int pgObject::GetType() const
{
    if (factory)
        return factory->GetId();
    return type;
}


int pgObject::GetMetaType() const
{
    if (factory)
        return factory->GetMetaType();
    return PGM_UNKNOWN;
}


wxString pgObject::GetTypeName() const
{
    return factory->GetTypeName();
}


wxString pgObject::GetTranslatedTypeName() const
{
    return wxString(wxGetTranslation(GetTypeName()));
}


int pgObject::GetIconId()
{
    int id=-1;
    if (factory)
        id= factory->GetIconId();

    wxASSERT(id != -1);
    return id;
}


int pgObject::GetTypeId(const wxString &typname)
{
    pgaFactory *factory=pgaFactory::GetFactory(typname);
    if (factory)
        return factory->GetId();

    return -1;
}


pgObject::pgObject(pgaFactory &_factory, const wxString& newName)
: wxTreeItemData()
{
    factory=&_factory;

    if (factory->IsCollection() && newName.IsEmpty())
        name = factory->GetTypeName();
    else
        name = newName;

    type = factory->GetId();
    expandedKids=false;
    needReread=false;
    hintShown=false;
}


pgObject::pgObject(int newType, const wxString& newName)
: wxTreeItemData()
{
    factory=pgaFactory::GetFactory(newType);

    // Set the typename and type
    type = newType;

    if (newName.IsEmpty())
        name = factory->GetTypeName();
    else
        name = newName;
    expandedKids=false;
    needReread=false;
    hintShown=false;
}

        
void pgObject::AppendMenu(wxMenu *menu, int type)
{
    if (menu)
        factory->AppendMenu(menu);
}


wxString pgObject::GetHelpPage(bool forCreate) const
{
    wxString page;

    if (!IsCollection())
        page=wxT("pg/sql-create") + GetTypeName().Lower();

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
}


bool pgObject::UpdateIcon(ctlTree *browser)
{
    int icon=GetIconId();
    if (GetId() && browser->GetItemImage(GetId(), wxTreeItemIcon_Normal) != icon)
    {
        browser->SetItemImage(GetId(), GetIconId(), wxTreeItemIcon_Normal);
        browser->SetItemImage(GetId(), GetIconId(), wxTreeItemIcon_Selected);
        return true;
    }
    return false;
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
                pgaFactory *depFactory=0;
                switch (typestr.c_str()[0])
                {
                    case 'c':
                    case 's':   // we don't know these; internally handled
                    case 't':   set->MoveNext(); continue;

                    case 'r':
					{
						if (StrToLong(typestr.Mid(1)) > 0)
							depFactory = &columnFactory;
						else
							depFactory=&tableFactory;
						break;
					}
                    case 'i':   depFactory=&indexFactory;    break;
                    case 'S':   depFactory=&sequenceFactory; break;
                    case 'v':   depFactory=&viewFactory;     break;
                    case 'p':   depFactory=&functionFactory; break;
                    case 'n':   depFactory=&schemaFactory;   break;
                    case 'y':   depFactory=&typeFactory;     break;
                    case 'T':   depFactory=&triggerFactory;  break;
                    case 'l':   depFactory=&languageFactory; break;
                    case 'R':
                    {
                        refname = _refname + wxT(" ON ") + refname + set->GetVal(wxT("ownertable"));
                        _refname=wxEmptyString;
                        depFactory=&ruleFactory;
                        break;
                    }
                    case 'C':
                    {
                        switch (typestr.c_str()[1])
                        {
                            case 'c':   depFactory=&checkFactory;        break;
                            case 'f':   
                                refname += set->GetVal(wxT("ownertable")) + wxT(".");
                                depFactory=&foreignKeyFactory;   break;
                            case 'p':   depFactory=&primaryKeyFactory;   break;
                            case 'u':   depFactory=&uniqueFactory;       break;
                            default:    break;
                        }
                        break;
                    }
                    default:
                        break;
                }

                refname += _refname;

                wxString typname;
                int icon;
                if (depFactory)
                {
                    typname = depFactory->GetTypeName();
                    icon = depFactory->GetIconId();
                }
                else
                {
                    typname = _("Unknown");
                    icon=-1;
                }

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


void pgObject::ShowDependencies(frmMain *form, ctlListView *Dependencies, const wxString &wh)
{
    wxString where;
    if (wh.IsEmpty())
        where = wxT(" WHERE dep.objid=") + GetOidStr();
    else
        where = wh;
    ShowDependency(GetDatabase(), Dependencies,
        wxT("SELECT DISTINCT deptype, refclassid, cl.relkind,\n")
        wxT("       CASE WHEN cl.relkind IS NOT NULL THEN cl.relkind || COALESCE(dep.refobjsubid::text, '')\n")
        wxT("            WHEN tg.oid IS NOT NULL THEN 'T'::text\n")
        wxT("            WHEN ty.oid IS NOT NULL THEN 'y'::text\n")
        wxT("            WHEN ns.oid IS NOT NULL THEN 'n'::text\n")
        wxT("            WHEN pr.oid IS NOT NULL THEN 'p'::text\n")
        wxT("            WHEN la.oid IS NOT NULL THEN 'l'::text\n")
        wxT("            WHEN rw.oid IS NOT NULL THEN 'R'::text\n")
        wxT("            WHEN co.oid IS NOT NULL THEN 'C'::text || contype\n")
        wxT("            ELSE '' END AS type,\n")
        wxT("       COALESCE(coc.relname, clrw.relname) AS ownertable,\n")
        wxT("       COALESCE(cl.relname || '.' || att.attname, cl.relname, conname, proname, tgname, typname, lanname, rulename, ns.nspname) AS refname,\n")
        wxT("       COALESCE(nsc.nspname, nso.nspname, nsp.nspname, nst.nspname, nsrw.nspname) AS nspname\n")
        wxT("  FROM pg_depend dep\n")
        wxT("  LEFT JOIN pg_class cl ON dep.refobjid=cl.oid\n")
		wxT("  LEFT JOIN pg_attribute att ON dep.refobjid=att.attrelid AND dep.refobjsubid=att.attnum")
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

    pgConn *conn=GetConnection();
    if (conn && conn->BackendMinimumVersion(8, 1) && where.Find(wxT("subid")) < 0)
    {
        int iconId=groupRoleFactory.GetCollectionFactory()->GetIconId();

        pgSetIterator set(conn, 
            wxT("SELECT rolname AS refname, refclassid, deptype\n")
            wxT("  FROM pg_shdepend dep\n")
            wxT("  LEFT JOIN pg_roles r ON refclassid=1260 AND refobjid=r.oid\n")
            + where + wxT("\n")
            wxT(" ORDER BY 1"));

        while (set.RowsLeft())
        {
            wxString refname = set.GetVal(wxT("refname"));
            wxString deptype = set.GetVal(wxT("deptype"));
            if (deptype == wxT("a"))
                deptype = wxT("ACL");
            else if (deptype == wxT("o"))
                deptype = _("Owner");

            if (set.GetOid(wxT("refclassid")) == PGOID_CLASS_PG_AUTHID)
                    Dependencies->AppendItem(iconId, wxT("Role"), refname, deptype);
        }
    }
}


void pgObject::ShowDependents(frmMain *form, ctlListView *referencedBy, const wxString &wh)
{
    if (this->IsCollection())
        return;

    wxString where;
    if (wh.IsEmpty())
        where = wxT(" WHERE dep.refobjid=") + GetOidStr();
    else
        where = wh;
    ShowDependency(GetDatabase(), referencedBy,
        wxT("SELECT DISTINCT deptype, classid, cl.relkind,\n")
        wxT("       CASE WHEN cl.relkind IS NOT NULL THEN cl.relkind || COALESCE(dep.objsubid::text, '')\n")
        wxT("            WHEN tg.oid IS NOT NULL THEN 'T'::text\n")
        wxT("            WHEN ty.oid IS NOT NULL THEN 'y'::text\n")
        wxT("            WHEN ns.oid IS NOT NULL THEN 'n'::text\n")
        wxT("            WHEN pr.oid IS NOT NULL THEN 'p'::text\n")
        wxT("            WHEN la.oid IS NOT NULL THEN 'l'::text\n")
        wxT("            WHEN rw.oid IS NOT NULL THEN 'R'::text\n")
        wxT("            WHEN co.oid IS NOT NULL THEN 'C'::text || contype\n")
        wxT("            ELSE '' END AS type,\n")
        wxT("       COALESCE(coc.relname, clrw.relname) AS ownertable,\n")
        wxT("       COALESCE(cl.relname || '.' || att.attname, cl.relname, conname, proname, tgname, typname, lanname, rulename, ns.nspname) AS refname,\n")
        wxT("       COALESCE(nsc.nspname, nso.nspname, nsp.nspname, nst.nspname, nsrw.nspname) AS nspname\n")
        wxT("  FROM pg_depend dep\n")
        wxT("  LEFT JOIN pg_class cl ON dep.objid=cl.oid\n")
		wxT("  LEFT JOIN pg_attribute att ON dep.objid=att.attrelid AND dep.objsubid=att.attnum")
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
        wxT("  LEFT JOIN pg_language la ON dep.objid=la.oid\n")
        wxT("  LEFT JOIN pg_namespace ns ON dep.objid=ns.oid\n")
        + where, wxT("classid"));
}


void pgObject::ShowTree(frmMain *form, ctlTree *browser, ctlListView *properties, ctlSQLBox *sqlPane)
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

        SetContextInfo(form);

        ctlListView *statistics=form->GetStatistics();
        if (statistics)
            ShowStatistics(form, statistics);

        ctlListView *Dependencies=form->GetDependencies();
        if (Dependencies)
        {
            Dependencies->ClearAll();
            if (!IsCollection())
                ShowDependencies(form, Dependencies);
        }
        ctlListView *referencedBy=form->GetReferencedBy();
        if (referencedBy)
        {
            referencedBy->ClearAll();
            if (!IsCollection())
                ShowDependents(form, referencedBy);
        }
    }

    ShowTreeDetail(browser, form, properties, sqlPane);
    if (form)
        form->EndMsg(!GetConnection() || GetConnection()->GetStatus() == PGCONN_OK);
}


wxTreeItemId pgObject::AppendBrowserItem(ctlTree *browser, pgObject *object)
{
    return browser->AppendObject(this, object);
}


wxString pgObject::GetCommentSql()
{
    wxString cmt;
    if (!comment.IsNull())
    {
        cmt = wxT("COMMENT ON ") + GetTypeName().Upper() + wxT(" ") + GetQuotedFullIdentifier() 
            + wxT(" IS ") + qtDbString(comment) + wxT(";\n");
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
        AppendRight(rights, acl, 'c', wxT("CONNECT"));
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
    pgDatabase *db=GetDatabase();
    if (db)
        return db->connection();
    
    pgServer *server;

    if (IsCreatedBy(serverFactory))
        server = (pgServer*)this;
    else
        server = GetServer();

    if (server)
        return server->connection();
    return 0;
}


//////////////////////////////////////////////////////////////

bool pgServerObject::CanDrop()
{
    if (GetMetaType() == PGM_DATABASE)
        return server->GetCreatePrivilege();
    else
	{
		if (server->GetConnection()->BackendMinimumVersion(8, 1) && GetMetaType() == PGM_ROLE)
			return server->GetCreateRole();
		else
			return server->GetSuperUser();
	}
}


bool pgServerObject::CanCreate()
{
    if (GetMetaType() == PGM_DATABASE)
        return server->GetCreatePrivilege();
    else
	{
		if (server->GetConnection()->BackendMinimumVersion(8, 1) && GetMetaType() == PGM_ROLE)
			return server->GetCreateRole();
		else
			return server->GetSuperUser();
	}
}


void pgServerObject::FillOwned(ctlTree *browser, ctlListView *referencedBy, const wxArrayString &dblist, const wxString &query)
{
    pgCollection *databases=0;

    wxCookieType cookie;
    wxTreeItemId item=browser->GetFirstChild(GetServer()->GetId(), cookie);
    while (item)
    {
        databases = (pgCollection*)browser->GetObject(item);
        if (databases && databases->GetMetaType() == PGM_DATABASE)
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
                pgDatabase *db=(pgDatabase*)browser->GetObject(item);
                if (db->GetMetaType() == PGM_DATABASE && db->GetName() == dbname)
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
                    wxString relname = qtIdent(set->GetVal(wxT("nspname")));
                    if (!relname.IsEmpty())
                        relname += wxT(".");
                    relname += qtIdent(set->GetVal(wxT("relname")));
                    pgaFactory *ownerFactory=0;

                    switch (set->GetVal(wxT("relkind")).c_str()[0])
                    {
                        case 'r':   ownerFactory=&tableFactory;            break;
                        case 'i':   ownerFactory=&indexFactory;        
                                    relname = qtIdent(set->GetVal(wxT("indname"))) + wxT(" ON ") + relname;
                                    break;
                        case 'S':   ownerFactory=&sequenceFactory;         break;
                        case 'v':   ownerFactory=&viewFactory;             break;
                        case 'c':   // composite type handled in PG_TYPE
                        case 's':   // special
                        case 't':   // toast
                                    break;
                        case 'n':   ownerFactory = &schemaFactory;           break;
                        case 'y':   ownerFactory = &typeFactory;             break;
                        case 'd':   ownerFactory = &domainFactory; break;
                        case 'C':   ownerFactory = &conversionFactory;       break;
                        case 'p':   ownerFactory=&functionFactory;         break;
                        case 'T':   ownerFactory=&triggerFunctionFactory;  break;
                        case 'o':   ownerFactory=&operatorFactory;
                                    relname = set->GetVal(wxT("relname"));  // unquoted
                                    break;
                    }

                    if (ownerFactory)
                    {
                        wxString typname;
                        int icon;
                        typname = ownerFactory->GetTypeName();
                        icon = ownerFactory->GetIconId();
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


void pgDatabaseObject::DisplayStatistics(ctlListView *statistics, const wxString& query)
{
    if (statistics)
    {
        wxLogInfo(wxT("Displaying statistics for %s %s"), GetTypeName().c_str(), GetFullIdentifier().c_str());

        // Add the statistics view columns
        CreateListColumns(statistics, _("Statistic"), _("Value"));

        pgSet *stats = database->ExecuteSet(query);
    
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


///////////////////////////////////////////////////////////////

void pgSchemaObject::SetSchema(pgSchema *newSchema)
{
    schema = newSchema;
    database = schema->GetDatabase();
}


void pgSchemaObject::UpdateSchema(ctlTree *browser, OID schemaOid)
{
    // used e.g. for triggers that use trigger functions from other namespaces

    if (schema->GetOid() != schemaOid)
    {
        pgObject *schemas=browser->GetObject(browser->GetItemParent(schema->GetId()));

        wxASSERT(schemas);
        treeObjectIterator schIt(browser, schemas);
        pgSchema *sch;

        while ((sch=(pgSchema*)schIt.GetNextObject()) != 0)
        {
            if (sch->GetOid() == schemaOid)
            {
                SetSchema(sch);
                return;
            }
        }
        wxMessageBox(_("The schema oid can't be located, please refresh all schemas!"), 
            _("Missing information"), wxICON_EXCLAMATION | wxOK, browser);
    }
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
//    form->SetDatabase(schema->GetDatabase());
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

wxString pgObject::qtDbString(const wxString &str) 
{ 
	// Use the server aware version if possible
	if (GetDatabase() && GetDatabase()->GetConnection())
	    return GetDatabase()->GetConnection()->qtDbString(str);
	else
	{
		wxString ret = str;
		ret.Replace(wxT("\\"), wxT("\\\\"));
        ret.Replace(wxT("'"), wxT("''"));
        ret.Append(wxT("'"));
        ret.Prepend(wxT("'"));
		return ret;
	}
}
