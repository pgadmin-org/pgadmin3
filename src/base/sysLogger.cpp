//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id: sysLogger.cpp 3942 2005-01-10 15:26:30Z dpage $
// Copyright (C) 2002 - 2006, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// sysLogger.cpp - Log handling class
//
//////////////////////////////////////////////////////////////////////////


// wxWindows headers
#include <wx/wx.h>
#include <wx/ffile.h>
#include <wx/datetime.h>
#include <wx/log.h>
#include "copyright.h"      // needed for APPNAME_L caption

// App headers
#include "base/sysLogger.h"

wxLogLevel sysLogger::logLevel=LOG_ERRORS;
wxString sysLogger::logFile=wxT("debug.log");

// IMPLEMENT_LOG_FUNCTION(Sql) from wx../common/log.c
void wxVLogSql(const wxChar *szFormat, va_list argptr)
{
    static wxChar s_szBuf[1024];

    if (sysLogger::logLevel >= LOG_SQL)
    {
        wxVsnprintf(s_szBuf, WXSIZEOF(s_szBuf), szFormat, argptr);
        wxLog::OnLog(wxLOG_Sql, s_szBuf, time(NULL));

    }
}

void wxLogSql(const wxChar *szFormat, ...)
{
    va_list argptr;
    va_start(argptr, szFormat);
    wxVLogSql(szFormat, argptr);
    va_end(argptr);
}



void wxVLogNotice(const wxChar *szFormat, va_list argptr)
{
    static wxChar s_szBuf[1024];

    if (sysLogger::logLevel >= LOG_NOTICE)
    {
        wxVsnprintf(s_szBuf, WXSIZEOF(s_szBuf), szFormat, argptr);
        wxLog::OnLog(wxLOG_Sql, s_szBuf, time(NULL));

    }
}

void wxLogNotice(const wxChar *szFormat, ...)
{
    va_list argptr;
    va_start(argptr, szFormat);
    wxVLogNotice(szFormat, argptr);
    va_end(argptr);
}

void sysLogger::DoLog(wxLogLevel level, const wxChar *msg, time_t timestamp)
{
    wxString msgtype, preamble;
    int icon = 0;

    switch (level) {
        case wxLOG_FatalError:
            msgtype = wxT("FATAL  ");
            preamble = _("A fatal error has occured:\n\n");
            icon = wxICON_ERROR;
            break;

        case wxLOG_Error:
            msgtype = wxT("ERROR  ");
            preamble = _("An error has occured:\n\n");
            icon = wxICON_ERROR;
            break;

        case wxLOG_Warning:
            msgtype = wxT("WARNING");
            preamble = _("Warning:\n\n");
            icon = wxICON_EXCLAMATION;
            break;

        case wxLOG_Message:
            msgtype = wxT("MESSAGE");
            preamble = wxT("");
            icon = wxICON_INFORMATION;
            break;

        case wxLOG_Info:
            msgtype = wxT("INFO   ");
            break;

        case wxLOG_Status:
            msgtype = wxT("STATUS ");
            break;

        case wxLOG_Notice:
            msgtype = wxT("NOTICE ");
            break;

        case wxLOG_Sql:
            msgtype = wxT("QUERY  ");
            break;

        case wxLOG_Trace:
            msgtype = wxT("TRACE  ");
            break;

        case wxLOG_Debug:
            msgtype = wxT("DEBUG  ");
            break;

        default:
            msgtype = wxT("UNKNOWN");
            icon = wxICON_INFORMATION;
            break;
    }

    // Convert the timestamp
    wxDateTime *time = new wxDateTime(timestamp);
    wxString fullmsg;

    // Build the message.
    fullmsg << time->FormatISODate() << wxT(" ") << 
		time->FormatISOTime() << wxT(" ") << msgtype << wxT(": ") << msg;

	// Make sure to delete the time that we allocated
	delete time;

    // Display the message if required
    switch (logLevel)
    {
        case LOG_NONE:
            break;

        case LOG_ERRORS:
            if (level == wxLOG_FatalError || 
                level == wxLOG_Error)
                WriteLog(fullmsg);
            break;

        case LOG_NOTICE:
            if (level == wxLOG_FatalError || 
                level == wxLOG_Error ||
                level == wxLOG_Notice)
                WriteLog(fullmsg);
            break;

        case LOG_SQL:
            if (level == wxLOG_FatalError ||
                level == wxLOG_Error ||
                level == wxLOG_Message ||
                level == wxLOG_Status ||
                level == wxLOG_Notice ||
                level == wxLOG_Sql)
                WriteLog(fullmsg);
            break;

        case LOG_DEBUG:
            WriteLog(fullmsg);
            break;
    }

    // Display a messagebox if required.
    if (icon != 0) wxMessageBox(preamble + wxGetTranslation(msg), APPNAME_L, 
		wxOK | wxCENTRE | icon);
}


void sysLogger::WriteLog(const wxString& msg)
{
    wxString pid, logfile;

    // Disable logging to prevent recursion in the event of a problem
    wxLogNull foo;

    pid.Printf(wxT("%ld"), wxGetProcessId());
    logfile.Printf(wxT("%s"), logFile.c_str());
    logfile.Replace(wxT("%ID"), pid);

#if wxCHECK_VERSION(2,5,0)
    wxFFile file(logfile, wxT("a"));
#else
    wxFFile file(logfile, "a");
#endif

    if (!file.IsOpened()) {
        wxMessageBox(_("Cannot open the logfile!"), _("FATAL"), wxOK | wxCENTRE | wxICON_ERROR);
        return;
    }

   file.Write(msg + wxT("\n"));
   file.Close();
}

