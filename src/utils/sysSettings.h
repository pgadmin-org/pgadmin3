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
    long GetFrmMainWidth();
    void SetFrmMainWidth(long lNewVal);

    long GetFrmMainHeight();
    void SetFrmMainHeight(long lNewVal);

    long GetFrmMainTop();
    void SetFrmMainTop(long lNewVal);

    long GetFrmMainLeft();
    void SetFrmMainLeft(long lNewVal);

    // Tip Of The Day
    bool GetShowTipOfTheDay();
    void SetShowTipOfTheDay(bool bNewVal);
    long GetNextTipOfTheDay();
    void SetNextTipOfTheDay(long lNewVal);

    // Log
    wxString GetLogFile();
    void SetLogFile(wxString szNewVal);
    long GetLogLevel();
    void SetLogLevel(long lNewVal);
private:

    // The main config object
    wxConfig sysConfig;

    // frmMain size/position
    long lFrmMainWidth;
    long lFrmMainHeight;
    long lFrmMainTop;
    long lFrmMainLeft;

    // Tip Of The Day
    bool bShowTipOfTheDay;
    long lNextTipOfTheDay;

    // Log
    wxString szLogFile;
    long lLogLevel;
};

#endif
