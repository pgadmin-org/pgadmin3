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
    wxLogDebug(wxT("Creating sSettings object and loading settings"));

    // frMain size/position
    lFrmMainWidth = sysConfig.Read(wxT("frmMain/Width"), 750);
    lFrmMainHeight = sysConfig.Read(wxT("frmMain/Height"), 550);
    lFrmMainTop = sysConfig.Read(wxT("frmMain/Top"), 50);
    lFrmMainLeft = sysConfig.Read(wxT("frmMain/Left"), 50);

    // Tip Of The Day
    sysConfig.Read(wxT("ShowTipOfTheDay"), &bShowTipOfTheDay, TRUE); 
    sysConfig.Read(wxT("NextTipOfTheDay"), &lNextTipOfTheDay, 0); 

    // Log
    sysConfig.Read(wxT("LogFile"), &szLogFile, wxT("pgadmin.log")); 
    sysConfig.Read(wxT("LogLevel"), &lLogLevel, LOG_INFO); 
}


sysSettings::~sysSettings()
{
    wxLogDebug(wxT("Destroying sysSettings object and saving settings"));
    // frMain size/position
    sysConfig.Write(wxT("frmMain/Width"), lFrmMainWidth);
    sysConfig.Write(wxT("frmMain/Height"), lFrmMainHeight);
    sysConfig.Write(wxT("frmMain/Top"), lFrmMainTop);
    sysConfig.Write(wxT("frmMain/Left"), lFrmMainLeft);
}

//////////////////////////////////////////////////////////////////////////
// frmMain size/position
//////////////////////////////////////////////////////////////////////////

long sysSettings::GetFrmMainWidth()
{
    return lFrmMainWidth;
}
void sysSettings::SetFrmMainWidth(long lNewVal)
{
    lFrmMainWidth = lNewVal;
}

long sysSettings::GetFrmMainHeight()
{
    return lFrmMainHeight;
}
void sysSettings::SetFrmMainHeight(long lNewVal)
{
    lFrmMainHeight = lNewVal;
}

long sysSettings::GetFrmMainTop()
{
    return lFrmMainTop;
}
void sysSettings::SetFrmMainTop(long lNewVal)
{
    lFrmMainTop = lNewVal;
}

long sysSettings::GetFrmMainLeft()
{
    return lFrmMainLeft;
}
void sysSettings::SetFrmMainLeft(long lNewVal)
{
    lFrmMainLeft = lNewVal;
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

long sysSettings::GetNextTipOfTheDay()
{
    return lNextTipOfTheDay;
}
void sysSettings::SetNextTipOfTheDay(long lNewVal)
{
    lNextTipOfTheDay = lNewVal;
    sysConfig.Write(wxT("NextTipOfTheDay"), lNextTipOfTheDay);
}

//////////////////////////////////////////////////////////////////////////
// Log
//////////////////////////////////////////////////////////////////////////

wxString sysSettings::GetLogFile()
{
    return szLogFile;
}
void sysSettings::SetLogFile(wxString& szNewVal)
{
    szLogFile = szNewVal;
    sysConfig.Write(wxT("LogFile"), szLogFile);
}

long sysSettings::GetLogLevel()
{
    return lLogLevel;
}
void sysSettings::SetLogLevel(long lNewVal)
{
    lLogLevel = lNewVal;
    sysConfig.Write(wxT("LogLevel"), lLogLevel);
}