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
#include "../pgAdmin3.h"

// Class declarations
class sysSettings
{
public:
    sysSettings();
    ~sysSettings();

    // frmMain size/position
    int GetFrmMainWidth();
    void SetFrmMainWidth(int iNewVal);

    int GetFrmMainHeight();
    void SetFrmMainHeight(int iNewVal);

    int GetFrmMainTop();
    void SetFrmMainTop(int iNewVal);

    int GetFrmMainLeft();
    void SetFrmMainLeft(int iNewVal);

    // Tip Of The Day
    bool GetShowTipOfTheDay();
    void SetShowTipOfTheDay(bool bNewVal);
    int GetNextTipOfTheDay();
    void SetNextTipOfTheDay(int iNewVal);

    // Log
    wxString GetLogFile() const;
    void SetLogFile(const wxString& szNewVal);
    int GetLogLevel();
    void SetLogLevel(int iNewVal);
private:

    // The main config object
    wxConfig sysConfig;

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
};

#endif
