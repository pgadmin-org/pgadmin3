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
#include "../pgAdmin3.h"
#include "sysLogger.h"

void sysLogger::DoLog(wxLogLevel level, const wxChar *msg, time_t timestamp)
{
    extern sysSettings *objSettings;
    wxString szType, szPreamble;
    int iIcon;

    switch (level) {
        case wxLOG_FatalError:
            szType = wxT("FATAL");
            szPreamble = wxT("A fatal error has occured:\n\n");
            iIcon = wxICON_ERROR;
            break;
        case wxLOG_Error:
            szType = wxT("ERROR");
            szPreamble = wxT("An error has occured:\n\n");
            iIcon = wxICON_ERROR;
            break;
        case wxLOG_Warning:
            szType = wxT("WARNING");
            szPreamble = wxT("Warning:\n\n");
            iIcon = wxICON_EXCLAMATION;
            break;
        case wxLOG_Message:
            szType = wxT("MESSAGE");
            szPreamble = wxT("Message:\n\n");
            iIcon = wxICON_INFORMATION;
            break;
        case wxLOG_Info:
            szType = wxT("INFO");
            iIcon = 0;
            break;
        case wxLOG_Status:
            szType = wxT("STATUS");
            iIcon = 0;
            break;
        case wxLOG_Trace:
            szType = wxT("TRACE");
            iIcon = 0;
            break;
        case wxLOG_Debug:
            szType = wxT("DEBUG");
            iIcon = 0;
            break;
        default:
            szType = wxT("UNKNOWN");
            iIcon = wxICON_INFORMATION;
            break;
    }

    // Convert the timestamp
    wxDateTime *dtTimestamp = new wxDateTime(timestamp);
    wxString szMsg;

    // Build the message.
    szMsg << dtTimestamp->FormatISODate() << " " << dtTimestamp->FormatISOTime() << " " << szType << ": " << msg;

    // Display the message if required
    switch (objSettings->GetLogLevel()) {
        case LOG_NONE:
            break;

        case LOG_ERRORS:
            if ((level == wxLOG_FatalError) || \
                (level == wxLOG_Error)) WriteLog(szMsg);
            break;

        case LOG_INFO:
            if ((level == wxLOG_FatalError) || \
                (level == wxLOG_Error) || \
                (level == wxLOG_Message) || \
                (level == wxLOG_Info) || \
                (level == wxLOG_Status)) WriteLog(szMsg);
            break;

        case LOG_DEBUG:
            WriteLog(szMsg);
            break;
    }

    // Display a messagebox if required.
    if (iIcon != 0) wxMessageBox(szPreamble + msg, szType, wxOK | wxCENTRE | iIcon);
}

void sysLogger::WriteLog(wxString szMsg)
{
    extern sysSettings *objSettings;
    wxFFile fpLog(objSettings->GetLogFile(), "a");
    if (!fpLog.IsOpened()) {
        wxMessageBox(wxT("Cannot open the logfile!"), wxT("FATAL"), wxOK | wxCENTRE | wxICON_ERROR);
        return;
    }

   fpLog.Write(szMsg + "\n");
   fpLog.Close();
}