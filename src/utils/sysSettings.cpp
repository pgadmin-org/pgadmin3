//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002 - 2003, The pgAdmin Development Team
// This software is released under the Artistic Licence
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
#include "misc.h"

sysSettings::sysSettings(const wxString& name) : wxConfig(name)
{
	// Keith 2003.03.05
	// Can't do this because the logger is set up *after* syssettings
	// so nothing will be logged, and it causes a memory leak
    // wxLogInfo(wxT("Creating sSettings object and loading settings"));

    // Tip Of The Day
    Read(wxT("ShowTipOfTheDay"), &showTipOfTheDay, TRUE); 
    Read(wxT("NextTipOfTheDay"), &nextTipOfTheDay, 0); 

    // Log
    Read(wxT("LogFile"), &logFile, wxT("pgadmin.log")); 
    Read(wxT("LogLevel"), &logLevel, LOG_ERRORS);

    // Last Connection
    Read(wxT("LastServer"), &lastServer, wxT("localhost")); 
    Read(wxT("LastDatabase"), &lastDatabase, wxT("template1")); 
    Read(wxT("LastUsername"), &lastUsername, wxT("postgres")); 
    Read(wxT("LastPort"), &lastPort, 5432);

    // Show System Objects
    Read(wxT("ShowSystemObjects"), &showSystemObjects, FALSE); 
    Read(wxT("SqlHelpSite"), &helpSite, wxT("http://developer.postgresql.org/docs/postgres/"));

    maxRows=Read(wxT("frmQuery/MaxRows"), 100L);
    maxColSize=Read(wxT("frmQuery/MaxColSize"), 256L);
    askSaveConfirmation=StrToBool(Read(wxT("AskSaveConfirmation"), wxT("Yes")));
    confirmDelete=StrToBool(Read(wxT("ConfirmDelete"), wxT("Yes")));
    showUsersForPrivileges=StrToBool(Read(wxT("ShowUsersForPrivileges"), wxT("No")));
}


sysSettings::~sysSettings()
{
    wxLogInfo(wxT("Destroying sysSettings object and saving settings"));
    // frMain size/position

    Write(wxT("ShowTipOfTheDay"), showTipOfTheDay);
    Write(wxT("frmQuery/MaxRows"), maxRows);
    Write(wxT("frmQuery/MaxColSize"), maxColSize);
    Write(wxT("AskSaveConfirmation"), BoolToYesNo(askSaveConfirmation));
    Write(wxT("ConfirmDelete"), BoolToYesNo(confirmDelete));
    Write(wxT("ShowUsersForPrivileges"), BoolToYesNo(showUsersForPrivileges));
    Write(wxT("SqlHelpSite"), helpSite);
}

//////////////////////////////////////////////////////////////////////////
// Tip of the Day
//////////////////////////////////////////////////////////////////////////

void sysSettings::SetShowTipOfTheDay(const bool newval)
{
    showTipOfTheDay = newval;
    
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

//////////////////////////////////////////////////////////////////////////
// The size of the query builder
//////////////////////////////////////////////////////////////////////////

wxSize sysSettings::GetFrmQueryBuilderSize()
{
    int width = this->Read(wxT("frmQueryBuilder/Width"), 750);
    int height = this->Read(wxT("frmQueryBuilder/Height"), 550);

	return wxSize(width, height);
}

void sysSettings::SetFrmQueryBuilderSize(wxSize size)
{
    this->Write(wxT("frmQueryBuilder/Width"), size.GetWidth());
    this->Write(wxT("frmQueryBuilder/Height"), size.GetHeight());
}

//////////////////////////////////////////////////////////////////////////
// The position of the query builder
//////////////////////////////////////////////////////////////////////////

wxPoint sysSettings::GetFrmQueryBuilderPos()
{
    int top = this->Read(wxT("frmQueryBuilder/Top"), 50);
    int left = this->Read(wxT("frmQueryBuilder/Left"), 50);

	return wxPoint(top, left);
}

void sysSettings::SetFrmQueryBuilderPos(wxPoint pos)
{
    this->Write(wxT("frmQueryBuilder/Top"), pos.x);
    this->Write(wxT("frmQueryBuilder/Left"), pos.y);
}
