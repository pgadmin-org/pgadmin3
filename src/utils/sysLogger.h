//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
//
// sysLogger.h - Log handling class
//
//////////////////////////////////////////////////////////////////////////

#ifndef SSYLOGGER_H
#define SYSLOGGER_H

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "../pgAdmin3.h"

enum LOG_LEVEL
{
    LOG_NONE = 0,
    LOG_ERRORS = 1,
    LOG_SQL = 2,
    LOG_DEBUG = 3
};

// Class declarations
class sysLogger : public wxLog
{
public:
    virtual void DoLog(wxLogLevel level, const wxChar *msg, time_t timestamp);

private:
    void WriteLog(const wxString& szMsg);
};

#endif
