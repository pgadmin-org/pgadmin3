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
class sysSettings
{
public:
    sysSettings();
    ~sysSettings();

    // frmMain size/position
    int GetFrmMainWidth();
    void SetFrmMainWidth(const int iNewVal);

    int GetFrmMainHeight();
    void SetFrmMainHeight(const int iNewVal);

    int GetFrmMainTop();
    void SetFrmMainTop(const int iNewVal);

    int GetFrmMainLeft();
    void SetFrmMainLeft(const int iNewVal);

    // Tip Of The Day
    bool GetShowTipOfTheDay();
    void SetShowTipOfTheDay(const bool bNewVal);
    int GetNextTipOfTheDay();
    void SetNextTipOfTheDay(const int iNewVal);

    // Log
    wxString GetLogFile() const;
    void SetLogFile(const wxString& szNewVal);
    int GetLogLevel();
    void SetLogLevel(const int iNewVal);

    // Last connection
    wxString GetLastServer() const;
    void SetLastServer(const wxString& szNewVal);
    wxString GetLastDatabase() const;
    void SetLastDatabase(const wxString& szNewVal);
    wxString GetLastUsername() const;
    void SetLastUsername(const wxString& szNewVal);
    int GetLastPort();
    void SetLastPort(const int iNewVal);

    // Show System Objects
    bool GetShowSystemObjects();
    void SetShowSystemObjects(const bool bNewShowSystemObjects);

    // The main config object
    wxConfig sysConfig;
private:

    // frmMain size/position
    int iFrmMainWidth;
    int iFrmMainHeight;
    int iFrmMainTop;
    int iFrmMainLeft;

    // Tip Of The Day
    bool bShowTipOfTheDay;
    int iNextTipOfTheDay;

    // Log
    wxString szLogFile;
    int iLogLevel;

    // Last connection
    wxString szLastServer;
    wxString szLastDatabase;
    wxString szLastUsername;
    int iLastPort;

    // Show System Objects
    bool bShowSystemObjects;
};

#endif
