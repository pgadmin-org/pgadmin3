//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
//
// pgUser.h - PostgreSQL User
//
//////////////////////////////////////////////////////////////////////////

#ifndef PGUSER_H
#define PGUSER_H

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "../../pgAdmin3.h"
#include "../../db/pg/pgConn.h"
#include "pgObject.h"
#include "pgServer.h"

// Class declarations
class pgUser : public pgObject
{
public:
    pgUser();
    ~pgUser();
    int GetType();
    wxString GetTypeName() const;
    pgServer GetServer();

    // User Specific
    int GetUserID();

private:
    pgServer objServer;
    int iUserID;

};

#endif
