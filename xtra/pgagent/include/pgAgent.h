//////////////////////////////////////////////////////////////////////////
//
// pgAgent - PostgreSQL Tools
// $Id$
// Copyright (C) 2003 - 2006 The pgAdmin Development Team
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

#ifndef _WIN32_
extern bool runInForeground;
#endif

// Log levels
enum
{
	LOG_ERROR = 0,
	LOG_WARNING,
	LOG_DEBUG
};

// Prototypes
void CheckForInterrupt();
void LogMessage(wxString msg, int level);
void MainLoop();

#endif // PGAGENT_H

