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
#include "pgDatabase.h"
#include "pgObject.h"
#include "pgServer.h"


pgDatabase::pgDatabase(const wxString& szNewName)
: pgObject()
{

    wxLogInfo(wxT("Creating a pgDatabase object"));

    // Call the 'virtual' ctor
    vCtor(PG_DATABASE, szNewName);

    bAllowConnections = TRUE;
    bConnected = FALSE;
}

pgDatabase::~pgDatabase()
{
    wxLogInfo(wxT("Destroying a pgDatabase object"));
}

int pgDatabase::Connect() {
    if (!bAllowConnections) {
        return PGCONN_REFUSED;
    }
    if (bConnected) {
        return cnDatabase->GetStatus();
    } else {

        cnDatabase = new pgConn(this->GetServer()->GetName(), this->GetName(), this->GetServer()->GetUsername(), this->GetServer()->GetPassword(), this->GetServer()->GetPort());
        if (cnDatabase->GetStatus() == PGCONN_OK) {

            // As we connected, we should now get the comments
            wxString szSQL, szComment;
            szSQL.Printf(wxT("SELECT description FROM pg_description WHERE objoid = %f"), this->GetOid());
            szComment = cnDatabase->ExecuteScalar(szSQL);
            if (szComment != "(null)") {
                this->iSetComment(szComment);
            }

            // Now we're connected.
            bConnected = TRUE;
            return PGCONN_OK;

        } else {

            wxString szMsg;
			#ifdef __WXMSW__
                szMsg.Printf(wxT("%s"), cnDatabase->GetLastError);
            #else
			    szMsg.Printf(wxT("%s"), cnDatabase->GetLastError().c_str());
            #endif
			wxLogError(szMsg);
            return PGCONN_BAD;
        }
    }
}

// Parent objects
pgServer *pgDatabase::GetServer() {
    return objServer;
}

void pgDatabase::SetServer(pgServer *objNewServer) {
    objServer = objNewServer;
}

wxString pgDatabase::GetPath() const
{
    return szPath;
}
void pgDatabase::iSetPath(const wxString& szNewVal)
{
    szPath = szNewVal;
}

wxString pgDatabase::GetEncoding() const
{
    return szEncoding;
}
void pgDatabase::iSetEncoding(const wxString& szNewVal)
{
    szEncoding = szNewVal;
}

wxString pgDatabase::GetVariables() const
{
    return szVariables;
}
void pgDatabase::iSetVariables(const wxString& szNewVal)
{
    szVariables = szNewVal;
}

bool pgDatabase::GetAllowConnections()
{
    return bAllowConnections;
}
void pgDatabase::iSetAllowConnections(bool bNewVal)
{
    bAllowConnections = bNewVal;
}

bool pgDatabase::GetConnected()
{
    return bConnected;
}

wxString pgDatabase::GetSql() const
{
    wxString szSQL;
    szSQL.Printf(wxT("CREATE DATABASE %s WITH ENCODING = %s;"),
                 this->GetQuotedIdentifier(), qtString(szEncoding));

    if (!this->GetComment().IsEmpty())
        szSQL.Printf(wxT("%s\nCOMMENT ON DATABASE %s IS %s;"),
                     szSQL.c_str(), this->GetQuotedIdentifier(), qtString(this->GetComment()));

    return szSQL;
}
