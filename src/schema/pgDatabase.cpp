//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
//
// pgDatabase.cpp - PostgreSQL Database
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "misc.h"
#include "pgDatabase.h"
#include "pgObject.h"
#include "pgServer.h"


pgDatabase::pgDatabase(const wxString& newName)
: pgObject(PG_DATABASE, newName)
{
    wxLogInfo(wxT("Creating a pgDatabase object"));

    allowConnections = TRUE;
    connected = FALSE;
}

pgDatabase::~pgDatabase()
{
    wxLogInfo(wxT("Destroying a pgDatabase object"));
}

int pgDatabase::Connect() {
    if (!allowConnections) {
        return PGCONN_REFUSED;
    }
    if (connected) {
        return database->GetStatus();
    } else {

        database = new pgConn(this->GetServer()->GetName(), this->GetName(), this->GetServer()->GetUsername(), this->GetServer()->GetPassword(), this->GetServer()->GetPort());
        if (database->GetStatus() == PGCONN_OK) {

            // As we connected, we should now get the comments
            wxString sql, rawcomment;
            sql.Printf(wxT("SELECT description FROM pg_description WHERE objoid = %s"), NumToStr(this->GetOid()).c_str());
            rawcomment = database->ExecuteScalar(sql);
            if (rawcomment != "(null)") {
                this->iSetComment(rawcomment);
            }

            // Now we're connected.
            connected = TRUE;
            return PGCONN_OK;

        } else {

            wxString msg;
			msg.Printf(wxT("%s"), database->GetLastError().c_str());
			wxLogError(msg);
            return PGCONN_BAD;
        }
    }
}

bool pgDatabase::GetSystemObject() const
{
    if (server) {
        if (this->GetName() == wxT("template0")) return TRUE;
        return (this->GetOid() <= server->GetLastSystemOID());
    } else {
        return FALSE;
    }
}

wxString pgDatabase::GetSql() const
{
    wxString sql;
    sql.Printf(wxT("CREATE DATABASE %s WITH ENCODING = %s;"),
                 this->GetQuotedIdentifier().c_str(), qtString(this->GetEncoding()).c_str());

    if (!this->GetComment().IsEmpty())
        sql.Printf(wxT("%s\nCOMMENT ON DATABASE %s IS %s;"),
                     sql.c_str(), this->GetQuotedIdentifier().c_str(), qtString(this->GetComment()).c_str());

    return sql;
}
