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
#include <wx/datetime.h>

// App headers
#include "../pgAdmin3.h"
#include "sysLogger.h"

void sysLogger::DoLog(wxLogLevel level, const wxChar *msg, time_t timestamp)
{
  wxString szMsg = "DoLog:\n\nMessage: ";
  szMsg.Append(msg);
  szMsg.Append("\nLevel: ");
  szMsg.Append(level);
  szMsg.Append("\nTimestamp: ");
  szMsg.Append(timestamp);
  wxMessageBox(szMsg);
}

//void sysLogger::DoLogString(const wxChar *msg, time_t timestamp)
//{
//    wxDateTime *dtTimestamp = new wxDateTime(timestamp);
//    wxString szMsg = "DoLogString:\n\nMessage: ";
//    szMsg.Append(msg);
//    szMsg.Append("\nTimestamp: ");
//    szMsg.Append(dtTimestamp->FormatISODate());
//    szMsg.Append(" ");
//    szMsg.Append(dtTimestamp->FormatISOTime());
//    wxMessageBox(szMsg);
//}
