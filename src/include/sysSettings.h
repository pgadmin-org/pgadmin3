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
    int GetFrmMainWidth() const { return frmMainWidth; }
    void SetFrmMainWidth(const int newval) { frmMainWidth = newval; }

    int GetFrmMainHeight() const { return frmMainHeight; }
    void SetFrmMainHeight(const int newval) { frmMainHeight = newval; }

    int GetFrmMainTop() const { return frmMainTop; }
    void SetFrmMainTop(const int newval) { frmMainTop = newval; }

    int GetFrmMainLeft() const { return frmMainLeft; }
    void SetFrmMainLeft(const int newval) { frmMainLeft = newval; }

    // frmQueryBuilder size/position
    wxSize GetFrmQueryBuilderSize();
    wxPoint GetFrmQueryBuilderPos();
	void SetFrmQueryBuilderPos(wxPoint pos);
	void SetFrmQueryBuilderSize(wxSize size);

    // Tip Of The Day
    bool GetShowTipOfTheDay() const { return showTipOfTheDay; }
    void SetShowTipOfTheDay(const bool newval);
    int GetNextTipOfTheDay() const { return nextTipOfTheDay; }
    void SetNextTipOfTheDay(const int newval);

    // Log
    wxString GetLogFile() const { return logFile; }
    void SetLogFile(const wxString& newval);
    int GetLogLevel() const { return logLevel; }
    void SetLogLevel(const int newval);

    // Last connection
    wxString GetLastServer() const { return lastServer; }
    void SetLastServer(const wxString& newval);
    wxString GetLastDatabase() const { return lastDatabase; }
    void SetLastDatabase(const wxString& newval);
    wxString GetLastUsername() const { return lastUsername; }
    void SetLastUsername(const wxString& newval);
    int GetLastPort() const { return lastPort; }
    void SetLastPort(const int newval);

    // Show System Objects
    bool GetShowSystemObjects() const { return showSystemObjects; }
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
