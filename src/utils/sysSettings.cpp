//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
//
// sysSettings.cpp - Settings handling class
//
// Note: This class stores and manages all the applications settings.
//       Settings are all read in the ctor, but may be written either in
//       the relevant SetXXX() member function for rarely written settings
//       or in the dtor for reguarly changed settings such as form sizes.
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>
#include <wx/config.h>

// App headers
#include "../pgAdmin3.h"
#include "sysSettings.h"
#include "sysLogger.h"

#ifdef __WXMSW__
sysSettings::sysSettings() : sysConfig(APPNAME_L)
#else
sysSettings::sysSettings() : sysConfig(APPNAME_S)
#endif
{
    wxLogInfo(wxT("Creating sSettings object and loading settings"));

    // frMain size/position
    iFrmMainWidth = sysConfig.Read(wxT("frmMain/Width"), 750);
    iFrmMainHeight = sysConfig.Read(wxT("frmMain/Height"), 550);
    iFrmMainTop = sysConfig.Read(wxT("frmMain/Top"), 50);
    iFrmMainLeft = sysConfig.Read(wxT("frmMain/Left"), 50);

    // Tip Of The Day
    sysConfig.Read(wxT("ShowTipOfTheDay"), &bShowTipOfTheDay, TRUE); 
    sysConfig.Read(wxT("NextTipOfTheDay"), &iNextTipOfTheDay, 0); 

    // Log
    sysConfig.Read(wxT("LogFile"), &szLogFile, wxT("pgadmin.log")); 
    sysConfig.Read(wxT("LogLevel"), &iLogLevel, LOG_INFO);

    // Last Connection
    sysConfig.Read(wxT("Last Server"), &szLastServer, wxT("localhost")); 
    sysConfig.Read(wxT("Last Database"), &szLastDatabase, wxT("template1")); 
    sysConfig.Read(wxT("Last Username"), &szLastUsername, wxT("postgres")); 
    sysConfig.Read(wxT("Last Port"), &iLastPort, 5432);

}


sysSettings::~sysSettings()
{
    wxLogInfo(wxT("Destroying sysSettings object and saving settings"));
    // frMain size/position
    sysConfig.Write(wxT("frmMain/Width"), iFrmMainWidth);
    sysConfig.Write(wxT("frmMain/Height"), iFrmMainHeight);
    sysConfig.Write(wxT("frmMain/Top"), iFrmMainTop);
    sysConfig.Write(wxT("frmMain/Left"), iFrmMainLeft);
}

//////////////////////////////////////////////////////////////////////////
// frmMain size/position
//////////////////////////////////////////////////////////////////////////

int sysSettings::GetFrmMainWidth()
{
    return iFrmMainWidth;
}
void sysSettings::SetFrmMainWidth(int iNewVal)
{
    iFrmMainWidth = iNewVal;
}

int sysSettings::GetFrmMainHeight()
{
    return iFrmMainHeight;
}
void sysSettings::SetFrmMainHeight(int iNewVal)
{
    iFrmMainHeight = iNewVal;
}

int sysSettings::GetFrmMainTop()
{
    return iFrmMainTop;
}
void sysSettings::SetFrmMainTop(int iNewVal)
{
    iFrmMainTop = iNewVal;
}

int sysSettings::GetFrmMainLeft()
{
    return iFrmMainLeft;
}
void sysSettings::SetFrmMainLeft(int iNewVal)
{
    iFrmMainLeft = iNewVal;
}

//////////////////////////////////////////////////////////////////////////
// Tip of the Day
//////////////////////////////////////////////////////////////////////////

bool sysSettings::GetShowTipOfTheDay()
{
    return bShowTipOfTheDay;
}
void sysSettings::SetShowTipOfTheDay(bool bNewVal)
{
    bShowTipOfTheDay = bNewVal;
    sysConfig.Write(wxT("ShowTipOfTheDay"), bShowTipOfTheDay);
}

int sysSettings::GetNextTipOfTheDay()
{
    return iNextTipOfTheDay;
}
void sysSettings::SetNextTipOfTheDay(int iNewVal)
{
    iNextTipOfTheDay = iNewVal;
    sysConfig.Write(wxT("NextTipOfTheDay"), iNextTipOfTheDay);
}

//////////////////////////////////////////////////////////////////////////
// Log
//////////////////////////////////////////////////////////////////////////

wxString sysSettings::GetLogFile() const
{
    return szLogFile;
}
void sysSettings::SetLogFile(const wxString& szNewVal)
{
    szLogFile = szNewVal;
    sysConfig.Write(wxT("LogFile"), szLogFile);
}

int sysSettings::GetLogLevel()
{
    return iLogLevel;
}
void sysSettings::SetLogLevel(int iNewVal)
{
    iLogLevel = iNewVal;
    sysConfig.Write(wxT("LogLevel"), iLogLevel);
}

//////////////////////////////////////////////////////////////////////////
// Last Connection
//////////////////////////////////////////////////////////////////////////

wxString sysSettings::GetLastServer() const
{
    return szLastServer;
}
void sysSettings::SetLastServer(const wxString& szNewVal)
{
    szLastServer = szNewVal;
    sysConfig.Write(wxT("Last Server"), szLastServer);
}

wxString sysSettings::GetLastDatabase() const
{
    return szLastDatabase;
}
void sysSettings::SetLastDatabase(const wxString& szNewVal)
{
    szLastDatabase = szNewVal;
    sysConfig.Write(wxT("Last Database"), szLastDatabase);
}

wxString sysSettings::GetLastUsername() const
{
    return szLastUsername;
}
void sysSettings::SetLastUsername(const wxString& szNewVal)
{
    szLastUsername = szNewVal;
    sysConfig.Write(wxT("Last Username"), szLastUsername);
}

int sysSettings::GetLastPort()
{
    return iLastPort;
}
void sysSettings::SetLastPort(int iNewVal)
{
    iLastPort = iNewVal;
    sysConfig.Write(wxT("Last Port"), iLastPort);
}
