//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
//
// pgConn.h - PostgreSQL Connection class
//
//////////////////////////////////////////////////////////////////////////

#ifndef PGCONN_H
#define PGCONN_H

// PostgreSQL headers
#include <libpq-fe.h>

// App headers
#include "../../pgAdmin3.h"

// Class declarations
class pgConn
{
public:
    pgConn(wxString& szServer = wxString(""), wxString& szDatabase = wxString(""), wxString& szUsername = wxString(""), wxString& szPassword = wxString(""), long lPort = 0);
    ~pgConn();

private:
    PGconn *objConn;
};

#endif
