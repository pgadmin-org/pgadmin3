//////////////////////////////////////////////////////////////////////////
//
// pgAgent - PostgreSQL Tools
// $Id$
// Copyright (C) 2003 The pgAdmin Development Team
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
    DBconn(const string &dbname);
    DBconn(const string &connectString, const string &name);
    ~DBconn();

public:
    static DBconn *Get(const string &dbname);
    static DBconn *InitConnection(const string &connectString);

    static void ClearConnections(bool allIncludingPrimary=false);
    static void SetBasicConnectString(const string &bcs) { basicConnectString = bcs; }

    string GetLastError();
    string GetDBname() {return dbname; }
    bool IsValid() { return conn != 0; }

    DBresult *Execute(const string &query);
    int ExecuteVoid(const string &query);
	void Return();

private:
    bool DBconn::Connect(const string &connectString);

protected:
    static string basicConnectString;
	static DBconn *primaryConn;

    string dbname, lastError;
    PGconn *conn;
	DBconn *next, *prev;
    bool inUse;

    friend class DBresult;

};


class DBresult
{
protected:
    DBresult(DBconn *conn, const string &query);

public:
    ~DBresult();

    string GetString(int col) const;
    string GetString(const char *colname) const;

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

