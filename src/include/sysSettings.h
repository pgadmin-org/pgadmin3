//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
//
// sysSettings.h - Settings handling class
//
//////////////////////////////////////////////////////////////////////////

#ifndef SYSSETTINGS_H
#define SYSSETTINGS_H

// wxWindows headers
#include <wx/wx.h>
#include <wx/config.h>

// App headers
#include "pgAdmin3.h"

// Class declarations
class sysSettings : public wxConfig
{
public:
    sysSettings(const wxString& name);
    ~sysSettings();

    // frmMain size/position
    int GetFrmMainWidth();
    void SetFrmMainWidth(const int newval);

    int GetFrmMainHeight();
    void SetFrmMainHeight(const int newval);

    int GetFrmMainTop();
    void SetFrmMainTop(const int newval);

    int GetFrmMainLeft();
    void SetFrmMainLeft(const int newval);

    // Tip Of The Day
    bool GetShowTipOfTheDay();
    void SetShowTipOfTheDay(const bool newval);
    int GetNextTipOfTheDay();
    void SetNextTipOfTheDay(const int newval);

    // Log
    wxString GetLogFile() const;
    void SetLogFile(const wxString& newval);
    int GetLogLevel();
    void SetLogLevel(const int newval);

    // Last connection
    wxString GetLastServer() const;
    void SetLastServer(const wxString& newval);
    wxString GetLastDatabase() const;
    void SetLastDatabase(const wxString& newval);
    wxString GetLastUsername() const;
    void SetLastUsername(const wxString& newval);
    int GetLastPort();
    void SetLastPort(const int newval);

    // Show System Objects
    bool GetShowSystemObjects();
    void SetShowSystemObjects(const bool newval);

private:

    // frmMain size/position
    int frmMainWidth;
    int frmMainHeight;
    int frmMainTop;
    int frmMainLeft;

    // Tip Of The Day
    bool showTipOfTheDay;
    int nextTipOfTheDay;

    // Log
    wxString logFile;
    int logLevel;

    // Last connection
    wxString lastServer;
    wxString lastDatabase;
    wxString lastUsername;
    int lastPort;

    // Show System Objects
    bool showSystemObjects;
};

#endif
