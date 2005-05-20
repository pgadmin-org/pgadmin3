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

DBconn *DBconn::primaryConn;
wxString DBconn::basicConnectString;
static wxMutex s_PoolLock;

DBconn::DBconn(const wxString &name)
{
    dbname = name;
	inUse = false;
	next=0;
	prev=0;
    Connect(basicConnectString  + wxT(" dbname=") + dbname);
}


DBconn::DBconn(const wxString &connectString, const wxString &name)
{
    dbname = name;
	inUse = false;
	next=0;
	prev=0;
    Connect(connectString);
}


bool DBconn::Connect(const wxString &connectString)
{
	LogMessage(_("Creating DB connection: ") + connectString, LOG_DEBUG);
	wxCharBuffer cstrUTF=connectString.mb_str(wxConvUTF8);
    conn=PQconnectdb(cstrUTF);
    if (PQstatus(conn) == CONNECTION_OK)
    {
    }
    else
    {
        lastError = wxString::FromAscii(PQerrorMessage(conn));
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


DBconn *DBconn::InitConnection(const wxString &connectString)
{
	wxMutexLocker lock(s_PoolLock);

    basicConnectString=connectString;
    wxString dbname;

    int pos=basicConnectString.find(wxT("dbname="));
    if (pos == -1)
        dbname = wxT("pgadmin");
    else
    {
        dbname = basicConnectString.substr(pos+7);
        basicConnectString = basicConnectString.substr(0, pos);
        pos = dbname.find(wxT(" "));
        if (pos != -1)
        {
            if (basicConnectString.length())
                basicConnectString += wxT(" ");
            basicConnectString += dbname.substr(pos+1);
            dbname = dbname.substr(0, pos);
        }
    }
    primaryConn = new DBconn(connectString, dbname);

    if (!primaryConn)
        LogMessage(_("Failed to create primary connection!"), LOG_ERROR);

	primaryConn->inUse = true;

    return primaryConn;
}


DBconn *DBconn::Get(const wxString &dbname)
{
	wxMutexLocker lock(s_PoolLock);

	DBconn *thisConn = primaryConn, *testConn;

    // find an existing connection
    do
    {
        if (dbname == thisConn->dbname && !thisConn->inUse)
		{
			LogMessage(_("Allocating existing connection to database ") + thisConn->dbname, LOG_DEBUG);
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
		LogMessage(_("Allocating new connection to database ") + newConn->dbname, LOG_DEBUG);
		newConn->inUse = true;
		newConn->prev = thisConn;
		thisConn->next = newConn;
    }
	else
		LogMessage(_("Failed to create new connection to database: ") + dbname, LOG_ERROR);

    return newConn;
}

void DBconn::Return()
{
	wxMutexLocker lock(s_PoolLock);

	LogMessage(_("Returning connection to database ") + this->dbname, LOG_DEBUG);
	inUse = false;
}

void DBconn::ClearConnections(bool all)
{
	wxMutexLocker lock(s_PoolLock);

	if (all)
		LogMessage(_("Clearing all connections"), LOG_DEBUG);
	else
		LogMessage(_("Clearing inactive connections"), LOG_DEBUG);

	DBconn *thisConn=primaryConn, *deleteConn;
	int total=0, free=0, deleted=0;

	if (thisConn)
	{

		total++;

		// Find the last connection
		while (thisConn->next != 0)
		{
			total++;
			
			if (!thisConn->inUse)
				free++;

			thisConn = thisConn->next;
		}
		if (!thisConn->inUse)
			free++;

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
				deleted++;
			}
			else
			{
				thisConn = thisConn->prev;
			}
		}

		if (all)
		{
			delete thisConn;
			deleted++;
		}

		wxString tmp;
		tmp.Printf(_("Connection stats: total - %d, free - %d, deleted - %d"), total, free, deleted);
		LogMessage(tmp, LOG_DEBUG);

	}
	else
		LogMessage(_("No connections found!"), LOG_DEBUG);

}


DBresult *DBconn::Execute(const wxString &query)
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


int DBconn::ExecuteVoid(const wxString &query)
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

wxString DBconn::GetLastError() 
{ 
	// Return the last error message, minus any trailing line ends
	if (lastError.substr(lastError.length()-2, 2) == wxT("\r\n")) // DOS
	    return lastError.substr(0, lastError.length()-2); 
    else if (lastError.substr(lastError.length()-1, 1) == wxT("\n")) // Unix
	    return lastError.substr(0, lastError.length()-1);
    else if (lastError.substr(lastError.length()-1, 1) == wxT("\r")) // Mac
	    return lastError.substr(0, lastError.length()-1);
	else
		return lastError;
}

///////////////////////////////////////////////////////7



DBresult::DBresult(DBconn *conn, const wxString &query)
{
    wxCharBuffer cstrUTF=query.mb_str(wxConvUTF8);
    result=PQexec(conn->conn, cstrUTF);
    currentRow=0;
    maxRows=0;

    if (result)
    {
        int rc=PQresultStatus(result);
        if (rc == PGRES_TUPLES_OK)
            maxRows = PQntuples(result);
        else if (rc != PGRES_COMMAND_OK)
        {
            conn->lastError = wxString::FromAscii(PQerrorMessage(conn->conn));
            LogMessage(wxT("Query error: ") + conn->lastError, LOG_WARNING);
            PQclear(result);
            result=0;
        }
    }
	else
		conn->lastError = wxString::FromAscii(PQerrorMessage(conn->conn));

}


DBresult::~DBresult()
{
    if (result)
        PQclear(result);
}


wxString DBresult::GetString(int col) const
{
    wxString str;

    if (result && currentRow < maxRows && col >= 0)
    {
        str = wxString::FromAscii(PQgetvalue(result, currentRow, col));
    }
    return str;
}


wxString DBresult::GetString(const wxString &colname) const
{
    wxCharBuffer cstrUTF=colname.mb_str(wxConvUTF8);
    int col=PQfnumber(result, cstrUTF);
    if (col < 0)
    {
        // fatal: not found
        return wxT("");
    }
    return GetString(col);
}

