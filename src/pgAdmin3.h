//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
//
// pgAdmin3.h - The main application header
//
//////////////////////////////////////////////////////////////////////////

#ifndef PGADMIN3_H
#define PGADMIN3_H

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "utils/sysSettings.h"

// Application Version & Name
#define VERSION "1.0.0 Devel"
#define APPNAME "pgAdmin III"

// Class declarations
class pgAdmin3 : public wxApp
{
public:
    virtual bool OnInit();
    virtual int OnExit();
};

#endif
