//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2005, The pgAdmin Development Team
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
