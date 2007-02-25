//////////////////////////////////////////////////////////////////////////
//
// pgAgent - PostgreSQL Tools
// $Id: pgAgent.h 4875 2006-01-06 21:06:46Z dpage $
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// pgAgent.h - main include
//
//////////////////////////////////////////////////////////////////////////


#ifndef PGAGENT_H
#define PGAGENT_H

#include <wx/wx.h>

#include "misc.h"
#include "connection.h"
#include "job.h"

extern long longWait;
extern long shortWait;
extern long minLogLevel;
extern wxString connectString;
extern wxString serviceDBname;
extern wxString backendPid;

#ifndef __WXMSW__
extern bool runInForeground;
extern wxString logFile;
#endif

// Log levels
enum
{
	LOG_ERROR = 0,
	LOG_WARNING,
	LOG_DEBUG
};

// Prototypes
void LogMessage(wxString msg, int level);
void MainLoop();

#ifdef __WIN32__
void CheckForInterrupt();
HANDLE win32_popen_r(const TCHAR *command);
#endif

#endif // PGAGENT_H

