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
#include "pgAdmin3.h"
#include "sysSettings.h"
#include "sysLogger.h"

sysSettings::sysSettings(const wxString& szName) : wxConfig(szName)
{
    wxLogInfo(wxT("Creating sSettings object and loading settings"));

    // frMain size/position
    iFrmMainWidth = this->Read(wxT("frmMain/Width"), 750);
    iFrmMainHeight = this->Read(wxT("frmMain/Height"), 550);
    iFrmMainTop = this->Read(wxT("frmMain/Top"), 50);
    iFrmMainLeft = this->Read(wxT("frmMain/Left"), 50);

    // Tip Of The Day
    this->Read(wxT("ShowTipOfTheDay"), &bShowTipOfTheDay, TRUE); 
    this->Read(wxT("NextTipOfTheDay"), &iNextTipOfTheDay, 0); 

    // Log
    this->Read(wxT("LogFile"), &szLogFile, wxT("pgadmin.log")); 
    this->Read(wxT("LogLevel"), &iLogLevel, LOG_ERRORS);

    // Last Connection
    this->Read(wxT("LastServer"), &szLastServer, wxT("localhost")); 
    this->Read(wxT("LastDatabase"), &szLastDatabase, wxT("template1")); 
    this->Read(wxT("LastUsername"), &szLastUsername, wxT("postgres")); 
    this->Read(wxT("LastPort"), &iLastPort, 5432);

    // Show System Objects
    this->Read(wxT("ShowSystemObjects"), &bShowSystemObjects, FALSE); 

}


sysSettings::~sysSettings()
{
    wxLogInfo(wxT("Destroying sysSettings object and saving settings"));
    // frMain size/position
    this->Write(wxT("frmMain/Width"), iFrmMainWidth);
    this->Write(wxT("frmMain/Height"), iFrmMainHeight);
    this->Write(wxT("frmMain/Top"), iFrmMainTop);
    this->Write(wxT("frmMain/Left"), iFrmMainLeft);
}

//////////////////////////////////////////////////////////////////////////
// frmMain size/position
//////////////////////////////////////////////////////////////////////////

int sysSettings::GetFrmMainWidth()
{
    return iFrmMainWidth;
}
void sysSettings::SetFrmMainWidth(const int iNewVal)
{
    iFrmMainWidth = iNewVal;
}

int sysSettings::GetFrmMainHeight()
{
    return iFrmMainHeight;
}
void sysSettings::SetFrmMainHeight(const int iNewVal)
{
    iFrmMainHeight = iNewVal;
}

int sysSettings::GetFrmMainTop()
{
    return iFrmMainTop;
}
void sysSettings::SetFrmMainTop(const int iNewVal)
{
    iFrmMainTop = iNewVal;
}

int sysSettings::GetFrmMainLeft()
{
    return iFrmMainLeft;
}
void sysSettings::SetFrmMainLeft(const int iNewVal)
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
void sysSettings::SetShowTipOfTheDay(const bool bNewVal)
{
    bShowTipOfTheDay = bNewVal;
    this->Write(wxT("ShowTipOfTheDay"), bShowTipOfTheDay);
}

int sysSettings::GetNextTipOfTheDay()
{
    return iNextTipOfTheDay;
}
void sysSettings::SetNextTipOfTheDay(const int iNewVal)
{
    iNextTipOfTheDay = iNewVal;
    this->Write(wxT("NextTipOfTheDay"), iNextTipOfTheDay);
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
    this->Write(wxT("LogFile"), szLogFile);
}

int sysSettings::GetLogLevel()
{
    return iLogLevel;
}
void sysSettings::SetLogLevel(const int iNewVal)
{
    iLogLevel = iNewVal;
    this->Write(wxT("LogLevel"), iLogLevel);
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
    this->Write(wxT("LastServer"), szLastServer);
}

wxString sysSettings::GetLastDatabase() const
{
    return szLastDatabase;
}
void sysSettings::SetLastDatabase(const wxString& szNewVal)
{
    szLastDatabase = szNewVal;
    this->Write(wxT("LastDatabase"), szLastDatabase);
}

wxString sysSettings::GetLastUsername() const
{
    return szLastUsername;
}
void sysSettings::SetLastUsername(const wxString& szNewVal)
{
    szLastUsername = szNewVal;
    this->Write(wxT("LastUsername"), szLastUsername);
}

int sysSettings::GetLastPort()
{
    return iLastPort;
}
void sysSettings::SetLastPort(const int iNewVal)
{
    iLastPort = iNewVal;
    this->Write(wxT("LastPort"), iLastPort);
}

//////////////////////////////////////////////////////////////////////////
// Show System Objects
//////////////////////////////////////////////////////////////////////////

bool sysSettings::GetShowSystemObjects()
{
    return bShowSystemObjects;
}
void sysSettings::SetShowSystemObjects(const bool bNewVal)
{
    bShowSystemObjects = bNewVal;
    this->Write(wxT("ShowSystemObjects"), bShowSystemObjects);
}