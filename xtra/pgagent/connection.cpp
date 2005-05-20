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

DBconn *DBconn::primaryConn;
string DBconn::basicConnectString;

DBconn::DBconn(const string &name)
{
    dbname = name;
	inUse = false;
	next=0;
	prev=0;
    Connect(basicConnectString  + " dbname=" + dbname);
}


DBconn::DBconn(const string &connectString, const string &name)
{
    dbname = name;
	inUse = false;
	next=0;
	prev=0;
    Connect(connectString);
}


bool DBconn::Connect(const string &connectString)
{
	LogMessage("Creating DB connection: " + connectString, LOG_DEBUG);
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
    basicConnectString=connectString;
    string dbname;

    int pos=basicConnectString.find("dbname=");
    if (pos == -1)
        dbname = "pgadmin";
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
    primaryConn = new DBconn(connectString, dbname);

    if (!primaryConn)
        LogMessage("Failed to create primary connection!", LOG_ERROR);

	primaryConn->inUse = true;

    return primaryConn;
}


DBconn *DBconn::Get(const string &dbname)
{
	DBconn *thisConn = primaryConn, *testConn;

    // find an existing connection
    do
    {
        if (dbname == thisConn->dbname && !thisConn->inUse)
		{
			LogMessage("Allocating existing connection to database " + thisConn->dbname, LOG_DEBUG);
			thisConn->inUse = true;
            return thisConn;
		}

		testConn = thisConn;
		if (thisConn->next != 0)
		    thisConn = thisConn->next;

    } while (testConn->next != 0);


	// No suitable connection was found, so create a new one.
    DBconn *newConn=new DBconn(dbname);
    if (newConn->conn)
    {
		LogMessage("Allocating new connection to database " + newConn->dbname, LOG_DEBUG);
		newConn->inUse = true;
		newConn->prev = thisConn;
		thisConn->next = newConn;
    }
	else
		LogMessage("Failed to create new connection to database: " + dbname, LOG_ERROR);

    return newConn;
}

void DBconn::Return()
{
	LogMessage("Returning connection to database " + this->dbname, LOG_DEBUG);
	inUse = false;
}

void DBconn::ClearConnections(bool all)
{
	if (all)
		LogMessage("Clearing all connections", LOG_DEBUG);
	else
		LogMessage("Clearing inactive connections", LOG_DEBUG);

	DBconn *thisConn=primaryConn, *deleteConn;

	// Find the last connection
	while (thisConn->next != 0)
		thisConn = thisConn->next;

	// Delete connections as required
	// If a connection is not in use, delete it, and reset the next and previous
	// pointers appropriately. If it is in use, don't touch it.
	while (thisConn->prev != 0)
	{
		if ((!thisConn->inUse) || all)
		{
			deleteConn = thisConn;

			thisConn = deleteConn->prev;
			
			thisConn->next = deleteConn->next;
			
			if (deleteConn->next)
				deleteConn->next->prev = deleteConn->prev;
			
			delete deleteConn;
		}
		else
		{
			thisConn = thisConn->prev;
		}
	}

	if (all)
		delete thisConn;

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

string DBconn::GetLastError() 
{ 
	// Return the last error message, minus any trailing line ends
	if (lastError.substr(lastError.length()-2, 2) == "\r\n") // DOS
	    return lastError.substr(0, lastError.length()-2); 
    else if (lastError.substr(lastError.length()-1, 1) == "\n") // Unix
	    return lastError.substr(0, lastError.length()-1);
    else if (lastError.substr(lastError.length()-1, 1) == "\r") // Mac
	    return lastError.substr(0, lastError.length()-1);
	else
		return lastError;
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
            LogMessage("Query error: " + conn->lastError, LOG_WARNING);
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

