//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
//
// sysLogger.h - Log handling class
//
//////////////////////////////////////////////////////////////////////////

#ifndef SQLLOGGER_H
#define SQLLOGGER_H

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "../pgAdmin3.h"

// Class declarations
class sysLogger : public wxLog
{
public:
    //virtual void DoLog(wxLogLevel level, const wxChar *msg, time_t timestamp);
    virtual void DoLogString(const wxChar *msg, time_t timestamp);
};

#endif
