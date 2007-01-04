//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// pgSet.h - PostgreSQL ResultSet class
//
//////////////////////////////////////////////////////////////////////////

#ifndef PGSET_H
#define PGSET_H
#include "base/pgSetBase.h"

class pgSet : public pgSetBase
{
public:
    pgSet(PGresult *newRes, pgConnBase *newConn, wxMBConv &cnv, bool needColQt);
};

class pgQueryThread : public pgQueryThreadBase
{
public:
    pgQueryThread(pgConnBase *_conn, const wxString &qry, int resultToRetrieve=-1);
    pgSet *DataSet();
};

#endif
