//////////////////////////////////////////////////////////////////////////
//
// pgAgent - PostgreSQL Tools
// $Id$
// Copyright (C) 2002 - 2006 The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// connection.h - database connection
//
//////////////////////////////////////////////////////////////////////////


#ifndef CONNECTION_H
#define CONNECTION_H

#include <libpq-fe.h>

class DBresult;


class DBconn
{
protected:
    DBconn(const wxString &db);
    DBconn(const wxString &connectString, const wxString &db);
    ~DBconn();

public:
    static DBconn *Get(const wxString &db);
    static DBconn *InitConnection(const wxString &connectString);

    static void ClearConnections(bool allIncludingPrimary=false);
    static void SetBasicConnectString(const wxString &bcs) { basicConnectString = bcs; }

    wxString GetLastError();
    wxString GetDBname() { return dbname; }
    bool IsValid() { return conn != 0; }

    DBresult *Execute(const wxString &query);
    int ExecuteVoid(const wxString &query);
	void Return();

private:
    bool Connect(const wxString &connectString);

protected:
    static wxString basicConnectString;
	static DBconn *primaryConn;

    wxString dbname, lastError;
    PGconn *conn;
	DBconn *next, *prev;
    bool inUse;

    friend class DBresult;

};


class DBresult
{
protected:
    DBresult(DBconn *conn, const wxString &query);

public:
    ~DBresult();

    wxString GetString(int col) const;
    wxString GetString(const wxString &colname) const;

    bool IsValid() const { return result != NULL; }
    bool HasData() const { return currentRow < maxRows; }
    void MoveNext() { if (currentRow < maxRows) currentRow++; }

    long RowsAffected() const { return atol(PQcmdTuples(result)); }

protected:
    PGresult *result;
    int currentRow, maxRows;

    friend class DBconn;
};

#endif // CONNECTION_H

