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
#include "sysSettings.h"

// Application Versions & Name
#define VERSION wxT("0.1.1 Devel")
#define APPNAME_L wxT("pgAdmin III")
#define APPNAME_S wxT("pgadmin3")
const float SERVER_MIN_VERSION = 7.3f;

// XRC Path
#ifdef __WXMSW__
  #define XRC_PATH wxT("/ui/win32")
#else
  #define XRC_PATH wxT("/ui/gtk")
#endif
#define XRC_COMMON_PATH wxT("/ui/common")

// Class declarations
class pgAdmin3 : public wxApp
{
public:
    virtual bool OnInit();
    virtual int OnExit();

private:
    bool LoadAllXrc(const wxString dir);
};


#endif // PGADMIN3_H
