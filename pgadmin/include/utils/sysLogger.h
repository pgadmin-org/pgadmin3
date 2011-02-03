//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2010, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// sysLogger.h - Log handling class
//
//////////////////////////////////////////////////////////////////////////

#ifndef SYSLOGGER_H
#define SYSLOGGER_H

// wxWindows headers
#include <wx/wx.h>

// App headers

enum LOG_LEVEL
{
	LOG_NONE = 0,
	LOG_ERRORS = 1,
	LOG_NOTICE = 2,
	LOG_SQL = 3,
	LOG_DEBUG = 4
};

// Class declarations
class sysLogger : public wxLog
{
public:
	virtual void DoLog(wxLogLevel level, const wxChar *msg, time_t timestamp);
	static wxLogLevel logLevel;
	static wxString logFile;

private:
	void WriteLog(const wxString &msg);
	bool SilenceMessage(const wxString &msg);
};

#define wxLOG_Notice (wxLOG_User+1)
#define wxLOG_Sql (wxLOG_User+2)
#define wxLOG_QuietError (wxLOG_User+3)
#define wxLOG_Script (wxLOG_User+4)
#define wxLOG_ScriptVerbose (wxLOG_User+5)

DECLARE_LOG_FUNCTION(Notice);
DECLARE_LOG_FUNCTION(Sql);
DECLARE_LOG_FUNCTION(QuietError);
DECLARE_LOG_FUNCTION(Script);
DECLARE_LOG_FUNCTION(ScriptVerbose);

#endif
