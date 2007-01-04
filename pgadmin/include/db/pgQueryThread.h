//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id: pgSet.h 5466 2006-10-12 09:31:39Z dpage $
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// pgQueryThread.h - PostgreSQL threaded query class header
//
//////////////////////////////////////////////////////////////////////////

#ifndef PGQUERYTHREAD_H
#define PGQUERYTHREAD_H

class pgSet;

class pgQueryThread : public wxThread
{
public:
    pgQueryThread(pgConn *_conn, const wxString &qry, int resultToRetrieve=-1);
    ~pgQueryThread();

    virtual void *Entry();
    bool DataValid() const { return dataSet != NULL; }
    pgSet *DataSet() { return dataSet; }
    int ReturnCode() const { return rc; }
    long RowsInserted() const { return rowsInserted; }
    OID InsertedOid() const { return insertedOid; }
    wxString GetMessagesAndClear();
    void appendMessage(const wxString &str);

protected:
    int rc;
    int resultToRetrieve;
    long rowsInserted;
    OID insertedOid;

    wxString query;
    pgConn *conn;
    PGresult *result;
    wxString messages;
    pgSet *dataSet;
    wxCriticalSection criticalSection;

    int execute();
};

#endif
