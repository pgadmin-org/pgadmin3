//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id: pgSet.cpp 4875 2006-01-06 21:06:46Z dpage $
// Copyright (C) 2002 - 2006, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// pgSet.cpp - PostgreSQL ResultSet class
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"
// wxWindows headers

// App headers
#include "pgSet.h"

pgSet::pgSet(PGresult *newRes, pgConnBase *newConn, wxMBConv &cnv, bool needColQt)
: pgSetBase(newRes, newConn, cnv, needColQt)
{
}


pgQueryThread::pgQueryThread(pgConnBase *_conn, const wxString &qry, int resultToRetrieve)
: pgQueryThreadBase(_conn, qry, resultToRetrieve)
{
}


pgSet *pgQueryThread::DataSet()
{
    return (pgSet*)pgQueryThreadBase::DataSet();
}
