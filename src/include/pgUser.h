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
#include "pgAdmin3.h"
#include "pgConn.h"
#include "pgObject.h"
#include "pgServer.h"

// Class declarations
class pgUser : public pgObject
{
public:
    pgUser(const wxString& newName = wxString(""));
    ~pgUser();
    pgServer *GetServer();
    void SetServer(pgServer *newServer);

    // User Specific
    int GetUserID();

private:
    int userId;
    pgServer *server;
};

#endif
