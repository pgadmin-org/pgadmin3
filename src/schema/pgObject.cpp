//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002 - 2003, The pgAdmin Development Team
// This software is released under the Artistic Licence
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
#include "menu.h"
#include "pgObject.h"
#include "pgServer.h"
#include "frmMain.h"


// Ordering must match the PG_OBJTYPE enumeration in pgObject.h


pgTypes typesList[] =
{
    {__("None"), 0, 0},
    {__("Servers"), 0, 0},          {__("Server"), __("New Server"), __("Create a new Server connection.") },
    {__("Databases"), 0, 0},        {__("Database"), __("New Database"), __("Create a new Database.") },
    {__("Groups"), 0, 0},           {__("Group"), __("New Group"), __("Create a new Group.") },
    {__("Users"), 0, 0},            {__("User"), __("New User"), __("Create a new User.") },
    {__("Languages"), 0, 0},        {__("Language"), __("New Language"), __("Create a new Language.") },
    {__("Schemas"), 0, 0},          {__("Schema"), __("New Schema"), __("Create a new Schema.") },
    {__("Aggregates"), 0, 0},       {__("Aggregate"), __("New Aggregate"), __("Create a new Aggregate.") },
    {__("Casts"), 0, 0},            {__("Cast"), __("New Cast"), __("Create a new Cast.") },
    {__("Conversions"), 0, 0},      {__("Conversion"), __("New Conversion"), __("Create a new Conversion.") },
    {__("Domains"), 0, 0},          {__("Domain"), __("New Domain"), __("Create a new Domain.") },
    {__("Functions"), 0, 0},        {__("Function"), __("New Function"), __("Create a new Function.") },
    {__("Trigger Functions"), 0, 0},{__("Trigger Function"), __("New Trigger Function"), __("Create a new Trigger Function.") },
    {__("Operators"), 0, 0},        {__("Operator"), __("New Operator"), __("Create a new Operator.") },
    {__("Operator Classes"), 0, 0}, {__("Operator Class"), __("New Operator Class"), __("Create a new Operator Class.") },
    {__("Sequences"), 0, 0},        {__("Sequence"), __("New Sequence"), __("Create a new Sequence.") },
    {__("Tables"), 0, 0},           {__("Table"), __("New Table"), __("Create a new Table.") },
    {__("Types"), 0, 0},            {__("Type"), __("New Type"), __("Create a new Type.") },
    {__("Views"), 0, 0},            {__("View"), __("New View"), __("Create a new View.") },
    {__("Columns"), 0, 0},          {__("Column"), __("New Column"), __("Add a new Column.") },
    {__("Indexes"), 0, 0},          {__("Index"), __("New Index"), __("Add a new Index.") },
    {__("Rules"), 0, 0},            {__("Rule"), __("New Rule"), __("Create a new Rule.") },
    {__("Triggers"), 0, 0},         {__("Trigger"), __("New Trigger"), __("Add a new Trigger.") },
    {__("Constraints"), 0, 0},      
        {__("Primary Key"), __("New Primary Key"), __("Create a Primary Key.") },
        {__("Unique"), __("New Unique Constraint"), __("Add a new Unique Constraint.") },
        {__("Check"), __("New Check Constraint"), __("Add a new Check Constraint.") },
        {__("Foreign Key"), __("New Foreign Key"), __("Add a new Foreign Key.") },
    {__("Unknown"), 0, 0}
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
        form->SetButtons(TRUE, CanCreate(), CanDrop(), CanEdit(), canSql, CanView(), CanMaintenance());
        SetContextInfo(form);
    }

    wxLogInfo(wxT("Displaying properties for ") + GetTypeName() + wxT(" ")+GetIdentifier());
    StartMsg(wxString::Format(_("Retrieving %s details"), wxGetTranslation(GetTypeName())));
    ShowTreeDetail(browser, form, properties, statistics, sqlPane);
    EndMsg();
}


void pgObject::CreateListColumns(wxListCtrl *list, const wxString &left, const wxString &right)
{
    list->ClearAll();
    list->InsertColumn(0, left, wxLIST_FORMAT_LEFT, 150);
    list->InsertColumn(1, right, wxLIST_FORMAT_LEFT, 700);
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
}


void pgObject::InsertListItem(wxListCtrl *list, const int pos, const wxString& str1, const wxString& str2)
{
    list->InsertItem(pos, str1, PGICON_PROPERTY);
    if (str2 != wxT(""))
        list->SetItem(pos, 1, str2);
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

void pgSchemaObject::DisplayStatistics(wxListCtrl *statistics, const wxString& query)
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
                fc += wxT("\n") + wxString(' ', indent);
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

