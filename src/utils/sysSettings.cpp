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

sysSettings::sysSettings(const wxString& name) : wxConfig(name)
{
    wxLogInfo(wxT("Creating sSettings object and loading settings"));

    // frMain size/position
    frmMainWidth = this->Read(wxT("frmMain/Width"), 750);
    frmMainHeight = this->Read(wxT("frmMain/Height"), 550);
    frmMainTop = this->Read(wxT("frmMain/Top"), 50);
    frmMainLeft = this->Read(wxT("frmMain/Left"), 50);

    // Tip Of The Day
    this->Read(wxT("ShowTipOfTheDay"), &showTipOfTheDay, TRUE); 
    this->Read(wxT("NextTipOfTheDay"), &nextTipOfTheDay, 0); 

    // Log
    this->Read(wxT("LogFile"), &logFile, wxT("pgadmin.log")); 
    this->Read(wxT("LogLevel"), &logLevel, LOG_ERRORS);

    // Last Connection
    this->Read(wxT("LastServer"), &lastServer, wxT("localhost")); 
    this->Read(wxT("LastDatabase"), &lastDatabase, wxT("template1")); 
    this->Read(wxT("LastUsername"), &lastUsername, wxT("postgres")); 
    this->Read(wxT("LastPort"), &lastPort, 5432);

    // Show System Objects
    this->Read(wxT("ShowSystemObjects"), &showSystemObjects, FALSE); 

}


sysSettings::~sysSettings()
{
    wxLogInfo(wxT("Destroying sysSettings object and saving settings"));
    // frMain size/position
    this->Write(wxT("frmMain/Width"), frmMainWidth);
    this->Write(wxT("frmMain/Height"), frmMainHeight);
    this->Write(wxT("frmMain/Top"), frmMainTop);
    this->Write(wxT("frmMain/Left"), frmMainLeft);
}

//////////////////////////////////////////////////////////////////////////
// Tip of the Day
//////////////////////////////////////////////////////////////////////////

void sysSettings::SetShowTipOfTheDay(const bool newval)
{
    showTipOfTheDay = newval;
    this->Write(wxT("ShowTipOfTheDay"), showTipOfTheDay);
}

void sysSettings::SetNextTipOfTheDay(const int newval)
{
    nextTipOfTheDay = newval;
    this->Write(wxT("NextTipOfTheDay"), nextTipOfTheDay);
}

//////////////////////////////////////////////////////////////////////////
// Log
//////////////////////////////////////////////////////////////////////////

void sysSettings::SetLogFile(const wxString& newval)
{
    logFile = newval;
    this->Write(wxT("LogFile"), logFile);
}

void sysSettings::SetLogLevel(const int newval)
{
    logLevel = newval;
    this->Write(wxT("LogLevel"), logLevel);
}

//////////////////////////////////////////////////////////////////////////
// Last Connection
//////////////////////////////////////////////////////////////////////////

void sysSettings::SetLastServer(const wxString& newval)
{
    lastServer = newval;
    this->Write(wxT("LastServer"), lastServer);
}

void sysSettings::SetLastDatabase(const wxString& newval)
{
    lastDatabase = newval;
    this->Write(wxT("LastDatabase"), lastDatabase);
}

void sysSettings::SetLastUsername(const wxString& newval)
{
    lastUsername = newval;
    this->Write(wxT("LastUsername"), lastUsername);
}

void sysSettings::SetLastPort(const int newval)
{
    lastPort = newval;
    this->Write(wxT("LastPort"), lastPort);
}

//////////////////////////////////////////////////////////////////////////
// Show System Objects
//////////////////////////////////////////////////////////////////////////

void sysSettings::SetShowSystemObjects(const bool newval)
{
    showSystemObjects = newval;
    this->Write(wxT("ShowSystemObjects"), showSystemObjects);
}