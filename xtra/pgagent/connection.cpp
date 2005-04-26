//////////////////////////////////////////////////////////////////////////
//
// pgAgent - PostgreSQL Tools
// $Id$
// Copyright (C) 2003 The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// connection.cpp - database connection
//
//////////////////////////////////////////////////////////////////////////

#include "pgAgent.h"
#include <libpq-fe.h>
#include <time.h>


// entries in the connection pool
int connPoolCount=5;


DBconn **DBconn::pool=0;
string DBconn::basicConnectString;


DBconn::DBconn(const string &name)
{
    dbname = name;
    Connect(basicConnectString  + " dbname=" + dbname);
}


DBconn::DBconn(const string &connectString, const string &name)
{
    dbname = name;
    Connect(connectString);
}


bool DBconn::Connect(const string &connectString)
{
    conn=PQconnectdb(connectString.c_str());
    if (PQstatus(conn) == CONNECTION_OK)
    {
    }
    else
    {
	    lastError=PQerrorMessage(conn);
        PQfinish(conn);
        conn=0;
    }
    return IsValid();
}

DBconn::~DBconn()
{
    // clear a single connection
    if (conn)
    {
        PQfinish(conn);
        conn=0;
    }
}


DBconn *DBconn::InitConnection(const string &connectString)
{
    if (!pool)
    {
        pool = new DBconn*[connPoolCount];
        if (pool)
            memset(pool, 0, sizeof(DBconn*) * connPoolCount);
    }
    if (!pool)
        fatal("Out of memory for connection pool");

    basicConnectString=connectString;
    string dbname;

    size_t pos=basicConnectString.find("dbname=");
    if (pos == -1)
        dbname = "dba";
    else
    {
        dbname = basicConnectString.substr(pos+7);
        basicConnectString = basicConnectString.substr(0, pos);
        pos = dbname.find(" ");
        if (pos != -1)
        {
            if (basicConnectString.length())
                basicConnectString += " ";
            basicConnectString += dbname.substr(pos+1);
            dbname = dbname.substr(0, pos);
        }
    }
    pool[0] = new DBconn(connectString, dbname);
    pool[0]->primary = true;

    return pool[0];
}


DBconn *DBconn::Get(const string &dbname, bool asPrimary)
{
    if (!pool)
    {
        pool = new DBconn*[connPoolCount];
        if (pool)
            memset(pool, 0, sizeof(DBconn*) * connPoolCount);
    }
    if (!pool)
        fatal("Out of memory for connection pool");

    int i;
    DBconn **emptyConn=0, **oldestConn=0;

    // find an existing connection
    for (i=0 ; i < connPoolCount ; i++)
    {
        if (pool[i])
        {
            if (dbname == pool[i]->dbname)
                return pool[i];

            // while searching, also mark the oldest non-primary connection
            if (!pool[i]->primary)
            {
                if (!oldestConn || pool[i]->timestamp < (*oldestConn)->timestamp)
                    oldestConn=pool+i;
            }
        }
        else
        {
            // while searching, mark the first empty slot
            if (!emptyConn)
                emptyConn=pool+i;
        }
    }
    if (!emptyConn)
    {
        delete *oldestConn;
        emptyConn=oldestConn;
        *emptyConn=0;
    }

    DBconn *conn=new DBconn(dbname);
    if (conn->conn)
    {
        *emptyConn=conn;
    }
    return *emptyConn;
}


void DBconn::ClearConnections(bool all)
{
    // clears all connections, except for the primary one.
    // if all is true, even the primary connection will be killed.
    if (pool)
    {
        int i;
        for (i=0 ; i < connPoolCount ; i++)
        {
            if (pool[i])
            {
                if (all || !pool[i]->primary)
                {
                    delete pool[i];
                    pool[i]=0;
                }
            }
        }
        if (all)
        {
            delete[] pool;
            pool=0;
        }
    }
}


DBresult *DBconn::Execute(const string &query)
{
    DBresult *res=new DBresult(this, query);
    if (!res->IsValid())
    {
        // error handling here

        delete res;
        return 0;
    }
    return res;
}


int DBconn::ExecuteVoid(const string &query)
{
    int rows=-1;
    DBresult *res=Execute(query);
    if (res)
    {
        rows = res->RowsAffected();
        delete res;
    }
    return rows;
}


///////////////////////////////////////////////////////7



DBresult::DBresult(DBconn *conn, const string &query)
{
    result=PQexec(conn->conn, query.c_str());
    currentRow=0;
    maxRows=0;

    if (result)
    {
        int rc=PQresultStatus(result);
        if (rc == PGRES_TUPLES_OK)
            maxRows = PQntuples(result);
        else if (rc != PGRES_COMMAND_OK)
        {
            conn->lastError = PQerrorMessage(conn->conn);
            PQclear(result);
            result=0;
        }
    }
	else
	    conn->lastError = PQerrorMessage(conn->conn);

}


DBresult::~DBresult()
{
    if (result)
        PQclear(result);
}


string DBresult::GetString(int col) const
{
    string str;

    if (result && currentRow < maxRows && col >= 0)
    {
        str = PQgetvalue(result, currentRow, col);
    }
    return str;
}


string DBresult::GetString(const char *colname) const
{
    int col=PQfnumber(result, colname);
    if (col < 0)
    {
        // fatal: not found
        return "";
    }
    return GetString(col);
}

