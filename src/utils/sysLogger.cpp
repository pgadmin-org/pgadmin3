//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
//
// sysLogger.cpp - Log handling class
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>
#include <wx/ffile.h>
#include <wx/datetime.h>

// App headers
#include "pgAdmin3.h"
#include "sysLogger.h"

void sysLogger::DoLog(wxLogLevel level, const wxChar *msg, time_t timestamp)
{
    extern sysSettings *settings;
    wxString msgtype, preamble;
    int icon = 0;

    switch (level) {
        case wxLOG_FatalError:
            msgtype = wxT("FATAL  ");
            preamble = wxT("A fatal error has occured:\n\n");
            icon = wxICON_ERROR;
            break;

        case wxLOG_Error:
            msgtype = wxT("ERROR  ");
            preamble = wxT("An error has occured:\n\n");
            icon = wxICON_ERROR;
            break;

        case wxLOG_Warning:
            msgtype = wxT("WARNING");
            preamble = wxT("Warning:\n\n");
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

        case wxLOG_Trace:
            msgtype = wxT("TRACE   ");
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
    fullmsg << time->FormatISODate() << " " << time->FormatISOTime() << " " << msgtype << ": " << msg;

    // Display the message if required
    switch (settings->GetLogLevel()) {
        case LOG_NONE:
            break;

        case LOG_ERRORS:
            if ((level == wxLOG_FatalError) || \
                (level == wxLOG_Error)) WriteLog(fullmsg);
            break;

        case LOG_SQL:
            if ((level == wxLOG_FatalError) || \
                (level == wxLOG_Error) || \
                (level == wxLOG_Message) || \
                (level == wxLOG_Status)) WriteLog(fullmsg);
            break;

        case LOG_DEBUG:
            WriteLog(fullmsg);
            break;
    }

    // Display a messagebox if required.
    if (icon != 0) wxMessageBox(preamble + msg, APPNAME_L, wxOK | wxCENTRE | icon);
}

void sysLogger::WriteLog(const wxString& msg)
{
    extern sysSettings *settings;
    wxString pid, logfile;

    pid.Printf("%d", wxGetProcessId());
    logfile.Printf("%s", settings->GetLogFile().c_str());
    logfile.Replace("%ID", pid);

    wxFFile file(logfile, "a");
    if (!file.IsOpened()) {
        wxMessageBox(wxT("Cannot open the logfile!"), wxT("FATAL"), wxOK | wxCENTRE | wxICON_ERROR);
        return;
    }

   file.Write(msg + "\n");
   file.Close();
}